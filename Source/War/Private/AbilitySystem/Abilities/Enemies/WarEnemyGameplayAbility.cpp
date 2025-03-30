// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemies/WarEnemyGameplayAbility.h"

#include "WarGameplayTags.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "Characters/WarEnemyCharacter.h"

/**
 * @brief アクター情報から敵キャラクターを取得する（キャッシュ利用）。
 */
AWarEnemyCharacter* UWarEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedWarEnemyCharacter.IsValid())
	{
		CachedWarEnemyCharacter = Cast<AWarEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarEnemyCharacter.IsValid() ? CachedWarEnemyCharacter.Get() : nullptr;
}

/**
 * @brief アクター情報から敵キャラクターの戦闘コンポーネントを取得する。
 */
UEnemyCombatComponent* UWarEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

/**
 * @brief 敵に与えるダメージエフェクトの仕様ハンドルを作成する。
 */
FGameplayEffectSpecHandle UWarEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat) const
{
	check(EffectClass);
	
	// コンテキストハンドルの生成と初期化
	FGameplayEffectContextHandle ContextHandle = GetWarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// エフェクトハンドルの作成
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarGameplayTags::Shared_SetByCaller_BaseDamage, InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));
	
	return EffectSpecHandle;
}

/**
 * @brief スキル終了時に、ログ出力と終了イベント通知を行う。
 */
void UWarEnemyGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ActorInfo->OwnerActor->GetName());
	OnAbilityEndedEvent.Broadcast(this);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
