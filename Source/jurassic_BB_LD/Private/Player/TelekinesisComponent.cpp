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
	UE_LOG(LogTemp, Warning, TEXT("Start Telekinesis"));

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Character not found"));
		return;
	}
	AActor* ActorHit = PlayerCharacter->HitDetection();
	if (!ActorHit || !ActorHit->ActorHasTag("TelekineticObject")) return;

	ATelekineticObject* Object = Cast<ATelekineticObject>(ActorHit);
	if (!Object) return;

	const FTelekineticObjectStats* Stats = Object->GetSizeStats();



	float DistanceToObject = FVector::Dist(PlayerCharacter->GetActorLocation(), Object->GetActorLocation());
	if (DistanceToObject > Stats->SafePullDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object is too far away! required distance :%f"), Stats->SafePullDistance);
		UE_LOG(LogTemp, Warning, TEXT("Distance to object : % f"), DistanceToObject);
		return;
	}

	if (CurrentStamina < Stats->StaminaCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough stamina to grab object!"));
		return;
	}

	ConsumeStamina(Stats->StaminaCost);

	HeldObject = Object;
	bIsHoldingObject = true;
	SafePullDistance = Stats->SafePullDistance;

	UE_LOG(LogTemp, Log, TEXT("Telekinetic Object Grabbed: %s"), *Object->GetName());

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
