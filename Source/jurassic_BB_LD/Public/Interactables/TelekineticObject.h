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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Telekinesis")
	float MeshScaleFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Telekinesis")
	float Damage = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Telekinesis")
	float StaminaCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Telekinesis")
	float ContinuousStaminaDrain = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Telekinesis")
	float SafePullDistance = 100.0f;
};

UENUM(BlueprintType)
enum class EObjectSize : uint8
{
	Small     UMETA(DisplayName = "Small"),
	Medium    UMETA(DisplayName = "Medium"),
	Large     UMETA(DisplayName = "Large")
};

UCLASS()
class JURASSIC_BB_LD_API ATelekineticObject : public AActor
{
	GENERATED_BODY()

public:
	ATelekineticObject();
	virtual void Tick(float DeltaTime) override;

	void MoveTowardsPlayer(const FVector& HoldPosition, float deltaTime);
	void MoveTowardsEnemy(const FVector& screenCenter);


	inline float GetSafePullDistance() const { return CachedStats.SafePullDistance; }
	inline const FTelekineticObjectStats* GetSizeStats() const { return &CachedStats; }

protected:
	virtual void BeginPlay() override;
	void SetSize(EObjectSize Size);


private:

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	EObjectSize ObjectSize;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float ThrowSpeed = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float LiftSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float MaxPullForce = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Telekinesis")
	float LiftEndLocation = 5000.0f;


	bool bHasBeenThrown;
	bool bHasBeenLifted;
	FTelekineticObjectStats CachedStats;

	static const TMap<EObjectSize, FTelekineticObjectStats> ObjectStatsMap;

	bool LiftObject(float DeltaTime);
};
