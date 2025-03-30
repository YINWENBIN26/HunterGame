// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotatorToFaceTarget.generated.h"


struct FRotatorToFaceTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};

/**
 * 
 */
UCLASS()
class WAR_API UBTTask_RotatorToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// コンストラクタ：ターゲットに向かって回転するタスクの初期化を行う
	UBTTask_RotatorToFaceTarget();
	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;

	// タスク開始時に、ターゲットとPawnの情報を設定する
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 毎フレーム、Pawnをターゲットの方向に回転させる
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// Pawnがターゲットに対して十分な角度精度に達しているかを判定する
	bool HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const;

protected:
	// 回転角度の許容誤差（°）
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision;

	// 回転補間速度
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed;

	// ターゲットのブラックボードキー
	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetToFaceKey;
};


