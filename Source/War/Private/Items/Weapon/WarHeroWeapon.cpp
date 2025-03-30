// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/WarHeroWeapon.h"

void AWarHeroWeapon::AssignGrantedAbilitySpecsHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	GrantedAbilitySpecsHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AWarHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecsHandles;
}
