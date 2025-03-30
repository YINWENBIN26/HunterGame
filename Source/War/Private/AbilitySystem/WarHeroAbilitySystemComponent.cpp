// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarHeroAbilitySystemComponent.h"
#include "WarGameplayTags.h"
#include "AbilitySystem/Abilities/WarGameplayAbility.h"
#include "Interface/Combat/PawnCombatInterface.h"

/**
 * @brief 入力押下時に対応するアビリティを起動またはキャンセルする
 */
void UWarHeroAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
		return;

	// 全ての起動可能なアビリティをチェック
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 入力タグに一致しないアビリティはスキップ
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag))
			continue;

		// トグル可能なアビリティの場合、アクティブならキャンセル、非アクティブなら起動
		if (InInputTag.MatchesTag(WarGameplayTags::InputTag_Toggleable))
		{
			if (AbilitySpec.IsActive())
			{
				CancelAbilityHandle(AbilitySpec.Handle);
			}
			else
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

/**
 * @brief 入力解除時に、持続型アビリティをキャンセルする
 */
void UWarHeroAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	// "必ず押し続ける"入力タグでなければ何もしない
	if (InInputTag.IsValid() | !InInputTag.MatchesTag(WarGameplayTags::InputTag_MustBeHeld))
	{
		return;
	}

	// 全ての起動可能なアビリティをチェックし、条件に合えばキャンセル
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

/**
 * @brief ヒーローの武器アビリティとスキルアビリティを付与する
 */
void UWarHeroAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FWarHeroAbilitySet>& InDefaultWarHeroAbilities,
	const TArray<FWarHeroSkillAbilitySet>& InSkillAbilities,
	int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWarHeroAbilities.IsEmpty())
		return;

	// 通常武器アビリティの付与
	for (const FWarHeroAbilitySet& AbilitySet : InDefaultWarHeroAbilities)
	{
		if (!AbilitySet.IsValid())
			continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		GiveAbility(AbilitySpec);
		OutGrantedAbilitySpecHandles.Add(AbilitySpec.Handle);
	}

	// スキルアビリティの付与
	for (const FWarHeroSkillAbilitySet& AbilitySet : InSkillAbilities)
	{
		if (!AbilitySet.IsValid())
			continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		GiveAbility(AbilitySpec);
		OutGrantedAbilitySpecHandles.Add(AbilitySpec.Handle);
	}
}

/**
 * @brief 付与済みの武器アビリティを削除する
 */
void UWarHeroAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
		return;

	for (const FGameplayAbilitySpecHandle SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}