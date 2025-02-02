// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystemComponent.generated.h"

class APlayerCharacter;
class AWeaponPickup;
class USkeletalMeshComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EEquippedWeapon : uint8
{
	EW_NONE = 0 UMETA(DisplayName = "NONE"),
	EW_RIFLE = 1 UMETA(DisplayName = "RIFLE"),
	EW_PISTOL = 2 UMETA(DisplayName = "PISTOL"),

	NUM				UMETA(Hidden)
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class JURASSIC_BB_LD_API UWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AWeaponPickup* GetCurrentWeapon() const;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") EEquippedWeapon EquipType = EEquippedWeapon::EW_NONE;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon Setting") TEnumAsByte<EEquippedWeapon> equippedWeapon = EEquippedWeapon::EW_NONE;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") AWeaponPickup* RifleSlot;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") AWeaponPickup* PistolSlot;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") UAnimMontage* AM_GetRifle;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") UAnimMontage* AM_GetPistol;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") UAnimMontage* AM_GetKnife;
	UPROPERTY(EditAnywhere, Category = "Weapon Setting") UAnimMontage* AM_Reload;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon Setting") bool  isFiring;
	UPROPERTY(BlueprintReadOnly, Category = "Player Character | Weapon Setting") bool bIsReloading;
	UFUNCTION(BlueprintCallable)	void Shoot();
	UFUNCTION(BlueprintCallable)	void StopShoot();
	UFUNCTION(BlueprintCallable)	void Reload();
	UFUNCTION(BlueprintCallable)	void switchGun();
	UFUNCTION(BlueprintCallable)	void EquipWeapon();
	UFUNCTION(BlueprintCallable)	void AttachWeapon(AWeaponPickup* slot, FName socketName);

	UFUNCTION(BlueprintCallable)	void EquipSpecificWeapon(AWeaponPickup* hitWeapon, AWeaponPickup*& slot, FName socketName, FName montageName, EEquippedWeapon weaponType);
	
	UFUNCTION() void DropMagazineNotify();
	UFUNCTION() void AttachMagazineNotify();
	void HandleReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PlayEquipWeaponMontage(FName SectionName, EEquippedWeapon weaponType);
	bool PlayReloadWeaponMontage(EEquippedWeapon weaponType);
	float GetReloadAnimationDuration(EEquippedWeapon weaponType) const;
	void FinishReload();


	void ChangeGun(int32 weaponIndex);
	bool IsValidWeaponSlot(int32 weaponIndex) const;
	int32 currentWeaponIndex;
	FTimerHandle ReloadTimerHandle;
public:
	UPROPERTY() APlayerCharacter* player;
	UPROPERTY() AWeaponPickup* weapon;


};
