// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAI/AIStateComponent.h"
#include "HuntStateComponent.generated.h"

/**
 * 
 */
class UAnimMontage;

UCLASS()
class JURASSIC_BB_LD_API UHuntStateComponent : public UAIStateComponent
{
	GENERATED_BODY()
	
public:
	void EnterState(ATrexAIController* AIController) override;
	void ExitState(ATrexAIController* AIController) override;
	void UpdateState(ATrexAIController* AIController, float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float FollowPlayerThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AttackPlayerThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AngleThresholdDegrees;



protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")	 UAnimMontage* BiteFWDMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")	 UAnimMontage* BiteLSMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")	 UAnimMontage* BiteRSMontage;
	ACharacter*   ControlledCharacter;
	UAnimInstance* characterAnimIns;

private:

	void PlayMontageOnce(ACharacter* ControlledCharacter, UAnimMontage* MontageToPlay, FName SectionName);
};
