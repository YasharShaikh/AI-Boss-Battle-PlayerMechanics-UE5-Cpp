// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TripWireComponent.generated.h"


class ATripAnchor;
class ATripWireSpline;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JURASSIC_BB_LD_API UTripWireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTripWireComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite) bool bFirstAnchorPlaced;
	UPROPERTY(BlueprintReadWrite) bool bSecondAnchorPlaced;

	UPROPERTY(EditAnywhere, Category = "Trap")	TSubclassOf<ATripAnchor> trapAnchorClass;

	ATripAnchor* firstAnchor;
	ATripAnchor* secondAnchor;


	UFUNCTION(BlueprintCallable)	void SetupTrap();
	UFUNCTION(BlueprintCallable)	void DestroyTrap();

	UPROPERTY(EditAnywhere, Category = "Trap") TSubclassOf<ATripWireSpline> tripWireSplineClass;

	ATripWireSpline* tripWireSplineActor;

private:
	FTimerHandle Timer;

};
