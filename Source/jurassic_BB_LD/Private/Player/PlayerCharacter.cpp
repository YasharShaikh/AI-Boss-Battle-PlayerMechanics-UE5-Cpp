// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/PlayerCharacter.h"
#include "Animation/AnimMontage.h"
#include "Interfaces/InteractInterface.h"
#include "Interactables/DamageInflictingObject.h"
#include "Interactables/InteractableForPlacement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Player/TripWireComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/WeaponSystemComponent.h"
#include "Weapon/WeaponPickup.h"

class DamageInflictingObject;
class InteractableForPlacement;

// Sets default values
APlayerCharacter::APlayerCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	frameCounter = 0;

	isSprinting = false;


	PlayerViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerViewCamera->SetupAttachment(RootComponent);
	PlayerViewCamera->bUsePawnControlRotation = true;


	objectHoldPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Object Hold Position"));
	objectHoldPosition->SetupAttachment(PlayerViewCamera);


	tripwireComponent = CreateDefaultSubobject<UTripWireComponent>(TEXT("Trip Wire Component"));

	WeaponSystemComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("Weapon Component"));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerViewCamera && GetMesh())
	{
		GetMesh()->AttachToComponent(PlayerViewCamera, FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetOnlyOwnerSee(true);
	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SprintMechanic(DeltaTime);

	DetectTelekineticObject();
	//Detector();



	HitDetection();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Axis Mapping
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &APlayerCharacter::LookRight);

	//Action Mapping
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Released, this, &APlayerCharacter::StopSprint);
	

	PlayerInputComponent->BindAction(FName("AimDownSight"), IE_Pressed, this, &APlayerCharacter::AimDownSight);
	PlayerInputComponent->BindAction(FName("AimDownSight"), IE_Released, this, &APlayerCharacter::StopAimDownSights);


	PlayerInputComponent->BindAction(FName("Crouch"), IE_Pressed, this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction(FName("Crouch"), IE_Released, this, &APlayerCharacter::StopCrouch);

	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &APlayerCharacter::interactPressed);
	PlayerInputComponent->BindAction(FName("Interact"), IE_Released, this, &APlayerCharacter::interactReleased);


}

void APlayerCharacter::MoveForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector() * axisValue);
}

void APlayerCharacter::MoveRight(float axisValue)
{
	AddMovementInput(GetActorRightVector() * axisValue);
}

void APlayerCharacter::LookUp(float axisValue)
{
	AddControllerPitchInput(axisValue);
}

void APlayerCharacter::LookRight(float axisValue)
{
	AddControllerYawInput(axisValue);
}

void APlayerCharacter::Sprint()
{
	isSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 750.0f;
}

void APlayerCharacter::StopSprint()
{
	isSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void APlayerCharacter::StartCrouch()
{
	Crouch();
}

void APlayerCharacter::StopCrouch()
{
	UnCrouch();
}

void APlayerCharacter::SprintMechanic(float deltaTime)
{
	if (isSprinting)
	{
		currentStamina -= sprintStaminaConsuptionRate * deltaTime;
		if (currentStamina < sprintStaminaConsuptionRate)
		{
			StopSprint();
		}
	}
	else
	{
		RegenerateStamina(deltaTime);
	}

}

void APlayerCharacter::RegenerateStamina(float deltaTime)
{
	if (currentStamina < Stamina)
	{
		currentStamina += staminaRegenerateRate * deltaTime;
		currentStamina = FMath::Clamp(currentStamina, 0.0f, Stamina);
	}
}

void APlayerCharacter::ResetExoCharge()
{
	canUseExoCharge = true;
}

void APlayerCharacter::interactPressed()
{
	bInteractPressed = true;

	if (WeaponSystemComponent)
	{
		WeaponSystemComponent->EquipWeapon();
	}

}

void APlayerCharacter::interactReleased()
{
	bInteractPressed = false;
}


#pragma region Telekenisis




//TODO: turn this into single common function for detection 
void APlayerCharacter::DetectTelekineticObject()
{

	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	FVector startLocation = CameraLocation;
	FVector endLocation = startLocation + CameraRotation.Vector() * maxPickRange;

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(maxPickRange);

	if (!bUsingTelekenisis)
	{
		bool bHasHit = GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECC_Visibility, queryParams);
		if (bHasHit)
		{

			AActor* hitActor = hitResult.GetActor();
			if (IInteractInterface* iinteractInterface = Cast<IInteractInterface>(hitResult.GetActor()))
			{

				if (ADamageInflictingObject* damageInflictingObject = Cast<ADamageInflictingObject>(hitResult.GetActor()))
				{
					bIsDamageInflictingObject = true;
					if (storeDamageInflictingObject && storeDamageInflictingObject != damageInflictingObject)
					{
						storeDamageInflictingObject->EnterState(EObjectState::EOS_NONE);
					}
					storeDamageInflictingObject = damageInflictingObject;
					storeDamageInflictingObject->EnterState(EObjectState::EOS_HIGHLIGHT);
				}
				else if (storeDamageInflictingObject)
				{
					bIsDamageInflictingObject = false;
					storeDamageInflictingObject->EnterState(EObjectState::EOS_NONE);
					storeDamageInflictingObject = nullptr;
				}
				if (AInteractableForPlacement* interactableForPlacement = Cast<AInteractableForPlacement>(hitResult.GetActor()))
				{
					bIsInteractableForPlacement = true;
					storeInteractableForPlacement = interactableForPlacement;
				}
				else
				{
					bIsInteractableForPlacement = false;
					storeInteractableForPlacement = nullptr;
				}


			}

		}

	}

}


void APlayerCharacter::Telekinesis_Pull()
{

	if (storeDamageInflictingObject)
	{

		bUsingTelekenisis = true;
		storeDamageInflictingObject->EnterState(EObjectState::EOS_PULL);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No object to pull"));
	}

}

void APlayerCharacter::Telekinesis_Push()
{
	if (storeDamageInflictingObject)
	{
		storeDamageInflictingObject->EnterState(EObjectState::EOS_PUSH);
		storeDamageInflictingObject = nullptr;
		bUsingTelekenisis = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No object to pull"));
	}

}
#pragma endregion


#pragma region Weapon System


void APlayerCharacter::AimDownSight()
{
	UE_LOG(LogTemp, Error, TEXT("Aiming down sights"));
}

void APlayerCharacter::StopAimDownSights()
{
	UE_LOG(LogTemp, Error, TEXT("Stopped Aiming down sights"));

}

void APlayerCharacter::PlayMontage(UAnimMontage* Montage, FName SectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && Montage)
	{
		animInstance->Montage_Play(Montage);
		animInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

AActor* APlayerCharacter::HitDetection()
{

	FVector cameraLocation;
	FRotator cameraRotation;

	GetActorEyesViewPoint(cameraLocation, cameraRotation);

	FVector endLocation = cameraLocation + (cameraRotation.Vector() * 1000.0f);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	bool hitActor = GetWorld()->LineTraceSingleByChannel(
		hitResult,
		cameraLocation,
		endLocation,
		ECC_Visibility,
		queryParams);


	if (hitActor)
	{

		return hitResult.GetActor();
	}
	return nullptr;
}




#pragma endregion


