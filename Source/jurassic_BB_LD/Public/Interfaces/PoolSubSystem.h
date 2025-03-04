#pragma once

#include "PoolableInterface.h"
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubSystem.generated.h"

/**
 *
 */
USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	bool IsEmpty() const
	{
		return ObjectPool.IsEmpty();
	}

	AActor* Pop()
	{
		return ObjectPool.Pop();
	}

	void Add(AActor* actor)
	{
		ObjectPool.Add(actor);
	}
};

UCLASS()
class JURASSIC_BB_LD_API UPoolSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Pool SubSystem", meta = (DeterminesOutputType = "PoolClass", DynamicOutputParam = "SpawnedActor"))
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector SpawnPosition, FRotator SpawnRotation, AActor*& SpawnedActor);

	template<typename T>
	T* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector SpawnPosition, FRotator SpawnRotation);

	UFUNCTION(BlueprintCallable, Category = "Pool SubSystem")
	void ReturnToPool(AActor* Poolable);

	UPROPERTY()
	TMap<UClass*, FPoolArray> ObjectPools;
};

// Now the function is part of the class, so ObjectPools is accessible
template<typename T>
T* UPoolSubSystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector SpawnPosition, FRotator SpawnRotation)
{
	T* pooledActor = nullptr;

	if (PoolClass && PoolClass->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolClass);

		UWorld* World = GetWorld(); // Get world from UWorldSubsystem
		if (!World) return nullptr;

		if (ObjectPool.IsEmpty())
		{
			FActorSpawnParameters spawnParameters;
			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			pooledActor = World->SpawnActor<T>(PoolClass, SpawnPosition, SpawnRotation, spawnParameters);
		}
		else
		{
			pooledActor = CastChecked<T>(ObjectPool.Pop());
			pooledActor->SetActorLocationAndRotation(SpawnPosition, SpawnRotation);
		}

		IPoolableInterface::Execute_OnSpawnFromPool(pooledActor);
	}

	return pooledActor;
}
