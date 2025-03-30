// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarEnemyAbilitySystemComponent.h"

/**
 * @brief 指定タグに一致するアビリティをランダムに起動する
 */
bool UWarEnemyAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityToActivate)
{
	check(AbilityToActivate.IsValid());
	TArray<FGameplayAbilitySpec*> FoundAbilitySpec;
	// 指定タグに一致する起動可能なアビリティを取得
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityToActivate.GetSingleTagContainer(), FoundAbilitySpec);
	if (!FoundAbilitySpec.Num() == 0)
	{
		// ランダムにアビリティを選択して起動を試みる
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpec.Num() - 1);
		const FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpec[RandomAbilityIndex];
		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}

/**
 * @brief 指定タグに一致するアビリティの一覧を取得する
 */
TArray<FGameplayAbilitySpec*> UWarEnemyAbilitySystemComponent::GetAbilitySpecFromTag(const FGameplayTag& AbilityTag) const
{
	TArray<FGameplayAbilitySpec*> FoundAbilitySpec;
	// 指定タグに一致する起動可能なアビリティを取得
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTag.GetSingleTagContainer(), FoundAbilitySpec);
	return FoundAbilitySpec;
}