// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

UCLASS()
class JURASSIC_BB_LD_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePickup();

	// Called every frame
	virtual void Tick(float DeltaTime) override;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* pickupStaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    USkeletalMeshComponent* pickupSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Highlight")
    UMaterialInstanceDynamic* HighlightMaterialInstance;

    // Original material instance for resetting highlight
    UPROPERTY()
    UMaterialInterface* OriginalMaterial;

    // Indicates if the pickup is highlighted
    bool bIsHighlighted;

    // Function to enable the highlight
    virtual void HighlightPickup();

    // Function to disable the highlight
    virtual void RemoveHighlight();

public:
    // Interactable tag
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
    FName PickupTag;

    // Called to interact with the pickup
    virtual void Interact();

    // Function to handle when the player looks at the pickup
    void OnFocused();

    // Function to handle when the player stops looking at the pickup
    void OnUnfocused();

    void SetPickupMesh(bool skeletalMesh);

    UMeshComponent* GetActiveMesh() const;
};
