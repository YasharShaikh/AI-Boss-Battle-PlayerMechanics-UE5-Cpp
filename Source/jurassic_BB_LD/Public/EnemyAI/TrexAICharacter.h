// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TrexAICharacter.generated.h"
class ATrexAIController;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrapTriggeredDelegate);

UCLASS()

class JURASSIC_BB_LD_API ATrexAICharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATrexAICharacter();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Trap") void setTripTriggered(bool bEntered);
#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnTrapTriggeredDelegate OnTrapTriggered;

#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trex Stats")bool bTriggerEntered;
#pragma endregion





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere) ATrexAIController* aTrexAIController;

private:

	ATrexAIController* trexController;

	UPROPERTY(EditAnywhere, Category = "Trip Wire Spline")UBoxComponent* TrapTrigger;

	


};
