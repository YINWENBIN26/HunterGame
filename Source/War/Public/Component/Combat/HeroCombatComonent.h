// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "HeroCombatComonent.generated.h"

class AWarHeroWeapon;
/**
 * 
 */
UCLASS()
class WAR_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
public:
	// キャラクターが持つ武器の中から指定タグの武器を取得する
	UFUNCTION(BlueprintCallable, Category = "War|Combat")
	AWarHeroWeapon* GetHeroCarriedWeaponByTag(const FGameplayTag InWeaponTag) const;
	
	// 現在装備中の武器を取得する
	UFUNCTION(BlueprintCallable, Category = "War|Combat")
	AWarHeroWeapon* GetHeroCurrentEquippedWeapon() const;
	
	// 装備中の武器のダメージレベルを取得する
	UFUNCTION(BlueprintCallable, Category = "War|Combat")
	float GetHeroCurrentEquippedWeaponDamageLevel(float InLevel) const; 
	
	// ヒットしたターゲットに対して攻撃イベントを送信する
	virtual void OnHitTargetActor(AActor* HitActor) override;
	
	// 武器がターゲットから引き抜かれた際にヒットポーズイベントを送信する
	virtual void OnWeaponPulledFromTarget(AActor* InteractedActor) override;
};

