// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PoolSubSystem.h"
#include  "Interfaces/PoolableInterface.h"



//[[__UNREAL_ENGINE_UFUNCTION__]] AActor* UPoolSubSystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector SpawnPosition, FRotator SpawnRotation)
//{
//	AActor* pooledActor = nullptr;
//
//
//	if (PoolClass.Get()->ImplementsInterface(UPoolableInterface::StaticClass()))
//	{
//		if (ObjectPool.IsEmpty())
//		{
//			FActorSpawnParameters spawnParameters;
//			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//            pooledActor = GetWorld()->SpawnActor<AActor>(PoolClass, SpawnPosition, SpawnRotation, spawnParameters);
//		}
//		else
//		{
//			pooledActor = ObjectPool.Pop();
//			pooledActor->SetActorLocationAndRotation(SpawnPosition, SpawnRotation);
//		}
//
//		IPoolableInterface::Execute_OnSpawnFromPool(pooledActor);
//	}
//
//	return pooledActor;
//}

[[__UNREAL_ENGINE_UFUNCTION__]] void UPoolSubSystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector SpawnPosition, FRotator SpawnRotation, AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, SpawnPosition, SpawnRotation);
}

void UPoolSubSystem::ReturnToPool(AActor* Poolable)
{
	const UClass* PoolableClass = Poolable->GetClass();
	if (PoolableClass->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturnToPool(Poolable);
		FPoolArray* ObjectPool = ObjectPools.Find(PoolableClass);
		ObjectPool->Add(Poolable);
	}
	else
	{
		Poolable->Destroy();
	}
}
