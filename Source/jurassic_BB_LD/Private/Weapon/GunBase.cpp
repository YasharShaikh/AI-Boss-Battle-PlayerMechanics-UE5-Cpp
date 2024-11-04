// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h"




// Sets default values
AGunBase::AGunBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	GunMeshComponent->SetupAttachment(RootComponent);


	UE_LOG(LogTemp, Warning, TEXT("from init Current ammo in gun: %d"), AmmoInClip);
	UE_LOG(LogTemp, Warning, TEXT("from init Current ClipSize in gun: %d"), ClipSize);
	UE_LOG(LogTemp, Warning, TEXT("from init Current MaxAmmo in gun: %d"), MaxAmmo);
}

// Called when the game starts or when spawned
void AGunBase::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateofFIre;

}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunBase::StartFire()
{
	if (AmmoInClip > 0)
	{
		if (WeaponType == EWeaponTypee::WT_PISTOL)
		{
			if (FireType == EFireWeaponTypee::FWT_AUTO)
			{
				UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: Pistol:FWT_AUTO"));
				float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

				GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AGunBase::Fire, TimeBetweenShots, true, FirstDelay);

			}
			else if (FireType == EFireWeaponTypee::FWT_SINGLE)
			{
				UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: Pistol:FWT_SINGLE"));
				Fire();

			}
			else if (FireType == EFireWeaponTypee::FWT_BURST)
			{
				UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: Pistol:FWT_BURST"));
			}
		}
		else if (WeaponType == EWeaponTypee::WT_RIFLE)
		{
			if (FireType == EFireWeaponTypee::FWT_AUTO)
			{
				float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

				GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AGunBase::Fire, TimeBetweenShots, true, FirstDelay);
			}
			else if (FireType == EFireWeaponTypee::FWT_SINGLE)
			{
				Fire();
			}
			else if (FireType == EFireWeaponTypee::FWT_BURST)
			{

				UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: WT_RIFLE:FWT_BURST"));
			}
		}

	}
}


void AGunBase::Fire()
{

	if (AmmoInClip > 0)
	{
		AmmoInClip -= 1;
		UE_LOG(LogTemp, Warning, TEXT("from fire Current ammo in gun: %d"), AmmoInClip);
		UE_LOG(LogTemp, Warning, TEXT("from fire Current MaxAmmo in gun: %d"), MaxAmmo);


		UGameplayStatics::SpawnEmitterAttached(muzzleFlash, GunMeshComponent, TEXT("muzzleFlash"));

		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn == nullptr)return;
		AController* OwnerController = OwnerPawn->GetController();
		//OwnerPawn->GetName();
		UE_LOG(LogTemp, Warning, TEXT("from fire Current MaxAmmo in gun: %s"), *OwnerPawn->GetName());
		if (OwnerController == nullptr) return;


		FVector location;
		FRotator rotation;
		OwnerController->GetPlayerViewPoint(location, rotation);

		FVector end = location + rotation.Vector() * MaxRange;




		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.AddIgnoredActor(OwnerPawn);
		if (GetWorld()->LineTraceSingleByChannel(hit, location, end, ECC_Visibility, params))
		{
			FVector shotDirection = -rotation.Vector();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactEffect, hit.Location, shotDirection.Rotation());

			AActor* hitActor = hit.GetActor();
			if (hitActor != nullptr)
			{
				FPointDamageEvent DamageEvent(BaseDamage, hit, shotDirection, nullptr);

				// Log details of the DamageEvent
				UE_LOG(LogTemp, Warning, TEXT("FPointDamageEvent Created: BaseDamage = %f, Hit Location = %s, Shot Direction = %s"),
					DamageEvent.Damage,
					*DamageEvent.HitInfo.ImpactPoint.ToString(),
					*DamageEvent.ShotDirection.ToString()
				);

				// Simulate applying damage to the actor (without actually affecting the enemy class)
				float AppliedDamage = hitActor->TakeDamage(BaseDamage, DamageEvent, OwnerController, this);

				// Log the result after applying the damage
				UE_LOG(LogTemp, Warning, TEXT("Applied Damage: %f to Actor: %s"), AppliedDamage, *hitActor->GetName());
			}
		}
		

		else
		{
			StopFire();
		}
		DrawDebugLine(GetWorld(), location, end, FColor::Blue, false, 1.0f, 0, 0.5f);
	}
}



void AGunBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AGunBase::Reload()
{
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
			MaxAmmo = 0; // No more ammo left in the total ammo pool
		}
	}
}
