// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "TripWireSpline.generated.h"

class USplineComponent;
class USplineMeshComponent;
UCLASS()
class JURASSIC_BB_LD_API ATripWireSpline : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATripWireSpline();
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable) void DrawSplineBetweenActor(AActor* actor1, AActor* actor2);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Trip Wire Spline") UStaticMesh* drawableMesh;



private:

	UPROPERTY(EditAnywhere, Category = "Trip Wire Spline")USplineComponent* TripWireSpline;
	UPROPERTY(EditAnywhere, Category = "Trip Wire Spline")	USplineMeshComponent* TripWireSplineMesh;


};
