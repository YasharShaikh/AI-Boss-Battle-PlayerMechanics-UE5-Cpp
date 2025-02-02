// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAI/HuntStateComponent.h"
#include "EnemyAI/TrexAIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

void UHuntStateComponent::EnterState(ATrexAIController* AIController)
{
	UE_LOG(LogTemp, Warning, TEXT("Entering Hunt State"));

	// Play roar montage
	if (AIController)
	{
		ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
		if (ControlledCharacter)
		{
			PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("inplace_roar_front"));
		}
	}

}

void UHuntStateComponent::UpdateState(ATrexAIController* AIController, float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("updating Hunt State"));

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);

	if (PlayerCharacter && AIController && ControlledCharacter)
	{
		FVector AIForwardVector = AIController->GetPawn()->GetActorForwardVector();
		FVector ToPlayerVector = PlayerCharacter->GetActorLocation() - AIController->GetPawn()->GetActorLocation();

		FVector AILocation = AIController->GetPawn()->GetActorLocation();
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();

		float DistanceToPlayer = FVector::Dist(AILocation, PlayerLocation);

		// Normalize the ToPlayerVector
		ToPlayerVector.Normalize();

		// Calculate the dot products for forward and right vectors
		float ForwardDotProduct = FVector::DotProduct(AIForwardVector, ToPlayerVector);
		FVector AIRightVector = AIController->GetPawn()->GetActorRightVector();
		float RightDotProduct = FVector::DotProduct(AIRightVector, ToPlayerVector);

		// Angle threshold for "in front" condition
		float CosineThreshold = FMath::Cos(FMath::DegreesToRadians(AngleThresholdDegrees));

		if (DistanceToPlayer <= FollowPlayerThreshold)
		{
			FVector playerVelocity = PlayerCharacter->GetVelocity();
			bool bIsPlayerCharacterMoving = playerVelocity.Size() > 0.1f;


			if (AIController->GetMoveStatus() != EPathFollowingStatus::Moving )
			{
				AIController->MoveToActor(PlayerCharacter, 5.0f);
			}

			// If the player is close enough for attack
			if (DistanceToPlayer <= AttackPlayerThreshold)
			{/*
				UE_LOG(LogTemp, Warning, TEXT("Now chopping down player"));*/


				if (bIsPlayerCharacterMoving)
				{
					PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("inplace_bite_front"));


					//if (ForwardDotProduct >= CosineThreshold) // Player is directly front of AI
					//{
					//	AIController->StopMovement();
					//	PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("walk_bite_front"));
					//}
					//else if (ForwardDotProduct > 0) //Player is in front but outside the angle threshold.
					//{
					//	UE_LOG(LogTemp, Warning, TEXT("Player is in front but outside the angle threshold."));
					//}
					//else
					//{
					//	UE_LOG(LogTemp, Warning, TEXT("Player is behind the AI."));
					//}

					//// Left or right check based on RightDotProduct
					//if (RightDotProduct > 0) //Player is on the right of the AI.
					//{
					//	AIController->StopMovement();
					//	PlayMontageOnce(ControlledCharacter, BiteRSMontage, FName("walk_bite_right"));
					//}
					//else if (RightDotProduct < 0)// Player is on the left of the AI.
					//{
					//	AIController->StopMovement();
					//	PlayMontageOnce(ControlledCharacter, BiteLSMontage, FName("walk_bite_left"));
					//}


				}
				else if(!bIsPlayerCharacterMoving)
				{
					AIController->StopMovement(); // Stop moving before attacking

					PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("inplace_bite_front"));

					//if (ForwardDotProduct >= CosineThreshold) // Player is directly front of AI
					//{
					//	PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("inplace_bite_front"));
					//}
					//else if (ForwardDotProduct > 0) //Player is in front but outside the angle threshold.
					//{
					//	UE_LOG(LogTemp, Warning, TEXT("Player is in front but outside the angle threshold."));
					//}
					//else
					//{
					//	UE_LOG(LogTemp, Warning, TEXT("Player is behind the AI."));
					//}

					//// Left or right check based on RightDotProduct
					//if (RightDotProduct > 0) //Player is on the right of the AI.
					//{
					//	PlayMontageOnce(ControlledCharacter, BiteRSMontage, FName("inplace_bite_right"));
					//}
					//else if (RightDotProduct < 0)// Player is on the left of the AI.
					//{
					//	PlayMontageOnce(ControlledCharacter, BiteLSMontage, FName("inplace_bite_left"));
					//}

				}
			}
			
		}
		//if (DistanceToPlayer <= CloseThreshold)
		//{
		    //if (ForwardDotProduct >= CosineThreshold)
		    //{
		    //    UE_LOG(LogTemp, Warning, TEXT("Player is directly in front of the AI."));
		    //    PlayMontageOnce(ControlledCharacter, BiteFWDMontage, FName("inplace_roar_front"));
		    //}
		    //else if (ForwardDotProduct > 0)
		    //{
		    //    UE_LOG(LogTemp, Warning, TEXT("Player is in front but outside the angle threshold."));
		    //}
		    //else
		    //{
		    //    UE_LOG(LogTemp, Warning, TEXT("Player is behind the AI."));
		    //}

		    //// Left or right check based on RightDotProduct
		    //if (RightDotProduct > 0)
		    //{
		    //    UE_LOG(LogTemp, Warning, TEXT("Player is on the right of the AI."));
		    //    PlayMontageOnce(ControlledCharacter, BiteRSMontage, FName("inplace_roar_right"));
		    //}
		    //else if (RightDotProduct < 0)
		    //{
		    //    UE_LOG(LogTemp, Warning, TEXT("Player is on the left of the AI."));
		    //    PlayMontageOnce(ControlledCharacter, BiteLSMontage, FName("inplace_roar_left"));
		    //}
		//}
		//else
		//{
		//   
		//}
	}
}

void UHuntStateComponent::ExitState(ATrexAIController* AIController)
{
	UE_LOG(LogTemp, Warning, TEXT("Exiting Hunt State"));
}

void UHuntStateComponent::PlayMontageOnce(ACharacter* ControlCharacter, UAnimMontage* MontageToPlay, FName SectionName)
{
	if (ControlCharacter)
	{
		characterAnimIns = ControlCharacter->GetMesh()->GetAnimInstance();
		if (MontageToPlay == NULL)
		{

			UE_LOG(LogTemp, Warning, TEXT("Issue with Montage to play"));
		}
		if (characterAnimIns && MontageToPlay && !characterAnimIns->Montage_IsPlaying(MontageToPlay))
		{
			UE_LOG(LogTemp, Warning, TEXT("Playing Montage: %s"), *MontageToPlay->GetName());
			characterAnimIns->Montage_Play(MontageToPlay);
			characterAnimIns->Montage_JumpToSection(SectionName, MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Montage is already playing or AnimInstance/MontageToPlay is NULL"));
		}
	}
}
