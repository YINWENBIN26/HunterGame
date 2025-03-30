// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UWarInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template <class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig *InInputConfig, const FGameplayTag &InInputTag, ETriggerEvent TriggerEvent, UserObject *ContextObject, CallbackFunc Func);

	template <class UserObject, typename CallbackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig *InInputConfig, UserObject *ContextObject,
								CallbackFunc InputPressedFunc,
								CallbackFunc InputReleasedFunc);	
};
template <class UserObject, typename CallbackFunc>
inline void UWarInputComponent::BindNativeInputAction(const UDataAsset_InputConfig *InInputConfig, const FGameplayTag &InInputTag, ETriggerEvent TriggerEvent, UserObject *ContextObject, CallbackFunc Func)
{
	check(InInputConfig);
	checkf(InInputConfig, TEXT("InputConfig is nullptr"));
	if (UInputAction *FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}

template <class UserObject, typename CallbackFunc>
inline void UWarInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig *InInputConfig, UserObject *ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	checkf(InInputConfig, TEXT("InputConfig is nullptr"));
	for (const FWarInputActionConfig &AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid())
			continue;

		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleasedFunc, AbilityInputActionConfig.InputTag);
	}
}
