#include "AI/Task/BTTask_MoveToWithDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToWithDistance::UBTTask_MoveToWithDistance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// ノード名の設定
	NodeName = "Move To With Distance";
	bNotifyTick = true;
	DistanceThreshold = 500.f;
}

void UBTTask_MoveToWithDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* OwningPawn = AIController->GetPawn();

	// 現在のタスク設定に基づき、ブラックボードキーからターゲット位置を取得（ActorまたはVector対応）
	FVector TargetLocation = FVector::ZeroVector;
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		if (AActor* TargetActor = Cast<AActor>(KeyValue))
		{
			TargetLocation = TargetActor->GetActorLocation();
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	// AI Pawnとターゲット間の距離を計算
	const float CurrentDistance = FVector::Dist(OwningPawn->GetActorLocation(), TargetLocation);
	if (CurrentDistance < DistanceThreshold)
	{
		// 距離が閾値未満の場合、現在の移動要求（あれば）を中断して停止
		if (AIController->GetPathFollowingComponent())
		{
			AIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::Blocked);
			AIController->StopMovement();
			OwningPawn->GetMovementComponent()->StopMovementImmediately();
		}
		// 移動速度のログ出力（デバッグ用）
		float Speed = OwningPawn->GetVelocity().Size2D();
		UE_LOG(LogTemp, Display, TEXT("GroundSpeed is %f"), Speed);
		// タスクを終了し、Succeededを返す
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 閾値未満でない場合は、親クラスのTickTaskで移動ロジックを継続実行
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
