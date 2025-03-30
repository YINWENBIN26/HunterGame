// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "WarAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UWarAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	bool DoseOwnerHaveTag(FGameplayTag TagToCheck)const;
};
