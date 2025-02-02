// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponSystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "Weapon/WeaponPickup.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"


// Sets default values for this component's properties
UWeaponSystemComponent::UWeaponSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<APlayerCharacter>(GetOwner());
	if (!player)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSystemComponent owner is not a PlayerCharacter!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("WeaponSystemComponent successfully initialized with owner: %s"), *player->GetName());
	}
}


// Called every frame
void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AWeaponPickup* UWeaponSystemComponent::GetCurrentWeapon() const
{
	switch (EquipType)
	{
	case EEquippedWeapon::EW_PISTOL:
		return PistolSlot;
	case EEquippedWeapon::EW_RIFLE:
		return RifleSlot;
	default:
		return nullptr;
	}
}

void UWeaponSystemComponent::Shoot()
{
	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot fire while reloading!"));
		return;
	}

	if (EquipType == EEquippedWeapon::EW_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Currently unarmed, cannot fire."));
		return;
	}

	AWeaponPickup* CurrentWeapon = (EquipType == EEquippedWeapon::EW_PISTOL) ? PistolSlot : (EquipType == EEquippedWeapon::EW_RIFLE) ? RifleSlot : nullptr;

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon not equipped"));
		return;
	}

	if (CurrentWeapon->AmmoInClip > 0 && !bIsReloading)
	{
		isFiring = true;
		CurrentWeapon->StartFire();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		CurrentWeapon->StopFire();
	}
}

void UWeaponSystemComponent::StopShoot()
{
	if (EquipType == EEquippedWeapon::EW_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Currently unarmed so cannot fire"));
		return;
	}

	AWeaponPickup* CurrentWeapon = (EquipType == EEquippedWeapon::EW_PISTOL) ? PistolSlot :
		(EquipType == EEquippedWeapon::EW_RIFLE) ? RifleSlot : nullptr;

	if (CurrentWeapon == nullptr)
	{
		return;
	}

	isFiring = false;
	CurrentWeapon->StopFire();
}

void UWeaponSystemComponent::Reload()
{
	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already reloading!"));
		return;
	}

	StopShoot();
	// Ensure we have an equipped weapon
	AWeaponPickup* CurrentWeapon = nullptr;
	if (EquipType == EEquippedWeapon::EW_RIFLE)
	{
		CurrentWeapon = RifleSlot;
	}
	else if (EquipType == EEquippedWeapon::EW_PISTOL)
	{
		CurrentWeapon = PistolSlot;
	}

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped, cannot reload."));
		return;
	}

	// Check if the weapon needs reloading
	if (CurrentWeapon->AmmoInClip >= CurrentWeapon->ClipSize)
	{
		UE_LOG(LogTemp, Log, TEXT("Weapon is already fully loaded."));
		return;
	}
	// Play reload montage
	if (PlayReloadWeaponMontage(EquipType))
	{
		bIsReloading = true;

		// Schedule reload logic to complete after montage duration
		float ReloadDuration = GetReloadAnimationDuration(EquipType);
		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, this, &UWeaponSystemComponent::FinishReload, ReloadDuration, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No reload montage available for this weapon."));
	}
}

void UWeaponSystemComponent::switchGun()
{
	const int32 MaxWeapons = 3;
	int32 StartIndex = currentWeaponIndex;
	bool bFoundValid = false;

	do
	{
		currentWeaponIndex = (currentWeaponIndex + 1) % MaxWeapons;
		if (IsValidWeaponSlot(currentWeaponIndex))
		{
			bFoundValid = true;
			break;
		}
	} while (currentWeaponIndex != StartIndex);

	if (bFoundValid)
	{
		ChangeGun(currentWeaponIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapons available."));
		EquipType = EEquippedWeapon::EW_NONE;
	}
}

void UWeaponSystemComponent::EquipWeapon()
{
	if (bIsReloading || isFiring)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip a weapon while reloading or firing."));
		return;
	}

	if (!player)
	{
		UE_LOG(LogTemp, Error, TEXT("Player is null! WeaponSystemComponent has no valid owner."));
		return;
	}

	AActor* actor = player->HitDetection();
	if (!actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actor detected in HitDetection."));
		return;
	}

	if (actor->ActorHasTag("Weapon"))
	{
		UE_LOG(LogTemp, Log, TEXT("Detected weapon: %s"), *actor->GetName());

		if (AWeaponPickup* hitWeapon = Cast<AWeaponPickup>(actor))
		{
			switch (hitWeapon->WeaponType)
			{
			case EWeaponType::WT_RIFLE:
				EquipSpecificWeapon(hitWeapon, RifleSlot, FName("Rifle_Socket"), FName("EquipRifle"), EEquippedWeapon::EW_RIFLE);
				if (PistolSlot)
					PistolSlot->Visible(false);
				break;

			case EWeaponType::WT_PISTOL:
				EquipSpecificWeapon(hitWeapon, PistolSlot, FName("Pistol_Socket"), FName("EquipPistol"), EEquippedWeapon::EW_PISTOL);
				if (RifleSlot)
					RifleSlot->Visible(false);
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown weapon type for %s"), *hitWeapon->GetName());
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon detected / actor lacks 'Weapon' tag."));
	}
}



void UWeaponSystemComponent::EquipSpecificWeapon(AWeaponPickup* hitWeapon, AWeaponPickup*& slot, FName socketName, FName montageName, EEquippedWeapon weaponType)
{
	if (!player) return;
	if (slot != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("already has a weapon equipped in this slot: %s"), *slot->GetName());
		return;
	}
	slot = hitWeapon;
	currentWeaponIndex = (weaponType == EEquippedWeapon::EW_RIFLE) ? 2 : 1;
	AttachWeapon(hitWeapon, socketName);
	PlayEquipWeaponMontage(montageName, weaponType);

	EquipType = weaponType;
	equippedWeapon = weaponType;
}

void UWeaponSystemComponent::AttachWeapon(AWeaponPickup* slot, FName socketName)
{

	if (!slot || !player)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to attach weapon: Invalid slot or player."));
		return;
	}

	USkeletalMeshComponent* Mesh = player->GetMesh();
	if (!Mesh || !Mesh->DoesSocketExist(socketName))
	{
		UE_LOG(LogTemp, Error, TEXT("Socket '%s' does not exist on the player's mesh!"), *socketName.ToString());
		return;
	}

	// Attach and set ownership
	slot->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);
	slot->SetOwner(player);
	slot->SetActorEnableCollision(false); // Disable collision when equipped
	UE_LOG(LogTemp, Log, TEXT("Equipped weapon: %s to socket: %s"), *slot->GetName(), *socketName.ToString());
}

void UWeaponSystemComponent::PlayEquipWeaponMontage(FName SectionName, EEquippedWeapon weaponType)
{
	switch (weaponType)
	{
	case EEquippedWeapon::EW_RIFLE:
		UE_LOG(LogTemp, Warning, TEXT("play EW_RIFLE montage"));
		player->PlayMontage(AM_GetRifle, SectionName);
		break;

	case EEquippedWeapon::EW_PISTOL:
		UE_LOG(LogTemp, Warning, TEXT("play EW_PISTOL montage"));

		player->PlayMontage(AM_GetPistol, SectionName);
		break;

	case EEquippedWeapon::EW_NONE:
		UE_LOG(LogTemp, Warning, TEXT("play EW_NONE montage"));
		player->PlayMontage(AM_GetKnife, SectionName);
		break;

	default:
		break;
	}
}

bool UWeaponSystemComponent::PlayReloadWeaponMontage(EEquippedWeapon weaponType)
{
	UAnimMontage* MontageToPlay = nullptr;

	// Select the appropriate montage based on weapon type
	switch (weaponType)
	{
	case EEquippedWeapon::EW_RIFLE:
		MontageToPlay = AM_GetRifle; // Assign rifle reload montage
		break;

	case EEquippedWeapon::EW_PISTOL:
		MontageToPlay = AM_GetPistol; // Assign pistol reload montage
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("No reload montage available for this weapon type!"));
		return false;
	}

	if (MontageToPlay)
	{
		UAnimInstance* AnimInstance = player->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
			AnimInstance->Montage_JumpToSection(FName("Reload"), MontageToPlay);

			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &UWeaponSystemComponent::HandleReloadMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
			// Ensure weapon exists before binding
			AWeaponPickup* Weapon = GetCurrentWeapon();
			if (Weapon)
			{
				// Bind magazine drop & attach events
				Weapon->OnDropMagazine.AddDynamic(this, &UWeaponSystemComponent::DropMagazineNotify);
				Weapon->OnAttachMagazine.AddDynamic(this, &UWeaponSystemComponent::AttachMagazineNotify);
			}

			return true;
		}
	}

	return false; // Fallback if no montage or anim instance
}
void UWeaponSystemComponent::HandleReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		// Cancel reload if interrupted
		bIsReloading = false;
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
	else
	{
		FinishReload();
	}
}
float UWeaponSystemComponent::GetReloadAnimationDuration(EEquippedWeapon weaponType) const
{
	switch (weaponType)
	{
	case EEquippedWeapon::EW_RIFLE:
		return AM_GetRifle ? AM_GetRifle->GetPlayLength() : 0.0f;

	case EEquippedWeapon::EW_PISTOL:
		return AM_GetPistol ? AM_GetPistol->GetPlayLength() : 0.0f;

	default:
		return 0.0f;
	}
}

void UWeaponSystemComponent::FinishReload()
{
	AWeaponPickup* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
		// Unbind delegates after reload completes
		CurrentWeapon->OnDropMagazine.RemoveDynamic(this, &UWeaponSystemComponent::DropMagazineNotify);
		CurrentWeapon->OnAttachMagazine.RemoveDynamic(this, &UWeaponSystemComponent::AttachMagazineNotify);
	}
	bIsReloading = false;
}

void UWeaponSystemComponent::ChangeGun(int32 weaponIndex)
{
	// Reset equipped type and visibility for all slots
	EquipType = EEquippedWeapon::EW_NONE;
	if (PistolSlot) PistolSlot->Visible(false);
	if (RifleSlot) RifleSlot->Visible(false);

	switch (weaponIndex)
	{
	case 0:
		// Unarmed
		PlayEquipWeaponMontage(FName("Equip"), EEquippedWeapon::EW_NONE);
		UE_LOG(LogTemp, Log, TEXT("Switched to unarmed mode."));
		break;

	case 1:
		if (PistolSlot)
		{
			EquipType = EEquippedWeapon::EW_PISTOL;
			equippedWeapon = EEquippedWeapon::EW_PISTOL;
			PlayEquipWeaponMontage(FName("EquipPistol"), EEquippedWeapon::EW_PISTOL);
			PistolSlot->Visible(true);
			UE_LOG(LogTemp, Log, TEXT("Switched to Pistol."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pistol slot is empty, cycling to the next weapon."));
			switchGun();
		}
		break;

	case 2:
		if (RifleSlot)
		{
			EquipType = EEquippedWeapon::EW_RIFLE;
			equippedWeapon = EEquippedWeapon::EW_RIFLE;
			PlayEquipWeaponMontage(FName("EquipRifle"), EEquippedWeapon::EW_RIFLE);
			RifleSlot->Visible(true);
			UE_LOG(LogTemp, Log, TEXT("Switched to Rifle."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Rifle slot is empty, cycling to the next weapon."));
			switchGun();
		}
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid weapon index: %d"), weaponIndex);
		break;
	}

}

bool UWeaponSystemComponent::IsValidWeaponSlot(int32 weaponIndex) const
{
	switch (weaponIndex)
	{
	case 0: // Unarmed
		return true;

	case 1: // Pistol
		return PistolSlot && EquipType != EEquippedWeapon::EW_PISTOL;

	case 2: // Rifle
		return RifleSlot && EquipType != EEquippedWeapon::EW_RIFLE;

	default:
		return false;
	}
}
void UWeaponSystemComponent::DropMagazineNotify()
{
	UE_LOG(LogTemp, Warning, TEXT("UWeaponSystemComponent: Drop Magazine Triggered"));
}

void UWeaponSystemComponent::AttachMagazineNotify()
{
	UE_LOG(LogTemp, Warning, TEXT("UWeaponSystemComponent: Attach Magazine Triggered"));
}