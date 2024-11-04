// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI/StunStateComponent.h"
#include "EnemyAI/TrexAIController.h"
#include "EnemyAI/TrexAICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

void UStunStateComponent::EnterState(ATrexAIController* AIController)
{
	if (AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trap Triggered: from Stun state Enter "));
	}
}

void UStunStateComponent::ExitState(ATrexAIController* AIController)
{
	UE_LOG(LogTemp, Warning, TEXT("Trap Triggered: from Stun state Exit "));
}

void UStunStateComponent::UpdateState(ATrexAIController* AIController, float DeltaTime)
{
	if (AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trap Triggered: from Stun state Update "));
		PlayMontageOnce(ControlledCharacter, StunMontage, FName("Trip_Stun"));
	}
}

void UStunStateComponent::PlayMontageOnce(ACharacter* ControlCharacter, UAnimMontage* MontageToPlay, FName SectionName)
{
	if (ControlCharacter)
	{
		characterAnimIns = ControlCharacter->GetMesh()->GetAnimInstance();
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
