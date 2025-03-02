// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/DamageInflictingObject.h"
#include "Components/PrimitiveComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ADamageInflictingObject::ADamageInflictingObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	currentState = EObjectStates::EOS_NONE;



	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;

	pullSpeed = 1000.f;

}

// Called when the game starts or when spawned
void ADamageInflictingObject::BeginPlay()
{
	Super::BeginPlay();

	playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

// Called every frame
void ADamageInflictingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!isWatchingMe)
	{
		currentState = EObjectStates::EOS_NONE;
		EnterState(currentState);
	}
	if (isLifting)
	{

		//UE_LOG(LogTemp, Error, TEXT("Lifting object"));

		FVector currentLocation = GetActorLocation();
		float newZ = FMath::InterpEaseOut(currentLocation.Z, liftEndLocation.Z, DeltaTime, LiftSpeed);

		FVector newLocation = FVector(currentLocation.X, currentLocation.Y, newZ);

		if (FMath::IsNearlyEqual(newZ, liftEndLocation.Z, 1.0f))
		{
			newLocation.Z = liftEndLocation.Z;  // Set to exact end location Z value
			isLifting = false;

			//start moving towards player hold location
			InitPull();
		}
		SetActorLocation(newLocation, false, nullptr, ETeleportType::None);
	}
	if (isPulling)
	{
		//UE_LOG(LogTemp, Error, TEXT("Pulling object"));

		Pull();
	}
}

void ADamageInflictingObject::Interact_Implementation()
{
	isWatchingMe = true;
}

void ADamageInflictingObject::EnterState(EObjectStates nextState)
{
	switch (nextState)
	{
	case EObjectStates::EOS_NONE:
		HandleNONEState();
		break;
	case EObjectStates::EOS_HIGHLIGHT:
		HandleHIGHLIGHTState();
		break;
	case EObjectStates::EOS_PULL:
		HandlePULLState();
		break;
	case EObjectStates::EOS_HOLD:
		HandleHOLDState();
		break;
	case EObjectStates::EOS_PUSH:
		HandlePUSHState();
		break;
	}
}


//void ADamageInflictingObject::ExitState(EObjectState currentState)
//{
//	UE_LOG(LogTemp, Error, TEXT("damage inflicting object Current State = NONE"));
//
//}

void ADamageInflictingObject::HandleNONEState()
{
	isWatchingMe = false;


	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetLinearDamping(0.1f);
}

void ADamageInflictingObject::HandleHIGHLIGHTState()
{
	isWatchingMe = true;
	//UE_LOG(LogTemp, Error, TEXT("damage inflicting object Current State = EOS_HIGHLIGHT"));
}

void ADamageInflictingObject::HandlePULLState()
{
	liftEndLocation = liftStartLocation;
	liftEndLocation.Z += LiftHeight;
	isLifting = true;
	StaticMesh->SetEnableGravity(false);
	StaticMesh->SetLinearDamping(20.f);
}

void ADamageInflictingObject::HandleHOLDState()
{
	//UE_LOG(LogTemp, Error, TEXT("damage inflicting object Current State = EOS_HOLD"));
}

void ADamageInflictingObject::HandlePUSHState()
{
	UE_LOG(LogTemp, Warning, TEXT("Push performed"));
	isPulling = false;
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetLinearDamping(0.1f);
	if (playerCharacter)
	{
		FVector forwardVector = playerCharacter->GetActorForwardVector();  // Get the forward vector of the player
		FVector impulse = forwardVector * pushSpeed;  // Calculate the impulse

		StaticMesh->AddImpulse(impulse, NAME_None, true);  // Apply the impulse to the static mesh

		//UE_LOG(LogTemp, Error, TEXT("damage inflicting object Current State = EOS_PUSH"));
	}

}

void ADamageInflictingObject::InitPull()
{
	//UE_LOG(LogTemp, Warning, TEXT("Init pull performed"));
	isLifting = false;

	holdPosition = playerCharacter->objectHoldPosition->GetComponentLocation();
	isPulling = true;
}

void ADamageInflictingObject::Pull()
{

	//UE_LOG(LogTemp, Warning, TEXT("pull performed"));
	if (playerCharacter)
	{
		holdPosition = playerCharacter->objectHoldPosition->GetComponentLocation();

		FVector currentLocation = GetActorLocation();
		float distanceToHoldPos = FVector::Dist(currentLocation, holdPosition);
		FVector direction = (holdPosition - currentLocation).GetSafeNormal();

		float pullForceMag = pullSpeed * distanceToHoldPos;
		FVector pullForce = direction * pullForceMag;

		StaticMesh->AddImpulse(pullForce, NAME_None, true);

		FVector angularImpulse = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f)).GetSafeNormal() * pullSpeed;
		StaticMesh->AddAngularImpulseInDegrees(angularImpulse, NAME_None, true);

		// Stop the object if it is close enough to the hold position
		if (distanceToHoldPos < 5.f) // Adjust this threshold as needed
		{
			SetActorLocation(holdPosition);
			StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector); // Stop the object's movement
		}
	}

}