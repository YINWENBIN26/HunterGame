// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Hero/WarHeroGameplayAbility.h"

#include "WarGameplayTags.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "Characters/WarHeroCharacter.h"
#include "Controllers/WarPlayerController.h"

/**
 * @brief アクター情報からヒーローキャラクターを取得する。
 */
AWarHeroCharacter* UWarHeroGameplayAbility::GetHeroCharacterFromActorInfo() 
{
	if (!CachedWarHeroCharacter.IsValid())
	{
		CachedWarHeroCharacter = Cast<AWarHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarHeroCharacter.IsValid() ? CachedWarHeroCharacter.Get() : nullptr;
}

/**
 * @brief アクター情報からプレイヤーコントローラーを取得する。
 */
AWarPlayerController* UWarHeroGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedWarHeroController.IsValid())
	{
		CachedWarHeroController = Cast<AWarPlayerController>(CurrentActorInfo->PlayerController);
	}
	return CachedWarHeroController.IsValid() ? CachedWarHeroController.Get() : nullptr;
}

/**
 * @brief ヒーローキャラクターの戦闘コンポーネントを取得する。
 */
UHeroCombatComponent* UWarHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

/**
 * @brief ヒーローダメージエフェクトの仕様ハンドルを作成する。
 */
FGameplayEffectSpecHandle UWarHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);
	FGameplayEffectContextHandle ContextHandle = GetWarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}
	return EffectSpecHandle;
}

/**
 * @brief 指定タグのクールダウン時間を取得する。
 */
bool UWarHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime,
	float& RemainingCooldown)
{
	check(InCooldownTag.IsValid());

	FGameplayEffectQuery CoolDownQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(InCooldownTag.GetSingleTagContainer());
	TArray<TPair<float, float>> TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CoolDownQuery);
	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldown = TimeRemainingAndDuration[0].Key;
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
	}
	return RemainingCooldown > 0.f;
}



