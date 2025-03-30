// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"

#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarGameplayAbility.h"
#include "WarTypes/WarStructTypes.h"


void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
	for (const FWarHeroAbilitySet& AbilitySet :HeroStartUpAbilitySets)
	{
		if (!AbilitySet.IsValid())continue;
		FGameplayAbilitySpec AbilitySpec (AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=InASCToGive->GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		InASCToGive->GiveAbility(AbilitySpec);
	}

	      
}
