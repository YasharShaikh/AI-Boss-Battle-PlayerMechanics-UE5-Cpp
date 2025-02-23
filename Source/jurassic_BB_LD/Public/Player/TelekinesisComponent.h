// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TelekinesisComponent.generated.h"


class UCurveFloat;
class APlayerCharacter;
class ATelekineticObject;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JURASSIC_BB_LD_API UTelekinesisComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTelekinesisComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)	void StartTelekinesis();
	UFUNCTION(BlueprintCallable)	void HoldTelekineticObject();
	UFUNCTION(BlueprintCallable)	void ReleaseTelekineticObject();

	/** Stamina Handling */
	void UpdateStamina(float DeltaTime);
	void ConsumeStamina(float Amount);
	void RegenerateStamina(float DeltaTime);
	bool CanUseTelekinesis() const;

private:
	/** Player Reference */
	APlayerCharacter* PlayerCharacter;

	/** Held Object */
	UPROPERTY()
	ATelekineticObject* HeldObject;

	/** Stamina Properties */
	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float StaminaRegenRate = 15.0f;

	/** Detection & Safe Distance */
	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float DetectionRadius = 1000.0f;

	/** Safe Distance for Object Pull */
	float SafePullDistance;

	/** Holding State */
	bool bIsHoldingObject;

};
