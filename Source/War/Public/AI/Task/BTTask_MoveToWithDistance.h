#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToWithDistance.generated.h"

/**
 * 自定义 MoveTo 任务：当 AI 与目标之间的距离小于设定阈值时，
 * 提前结束任务并返回 Succeeded
 */
UCLASS()
class WAR_API UBTTask_MoveToWithDistance : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	// コンストラクタ：距離に応じた移動タスクの初期化を行う
	UBTTask_MoveToWithDistance(const FObjectInitializer& ObjectInitializer);

protected:
	// 距離がこの値未満の場合、タスクは成功として終了する
	UPROPERTY(EditAnywhere, Category = "Custom")
	float DistanceThreshold = 500.f;
    
	// 毎フレーム呼ばれ、ターゲットまでの距離をチェックして移動処理を実行する
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

