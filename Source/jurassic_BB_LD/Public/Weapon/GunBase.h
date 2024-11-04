// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"


UENUM(BlueprintType)
enum class EWeaponTypee : uint8
{
	WT_PISTOL UMETA(DisplayName = "Pistol"),
	WT_RIFLE UMETA(DisplayName = "Rifle")
};
UENUM(BlueprintType)
enum class EFireWeaponTypee : uint8
{
	FWT_AUTO UMETA(DisplayName = "AUTO"),
	FWT_SINGLE UMETA(DisplayName = "SINGLE"),
	FWT_BURST UMETA(DisplayName = "BURST")
};

//Foward Declerations
class USkeletalMeshComponent;




UCLASS()
class JURASSIC_BB_LD_API AGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fire();
	void StartFire();
	void StopFire();
	void Reload();


		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Type Setting")
		EWeaponTypee WeaponType = EWeaponTypee::WT_PISTOL;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USkeletalMeshComponent* GunMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Type Setting")
	EFireWeaponTypee FireType = EFireWeaponTypee::FWT_AUTO;

	//Weapons SFX/VFX
	UPROPERTY(EditAnywhere, Category = "Weapon | VFX ") UParticleSystem* muzzleFlash;
	UPROPERTY(EditAnywhere, Category = "Weapon | VFX ") UParticleSystem* impactEffect;

	//Weapon properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float MaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BulletSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float RateofFIre;

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;
	float TimeBetweenShots;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Properties") int32 ClipSize;			//Loaded Ammo
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Properties ")  int32 MaxAmmo;			//Max Loaded Ammo
	int32 AmmoDifference;	//Spare Ammo	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Properties") int32 AmmoInClip;		//
};
