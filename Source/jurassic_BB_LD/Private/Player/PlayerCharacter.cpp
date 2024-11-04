// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/PlayerCharacter.h"
#include "Animation/AnimMontage.h"
#include "Interfaces/InteractInterface.h"
#include "Interactables/DamageInflictingObject.h"
#include "Interactables/InteractableForPlacement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/WeaponBase.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Player/TripWireComponent.h"


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

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SprintMechanic(DeltaTime);

	DetectTelekineticObject();
	Detector();
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

	PlayerInputComponent->BindAction(FName("Shoot"), IE_Pressed, this, &APlayerCharacter::Shoot);
	PlayerInputComponent->BindAction(FName("Shoot"), IE_Released, this, &APlayerCharacter::StopShoot);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Released, this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction(FName("AimDownSight"), IE_Pressed, this, &APlayerCharacter::AimDownSight);
	PlayerInputComponent->BindAction(FName("AimDownSight"), IE_Released, this, &APlayerCharacter::StopAimDownSights);


	PlayerInputComponent->BindAction(FName("Crouch"), IE_Pressed, this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction(FName("Crouch"), IE_Released, this, &APlayerCharacter::StopCrouch);

	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &APlayerCharacter::interactPressed);
	PlayerInputComponent->BindAction(FName("Interact"), IE_Released, this, &APlayerCharacter::interactReleased);


}

void APlayerCharacter::Detector()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + CameraRotation.Vector() * gunMaxPickRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);

	static AWeaponBase* PreviouslyHighlightedWeapon = nullptr;

	if (bHasHit)
	{
		if (IInteractInterface* InteractInterface = Cast<IInteractInterface>(HitResult.GetActor()))
		{
			AWeaponBase* HitWeapon = Cast<AWeaponBase>(HitResult.GetActor());

			if (HitWeapon)
			{
				if (PreviouslyHighlightedWeapon && PreviouslyHighlightedWeapon != HitWeapon)
				{
					PreviouslyHighlightedWeapon->currentState = EPickState::EPS_NONE;
				}

				HitWeapon->currentState = EPickState::EPS_HIGHLIGHT;
				PreviouslyHighlightedWeapon = HitWeapon;

				if (bInteractPressed)
				{
					if (HitWeapon->WeaponType == EWeaponType::WT_RIFLE)
					{
						if (RifleSlot != nullptr)
						{
							UE_LOG(LogTemp, Warning, TEXT("Already has rifle"));
							// Optionally handle swapping or ignoring pickup
							return;
						}
						else
						{
							EquipType = EEquippedWeapon::EW_RIFLE;
							equippedWeapon = EEquippedWeapon::EW_RIFLE;
							PlayEquipWeaponMontage(FName("EquipRifle"), EEquippedWeapon::EW_RIFLE);
							RifleSlot = HitWeapon;
							if (PistolSlot) PistolSlot->Visible(false);
							// Attach rifle to player's mesh
							RifleSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Rifle_Socket"));
							RifleSlot->SetOwner(this);

							UE_LOG(LogTemp, Log, TEXT("Equipped rifle"));
						}
					}
					else if (HitWeapon->WeaponType == EWeaponType::WT_PISTOL)
					{
						if (PistolSlot != nullptr)
						{
							UE_LOG(LogTemp, Warning, TEXT("Already has Pistol"));
							// Optionally handle swapping or ignoring pickup
							return;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Picked up weapon: Pistol"));
							EquipType = EEquippedWeapon::EW_PISTOL;
							equippedWeapon = EEquippedWeapon::EW_PISTOL;
							PlayEquipWeaponMontage(FName("EquipPistol"), EEquippedWeapon::EW_PISTOL);
							PistolSlot = HitWeapon;
							if (RifleSlot) RifleSlot->Visible(false);
							// Attach pistol to player's mesh
							PistolSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Socket"));
							PistolSlot->SetOwner(this);

							UE_LOG(LogTemp, Log, TEXT("Equipped pistol"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Unknown weapon type"));
					}
				}
			}
		}
	}
	else
	{
		if (PreviouslyHighlightedWeapon)
		{
			PreviouslyHighlightedWeapon->currentState = EPickState::EPS_NONE;
			PreviouslyHighlightedWeapon = nullptr;
		}
	}

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


void APlayerCharacter::Shoot()
{
	if (EquipType == EEquippedWeapon::EW_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Currently unarmed so cannot fire"));
		return;
	}

	// Assign the current weapon based on EquipType
	AWeaponBase* CurrentWeapon = (EquipType == EEquippedWeapon::EW_PISTOL) ? PistolSlot :
		(EquipType == EEquippedWeapon::EW_RIFLE) ? RifleSlot : nullptr;

	if (CurrentWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon not equipped"));
		return;
	}

	// Check if there is ammo and start firing
	if (CurrentWeapon->AmmoInClip > 0)
	{
		isFiring = true;
		CurrentWeapon->StartFire();
	}
	else
	{
		CurrentWeapon->StopFire();
	}
}

void APlayerCharacter::StopShoot()
{
	if (EquipType == EEquippedWeapon::EW_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Currently unarmed so cannot fire"));
		return;
	}

	AWeaponBase* CurrentWeapon = (EquipType == EEquippedWeapon::EW_PISTOL) ? PistolSlot :
		(EquipType == EEquippedWeapon::EW_RIFLE) ? RifleSlot : nullptr;

	if (CurrentWeapon == nullptr)
	{
		return;
	}

	isFiring = false;
	CurrentWeapon->StopFire();
}





void APlayerCharacter::Reload()
{

	if (EquipType == EEquippedWeapon::EW_RIFLE)
	{
		if (RifleSlot->AmmoInClip < RifleSlot->ClipSize)
		{
			RifleSlot->Reload();
			PlayReloadWeaponMontage(FName("Reload_Rifle"), EEquippedWeapon::EW_RIFLE);
		}
	}
	else if (EquipType == EEquippedWeapon::EW_PISTOL)
	{
		if (PistolSlot->AmmoInClip < PistolSlot->ClipSize)
		{
			PistolSlot->Reload();
			PlayReloadWeaponMontage(FName("Reload_Pistol"), EEquippedWeapon::EW_PISTOL);
		}

	}

}


void APlayerCharacter::switchGun()
{
	currentWeaponIndex = (currentWeaponIndex + 1) % 3; // Cycle through 0, 1, 2
	ChangeGun(currentWeaponIndex);
}

void APlayerCharacter::ChangeGun(int32 weaponIndex)
{
	// Reset visibility and equip type
	EquipType = EEquippedWeapon::EW_NONE;
	equippedWeapon = EEquippedWeapon::EW_NONE;
	if (PistolSlot) PistolSlot->Visible(false);
	if (RifleSlot) RifleSlot->Visible(false);

	switch (weaponIndex)
	{
	case 0:
		// Unarmed
		PlayEquipWeaponMontage(FName("Equip"), EEquippedWeapon::EW_NONE);
		break;

	case 1:
		// Equip Pistol if available
		if (PistolSlot)
		{
			EquipType = EEquippedWeapon::EW_PISTOL;
			equippedWeapon = EEquippedWeapon::EW_PISTOL;
			PlayEquipWeaponMontage(FName("EquipPistol"), EEquippedWeapon::EW_PISTOL);
			PistolSlot->Visible(true);
		}
		else
		{
			// If pistol not available, switch to the next weapon
			switchGun();
		}
		break;

	case 2:
		// Equip Rifle if available
		if (RifleSlot)
		{
			EquipType = EEquippedWeapon::EW_RIFLE;
			equippedWeapon = EEquippedWeapon::EW_RIFLE;
			PlayEquipWeaponMontage(FName("EquipRifle"), EEquippedWeapon::EW_RIFLE);
			RifleSlot->Visible(true);
		}
		else
		{
			// If rifle not available, switch to the next weapon
			switchGun();
		}
		break;

	default:
		break;
	}
}


void APlayerCharacter::AttachRifle()
{
	RifleSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Rifle_Socket"));
	RifleSlot->SetOwner(this);
	UE_LOG(LogTemp, Error, TEXT("Equiped rifle"));
}

void APlayerCharacter::AttachPistol()
{
	PistolSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Rifle_Socket"));
	PistolSlot->SetOwner(this);
	UE_LOG(LogTemp, Error, TEXT("Equiped Pistol"));
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

void APlayerCharacter::PlayEquipWeaponMontage(FName SectionName, EEquippedWeapon weaponType)
{
	switch (weaponType)
	{
	case EEquippedWeapon::EW_RIFLE:
		UE_LOG(LogTemp, Warning, TEXT("play rifle montage"));
		PlayMontage(AM_GetRifle, SectionName);
		break;

	case EEquippedWeapon::EW_PISTOL:
		UE_LOG(LogTemp, Warning, TEXT("play EW_PISTOL montage"));

		PlayMontage(AM_GetPistol, SectionName);
		break;

	case EEquippedWeapon::EW_NONE:
		UE_LOG(LogTemp, Warning, TEXT("play EW_NONE montage"));

		PlayMontage(AM_GetKnife, SectionName);
		break;

	default:
		break;
	}


}

void APlayerCharacter::PlayReloadWeaponMontage(FName SectionName, EEquippedWeapon weaponType)
{
	switch (weaponType)
	{
	case EEquippedWeapon::EW_RIFLE:
		UE_LOG(LogTemp, Warning, TEXT("play rifle montage"));
		PlayMontage(AM_Reload, SectionName);
		break;

	case EEquippedWeapon::EW_PISTOL:
		UE_LOG(LogTemp, Warning, TEXT("play EW_PISTOL montage"));

		PlayMontage(AM_Reload, SectionName);
		break;

	default:
		break;
	}

}


#pragma endregion


