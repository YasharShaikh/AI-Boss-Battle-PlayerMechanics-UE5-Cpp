// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TripWireComponent.h"
#include "Player/TripAnchor.h"
#include "Player/TripWireSpline.h"


// Sets default values for this component's properties
UTripWireComponent::UTripWireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTripWireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}



// Called every frame
void UTripWireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

	void UTripWireComponent::SetupTrap()
	{
		FVector actorLocation;
		if (AActor* owner = GetOwner())
		{
			actorLocation = owner->GetActorLocation();

			// Place the first anchor
			if (!bFirstAnchorPlaced && !bSecondAnchorPlaced)
			{
				bFirstAnchorPlaced = true;
				FVector firstAnchorLocation = actorLocation;
				FRotator firstAnchorRotation = FRotator::ZeroRotator;

				firstAnchor = GetWorld()->SpawnActor<ATripAnchor>(trapAnchorClass, firstAnchorLocation, firstAnchorRotation);

				UE_LOG(LogTemp, Warning, TEXT("First anchor placed"));
			}
			// Place the second anchor
			else if (bFirstAnchorPlaced && !bSecondAnchorPlaced)
			{
				bSecondAnchorPlaced = true;
				FVector secondAnchorLocation = actorLocation;
				FRotator secondAnchorRotation = FRotator::ZeroRotator;

				secondAnchor = GetWorld()->SpawnActor<ATripAnchor>(trapAnchorClass, secondAnchorLocation, secondAnchorRotation);

				UE_LOG(LogTemp, Warning, TEXT("Second anchor placed"));

				if (tripWireSplineClass)
				{
					// Spawn the TripWireSpline actor
					tripWireSplineActor = GetWorld()->SpawnActor<ATripWireSpline>(tripWireSplineClass);
					if (tripWireSplineActor)
					{
						// Use the DrawSplineBetweenActor method to connect the anchors
						tripWireSplineActor->DrawSplineBetweenActor(firstAnchor, secondAnchor);
						UE_LOG(LogTemp, Warning, TEXT("Tripwire placed between anchors."));

						GetWorld()->GetTimerManager().SetTimer(Timer, this, &UTripWireComponent::DestroyTrap, 10.0f, false);

					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to spawn tripwire spline actor."));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("TripWireSplineClass not set."));
				}
			}
		
		}
	}

	void UTripWireComponent::DestroyTrap()
	{

		if (firstAnchor != nullptr && secondAnchor != nullptr && tripWireSplineActor!= nullptr)
		{
			firstAnchor->Destroy();
			firstAnchor = nullptr;

			secondAnchor->Destroy();
			secondAnchor = nullptr;

			tripWireSplineActor->Destroy();
			tripWireSplineActor = nullptr;
			bFirstAnchorPlaced = bSecondAnchorPlaced = false;
			UE_LOG(LogTemp, Warning, TEXT("Trap reset"));
		}
	}

