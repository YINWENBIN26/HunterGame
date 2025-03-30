// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Items/Weapon/WarWeaponBase.h"
#include "WarTypes/WarStructTypes.h"
#include "WarHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API AWarHeroWeapon : public AWarWeaponBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WeaponData")
	FWarHeroWeaponData HeroWeaponData;
	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecsHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);
	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecsHandles;
};
