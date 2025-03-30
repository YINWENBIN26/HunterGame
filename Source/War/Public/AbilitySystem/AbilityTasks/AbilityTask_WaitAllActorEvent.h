// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitAllActorEvent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEventsReceived);
/**
 * 
 */
UCLASS()
class WAR_API UAbilityTask_WaitAllActorEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	// Blueprintでバインド可能な委任
	UPROPERTY(BlueprintAssignable)
	FOnAllEventsReceived OnAllEventsReceived;

	// 指定されたActor群から全てのGameplayイベントを待つタスクを生成する
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitAllActorEvent* WaitForAllGameplayEvents(UGameplayAbility* OwningAbility, const TArray<AActor*>& Actors, FGameplayTag EventTag, bool bTriggerOnce = true);

protected:
	// イベント受信待ちの対象Actor集合
	TSet<AActor*> PendingActors;

	// 監視対象のGameplay Tag
	FGameplayTag TargetEventTag;

	// 一度のみトリガーするかどうか
	bool bTriggerOnce;

	// Gameplayイベント受信時の処理
	void OnGameplayEventReceived(const FGameplayEventData* Payload);

	// タスクをアクティブ化し、各ActorのAbilitySystemComponentにイベントリスナーを登録する
	virtual void Activate() override;
	
	virtual void OnDestroy(bool AbilityEnded) override;
};