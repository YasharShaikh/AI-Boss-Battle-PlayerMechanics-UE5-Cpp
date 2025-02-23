// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponPickup.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapon/AWeaponMagazine.h"

AWeaponPickup::AWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	FireSound = CreateDefaultSubobject<USoundBase>(TEXT("Fire SFX"));
	LastFireTime = -1.0f;
}


void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	SetPickupMesh(true);
	InitializeMagazinePool();
	if (RateofFIre > 0)
	{
		TimeBetweenShots = 60 / RateofFIre;
	}
	else
	{
		TimeBetweenShots = 0.1f; // Default value to prevent division by zero
	}
}
void AWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeaponPickup::Fire()
{
	if (!bIsFiring) // Stop firing if the button is no longer held
	{
		StopFire();
		return;
	}

	if (AmmoInClip <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo left! Stopping fire."));
		StopFire();
		return;
	}
	PlayFireSound();
	PlayFireEffect();
	AmmoInClip--;
	UE_LOG(LogTemp, Warning, TEXT("Ammo remaining: %d"), AmmoInClip);

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (!ownerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null!"));
		StopFire();
		return;
	}

	AController* ownerController = ownerPawn->GetController();
	FVector startLocation;
	FRotator rotation;
	ownerController->GetPlayerViewPoint(startLocation, rotation);

	FVector end = startLocation + rotation.Vector() * MaxRange;

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(ownerPawn);

	if (GetWorld()->LineTraceSingleByChannel(hit, startLocation, end, ECC_Visibility, params))
	{
		FVector shotDirection = -rotation.Vector();
		AActor* hitActor = hit.GetActor();
		if (hitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *hitActor->GetName());
			FPointDamageEvent DamageEvent(BaseDamage, hit, shotDirection, nullptr);
			hitActor->TakeDamage(BaseDamage, DamageEvent, ownerController, this);
		}
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(GetWorld(), startLocation, end, FColor::Blue, false, 1.0f, 0, 0.5f);
#endif
}

void AWeaponPickup::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("StartFire() called"));

	if (AmmoInClip <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo to fire!"));
		StopFire();
		return;
	}

	bIsFiring = true; // Indicate that the player is holding the fire button

	switch (fireMode)
	{
	case EFireMode::FM_AUTO:
		if (!GetWorldTimerManager().IsTimerActive(TimerHandle_TimeBetweenShots))
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting Auto Fire Mode"));
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponPickup::Fire, TimeBetweenShots, true);
		}
		break;

	case EFireMode::FM_SINGLE:
		Fire(); // Fire only once for single-fire mode
		break;

	case EFireMode::FM_BURST:
		UE_LOG(LogTemp, Warning, TEXT("Burst fire mode is not implemented yet!"));
		break;

	default:
		Fire();
		break;
	}
}


void AWeaponPickup::StopFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Stopping Fire"));
	bIsFiring = false; // Prevent continuous firing
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AWeaponPickup::Reload()
{
	if (MaxAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo available to reload!"));
		return;
	}
	AmmoDifference = ClipSize - AmmoInClip;

	if (AmmoInClip >= 0 && AmmoInClip < ClipSize)
	{
		if (MaxAmmo >= AmmoDifference)
		{
			AmmoInClip += AmmoDifference;
			MaxAmmo -= AmmoDifference;
		}
		else
		{
			AmmoInClip += MaxAmmo;
			MaxAmmo = 0;
		}
	}
}

void AWeaponPickup::DropMagazineNotify()
{
	if (CurrentMagazine)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropping Magazine..."));

		CurrentMagazine->DropMagazine();
		CurrentMagazine = nullptr;
	}

	// Call event dispatcher (so any bound function gets executed)
	OnDropMagazine.Broadcast();
}
void AWeaponPickup::AttachMagazineNotify()
{
	UE_LOG(LogTemp, Error, TEXT("Called from attachm magzine notify."));
	if (!MagazineClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MagazineClass is NULL! Assign a magazine class in the WeaponPickup properties."));
		return;
	}
	AAWeaponMagazine* NewMagazine = GetPooledMagazine();
	if (!NewMagazine)
	{
		UE_LOG(LogTemp, Error, TEXT("Magazine pool exhausted!"));
		return;
	}

	CurrentMagazine = NewMagazine;
	CurrentMagazine->AttachToWeapon(this);
	//// Try reusing a pooled magazine
	//for (TActorIterator<AWeaponMagazine> It(GetWorld()); It; ++It)
	//{
	//	if (It->IsHidden())
	//	{
	//		CurrentMagazine = *It;
	//		CurrentMagazine->SetActorHiddenInGame(false);
	//		CurrentMagazine->SetActorEnableCollision(false);
	//		break;
	//	}
	//}

	// Spawn a new magazine if needed
	if (!CurrentMagazine)
	{
		CurrentMagazine = GetWorld()->SpawnActor<AAWeaponMagazine>(MagazineClass);
	}

	if (CurrentMagazine)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attaching New Magazine..."));
		CurrentMagazine->AttachToWeapon(this);
	}

	// Call event dispatcher (so any bound function gets executed)
	OnAttachMagazine.Broadcast();
}


void AWeaponPickup::Visible(bool showMesh)
{
	if (pickupStaticMesh)
	{
		pickupStaticMesh->SetVisibility(showMesh);
	}
	if (pickupSkeletalMesh)
	{
		pickupSkeletalMesh->SetVisibility(showMesh);
	}
}

void AWeaponPickup::PlayFireEffect()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, pickupSkeletalMesh, MuzzleSocketName);
	}
}


void AWeaponPickup::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::SpawnSoundAttached(
			FireSound,
			pickupSkeletalMesh,
			MuzzleSocketName,
			FVector::ZeroVector,
			EAttachLocation::SnapToTarget,
			true
		);
	}
}
void AWeaponPickup::InitializeMagazinePool()
{
	if (!MagazineClass) return;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		AAWeaponMagazine* Magazine = GetWorld()->SpawnActor<AAWeaponMagazine>(MagazineClass);
		Magazine->SetActorHiddenInGame(true);
		Magazine->SetActorEnableCollision(false);
		MagazinePool.Add(Magazine);
	}
}

AAWeaponMagazine* AWeaponPickup::GetPooledMagazine()
{
	for (AAWeaponMagazine* Magazine : MagazinePool)
	{
		if (Magazine && Magazine->IsHidden())
		{
			Magazine->SetActorHiddenInGame(false);
			return Magazine;
		}
	}
	return nullptr; // Pool exhausted
}

void AWeaponPickup::Interact()
{
}
