// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hero/WarHeroGameplayAbility.h"
#include "Characters/WarHeroCharacter.h"
#include "Widgets/WarWidgetBase.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class WAR_API UHeroGameplayAbility_TargetLock : public UWarHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	// ターゲットロック能力を起動し、ターゲットのロック処理を実行する
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	// ターゲットロック能力を終了し、後処理を実施する
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	// 毎フレームのターゲットロック処理を実行する
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);
	
	// 入力に基づき左右のターゲットを切り替える
	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
		
private:
	// ターゲットロックを試みる
	void TryLockOnTarget();
	// Box Traceで利用可能なターゲット候補を取得する
	void GetAvailableActorsToLock();
	// 利用可能なターゲットから最も近いものを選択する
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	// 現在のターゲット周辺から左右のターゲット候補を取得する
	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight);
	// ターゲットロックウィジェットを表示する
	void DrawTargetLockWidget();
	// ターゲットロックウィジェットの位置を更新する
	void SetTargetLockWidgetPosition();
	// ターゲットロック用の入力マッピングを有効化する
	void InitTargetLockMappingContext();
	// ターゲットロックをキャンセルする
	void CancelLockOnTarget();
	// ターゲットロックの後処理を実施する
	void CleanUpLockOnTarget();
	// ターゲットロック用の入力マッピングを無効化する
	void ResetTargetLockMappingContext();
	
	// ボックストレースの距離
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	float BoxTraceDistance = 1500.f;
	
	// トレース用ボックスのサイズ
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	FVector TraceBoxSize = FVector(1500.f,1500.f,300.f);
	
	// ボックストレースで対象とするオブジェクトタイプ
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;
	
	// デバッグ用形状の表示フラグ
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	bool bShowDebugShapes = false;
	
	// ターゲットロックウィジェットのクラス
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	TSubclassOf<UWarWidgetBase> TargetLockWidgetClass;
	
	// ターゲットロック時の回転補間速度
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	float TargetLockRotationInterpSpeed = 5.0f;
	
	// ターゲットロック用の入力マッピングコンテキスト
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	UInputMappingContext* TargetLockMappingContext;
	
	// カメラオフセット距離
	UPROPERTY(EditDefaultsOnly, Category="TargetLock")
	float TargetLockCameraOffsetDistance = 20.0f;
	
	// 利用可能なターゲット候補
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;
	
	// 現在ロック中のターゲット
	UPROPERTY()
	AActor* CurrentLockedActor;
	// 表示中のターゲットロックウィジェット
	UPROPERTY()
	UWarWidgetBase* CurrentLockedWidget;
	// ターゲットロックウィジェットのサイズ
	UPROPERTY()
	FVector2D TargetLockWidgetSize;
};
