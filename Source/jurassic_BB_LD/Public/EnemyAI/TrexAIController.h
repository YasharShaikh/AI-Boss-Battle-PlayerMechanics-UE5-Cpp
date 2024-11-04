// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TrexAIController.generated.h"

/**
 *
 */

class UAIStateComponent;
class UIdleStateComponent;
class UDetectStateComponent;
class UHuntStateComponent;
class UExhaustStateComponent;
class UStunStateComponent;
class ATrexAICharacter;

class UAISenseConfig_Sight;


UENUM(BlueprintType)
enum class EAIState : uint8
{
	EAIS_IDLE				UMETA(DisplayName = "IDLE"),				//when the AI doesnt know about player whereabouts
	EAIS_DETECTION			UMETA(DisplayName = "DETECTION"),			//when the AI knows the player is near but doesnt know location
	EAIS_HUNT				UMETA(DisplayName = "HUNT"),				//when the AI see's the player and chases it 
	EAIS_ATTACK				UMETA(DisplayName = "ATTACK"),				//when the player is in the attack range of AI
	EAIS_DEFENSE			UMETA(DisplayName = "DEFENSE"),				//when the AI has taken too much damage in short time 
	EAIS_RAGE				UMETA(DisplayName = "RAGE"),				//when the AI RAGE GUAGE reaches max 
	EAIS_EXHAUSTION			UMETA(DisplayName = "EXHAUSTION"),			//when the AI FATIGUE GUAGE reaches max 
	EAIS_SCRIPTED			UMETA(DisplayName = "SCRIPTED EVENT"),		//when the AI SCRIPTED EVENTs
};

UCLASS()
class JURASSIC_BB_LD_API ATrexAIController : public AAIController
{
	GENERATED_BODY()
public:
	ATrexAIController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") bool bTrapTriggered;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") EAIState currentState;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
#pragma region Variables


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float MenanceGuage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float MaxMenanceGuage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float RageGuage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float MaxRageGuage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float FatigueGuage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float MaxFatigueGuage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AISightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AISightAge; // how long the actor stays in the Sight perception
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AILoseSightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") float AIFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") UAISenseConfig_Sight* SightConfig;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") bool bInRage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats") bool bFoundPlayer;
#pragma endregion

#pragma region States
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")	UIdleStateComponent* idleState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")	UDetectStateComponent* detectState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")	UHuntStateComponent* huntState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")	UExhaustStateComponent* exhaustionState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")	UStunStateComponent* stunState;
#pragma endregion


#pragma region Methods

	UFUNCTION() void OnPawnDetected(const TArray<AActor*>& DetectedPawns);
	virtual FRotator GetControlRotation() const override;
#pragma endregion








private:

	UPROPERTY(EditAnywhere, Category = "AI Stats")UAIStateComponent* currentStateComponent = nullptr;

	UPROPERTY()ATrexAICharacter* trexAICharacter;
	void VarInit();


	void HandleTrapTriggered();




public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	FString InitialState;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	TMap<FString, TSubclassOf<UAIStateComponent>> AvailableStates;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine Debug")
	bool bDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAIStateComponent*> StateHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "State Machine Debug", meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 StateHistoryLength;
	UPROPERTY(BlueprintReadOnly)
	UAIStateComponent* AICurrentStateComp = nullptr;

	UPROPERTY()
	TMap<FString, UAIStateComponent*> StateMap;

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void SwitchStateByKey(FString StateKey);
	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void InitStateManager();


	private:
		bool bCanTickState = false;
		void InitializeStates();


};
