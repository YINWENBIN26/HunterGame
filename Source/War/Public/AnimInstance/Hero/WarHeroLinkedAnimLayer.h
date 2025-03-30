// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/WarAnimInstanceBase.h"
#include "WarHeroLinkedAnimLayer.generated.h"

class UWarHeroAnimInstance;
/**
 * 
 */
UCLASS()
class WAR_API UWarHeroLinkedAnimLayer : public UWarAnimInstanceBase
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	UWarHeroAnimInstance *GetHeroAnimInstance() const;
};
