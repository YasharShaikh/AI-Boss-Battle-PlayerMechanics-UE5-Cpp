// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAI/AIStateComponent.h"
#include "StunStateComponent.generated.h"

/**
 * 
 */
class ATrexAICharacter;
class UAnimMontage;
UCLASS()
class JURASSIC_BB_LD_API UStunStateComponent : public UAIStateComponent
{
	GENERATED_BODY()


public:
	void EnterState(ATrexAIController* AIController) override;
	void ExitState(ATrexAIController* AIController) override;
	void UpdateState(ATrexAIController* AIController, float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")	 UAnimMontage* StunMontage;
private:
	UPROPERTY()ATrexAICharacter* trexAICharacter;


	ACharacter* ControlledCharacter;
	UAnimInstance* characterAnimIns;

	void PlayMontageOnce(ACharacter* ControlledCharacter, UAnimMontage* MontageToPlay, FName SectionName);
};
