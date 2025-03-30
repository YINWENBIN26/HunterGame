// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitAllActorEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

/**
 * @brief 指定されたActor群から全てのGameplayイベントを待つタスクを生成する。
 */
UAbilityTask_WaitAllActorEvent* UAbilityTask_WaitAllActorEvent::WaitForAllGameplayEvents(UGameplayAbility* OwningAbility, const TArray<AActor*>& Actors, FGameplayTag EventTag, bool bTriggerOnce)
{
	UAbilityTask_WaitAllActorEvent* Task = NewAbilityTask<UAbilityTask_WaitAllActorEvent>(OwningAbility);
	Task->TargetEventTag = EventTag;
	Task->bTriggerOnce = bTriggerOnce;

	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			Task->PendingActors.Add(Actor);
		}
	}

	return Task;
}

/**
 * @brief タスクをアクティブ化し、各ActorのAbilitySystemComponentにイベントリスナーを登録する。
 */
void UAbilityTask_WaitAllActorEvent::Activate()
{
	for (AActor* Actor : PendingActors)
	{
		if (!Actor) continue;

		// ActorのAbilitySystemComponentを取得する
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		if (ASC)
		{
			// TargetEventTagに対してイベントリスナーを登録
			ASC->GenericGameplayEventCallbacks.FindOrAdd(TargetEventTag).AddUObject(this, &UAbilityTask_WaitAllActorEvent::OnGameplayEventReceived);
		}
	}
}

/**
 * @brief Gameplayイベント受信時の処理。
 *
 * イベントのInstigatorがPendingActorsに含まれていれば削除し、
 * 全てのActorからイベントを受信したらOnAllEventsReceivedをブロードキャストする。
 */
void UAbilityTask_WaitAllActorEvent::OnGameplayEventReceived(const FGameplayEventData* Payload)
{
	const AActor* EventInstigator = Cast<AActor>(Payload->Instigator);
	if (EventInstigator && PendingActors.Contains(EventInstigator))
	{
		PendingActors.Remove(EventInstigator);
	}

	// 全てのActorからイベントを受信した場合、委任をブロードキャスト
	if (PendingActors.Num() == 0)
	{
		OnAllEventsReceived.Broadcast();

		if (bTriggerOnce)
		{
			EndTask();
		}
	}
}
void UAbilityTask_WaitAllActorEvent::OnDestroy(bool AbilityEnded)
{
	for (AActor* Actor : PendingActors)
	{
		if (!Actor) continue;

		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		if (ASC)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(TargetEventTag).RemoveAll(this);
		}
	}

	Super::OnDestroy(AbilityEnded);
}
