// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TripAnchor.generated.h"

class UStaticMeshComponent;
class UTripWireSpline;
UCLASS()
class JURASSIC_BB_LD_API ATripAnchor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATripAnchor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trip Anchor Mesh") UStaticMeshComponent* TripAnchorMesh;


};
