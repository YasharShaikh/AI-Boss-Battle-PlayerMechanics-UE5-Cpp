// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

// Forward Declarations
class UAnimMontage;
class UCameraComponent;
class UCharacterMovementComponent;
class AWeaponBase;
class ADamageInflictingObject;
class AInteractableForPlacement;
class UStaticMeshComponent;
class USceneComponent;
class UCurveFloat;
class USplineComponent;
class ATrapTripWire;
class UTripWireComponent;
class UBoxComponent;



UENUM(BlueprintType)
enum class EEquippedWeapon : uint8
{
	EW_NONE = 0 UMETA(DisplayName = "NONE"),
	EW_RIFLE = 1 UMETA(DisplayName = "RIFLE"),
	EW_PISTOL = 2 UMETA(DisplayName = "PISTOL"),

	NUM				UMETA(Hidden)
};


UCLASS()
class JURASSIC_BB_LD_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



protected:
	virtual void BeginPlay() override;

#pragma region Components

public:
	UPROPERTY(VisibleAnywhere)		USceneComponent* objectHoldPosition;
protected:
	UPROPERTY(VisibleAnywhere)		UCameraComponent* PlayerViewCamera;

#pragma endregion



#pragma region Movement System

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		bool bInteractPressed = false;
	UPROPERTY(BlueprintReadWrite) bool  isSprinting;
	UPROPERTY(BlueprintReadWrite) bool  isCrouching;

	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float Stamina;
	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float sprintStaminaConsuptionRate;
	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float staminaRegenerateRate;
	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float currentStamina;
	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float jumpHeight;
	UPROPERTY(EditAnywhere, Category = "Player Character | Movement System ")float crouchHeight;

private:

	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	void LookUp(float axisValue);
	void LookRight(float axisValue);

	void Sprint();
	void StopSprint();

	void StartCrouch();
	void StopCrouch();

	void Detector();


	void interactPressed();
	void interactReleased();


#pragma endregion



#pragma region EXO Skeleton System


protected:
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Skeleton System") float exoChargeDashLaunchDistance;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Skeleton System")float dashExoChargeCooldownTime;
	UPROPERTY(BlueprintReadWrite) bool  canUseExoCharge = true;

	void SprintMechanic(float deltaTime);
	void RegenerateStamina(float deltaTime);

#pragma endregion



#pragma region EXO Telekinesis
private:
	UPROPERTY(VisibleAnywhere, Category = "Player Character | EXO Telekinesis System")		bool bUsingTelekinesis;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")			float  maxPickRange;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")			float pullForce;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")			float pushForce;
	// The curve that will drive the timeline
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")
	UCurveFloat* DashCurve = nullptr;

	UFUNCTION(BlueprintCallable)	void Telekinesis_Pull();
	UFUNCTION(BlueprintCallable)	void Telekinesis_Push();
	//UFUNCTION(BlueprintCallable)	void Dash();

	void ResetExoCharge();
	void DetectTelekineticObject();
#pragma endregion

#pragma region Firing System

protected:

	UPROPERTY(BlueprintReadWrite) bool  isFiring;


	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon System") float  gunMaxPickRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character | Weapon Setting")EEquippedWeapon EquipType = EEquippedWeapon::EW_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character | Weapon Setting") TSubclassOf<AWeaponBase> weaponBase;
	UPROPERTY(BlueprintReadWrite, Category = "Player Character | Weapon Setting")TEnumAsByte<EEquippedWeapon> equippedWeapon;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting")AWeaponBase* PistolSlot;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting")AWeaponBase* RifleSlot;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting") UAnimMontage* AM_GetRifle;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting") UAnimMontage* AM_GetPistol;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting") UAnimMontage* AM_GetKnife;
	UPROPERTY(EditAnywhere, Category = "Player Character | Weapon Setting") UAnimMontage* AM_Reload;



	UFUNCTION(BlueprintCallable)	void Shoot();
	UFUNCTION(BlueprintCallable)	void StopShoot();
	UFUNCTION(BlueprintCallable)	void Reload();
	UFUNCTION(BlueprintCallable)	void AimDownSight();
	UFUNCTION(BlueprintCallable)	void StopAimDownSights();
	UFUNCTION(BlueprintCallable)	void switchGun();


private:


	UPROPERTY() AWeaponBase* weapon;

	void ChangeGun(int32 weaponIndex);
	void AttachRifle();
	void AttachPistol();
	void PlayMontage(UAnimMontage* Montage, FName SectionName); // helper 
	void PlayEquipWeaponMontage(FName SectionName, EEquippedWeapon weaponType);
	void PlayReloadWeaponMontage(FName SectionName, EEquippedWeapon weaponType);
#pragma endregion


#pragma region Traps

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Trap")	UTripWireComponent* tripwireComponent;

#pragma endregion










private:
	// Variables
	int32 frameCounter;
	FTimerHandle DashCooldownTimerHandle;

	//Telekinesis
	bool bIsInteractableForPlacement = false;
	bool bIsDamageInflictingObject = false;
	bool bUsingTelekenisis = false;
	ADamageInflictingObject* storeDamageInflictingObject;
	AInteractableForPlacement* storeInteractableForPlacement;

	int32 currentWeaponIndex;
	
	//Methods






};
