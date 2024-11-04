// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI/TrexAICharacter.h"
#include "EnemyAI/TrexAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "EnemyAI/TrexAICharacter.h"	


// Sets default values
ATrexAICharacter::ATrexAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ATrexAIController::StaticClass();

	TrapTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trap Trigger"));
	TrapTrigger->SetupAttachment(RootComponent);



	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

}

// Called when the game starts or when spawned
void ATrexAICharacter::BeginPlay()
{
	Super::BeginPlay();
	trexController = Cast<ATrexAIController>(GetController());
}


// Called every frame
void ATrexAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*if (bTriggerEntered)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trap triggered"));
	}*/
}

// Called to bind functionality to input
void ATrexAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATrexAICharacter::setTripTriggered(bool bEntered)
{

	bTriggerEntered = bEntered;
	if (bEntered)
	{
		UE_LOG(LogTemp, Warning, TEXT("From SetTrapTriggered TrexAICHaracter"));
		OnTrapTriggered.Broadcast();
	}

}

