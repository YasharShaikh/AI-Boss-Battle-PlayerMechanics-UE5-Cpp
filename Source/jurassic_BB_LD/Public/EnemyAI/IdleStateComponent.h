// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAI/AIStateComponent.h"
#include "IdleStateComponent.generated.h"

/**
 *
 */

UCLASS(Blueprintable)
class JURASSIC_BB_LD_API UIdleStateComponent : public UAIStateComponent
{
	GENERATED_BODY()

public:
	void EnterState(ATrexAIController* AIController) override;
	void ExitState(ATrexAIController* AIController) override;
	void UpdateState(ATrexAIController* AIController, float DeltaTime) override;


	virtual void OnPawnDetected(ATrexAIController* AIController, AActor* DetectedPawn) override;
};
