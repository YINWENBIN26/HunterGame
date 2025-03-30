// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTaskNode_FollowTarget.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RunOrMoveTo.generated.h"

class USplineComponent;

struct FRunOrMoveToMemory
{
	TWeakObjectPtr<AAIController> AIController;
	TWeakObjectPtr<APawn> OwningPawn;

	// Spline移動モードかどうか
	bool bUseSpline = false;
	// 目的地に到達したか
	bool bReachedDestination = false;

	// Splineアクターの弱参照
	TWeakObjectPtr<AActor> SplineActor;
	// Spline上での進行距離
	float CurrentDistance = 0.f;
	// 目標位置
	FVector TargetLocation = FVector::ZeroVector;
	
	// メモリが有効か確認
	bool IsValid() const
	{
		return AIController.IsValid() && OwningPawn.IsValid();
	}

	// メモリ状態をリセットする
	void Reset()
	{
		bUseSpline = false;
		bReachedDestination = false;
		SplineActor.Reset();
		CurrentDistance = 0.f;
		TargetLocation = FVector::ZeroVector;
	}
};

/**
 * ターゲットへの直接移動またはSpline移動を行うBTタスク
 */
UCLASS()
class WAR_API UBTTask_RunOrMoveTo : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UBTTask_RunOrMoveTo(const FObjectInitializer& ObjectInitializer);

	// ブラックボードから取得する目標位置のキー
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	// 角度がこの閾値より大きい場合、Spline移動を使用する
	UPROPERTY(EditAnywhere, Category = "AI")
	float AngleThreshold = 10.f;
	
	// 移動速度
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float RunSpeed = 600.f;

	// Spline移動時の回転補間速度
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float RotationInterpSpeed = 5.f;

protected:
	// タスク開始時の初期化処理
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 毎フレーム呼ばれるタスク処理
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// タスク終了時の後処理
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, FRunOrMoveToMemory* Memory);
	// タスクインスタンス用のメモリサイズを返す
	virtual uint16 GetInstanceMemorySize() const override;

	// ヘルパー関数：Pawnの前方向と目標位置との角度（度）を計算する
	float ComputeAngleBetween(APawn* Pawn, const FVector& TargetPos) const;

	// ヘルパー関数：Spline移動の初期化を行う
	EBTNodeResult::Type SetupSplineMovement(FRunOrMoveToMemory* Memory);

	// ヘルパー関数：Splineの制御点を設定する
	void SetupSplinePoints(APawn* OwningPawn, const FVector& TargetPos, USplineComponent* SplineComp);

	// ヘルパー関数：Spline移動中の更新処理を行う
	void UpdateSplineMovement(APawn* OwningPawn, FRunOrMoveToMemory* Memory, float DeltaSeconds, UBehaviorTreeComponent& OwnerComp);
};