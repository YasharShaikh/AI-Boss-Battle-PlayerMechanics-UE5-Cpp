// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/WeaponAttachmentInterface.h"
#include "AWeaponMagazine.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

UCLASS()
class JURASSIC_BB_LD_API AAWeaponMagazine : public AActor, public IWeaponAttachmentInterface
{
		GENERATED_BODY()
	
public:	
	AAWeaponMagazine();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void AttachToWeapon(AWeaponPickup* Weapon) override;
	virtual void DetachFromWeapon() override;
	virtual EAttachmentType GetAttachmentType() const override { return EAttachmentType::AT_Magazine; }

	void DropMagazine();

	void AttachMagazine(AWeaponPickup* Weapon);

	void ResetMagazine();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magazine")
	USkeletalMeshComponent* MagazineMesh;

};
