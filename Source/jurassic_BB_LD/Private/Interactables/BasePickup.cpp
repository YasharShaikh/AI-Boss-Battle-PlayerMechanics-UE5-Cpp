// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/BasePickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// Initialize Static Mesh Component
	pickupStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Static Mesh"));
	pickupStaticMesh->SetupAttachment(RootComponent);

	// Initialize Skeletal Mesh Component
	pickupSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pickup Skeletal Mesh"));
	pickupSkeletalMesh->SetupAttachment(RootComponent);

	// Ensure both are initially hidden
	pickupStaticMesh->SetVisibility(false);
	pickupStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupSkeletalMesh->SetVisibility(false);
	pickupSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsHighlighted = false;
}

// Called when the game starts or when spawned
void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABasePickup::HighlightPickup()
{
	if (!bIsHighlighted)
	{
		UMeshComponent* MeshToHighlight = GetActiveMesh();
		if (MeshToHighlight && !HighlightMaterialInstance)
		{
			OriginalMaterial = MeshToHighlight->GetMaterial(0);
			HighlightMaterialInstance = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (HighlightMaterialInstance)
			{
				HighlightMaterialInstance->SetVectorParameterValue("EmissiveColor", FLinearColor::Yellow);
				MeshToHighlight->SetMaterial(0, HighlightMaterialInstance);
			}
		}
		bIsHighlighted = true;
	}
}

void ABasePickup::RemoveHighlight()
{
	if (bIsHighlighted)
	{
		UMeshComponent* MeshToReset = GetActiveMesh();
		if (MeshToReset && OriginalMaterial)
		{
			MeshToReset->SetMaterial(0, OriginalMaterial);
		}
		bIsHighlighted = false;
	}
}

void ABasePickup::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Picked up: %s"), *GetName());
	Destroy(); // Destroy the pickup after interaction
}

void ABasePickup::SetPickupMesh(bool bUseSkeletalMesh)
{
	if (bUseSkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Skeletal Mesh for Pickup"));
		if (pickupStaticMesh)
		{
			pickupStaticMesh->SetVisibility(false);
			pickupStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (pickupSkeletalMesh)
		{
			pickupSkeletalMesh->SetVisibility(true);
			pickupSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Static Mesh for Pickup"));
		if (pickupSkeletalMesh)
		{
			pickupSkeletalMesh->SetVisibility(false);
			pickupSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (pickupStaticMesh)
		{
			pickupStaticMesh->SetVisibility(true);
			pickupStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

UMeshComponent* ABasePickup::GetActiveMesh() const
{
	if (pickupStaticMesh && pickupStaticMesh->IsVisible())
	{
		return pickupStaticMesh;
	}
	if (pickupSkeletalMesh && pickupSkeletalMesh->IsVisible())
	{
		return pickupSkeletalMesh;
	}
	return nullptr;
}
