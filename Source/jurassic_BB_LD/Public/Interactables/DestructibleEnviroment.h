// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleEnviroment.generated.h"

class UStaticMeshComponent;
class ATelekineticObject;

UCLASS()
class JURASSIC_BB_LD_API ADestructibleEnviroment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleEnviroment();

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



    UFUNCTION(BlueprintCallable) void TakeDamage(float damageAmount);
	UFUNCTION(BlueprintCallable) void Destruct();
    
	UPROPERTY(BlueprintReadWrite, Category = "Destructible Enviroment") float MaxHealth;
	
	UPROPERTY(BlueprintReadWrite, Category = "Destructible Enviroment") float CurrentHealth;
	

private:
	UPROPERTY(VisibleAnywhere, Category = "Telekinesis") UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Destructible", meta = (MustImplement = "PoolableInterface"))
	TSubclassOf<AActor> TelekineticObjectClass;

};
