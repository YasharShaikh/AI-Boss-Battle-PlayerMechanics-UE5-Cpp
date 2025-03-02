#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TelekineticObject.generated.h"

class UStaticMeshComponent;
class APlayerCharacter;

USTRUCT(BlueprintType)
struct FTelekineticObjectStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float MeshScaleFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float StaminaCost = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float SafePullDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxPullForce = 5000.0f;

	UPROPERTY(EditDefaultsOnly)
	float LiftSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly)
	float ThrowForce = 1500.0f;

	UPROPERTY(EditDefaultsOnly)
	float Mass = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float PushSpeed = 100.0f;
};
UENUM(BlueprintType)
enum class EObjectSize : uint8
{
	Small     UMETA(DisplayName = "Small"),
	Medium    UMETA(DisplayName = "Medium"),
	Large     UMETA(DisplayName = "Large")
};

UENUM(BlueprintType)
enum class ETObjectState : uint8
{
	Idle,
	Lifting,
	Held,
	Thrown
};

UCLASS()
class JURASSIC_BB_LD_API ATelekineticObject : public AActor
{
	GENERATED_BODY()

public:
	ATelekineticObject();
	virtual void Tick(float DeltaTime) override;


	inline float GetSafePullDistance() const { return CachedStats.SafePullDistance; }
	inline const FTelekineticObjectStats* GetSizeStats() const { return &CachedStats; }

	void EnterState(ETObjectState NewState);

protected:


	virtual void BeginPlay() override;
	void SetSize(EObjectSize Size);
	APlayerCharacter* playerCharacter;
	FVector TargetPostion;


private:

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	EObjectSize ObjectSize;

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	ETObjectState ObjectState;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float ThrowSpeed = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float LiftSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float MaxPullForce = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	ETObjectState CurrentState = ETObjectState::Idle;

	FTelekineticObjectStats CachedStats;

	static const TArray<FTelekineticObjectStats> ObjectStatsArray;

	void UpdatePhysicsSettings(bool bHoverMode);
	void ResetPhysicsState();
};
