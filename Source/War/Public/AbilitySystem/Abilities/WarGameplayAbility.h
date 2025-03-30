// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "WarTypes/WarEnumTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarGameplayAbility.generated.h"

class UWarAbilitySystemComponent;

UENUM(BlueprintType)
enum class EWarAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven,
};
/**
 * 
 */
UCLASS()
class WAR_API UWarGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// アビリティ付与時に自動起動する（OnGivenポリシーの場合）
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// アビリティ終了時に、OnGivenポリシーの場合は能力をクリアする
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// AvatarからPawnCombatComponentを取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	// ActorInfoのAbilitySystemComponentをUWarAbilitySystemComponentにキャストして取得する
	UFUNCTION(BlueprintPure, Category = "War|Ability")
	UWarAbilitySystemComponent* GetWarAbilitySystemComponentFromActorInfo() const;

	// 指定ターゲットにエフェクト仕様ハンドルを適用する
	FActiveGameplayEffectHandle Native_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle InSpecHandle) const;

	// Blueprint用：ターゲットにエフェクトを適用し、適用結果（成功／失敗）を出力する
	UFUNCTION(BlueprintCallable, Category = "War|Ability", meta = (ExpandEnumAsExecs="OutSuccessType", DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle InSpecHandle, EWarSuccessType& OutSuccessType);

	// ヒット結果に対してエフェクトを適用し、ヒットリアクションイベントを送信する
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);

protected:
	// アビリティ起動ポリシー（デフォルトはOnTriggered）
	UPROPERTY(EditDefaultsOnly, Category = "WarAbility")
	EWarAbilityActivationPolicy AbilityActivationPolicy = EWarAbilityActivationPolicy::OnTriggered;
};
