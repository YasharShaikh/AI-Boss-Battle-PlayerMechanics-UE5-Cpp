// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BasePickup.h"
#include "WeaponPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagazineAction);

class UParticleSystem;
class AAWeaponMagazine;


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_PISTOL UMETA(DisplayName = "PISTOL"),
	WT_RIFLE UMETA(DisplayName = "RIFLE"),
	WT_SHOTGUN UMETA(DisplayName = "SHOTGUN")
};
UENUM(BlueprintType)
enum class EFireMode : uint8
{
	FM_AUTO UMETA(DisplayName = "AUTO"),
	FM_SINGLE UMETA(DisplayName = "SINGLE"),
	FM_BURST UMETA(DisplayName = "BURST")
};
UENUM(BlueprintType)
enum class ESurfaceType
{
	ST_DEFAULT UMETA(DisplayName = "DEFAULT"),
	ST_FLESH UMETA(DisplayName = "FLESH"),
};
/**
 *
 */
UCLASS()
class JURASSIC_BB_LD_API AWeaponPickup : public ABasePickup
{
	GENERATED_BODY()


public:
	AWeaponPickup();



protected:
	virtual void BeginPlay() override;


public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Properties")EWeaponType WeaponType = EWeaponType::WT_PISTOL;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Properties") EFireMode fireMode = EFireMode::FM_SINGLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") int32 ClipSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") int32 MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") int32 AmmoInClip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float MaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BulletSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float RateofFIre;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") FName MuzzleSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") UParticleSystem* MuzzleEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") UParticleSystem* DefaultImpactEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") UParticleSystem* FleshImpactEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") UParticleSystem* TracerEffect;

	UPROPERTY(BlueprintAssignable, Category = "Weapon | Properties") FOnMagazineAction OnDropMagazine;
	UPROPERTY(BlueprintAssignable, Category = "Weapon | Properties") FOnMagazineAction OnAttachMagazine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties")	AAWeaponMagazine* CurrentMagazine;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Properties") TSubclassOf<class AAWeaponMagazine> MagazineClass;

	UFUNCTION() void DropMagazineNotify();
	UFUNCTION() void AttachMagazineNotify();

	TArray<AAWeaponMagazine*> MagazinePool;
	int32 PoolSize = 5;

	void InitializeMagazinePool();
	AAWeaponMagazine* GetPooledMagazine();

	int32 AmmoDifference;
	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;
	float TimeBetweenShots;
	bool bIsFiring = false;
	bool isReloading = false;

	void Fire();
	void StartFire();
	void StopFire();
	void Reload();
	void Visible(bool showMesh);
	void PlayFireEffect();
	void PlayFireSound();
	void SetFireType(EFireMode fireType);
	virtual void Interact() override;

};
