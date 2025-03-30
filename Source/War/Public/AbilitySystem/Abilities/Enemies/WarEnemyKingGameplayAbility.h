// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/WarEnemyGameplayAbility.h"
#include "WarEnemyKingGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UWarEnemyKingGameplayAbility : public UWarEnemyGameplayAbility
{
	GENERATED_BODY()
public:
	// 指定アクターの周辺から、特定タグを持つ敵アクターを収集する
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem|King")
	TArray<AActor*> GetAllEnemiesHasTag(AActor* Owner, FGameplayTag EnemyTag, FVector BoxExtent);

	// 対象アクター群のブラックボードに、キングアクターとして自身を設定する
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem|King", meta = (DisplayName = "Assign King to Selected Actors", Tooltip = "Assigns the King actor to the specified list of actors."))
	void SetKingOfActors(const TArray<AActor*>& TargetActors);
};
