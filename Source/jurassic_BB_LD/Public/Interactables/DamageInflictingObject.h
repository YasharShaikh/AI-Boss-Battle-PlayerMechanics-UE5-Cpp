// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "DamageInflictingObject.generated.h"


class UPrimitiveComponent;
class UStaticMeshComponent;
class APlayerCharacter;



UCLASS()

class JURASSIC_BB_LD_API ADamageInflictingObject : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageInflictingObject();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "EXO Telekinesis System") float LiftHeight;
	UPROPERTY(EditAnywhere, Category = "EXO Telekinesis System") float LiftSpeed;
	UPROPERTY(EditAnywhere, Category = "EXO Telekinesis System") float rotateSpeed;
	UPROPERTY(EditAnywhere, Category = "EXO Telekinesis System") float pullSpeed;
	UPROPERTY(EditAnywhere, Category = "EXO Telekinesis System") float pushSpeed;

	APlayerCharacter* playerCharacter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	//Methods
	virtual void Interact_Implementation() override;


	UFUNCTION(BlueprintCallable, Category = "Handle Object state")
	void EnterState(EObjectStates nextState);
	//void ExitState(EObjectState currentState);
	void HandleNONEState();
	void HandleHIGHLIGHTState();
	void HandlePULLState();
	void HandleHOLDState();
	void HandlePUSHState();




	//Variables
	bool isWatchingMe;

	UPROPERTY(BlueprintReadWrite, Category = "Object state") EObjectStates currentState;
	UPROPERTY(BlueprintReadWrite) bool ObjectLifted = false;


private:


	bool isLifting, isPulling;
	FVector holdPosition;
	FVector endLocation;
	FVector liftStartLocation, liftEndLocation;
	//UPrimitiveComponent* primitiveComponent;
	void InitPull();
	void Pull();

};
