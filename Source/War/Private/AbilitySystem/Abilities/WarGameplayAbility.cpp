// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "Component/Combat/PawnCombatComponent.h"

/**
 * @brief Ability が付与された際に、自動で起動する（OnGiven ポリシーの場合）。
 */
void UWarGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EWarAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
		else if (!ActorInfo)
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilityActivationPolicy OnGiven ActorInfo is nullptr"));
		}
		else if (Spec.IsActive())
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilityActivationPolicy OnGiven Spec is not active"));
		}
	}
}

/**
 * @brief Ability 終了時に、OnGiven ポリシーの場合は能力をクリアする。
 */
void UWarGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EWarAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

/**
 * @brief Avatar から PawnCombatComponent を取得する。
 */
UPawnCombatComponent *UWarGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

/**
 * @brief ActorInfo の AbilitySystemComponent を UWarAbilitySystemComponent 型にキャストして取得する。
 */
UWarAbilitySystemComponent *UWarGameplayAbility::GetWarAbilitySystemComponentFromActorInfo() const
{
	return Cast<UWarAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

/**
 * @brief 指定ターゲットに、エフェクト仕様ハンドルを適用する。
 */
FActiveGameplayEffectHandle UWarGameplayAbility::Native_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle InSpecHandle) const
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	check(TargetASC);
	check(InSpecHandle.IsValid());
	return GetWarAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC);
}

/**
 * @brief Blueprint 用：ターゲットにエフェクトを適用し、適用結果を出力する。
 */
FActiveGameplayEffectHandle UWarGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle InSpecHandle, EWarSuccessType& OutSuccessType)
{
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = Native_ApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarSuccessType::Successful : EWarSuccessType::Failed;
	return ActiveGameplayEffectHandle;
}

/**
 * @brief ヒット結果に対してエフェクトを適用し、ヒットリアクションイベントを送信する。
 */
void UWarGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle,
	const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())
	{
		return;
	}

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (UWarFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = Native_ApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{	
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;

					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						WarGameplayTags::Shared_Event_HitReact,
						Data
					);
				}
			}
		}
	}
}
