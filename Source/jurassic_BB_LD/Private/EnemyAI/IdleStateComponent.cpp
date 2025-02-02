// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI/IdleStateComponent.h"
#include "EnemyAI/TrexAIController.h"

void UIdleStateComponent::EnterState(ATrexAIController* AIController)
{
    //UE_LOG(LogTemp, Warning, TEXT("Entering Idle State"));
     //Additional logic for entering Idle state
}

void UIdleStateComponent::UpdateState(ATrexAIController* AIController, float DeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT("inside IDLE State"));
}

void UIdleStateComponent::OnPawnDetected(ATrexAIController* AIController, AActor* DetectedPawn)
{

   
        if (DetectedPawn)
        {
            // Check if the detected pawn is a player character (or any actor type you want)
            if (ACharacter* PlayerCharacter = Cast<ACharacter>(DetectedPawn))
            {
                UE_LOG(LogTemp, Warning, TEXT("Detected Player in Idle State. Switching to Hunt State."));

                // Perform the state switch to HuntState
                AIController->SwitchStateByKey(TEXT("hunt"));
            }
        }

}

void UIdleStateComponent::ExitState(ATrexAIController* AIController) 
{
    UE_LOG(LogTemp, Warning, TEXT("Exiting Idle State"));
    // Additional logic for exiting Idle state
}
