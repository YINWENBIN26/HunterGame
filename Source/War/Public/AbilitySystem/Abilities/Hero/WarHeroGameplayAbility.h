// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarGameplayAbility.h"
#include "WarHeroGameplayAbility.generated.h"


class UHeroCombatComponent;
class AWarPlayerController;
class AWarHeroCharacter;
/**
 * 
 */
UCLASS()
class WAR_API UWarHeroGameplayAbility : public UWarGameplayAbility
{
	GENERATED_BODY()
public:
	// アクター情報からヒーローキャラクターを取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	AWarHeroCharacter* GetHeroCharacterFromActorInfo();

	// アクター情報からプレイヤーコントローラーを取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	AWarPlayerController* GetPlayerControllerFromActorInfo();

	// ヒーローキャラクターの戦闘コンポーネントを取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	// ヒーローダメージエフェクトの仕様ハンドルを作成する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	// 指定タグのクールダウン時間を取得する
	UFUNCTION(BlueprintCallable, Category = "Warrior Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldown);
private:
	TWeakObjectPtr<AWarHeroCharacter> CachedWarHeroCharacter;
	TWeakObjectPtr<AWarPlayerController> CachedWarHeroController;
};