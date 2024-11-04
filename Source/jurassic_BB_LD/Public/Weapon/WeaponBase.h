// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "WeaponBase.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_PISTOL UMETA(DisplayName = "PISTOL"),
	WT_RIFLE UMETA(DisplayName = "RIFLE")
};
UENUM(BlueprintType)
enum class EFireWeaponType : uint8
{
	FWT_AUTO UMETA(DisplayName = "AUTO"),
	FWT_SINGLE UMETA(DisplayName = "SINGLE"),
	FWT_BURST UMETA(DisplayName = "BURST")
};

#pragma region Forward Declerations

class USkeletalMeshComponent;
class USoundBase;
#pragma endregion


UCLASS()
class JURASSIC_BB_LD_API AWeaponBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:

	AWeaponBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation() override;


	void Fire();
	void StartFire();
	void StopFire();
	void Reload();
	void Visible(bool showMesh);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Type Setting")
	EWeaponType WeaponType = EWeaponType::WT_PISTOL;

	UFUNCTION(BlueprintCallable, Category = "Handle Object state")
	void EnterState(EPickState nextState);


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Object state") EPickState currentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isWatchingMe;



	UPROPERTY(EditAnywhere, Category = "Weapon | Properties") int32 ClipSize;			//Loaded Ammo
	UPROPERTY(EditAnywhere, Category = "Weapon | Properties ")  int32 MaxAmmo;			//Max Loaded Ammo
	int32 AmmoDifference;	//Spare Ammo	
	UPROPERTY(EditAnywhere, Category = "Weapon | Properties") int32 AmmoInClip;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USkeletalMeshComponent* GunMeshComponent;
#pragma endregion



#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Type Setting")
	EFireWeaponType FireType;

	//Weapons SFX/VFX
	UPROPERTY(EditAnywhere, Category = "Weapon | VFX ") UParticleSystem* muzzleFlash;
	UPROPERTY(EditAnywhere, Category = "Weapon | VFX ") UParticleSystem* impactEffect;
	UPROPERTY(EditAnywhere, Category = "Weapon | VFX ") USoundBase* FireSound;
	//Weapon properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float MaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float BulletSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Properties") float RateofFIre;

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;
	float TimeBetweenShots;

	//

#pragma endregion



#pragma region Methods


	void HandleNoneState();
	void HandleHighlightState();
	void HandlePicktState();
	void HandleDroptState();

#pragma endregion



private:

};
