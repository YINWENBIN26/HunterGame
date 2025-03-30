// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ExecuteTaskOnTick.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTasKTickDelegate, float, DeltaTime);
/**
 * 
 */
UCLASS()
class WAR_API UAbilityTask_ExecuteTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_ExecuteTaskOnTick();
	UFUNCTION(BlueprintCallable, Category = "War|AbilityTasks",meta=(HidePin="OwingAbility",DefaultToSelf="OwingAbility",BlueprintInternalUseOnly=True))
	static UAbilityTask_ExecuteTaskOnTick* ExecuteTaskOnTick(UGameplayAbility* OwingAbility);
	
	//GameplayTask Interface
	virtual void TickTask(float DeltaTime) override;
	UPROPERTY(BlueprintAssignable, Category="War|AbilityTasks")
	FOnAbilityTasKTickDelegate OnAbilityTasKTick;
};
