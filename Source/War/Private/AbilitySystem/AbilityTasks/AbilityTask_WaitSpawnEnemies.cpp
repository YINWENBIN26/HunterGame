// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Characters/WarEnemyCharacter.h"
#include "Engine/AssetManager.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwingAbility,
                                                                               FGameplayTag EventTag, TSoftClassPtr<AWarEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                               const FVector& SpawnOrigin, float RandomSpawnRadius)
{
	UAbilityTask_WaitSpawnEnemies* Task=NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwingAbility);
	Task->CachedEventTag=EventTag;
	Task->CachedSoftEnemyClassToSpawn=SoftEnemyClassToSpawn;
	Task->CachedNumToSpawn=NumToSpawn;
	Task->CachedSpawnOrigin=SpawnOrigin;
	Task->CachedRandomSpawnRadius=RandomSpawnRadius;

	return Task;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	 FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	DelegateHandle= Delegate.AddUObject(this,&ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	Delegate.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	//只在第一次
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
		);
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarEnemyCharacter*>());
		}
		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass= CachedSoftEnemyClassToSpawn.Get();
	UWorld* World =GetWorld();
	if (!LoadedClass||!World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarEnemyCharacter*>());
		}
		EndTask();
	}
	TArray<AWarEnemyCharacter*> SpawnedEnemies;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i=0;i<CachedNumToSpawn;i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this,CachedSpawnOrigin,RandomLocation,CachedRandomSpawnRadius);

		RandomLocation+=FVector(0.f,0.f,150.f);

		const FRotator  SpawnFacingRotation=AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
		AWarEnemyCharacter* SpawnedEnemy=World->SpawnActor<AWarEnemyCharacter>(LoadedClass,RandomLocation,SpawnFacingRotation,SpawnParams);

		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<AWarEnemyCharacter*>());
		}
	}
	EndTask();
}
