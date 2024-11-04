// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TripAnchor.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ATripAnchor::ATripAnchor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TripAnchorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trip Anchor Mesh Component"));
	TripAnchorMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATripAnchor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATripAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

