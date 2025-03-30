// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CircleAround.generated.h"

USTRUCT()
struct FCircleAroundMemory
{
	GENERATED_BODY()

	/** 记录AI持续距离过近的时间 */
	float TimeSinceClose = 0.f;
};

/**
 * ターゲットの周囲を円形移動するタスク
 */
UCLASS()
class WAR_API UUBTTask_CircleAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UUBTTask_CircleAround(const FObjectInitializer& ObjectInitializer);

	// タスク開始処理
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 毎フレーム処理
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// 必要なメモリサイズ
	virtual uint16 GetInstanceMemorySize() const override
	{
		return sizeof(FCircleAroundMemory);
	}

protected:
	// ターゲットActorのブラックボードキー
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKey;

	// 円軌道の希望半径
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float DesiredCircleRadius = 1000.f;

	// 基本の移動速度
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float MoveSpeed = 300.f;

	// 回転補間速度
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float RotationInterpSpeed = 5.f;

	// 許容距離誤差
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float AcceptableRadiusError = 100.f;

	// 外向き補正開始までの遅延時間
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float AdjustOutwardDelay = 2.f;

	// 距離補正用の追加速度
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float AdjustDistanceSpeed = 200.f;

	// 接線方向の重み
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float WeightTangent = 0.7f;
	// 径方向補正の重み
	UPROPERTY(EditAnywhere, Category="CircleSettings")
	float WeightRadial  = 0.3f;

private:
	/**
	 * @brief 移動パラメータの計算
	 *
	 * Pawnとターゲット位置から、最終的な前方ベクトルと必要な移動速度を算出する。
	 */
	void CalculateMovementParameters(APawn* Pawn, const FVector& TargetLocation, float DeltaSeconds, FCircleAroundMemory* Memory, FVector& OutDesiredForward, float& OutRequiredMoveSpeed) const;
};

