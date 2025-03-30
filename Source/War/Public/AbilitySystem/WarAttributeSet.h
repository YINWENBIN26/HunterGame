// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "WarAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
class IPawnUIInterface;
/**
 * 
 */
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

UCLASS()
class WAR_API UWarAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UWarAttributeSet();

virtual void PostGameplayEffectExecute (const struct FGameplayEffectModCallbackData &Data) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	/*
     * Vital Attributes
     */

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Rage;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, Rage);
	
	UPROPERTY(BlueprintReadOnly, Category = "Primary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly,Category = "Primary Attributes")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, MaxRage);
	
	UPROPERTY(BlueprintReadOnly,Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, AttackPower);

	UPROPERTY(BlueprintReadOnly,Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, DefensePower);

	UPROPERTY(BlueprintReadOnly,Category = "DamageTaken")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UWarAttributeSet, DamageTaken);
private:
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;
};