// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "PlayerDash.generated.h"


// Forward Declarations
class UCurveFloat;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JURASSIC_BB_LD_API UPlayerDash : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerDash();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Player Character | EXO Telekinesis System")
	UCurveFloat* DashCurve = nullptr;

private:

	FTimeline DashTimeline;


	void MoveOverTime(FVector DashDirection, FVector CurrentLocation);
		
};
