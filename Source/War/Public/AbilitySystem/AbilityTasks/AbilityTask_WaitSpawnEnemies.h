// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

class AWarEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate,const TArray<AWarEnemyCharacter*>&,SpawnEnemy);
/**
 * 
 */
UCLASS()
class WAR_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "War|AbilityTasks",meta=(DisplayName="Wait Gameplay Event and Spawn Enemies",HidePin="OwingAbility",DefaultToSelf="OwingAbility",BlueprintInternalUseOnly="true",NumToSpawn="1",RandomSpawnRadius="200"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(UGameplayAbility* OwingAbility,FGameplayTag EventTag,TSoftClassPtr<AWarEnemyCharacter> SoftEnemyClassToSpawn,int32 NumToSpawn,const FVector& SpawnOrigin,float RandomSpawnRadius);

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;
//begin UGameplayTask Interface
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//End UGameplayTask Interface
private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AWarEnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
    FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	FDelegateHandle DelegateHandle;

	void OnGameplayEventReceived(const FGameplayEventData* InPayload);

	void OnEnemyClassLoaded();
};

