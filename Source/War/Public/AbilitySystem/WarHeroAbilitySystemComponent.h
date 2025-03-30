// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "WarHeroAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UWarHeroAbilitySystemComponent : public UWarAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// 入力押下時に対応するアビリティを起動またはキャンセルする
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);

	// 入力解除時に、持続型アビリティをキャンセルする
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	// ヒーローの武器アビリティとスキルアビリティを付与する
	UFUNCTION(BlueprintCallable, Category = "War|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarHeroAbilitySet>& InDefaultWarHeroAbilities, const TArray<FWarHeroSkillAbilitySet>& InSkillAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	// 付与済みの武器アビリティを削除する
	UFUNCTION(BlueprintCallable, Category = "War|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
};
