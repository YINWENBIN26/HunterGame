// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/WarEnemyGameplayAbility.h"
#include "EnemyGameplayAbility_FollowMove.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UEnemyGameplayAbility_FollowMove : public UWarEnemyGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "War|Ability")
	float FollowMoveSpeed;
	UPROPERTY(BlueprintReadWrite,Category = "War|Ability")
	float DefaultSpeed;
	
};
