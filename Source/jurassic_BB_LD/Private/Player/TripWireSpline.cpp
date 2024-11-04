// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TripWireSpline.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"

// Sets default values
ATripWireSpline::ATripWireSpline()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TripWireSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TripWireSpline"));
	SetRootComponent(TripWireSpline);

	TripWireSplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("TripWireSplineMesh"));
	TripWireSplineMesh->SetupAttachment(TripWireSpline);

}

// Called when the game starts or when spawned
void ATripWireSpline::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ATripWireSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATripWireSpline::DrawSplineBetweenActor(AActor* actor1, AActor* actor2)
{
    if (!drawableMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Drawable mesh not found"));
        return;
    }

    // Ensure both actors are valid
    if (actor1 && actor2)
    {
        // Get start and end positions
        FVector startPosition = actor1->GetActorLocation();
        FVector endPosition = actor2->GetActorLocation();

        // Clear any existing spline points and add new ones
        TripWireSpline->ClearSplinePoints();
        TripWireSpline->AddSplinePoint(startPosition, ESplineCoordinateSpace::World);
        TripWireSpline->AddSplinePoint(endPosition, ESplineCoordinateSpace::World);

        // Now that the spline points are set, we can set up the mesh
        TripWireSplineMesh->SetStaticMesh(drawableMesh);
        TripWireSplineMesh->SetMobility(EComponentMobility::Movable);
        TripWireSplineMesh->RegisterComponentWithWorld(GetWorld());

        // Attach the mesh to the spline component
        TripWireSplineMesh->AttachToComponent(TripWireSpline, FAttachmentTransformRules::KeepRelativeTransform);

        // Get the tangents at the spline points
        FVector startTangent = TripWireSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
        FVector endTangent = TripWireSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

        // Set the mesh's start and end points using the spline's position and tangent data
        TripWireSplineMesh->SetStartAndEnd(startPosition, startTangent, endPosition, endTangent);



        UE_LOG(LogTemp, Warning, TEXT("Spline drawn between actors successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("One or both actors are not valid."));
    }
}
