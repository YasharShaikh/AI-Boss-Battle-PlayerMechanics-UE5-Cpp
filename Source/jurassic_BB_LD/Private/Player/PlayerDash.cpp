// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerDash.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UPlayerDash::UPlayerDash()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerDash::BeginPlay()
{
	Super::BeginPlay();

	// ...

	FOnTimelineFloat progressUpdate;
	progressUpdate.BindUFunction(this, " ");

	FOnTimelineEvent finishedEvent;
	finishedEvent.BindUFunction(this, " ");


	DashTimeline.AddInterpFloat(DashCurve, progressUpdate);
	DashTimeline.SetTimelineFinishedFunc(finishedEvent);

}


// Called every frame
void UPlayerDash::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerDash::MoveOverTime(FVector DashDirection, FVector CurrentLocation)
{

	const FVector currentLocation = CurrentLocation;
	const FVector dashDirection = DashDirection;



}

