// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/AWeaponMagazine.h"
#include "Weapon/WeaponPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AAWeaponMagazine::AAWeaponMagazine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MagazineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Magazine Mesh"));
	SetRootComponent(MagazineMesh);
}

// Called when the game starts or when spawned
void AAWeaponMagazine::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAWeaponMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAWeaponMagazine::AttachToWeapon(AWeaponPickup* Weapon)
{
	if (Weapon)
	{
		AttachToComponent(Weapon->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Magzine_Socket");

		SetActorHiddenInGame(false);
		SetActorEnableCollision(false);
		MagazineMesh->SetSimulatePhysics(false);
	}
}

void AAWeaponMagazine::DetachFromWeapon()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	SetActorEnableCollision(true);
	MagazineMesh->SetSimulatePhysics(true);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAWeaponMagazine::ResetMagazine, 3.0f, false);
}

void AAWeaponMagazine::DropMagazine()
{
	DetachFromWeapon();
	SetLifeSpan(5.0f); // Destroy after 5 seconds if not using pooling
}

void AAWeaponMagazine::AttachMagazine(AWeaponPickup* Weapon)
{
	if (Weapon)
	{
		AttachToWeapon(Weapon);
	}
}

void AAWeaponMagazine::ResetMagazine()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

