// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Characters/WarCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "WarHeroCharacter.generated.h"

class UHeroCombatComponent;
struct FGameplayTag;
struct FInputActionValue;
class UDataAsset_InputConfig;
/**
 * 
 */
UCLASS()
class WAR_API AWarHeroCharacter : public AWarCharacterBase
{
	GENERATED_BODY()
public:
	// コンストラクタ：各コンポーネントの生成と初期設定を行う
	AWarHeroCharacter();

	//~ Begin IPawnCombatInterface Interface
	// PawnCombatComponent を返す（戦闘処理用コンポーネント）
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface
	// PawnUIComponent を返す（UI表示用コンポーネント）
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	// ヒーロー専用の UI コンポーネントを返す
	virtual UHeroUIComponent* GetHeroUIComponent() const override; 
	//~ End IPawnUIInterface Interface

private:
#pragma region component
	// カメラブーム（カメラアーム）：カメラ位置の制御用コンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// フォローカメラ：プレイヤー追従用カメラコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	// ヒーローコンバットコンポーネント：戦闘処理用コンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;
#pragma endregion component

#pragma region Input
	// 入力コンフィグデータ：入力マッピングの設定データ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;
#pragma endregion

	// 移動入力処理
	void Input_Move(const FInputActionValue &InputActionValue);
	// 視点入力処理
	void Input_Look(const FInputActionValue &InputActionValue);
	// ターゲット切り替え入力（トリガー時）の処理
	void Input_SwitchTargetTriggered(const FInputActionValue &InputActionValue);
	// ターゲット切り替え入力（完了時）の処理
	void Input_SwitchTargetCompleted(const FInputActionValue &InputActionValue);

	// ターゲット切り替え用入力値保持変数
	FVector2d SwitchDirection = FVector2d::ZeroVector;

	// アビリティ入力（押下）処理
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	// アビリティ入力（離す）処理
	void Input_AbilityInputRelease(FGameplayTag InInputTag);

protected:
	//~ Begin APawn Interface.
	// Possessed された際の初期化処理（Ability 初期化等）
	virtual void PossessedBy(AController *NewController) override;
	//~ End APawn Interface

	// ゲーム開始時の処理
	virtual void BeginPlay() override;
	// 入力バインドの設定処理
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

public:
	// ヒーローコンバットコンポーネントの取得（戦闘処理用）
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};

