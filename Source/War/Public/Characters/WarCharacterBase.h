// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/Combat/PawnCombatInterface.h"
#include "Interface/UI/PawnUIInterface.h"
#include "WarCharacterBase.generated.h"


class UHeroUIComponent;
class UWarAbilitySystemComponent;
class UWarAttributeSet;
class UDataAsset_StartUpDataBase;
class UMotionWarpingComponent;
UCLASS()
class WAR_API AWarCharacterBase : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// デフォルト値を設定するコンストラクタ
	AWarCharacterBase();

	// Ability System Component を返す
	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
	// PawnCombatComponent を返す
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	// PawnUIComponent を返す
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	// ヒーローUIコンポーネント
	UHeroUIComponent* HeroUIComponent;

protected:
	// Possessed されたときの処理：Ability System Component の初期化とスタートアップデータの確認
	virtual void PossessedBy(AController *NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	// WarAbilitySystemComponent の参照
	TObjectPtr<UWarAbilitySystemComponent> WarAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem") 
	// WarAttributeSet の参照
	TObjectPtr<UWarAttributeSet> WarAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MotionWarping")
	// MotionWarpingComponent の参照
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	// キャラクタースタートアップデータ
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	// WarAbilitySystemComponent を取得する
	FORCEINLINE UWarAbilitySystemComponent *GetWarriorAbilitySystemComponent() const { return WarAbilitySystemComponent; }
	// WarAttributeSet を取得する
	FORCEINLINE UWarAttributeSet *GetWarriorAttributeSet() const { return WarAttributeSet; }
};