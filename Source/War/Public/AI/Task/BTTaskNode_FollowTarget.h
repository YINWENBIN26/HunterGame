// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FollowTarget.generated.h"


/**
 * @brief 敵の移動状態の定義
 */
UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	Walking,
	Running	
};

/**
 * @brief フォロータスク用メモリ構造体
 */
struct FFollowTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;
	// 最後に設定された移動状態
	EEnemyMovementState LastMovementState = EEnemyMovementState::Walking;

	// 有効かどうかを判定（Pawnとターゲットが有効ならtrue）
	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}

	// メモリの状態をリセットする
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
		LastMovementState = EEnemyMovementState::Walking;
	}
};

/**
 * @brief ターゲットをフォローするBTタスクノード
 */
UCLASS()
class WAR_API UBTTaskNode_FollowTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UBTTaskNode_FollowTarget();

protected:
	/** 目標との距離がこれより大きければ走る */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float RunDistanceThreshold;

	/** 目標との距離がこれより小さければ歩く */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float WalkDistanceThreshold;

	/** 目標との距離がこれ以下なら停止 */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float StopFollowDistance;

	/** フォロー対象のActorを取得するブラックボードキー */
	UPROPERTY(EditAnywhere, Category = "Follow")
	FBlackboardKeySelector TargetToFollowKey;

	/** 歩行速度 */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float WalkSpeed;

	/** 走行速度 */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float RunSpeed;

	/** ターゲット後方からのオフセット距離 */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float BehindDistance = 200.f;

	/** 横方向のオフセット */
	UPROPERTY(EditAnywhere, Category = "Follow")
	float SideOffset = 100.f;

	/** デバッグ表示を有効にするか */
	UPROPERTY(EditAnywhere, Category = "Follow")
	bool bInDebug = false;

	// ~ Begin UBTNode インターフェース
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode インターフェース

	/** タスク開始時の処理 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** 毎フレーム呼ばれるタスク処理 */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/** 距離に応じてPawnの移動速度・状態を更新する */
	void UpdateMovementSpeed(APawn* Pawn, float DistanceToTarget, FFollowTargetTaskMemory* Memory);

	/** 指定距離に到達しているか判定する */
	bool HasReachedStopDistance(float DistanceToTarget) const;

	/** ターゲット後方のオフセットを計算する */
	FVector GetFollowOffset(APawn* Pawn, AActor* Target) const;

	/** 指定位置をNavMesh上の有効な位置に投影する */
	bool ProjectLocationToNavMesh(const FVector& InLocation, FVector& OutLocation) const;

	/** 移動中ならTick処理をスキップするか判定する */
	bool ShouldSkipTick(AAIController* AIController, APawn* Pawn) const;

	/** タスク終了時の後処理 */
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, FFollowTargetTaskMemory* Memory);
};
