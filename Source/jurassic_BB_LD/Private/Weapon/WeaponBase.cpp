// Fill out your copyright notice in the Description page of Project Settings.





#include "Weapon/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/DamageEvents.h"


// Sets default values
AWeaponBase::AWeaponBase()
{

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	GunMeshComponent->SetupAttachment(RootComponent);

	FireSound = CreateDefaultSubobject<USoundBase>(TEXT("Fire sfx"));


	isWatchingMe = false;

	currentState = EPickState::EPS_NONE;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();


	TimeBetweenShots = 60 / RateofFIre;

}

void AWeaponBase::HandleNoneState()
{
}

void AWeaponBase::HandleHighlightState()
{
	Interact_Implementation();
}

void AWeaponBase::HandlePicktState()
{
	isWatchingMe = false;


}

void AWeaponBase::HandleDroptState()
{
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EnterState(currentState);
	/*if (!isWatchingMe)
	{
		currentState = EPickState::EPS_NONE;
		EnterState(currentState);
	}*/
}


void AWeaponBase::Interact_Implementation()
{
	isWatchingMe = true;
}


void AWeaponBase::Fire()
{

	if (AmmoInClip > 0)
	{
		AmmoInClip -= 1;

		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		/*UE_LOG(LogTemp, Warning, TEXT("Current ammo in gun: %d"), AmmoInClip);
		UE_LOG(LogTemp, Warning, TEXT("Current MaxAmmo in gun: %d"), MaxAmmo);*/
		UGameplayStatics::SpawnEmitterAttached(muzzleFlash, GunMeshComponent, TEXT("muzzleFlash"));



		//Get playerhcaratcer basically
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn == nullptr)return;
		AController* OwnerController = OwnerPawn->GetController();
		if (OwnerController == nullptr) return;


		FVector startLocation;
		FRotator rotation;
		OwnerController->GetPlayerViewPoint(startLocation, rotation);

		FVector end = startLocation + rotation.Vector() * MaxRange;




		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.AddIgnoredActor(OwnerPawn);
		if (GetWorld()->LineTraceSingleByChannel(hit, startLocation, end, ECC_Visibility, params))
		{
			FVector shotDirection = -rotation.Vector();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactEffect, hit.Location, shotDirection.Rotation());

			AActor* hitActor = hit.GetActor();
			if (hitActor != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("from raycast  ammo in gun: %s"), *hitActor->GetName());

				FPointDamageEvent DamageEvent(BaseDamage, hit, shotDirection, nullptr);
				hitActor->TakeDamage(BaseDamage, DamageEvent, OwnerController, this);
			}
		}

		else
		{
			StopFire();
		}
		DrawDebugLine(GetWorld(), startLocation, end, FColor::Blue, false, 1.0f, 0, 0.5f);
	}
	else
	{
		StopFire();
	}

}

void AWeaponBase::StartFire()
{


	if (WeaponType == EWeaponTypo::WT_PISTOL)
	{
		if (FireType == EFireModo::FM_AUTO)
		{
			float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponBase::Fire, TimeBetweenShots, true, FirstDelay);
		}
		else if (FireType == EFireModo::FM_SINGLE)
		{
			Fire();
		}
		else if (FireType == EFireModo::FM_BURST)
		{
			UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: Pistol:FWT_BURST"));
		}
	}
	else if (WeaponType == EWeaponTypo::WT_RIFLE)
	{
		if (FireType == EFireModo::FM_AUTO)
		{
			float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponBase::Fire, TimeBetweenShots, true, FirstDelay);
		}
		else if (FireType == EFireModo::FM_SINGLE)
		{
			Fire();
		}
		else if (FireType == EFireModo::FM_BURST)
		{

			UE_LOG(LogTemp, Warning, TEXT("Your current weapon is of type: WT_RIFLE:FWT_BURST"));
		}
	}

	//}
}


void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AWeaponBase::Reload()
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



void AWeaponBase::Visible(bool showMesh)
{
	GunMeshComponent->SetVisibility(showMesh);
}


void AWeaponBase::EnterState(EPickState nextState)
{

	switch (nextState)
	{
	case EPickState::EPS_NONE:
		HandleNoneState();
		break;
	case EPickState::EPS_HIGHLIGHT:
		HandleHighlightState();
		break;
	case EPickState::EPS_PICK:
		HandlePicktState();
		break;
	case EPickState::EPS_DROP:
		HandleDroptState();
		break;
	default:
		break;
	}
}

