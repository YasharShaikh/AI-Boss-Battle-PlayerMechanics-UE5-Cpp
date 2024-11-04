// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableForPlacement.h"

// Sets default values
AInteractableForPlacement::AInteractableForPlacement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableForPlacement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableForPlacement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableForPlacement::Interact_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Placement object detected"));

}
