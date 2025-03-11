#include "Player/TelekinesisComponent.h"
#include "Curves/CurveFloat.h"
#include "Player/PlayerCharacter.h"
#include "Interactables/TelekineticObject.h"

UTelekinesisComponent::UTelekinesisComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsHoldingObject = false;
	CurrentStamina = MaxStamina;
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}

void UTelekinesisComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTelekinesisComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsHoldingObject)
		UpdateStamina(DeltaTime);
	else
		RegenerateStamina(DeltaTime);
}

void UTelekinesisComponent::StartTelekinesis()
{

    if (!PlayerCharacter)
    {
        return;
    }

    AActor* ActorHit = PlayerCharacter->HitDetection();
    if (!ActorHit)
    {
        return;
    }

    if (!ActorHit->ActorHasTag("TelekineticObject"))
    {
        return;
    }

    ATelekineticObject* Object = Cast<ATelekineticObject>(ActorHit);
    if (!Object)
    {
        return;
    }

    // Fetch stats
    const FTelekineticObjectStats* Stats = Object->GetSizeStats();
    if (!Stats)
    {
        return;
    }

    // Calculate distance
    float DistanceToObject = FVector::Dist(PlayerCharacter->GetActorLocation(), Object->GetActorLocation());

    if (DistanceToObject > Stats->SafePullDistance)
    {
        return;
    }

    // Check stamina
    if (CurrentStamina < Stats->StaminaCost)
    {
        return;
    }

    // Consume stamina
    ConsumeStamina(Stats->StaminaCost);

    // Assign the object
    HeldObject = Object;
    bIsHoldingObject = true;
    SafePullDistance = Stats->SafePullDistance;


    HoldTelekineticObject();
}

void UTelekinesisComponent::HoldTelekineticObject()
{
	if (!HeldObject) return;
	HeldObject->EnterState(ETObjectState::Lifting);
	if (CurrentStamina == 0.0f)
	{
		HeldObject->EnterState(ETObjectState::Thrown);
		bIsHoldingObject = false;

	}
}

void UTelekinesisComponent::ReleaseTelekineticObject()
{

	if (!HeldObject) return;

	HeldObject->EnterState(ETObjectState::Thrown);
	bIsHoldingObject = false;
}

void UTelekinesisComponent::UpdateStamina(float DeltaTime)
{

	if (PlayerCharacter && HeldObject)
	{
		ConsumeStamina(SafePullDistance * DeltaTime);
	}
}

void UTelekinesisComponent::ConsumeStamina(float Amount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
}

void UTelekinesisComponent::RegenerateStamina(float DeltaTime)
{
	if (!bIsHoldingObject)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenRate * DeltaTime), 0.0f, MaxStamina);
	}
}

bool UTelekinesisComponent::CanUseTelekinesis() const
{
	return CurrentStamina > 0;
}
