// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarStructTypes.generated.h"

class UInputMappingContext;
struct FWarHeroAbilitySet;
class UWarGameplayAbility;
class UWarHeroLinkedAnimLayer;


USTRUCT(BlueprintType)
struct FWarHeroAbilitySet
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	TSubclassOf<UWarGameplayAbility > AbilityToGrant;
	bool IsValid()const;
};

USTRUCT(BlueprintType)
struct FWarHeroSkillAbilitySet:public FWarHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInstance> SkillMaterial;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly ,meta = (Categories = "CoolDown"))
	FGameplayTag AbilityCoolDownTag;
};


USTRUCT(BlueprintType)
struct FWarHeroWeaponData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarHeroLinkedAnimLayer> WeaponANimLayerToLink;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarHeroAbilitySet> DefaultWeaponAbilities;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarHeroSkillAbilitySet> SkillAbilities;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
USTRUCT(BlueprintType)
struct FWarNinjutsuData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FGameplayTag> InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	TSubclassOf<UWarGameplayAbility > AbilityToActive;
	bool IsValid()const;
};