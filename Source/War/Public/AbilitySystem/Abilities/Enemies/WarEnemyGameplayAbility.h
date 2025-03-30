// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarGameplayAbility.h"
#include "WarEnemyGameplayAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityEnded, UWarEnemyGameplayAbility*, AbilityInstance);

class UEnemyCombatComponent;
class AWarEnemyCharacter;
/**
 * 
 */
#pragma once

// アクター情報から敵キャラクターを取得するための能力クラス
UCLASS()
class WAR_API UWarEnemyGameplayAbility : public UWarGameplayAbility
{
	GENERATED_BODY()
public:
	// アクター情報から敵キャラクターをキャッシュ利用で取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	AWarEnemyCharacter* GetEnemyCharacterFromActorInfo();

	// アクター情報から敵キャラクターの戦闘コンポーネントを取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	// スケーラブルなダメージ値に基づき敵へのダメージエフェクト仕様ハンドルを作成する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat) const;

	// スキル終了時にログ出力と終了イベントの通知を行う
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo,
							bool bReplicateEndAbility,
							bool bWasCancelled) override;

	// 能力終了時にイベントをブロードキャストするデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FOnAbilityEnded OnAbilityEndedEvent;

private:
	// キャッシュされた敵キャラクター（無効な場合は再取得）
	TWeakObjectPtr<AWarEnemyCharacter> CachedWarEnemyCharacter;
};
