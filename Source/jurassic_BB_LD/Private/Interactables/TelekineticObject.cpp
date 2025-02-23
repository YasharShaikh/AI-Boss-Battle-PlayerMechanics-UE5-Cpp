#include "Interactables/TelekineticObject.h"
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

// **Optimized Static TMap (Stored in Read-Only Memory)**
const TMap<EObjectSize, FTelekineticObjectStats> ATelekineticObject::ObjectStatsMap = {
	{ EObjectSize::Small,  { 0.5f, 5.0f, 10.0f, 5.0f,5000.0f } },
	{ EObjectSize::Medium, { 1.0f, 10.0f, 05.0f, 0.05f, 5000.0f } },
	{ EObjectSize::Large,  { 1.5f, 15.0f, 30.0f, 12.0f, 50.0f } }
};

// **Constructor: Initialize Default Size**
ATelekineticObject::ATelekineticObject()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = MeshComponent;
	ObjectSize = EObjectSize::Medium;

	bHasBeenLifted = false;
	bHasBeenThrown = false;
}

void ATelekineticObject::Tick(float DeltaTime) {}

void ATelekineticObject::BeginPlay()
{
	Super::BeginPlay();
	SetSize(ObjectSize);
}

void ATelekineticObject::SetSize(EObjectSize Size)
{
	ObjectSize = Size;

	const FTelekineticObjectStats* Stats = ObjectStatsMap.Find(Size);
	if (Stats)
	{
		CachedStats = *Stats;
		MeshComponent->SetRelativeScale3D(FVector(CachedStats.MeshScaleFactor));

		UE_LOG(LogTemp, Warning, TEXT("Size Set: %d | Damage: %.1f | Stamina Cost: %.1f"),
			static_cast<int32>(Size), CachedStats.Damage, CachedStats.StaminaCost);
	}
}
bool ATelekineticObject::LiftObject(float DeltaTime)
{
	if (bHasBeenLifted)
		return true;

	FVector CurrentLocation = GetActorLocation();

	// **Use Exponential Decay for Smoother Motion**
	float Alpha = 1.0f - FMath::Exp(-LiftSpeed * DeltaTime);
	float NewZLocation = FMath::Lerp(CurrentLocation.Z, LiftEndLocation, Alpha);

	SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZLocation));

	if (FMath::IsNearlyEqual(NewZLocation, LiftEndLocation, 1.0f))
	{
		UE_LOG(LogTemp, Log, TEXT("Telekinetic object Lift Complete "));
		bHasBeenLifted = true; // Prevents unnecessary lifting
		return true;
	}
	UE_LOG(LogTemp, Log, TEXT("Telekinetic object LIFTING "));
	return false;
}

// **Optimized MoveTowardsPlayer**
void ATelekineticObject::MoveTowardsPlayer(const FVector& HoldPosition, float DeltaTime)
{
	if (!bHasBeenLifted && !LiftObject(DeltaTime))
		return; // Ensure object is lifted first

	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetLinearDamping(20.f);

	FVector CurrentLocation = GetActorLocation();
	FVector Direction = (HoldPosition - CurrentLocation).GetSafeNormal();
	float DistanceToHoldPosSq = FVector::DistSquared(CurrentLocation, HoldPosition);

	// **Check if the object is outside the safe pull distance**
	if (DistanceToHoldPosSq > FMath::Square(CachedStats.SafePullDistance))
	{
		float PullForceMagnitude = FMath::Clamp(600.0f * FMath::Sqrt(DistanceToHoldPosSq), 0.0f, MaxPullForce);

		MeshComponent->AddImpulse(Direction * PullForceMagnitude, NAME_None, true);

		// **Add Rotational Force for Realistic Motion**
		FVector AngularImpulse = FVector(FMath::RandRange(-1.f, 1.f),
			FMath::RandRange(-1.f, 1.f),
			FMath::RandRange(-1.f, 1.f)).GetSafeNormal() * PullForceMagnitude * 0.1f;

		MeshComponent->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);
	}
}

// **Move Towards Enemy (Throw Mechanic)**
void ATelekineticObject::MoveTowardsEnemy(const FVector& ScreenCenter)
{
	if (bHasBeenThrown)
		return; // Prevent multiple throw forces

	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetLinearDamping(0.1f);

	FVector ForwardVector = ScreenCenter.GetSafeNormal();
	FVector Impulse = ForwardVector * ThrowSpeed;

	MeshComponent->AddImpulse(Impulse, NAME_None, true);

	// Mark as thrown to avoid multiple impulse applications
	bHasBeenThrown = true;

	UE_LOG(LogTemp, Warning, TEXT("Telekinetic Object Thrown at Speed: %f"), ThrowSpeed);
}
