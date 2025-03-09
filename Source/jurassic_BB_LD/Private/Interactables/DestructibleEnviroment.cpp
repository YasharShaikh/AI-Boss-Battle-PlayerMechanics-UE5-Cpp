// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/DestructibleEnviroment.h"
#include "Interactables/TelekineticObject.h"
#include "Components/StaticMeshComponent.h"
#include "Interfaces/PoolSubSystem.h"
#include "Interactables/TelekineticObject.h"

// Sets default values
ADestructibleEnviroment::ADestructibleEnviroment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MaxHealth = 100.0f;
	MaxHealth = CurrentHealth;
}

// Called when the game starts or when spawned
void ADestructibleEnviroment::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ADestructibleEnviroment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleEnviroment::TakeDamage(float damageAmount)
{
	CurrentHealth -= damageAmount;
}

void ADestructibleEnviroment::Destruct()
{
    if (TelekineticObjectClass)
    {
        if (UPoolSubSystem* PoolSubSystem = GetWorld()->GetSubsystem<UPoolSubSystem>())
        {
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = GetActorRotation();

            AActor* SpawnedActor = nullptr;
            PoolSubSystem->SpawnFromPool(
                TSubclassOf<AActor>(TelekineticObjectClass), // Explicit cast
                SpawnLocation,
                SpawnRotation,
                SpawnedActor
            );
        }
    }
}