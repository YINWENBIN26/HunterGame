// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/PawnExtensionComponentBase.h"
#include "Items/Weapon/WarWeaponBase.h"
#include "PawnCombatComponent.generated.h"
UENUM(BlueprintType)
enum  class EToggleDamageType:uint8
{
	CurrentEquippedWeapon,
    LeftHand,
    RightHand
};
/**
 * 
 */
UCLASS()
class WAR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	// 生成された武器を登録し、イベントバインドを行う
	UFUNCTION(BlueprintCallable, Category = "War|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	// 指定されたタグに一致する武器を取得する
	UFUNCTION(BlueprintCallable, Category = "War|Combvat")
	AWarWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	// 現在装備中の武器タグ
	UPROPERTY(BlueprintReadWrite, Category = "War|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	// 現在装備中の武器を取得する
	UFUNCTION(BlueprintCallable, Category = "War|Combvat")
	AWarWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	// 武器の衝突判定を切り替える
	UFUNCTION(BlueprintCallable, Category = "War|Combvat")
	void ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);

	// 武器でターゲットにヒットした際の処理
	virtual void OnHitTargetActor(AActor* HitActor);
	// 武器がターゲットから引き抜かれた際の処理
	virtual void OnWeaponPulledFromTarget(AActor* InteractedActor);

protected:
	// ヒット済みアクターリスト
	TArray<AActor*> OverlappedActors;

private:
	// 登録された武器のマップ（タグと武器の対応）
	TMap<FGameplayTag, AWarWeaponBase*> ChararcterCarriedWeaponMap;
};
