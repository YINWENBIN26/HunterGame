// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "WarEnemyAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UWarEnemyAbilitySystemComponent : public UWarAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// 指定タグに一致するアビリティをランダムに起動する
	UFUNCTION(BlueprintCallable, Category = "War|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityToActivate);

	// 指定タグに一致するアビリティの一覧を取得する
	TArray<FGameplayAbilitySpec*> GetAbilitySpecFromTag(const FGameplayTag& AbilityTag) const;
};