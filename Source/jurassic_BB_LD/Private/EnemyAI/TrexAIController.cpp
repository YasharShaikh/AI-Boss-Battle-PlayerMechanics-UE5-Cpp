// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI/TrexAIController.h"

#include "EnemyAI/AIStateComponent.h"
#include "EnemyAI/IdleStateComponent.h"
#include "EnemyAI/AttackStateComponent.h"
#include "EnemyAI/DetectStateComponent.h"
#include "EnemyAI/HuntStateComponent.h"
#include "EnemyAI/RageStateComponent.h"
#include "EnemyAI/ExhaustStateComponent.h"
#include "EnemyAI/DefenseStateComponent.h"
#include "EnemyAI/StunStateComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h" 
#include "GameFramework/Character.h"
#include "Interfaces/StateData.h"
#include "EnemyAI/TrexAICharacter.h"
ATrexAIController::ATrexAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the AI Perception Component and Sight Config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Perception"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	// Check for component creation success
	// Configure Sight Config properties
	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AISightRadius; // Note: You might mean another variable here
	SightConfig->SetMaxAge(AISightAge);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Attach Sight Config to Perception Component
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ATrexAIController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);


	// Initialize variables
	VarInit();

}


void ATrexAIController::BeginPlay()
{
	Super::BeginPlay();
	trexAICharacter = Cast<ATrexAICharacter>(GetCharacter());
	InitializeStates();
	InitStateManager();
}

void ATrexAIController::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (bCanTickState)
	{
		currentStateComponent->UpdateState(this, DeltaTime);
	}
	if (bDebug)
	{
		if (currentStateComponent)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->Owner.GetName() + "'s current state: " + currentStateComponent->StateDisplayName.GetPlainNameString());
		}
	}

}



void ATrexAIController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	for (AActor* DetectedPawn : DetectedPawns)
	{
		if (currentStateComponent)
		{
			// Notify the current state of the detected pawn(s)
			currentStateComponent->OnPawnDetected(this, DetectedPawn);
		}
	}
}

FRotator ATrexAIController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void ATrexAIController::VarInit()
{
	MaxMenanceGuage = 100.0f;
	MaxRageGuage = 100.0f;
	MaxFatigueGuage = 100.0f;

	MenanceGuage = 0.0f;
	RageGuage = 0.0f;
	FatigueGuage = 0.0f;

	AISightRadius = 10000.0f;
	AISightAge = 5.0f;
	AILoseSightRadius = AISightRadius + 50.0f;
	AIFOV = 90.0f;


	bTrapTriggered = false;
}

void ATrexAIController::HandleTrapTriggered()
{

	UE_LOG(LogTemp, Warning, TEXT("Trap Triggered from Delgeate->TrexAIController"));
}



#pragma region Updated
void ATrexAIController::SwitchStateByKey(FString StateKey)
{

	UAIStateComponent* newState = StateMap.FindRef(StateKey);


	if (newState)
	{
		if (!currentStateComponent)
		{
			currentStateComponent = newState;
		}
		else
		{
			bCanTickState = false;
			currentStateComponent->ExitState(this);
			if (StateHistory.Num() < StateHistoryLength)
			{
				StateHistory.Push(currentStateComponent);
			}
			else
			{
				StateHistory.RemoveAt(0);
				StateHistory.Push(currentStateComponent);
			}
			currentStateComponent = newState;
		}
		if (currentStateComponent)
		{
			currentStateComponent->EnterState(this);
			bCanTickState = true;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed. " + "Invalid state!");
	}

}

void ATrexAIController::InitStateManager()
{
	SwitchStateByKey(InitialState);
}

void ATrexAIController::InitializeStates()
{
	for (auto it = AvailableStates.CreateConstIterator(); it; ++it)
	{
		if (it->Value)
		{
			// Create the state component
			UAIStateComponent* State = NewObject<UAIStateComponent>(this, it->Value);

			// Ensure the state component was successfully created
			if (State)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "'s initialized state: " + State->GetClass()->GetName());
				StateMap.Add(it->Key, State);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Failed to create state for key: " + it->Key);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Invalid state class for key: " + it->Key);
		}
	}
}

#pragma endregion




