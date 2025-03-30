// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarGameplayTags.h"

/**
 * @brief ヒットしたターゲットアクターに対して、メレー攻撃ヒットイベントを送信する
 *
 * ・既に OverlappedActors に存在する場合は何もしない
 * ・HitActor を OverlappedActors に追加し、イベントデータを作成してイベントを送信する
 */
void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// 既にヒット対象として登録済みの場合は処理しない
	if (OverlappedActors.Contains(HitActor))
		return;

	// ヒット対象を一意に追加する
	OverlappedActors.AddUnique(HitActor);

	bool BIsValidBlock = false;
	const bool bIsPlayerBlocking = false;
	const bool bIsMyAttackUnblockable = false;

	// プレイヤーがブロック中で、かつ攻撃がブロック不可能でない場合の処理
	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		// TODO: ブロック時の処理を実装する
	}

	// イベントデータを作成して送信
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
    
	if (BIsValidBlock)
	{
		// TODO: ブロックが有効な場合の処理を実装する
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarGameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}

/**
 * @brief 武器がターゲットから引き抜かれたときの処理
 *
 * スーパークラスの処理をそのまま呼び出す
 */
void UEnemyCombatComponent::OnWeaponPulledFromTarget(AActor* InteractedActor)
{
	Super::OnWeaponPulledFromTarget(InteractedActor);
}
