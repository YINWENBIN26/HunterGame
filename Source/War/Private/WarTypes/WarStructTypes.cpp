// Fill out your copyright notice in the Description page of Project Settings.


#include "WarTypes/WarStructTypes.h"

#include "AbilitySystem/Abilities/WarGameplayAbility.h"

bool FWarHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
