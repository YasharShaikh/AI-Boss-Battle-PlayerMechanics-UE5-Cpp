#include "Interactables/TelekineticObject.h"
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/PoolSubSystem.h"


const TArray<FTelekineticObjectStats> ATelekineticObject::ObjectStatsArray = {

	// Scale, Dmg, Stam, Distance,  MaxPull, Lift, Throw, Mass,  PushSpeed
	{ 0.5f, 5.0f, 10.0f, 500.0f, 5000.0f, 100.0f, 2000.0f, 50.0f, 1500.0f  }, // Small  
	{ 1.0f, 10.0f, 0.50f, 1000.0f, 3000.0f, 0.50f, 200.0f, 100.0f, 5000.0f  }, // Medium
	{ 1.5f, 15.0f, 30.0f, 1500.0f, 1000.0f,100.0f, 800.0f, 200.0f, 500.0f  } // Large
};

ATelekineticObject::ATelekineticObject()
{
	PrimaryActorTick.bCanEverTick = true;


	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = MeshComponent;
	ObjectSize = EObjectSize::Medium;
}

void ATelekineticObject::BeginPlay()
{
	Super::BeginPlay();
	playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	SetSize(ObjectSize);
}
void ATelekineticObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!playerCharacter || !playerCharacter->objectHoldPosition)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing player reference!"));
		return;
	}

	switch (CurrentState)
	{
	case ETObjectState::Lifting:
	{
		TargetPostion = playerCharacter->objectHoldPosition->GetComponentLocation();
		const FVector NewLocation = FMath::VInterpTo(
			GetActorLocation(),
			TargetPostion,
			DeltaTime,
			CachedStats.LiftSpeed
		);

		// Check if reached target with threshold
		const bool bReached = FVector::DistSquared(NewLocation, TargetPostion) < FMath::Square(50.0f);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

		if (bReached) EnterState(ETObjectState::Held);
	}
	break;

	case ETObjectState::Held:
	{
		// Smoothly maintain position
		TargetPostion = playerCharacter->objectHoldPosition->GetComponentLocation();
		const FVector NewLocation = FMath::VInterpTo(
			GetActorLocation(),
			TargetPostion,
			DeltaTime,
			CachedStats.LiftSpeed * 0.5f  // Slower follow speed
		);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::None);

		// Controlled rotation
		const FRotator TargetRotation = FRotator(
			FMath::Sin(GetWorld()->GetTimeSeconds()) * 45.0f,
			FMath::Cos(GetWorld()->GetTimeSeconds()) * 45.0f,
			0.0f
		);
		MeshComponent->SetWorldRotation(FMath::RInterpTo(
			GetActorRotation(),
			TargetRotation,
			DeltaTime,
			CachedStats.LiftSpeed * 0.3f
		));
	}
	break;

	//case ETObjectState::Thrown:
	//{
	//	// Check velocity magnitude to determine if stopped
	//	const float SpeedSq = MeshComponent->GetPhysicsLinearVelocity().SizeSquared();
	//	if (SpeedSq < FMath::Square(0.10f))  // 50 cm/s threshold
	//	{
	//		EnterState(ETObjectState::Idle);
	//	}
	//}
	//break;
	}
}

void ATelekineticObject::SetSize(EObjectSize Size)
{

	const int32 Index = static_cast<int32>(Size);
	if (ObjectStatsArray.IsValidIndex(Index))
	{
		CachedStats = ObjectStatsArray[Index];
		MeshComponent->SetRelativeScale3D(FVector(CachedStats.MeshScaleFactor));
		MeshComponent->SetMassOverrideInKg(NAME_None, CachedStats.Mass);
	}
}


void ATelekineticObject::EnterState(ETObjectState NewState)
{

	if (CurrentState == NewState) return;

	// Exit current state
	switch (CurrentState)
	{
	case ETObjectState::Held:
		MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		break;
	}

	CurrentState = NewState;

	switch (NewState)
	{
	case ETObjectState::Idle:
		SetActorTickEnabled(false);
		UpdatePhysicsSettings(true);
		ResetPhysicsState();
		break;

	case ETObjectState::Lifting:
		SetActorTickEnabled(true);
		UpdatePhysicsSettings(false);
		ResetPhysicsState();
		break;

	case ETObjectState::Held:
	{
		SetActorTickEnabled(true);
		UpdatePhysicsSettings(true);
		break;
	}


	case ETObjectState::Thrown:
		SetActorTickEnabled(true);
		UpdatePhysicsSettings(false);
		ResetPhysicsState();
		if (playerCharacter)
		{
			FVector ForwardVector = playerCharacter->GetActorForwardVector();
			FVector Impulse = ForwardVector * CachedStats.PushSpeed;
			MeshComponent->AddImpulse(Impulse, NAME_None, true);
		}
		break;

	default:
		break;
	}
}



void ATelekineticObject::UpdatePhysicsSettings(bool bHoverMode)
{
	MeshComponent->SetEnableGravity(!bHoverMode);
	MeshComponent->SetLinearDamping(bHoverMode ? 20.0f : 0.1f);
	MeshComponent->SetAngularDamping(bHoverMode ? 2.0f : 0.5f);
}

void ATelekineticObject::ResetPhysicsState()
{
	MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

void ATelekineticObject::ReturnToPool()
{
	if (CurrentState == ETObjectState::Thrown)
	{
		if (UPoolSubSystem* PoolSubSystem = GetWorld()->GetSubsystem<UPoolSubSystem>())
		{
			PoolSubSystem->ReturnToPool(this);
		}
	}
}

void ATelekineticObject::OnSpawnFromPool_Implementation()
{    // Basic activation (you NEED at least this)
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Required state reset
	CurrentState = ETObjectState::Idle;
}

void ATelekineticObject::OnReturnToPool_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ResetPhysicsState();

	// Optional: Reset any other object-specific state
	CurrentState = ETObjectState::Idle;
}