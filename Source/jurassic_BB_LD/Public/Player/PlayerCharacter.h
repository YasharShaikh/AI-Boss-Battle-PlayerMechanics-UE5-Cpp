//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

// Forward Declarations
class UAnimMontage;
class UCameraComponent;
class UCharacterMovementComponent;
class ADamageInflictingObject;
class AInteractableForPlacement;
class UStaticMeshComponent;
class USceneComponent;
class UCurveFloat;
class USplineComponent;
class ATrapTripWire;
class UTripWireComponent;
class UBoxComponent;
class USkeletalMeshComponent;
class AWeaponPickup;
class UWeaponSystemComponent;
class UTelekinesisComponent;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)		bool bInteractPressed = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)		UCameraComponent* PlayerViewCamera;
	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;*/
#pragma endregion



#pragma region Movement System

protected:

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

	//void Detector();


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
	UPROPERTY(VisibleAnywhere, Category = "Player Character | EXO Telekinesis System")	bool bUsingTelekinesis;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")		float  maxPickRange;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")		float pullForce;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")		float pushForce;
	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")		UCurveFloat* DashCurve = nullptr;

	UFUNCTION(BlueprintCallable)	void Telekinesis_Pull();
	UFUNCTION(BlueprintCallable)	void Telekinesis_Push();

	void ResetExoCharge();
	void DetectTelekineticObject();
#pragma endregion

#pragma region Firing System

protected:


public:
	UFUNCTION(BlueprintCallable)	void AimDownSight();
	UFUNCTION(BlueprintCallable)	void StopAimDownSights();
	AActor* HitDetection();
	void PlayMontage(UAnimMontage* Montage, FName SectionName);



private:



#pragma endregion


#pragma region Traps

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Trap")	UTripWireComponent* tripwireComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UWeaponSystemComponent* WeaponSystemComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UTelekinesisComponent* TelekinesisComponent;


	UFUNCTION()
	void StartTelekinesis();

	UFUNCTION()
	void ReleaseTelekineticObject();
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
