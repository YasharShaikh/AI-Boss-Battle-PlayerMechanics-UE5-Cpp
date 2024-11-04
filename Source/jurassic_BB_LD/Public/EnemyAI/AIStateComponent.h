// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "AIStateComponent.generated.h"


class ATrexAIController;


UCLASS(Blueprintable)
class JURASSIC_BB_LD_API UAIStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIStateComponent();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName StateDisplayName;

	virtual void EnterState(ATrexAIController* AIController) {}
	virtual void ExitState(ATrexAIController* AIController) {}
	virtual  void UpdateState(ATrexAIController* AIController, float DeltaTime) {}

	virtual void OnPawnDetected(ATrexAIController* AIController, AActor* DetectedPawn) {}

};
