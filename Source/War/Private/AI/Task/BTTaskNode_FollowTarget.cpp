// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_FollowTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" // 若需要修改 CharacterMovement
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

UBTTaskNode_FollowTarget::UBTTaskNode_FollowTarget()
{
    NodeName = TEXT("Follow Target");

    // デフォルトの距離パラメータ
    RunDistanceThreshold = 600.f;   // 600以上なら走る
    WalkDistanceThreshold = 300.f;  // 300未満なら歩く
    StopFollowDistance    = 150.f;  // 150以下なら停止と判断

    // Tick更新とタスク終了通知を有効にする
    bNotifyTick = true;
    bNotifyTaskFinished = true;
    bCreateNodeInstance = false;

    // ブラックボードキーはActor型のみを許容する
    TargetToFollowKey.AddObjectFilter(
        this,
        GET_MEMBER_NAME_CHECKED(UBTTaskNode_FollowTarget, TargetToFollowKey),
        AActor::StaticClass()
    );
}

void UBTTaskNode_FollowTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        TargetToFollowKey.ResolveSelectedKey(*BBAsset);
    }
}

uint16 UBTTaskNode_FollowTarget::GetInstanceMemorySize() const
{
    return sizeof(FFollowTargetTaskMemory);
}

FString UBTTaskNode_FollowTarget::GetStaticDescription() const
{
    return FString::Printf(
        TEXT("目標まで %f 以下になるまでフォローします。\n距離が %f 超なら走り、%f 未満なら歩きます。"),
        StopFollowDistance,
        RunDistanceThreshold,
        WalkDistanceThreshold
    );
}

/**
 * @brief タスク開始時の処理
 *
 * ブラックボードからフォロー対象のActorを取得し、AIのPawnとターゲットをタスクメモリに保存する。
 * すでに停止距離に到達していれば、タスクを成功として終了する。
 */
EBTNodeResult::Type UBTTaskNode_FollowTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // ブラックボードから対象Actorを取得
    UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetToFollowKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }
    
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* OwningPawn = AIController->GetPawn();

    // タスクメモリに必要な情報を保存
    FFollowTargetTaskMemory* Memory = CastInstanceNodeMemory<FFollowTargetTaskMemory>(NodeMemory);
    Memory->OwningPawn = OwningPawn;
    Memory->TargetActor = TargetActor;
    if (!Memory->IsValid())
    {
        return EBTNodeResult::Failed;
    }

    // Pawnとターゲット間の距離を計算し、停止距離に到達していればタスク成功
    float DistanceToTarget = FVector::Distance(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
    if (HasReachedStopDistance(DistanceToTarget))
    {
        Memory->Reset();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::InProgress;
}

/**
 * @brief 毎フレーム呼ばれるタスク処理
 *
 * ターゲットの後方オフセット位置へ移動し、到達判定および移動速度の更新を行う。
 */
void UBTTaskNode_FollowTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FFollowTargetTaskMemory* Memory = CastInstanceNodeMemory<FFollowTargetTaskMemory>(NodeMemory);
    if (!Memory || !Memory->IsValid())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* Pawn = Memory->OwningPawn.Get();
    AActor* Target = Memory->TargetActor.Get();
    
    if (ShouldSkipTick(AIController, Pawn))
    {
        return;
    }

    // ターゲット後方のオフセット位置を計算
    FVector DesiredLocation = Target->GetActorLocation() + GetFollowOffset(Pawn, Target);

    // NavMesh上の有効な位置に投影する
    FVector FinalMoveLocation;
    if (!ProjectLocationToNavMesh(DesiredLocation, FinalMoveLocation))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // Pawnとの距離を計算し、到達していれば移動を停止
    float DistanceToOffset = FVector::Distance(Pawn->GetActorLocation(), FinalMoveLocation);
    if (DistanceToOffset <= StopFollowDistance)
    {
        AIController->StopMovement();
        Memory->Reset();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // デバッグ表示
    if (bInDebug)
    {
        DrawDebugSphere(GetWorld(), DesiredLocation, 25.f, 12, FColor::Red, false, 0.f);
        DrawDebugSphere(GetWorld(), FinalMoveLocation, 25.f, 12, FColor::Green, false, 0.f);
        FString DebugStr = FString::Printf(
            TEXT("距離: %.1f, Desired: (%.1f,%.1f,%.1f), Final: (%.1f,%.1f,%.1f)"),
            DistanceToOffset,
            DesiredLocation.X, DesiredLocation.Y, DesiredLocation.Z,
            FinalMoveLocation.X, FinalMoveLocation.Y, FinalMoveLocation.Z
        );
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, DebugStr);
        }
    }

    // 移動速度を更新する
    UpdateMovementSpeed(Pawn, DistanceToOffset, Memory);

    // MoveToリクエストを発行する
    FAIMoveRequest MoveReq;
    MoveReq.SetGoalLocation(FinalMoveLocation);
    MoveReq.SetAcceptanceRadius(StopFollowDistance);
    AIController->MoveTo(MoveReq);
}

/**
 * @brief ターゲット後方のオフセットを計算する
 *
 * ターゲットの前方・右方向の情報から、後方のオフセットベクトルを返す。
 */
FVector UBTTaskNode_FollowTarget::GetFollowOffset(APawn* Pawn, AActor* Target) const
{
    const FVector TargetForward = Target->GetActorForwardVector();
    const FVector TargetRight = Target->GetActorRightVector();
    FVector Offset = -TargetForward * BehindDistance;

    // Pawnのタグに応じて左右オフセットを調整する
    if (UWarFunctionLibrary::Native_DoesActorHaveTag(Pawn, WarGameplayTags::Enemy_Wolf_Status_FollowMoving_AtRight))
    {
        Offset += TargetRight * SideOffset;
    }
    else if (UWarFunctionLibrary::Native_DoesActorHaveTag(Pawn, WarGameplayTags::Enemy_Wolf_Status_FollowMoving_AtLeft))
    {
        Offset -= TargetRight * SideOffset;
    }
    return Offset;
}

/**
 * @brief 指定位置をNavMesh上の有効な位置に投影する
 */
bool UBTTaskNode_FollowTarget::ProjectLocationToNavMesh(const FVector& InLocation, FVector& OutLocation) const
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        return false;
    }

    FNavLocation ProjectedLocation;
    FVector SearchExtent(50.f, 50.f, 200.f);
    bool bProjected = NavSys->ProjectPointToNavigation(InLocation, ProjectedLocation, SearchExtent);
    if (bProjected)
    {
        OutLocation = ProjectedLocation.Location;
    }
    else if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("DesiredLocationの近くに有効な位置がありません！"));
    }
    return bProjected;
}

/**
 * @brief 移動中ならTick処理をスキップするか判定する
 */
bool UBTTaskNode_FollowTarget::ShouldSkipTick(AAIController* AIController, APawn* Pawn) const
{
    if (!AIController || !Pawn) return true;
    FVector CurrentTarget = AIController->GetPathFollowingComponent()->GetCurrentTargetLocation();
    return (FVector::Distance(CurrentTarget, Pawn->GetActorLocation()) > 200.f &&
            AIController->GetMoveStatus() == EPathFollowingStatus::Moving);
}

/**
 * @brief Pawnの移動速度を距離に応じて更新する
 *
 * 距離が大きければ走り、小さければ歩く（または停止）ように状態を切り替える。
 */
void UBTTaskNode_FollowTarget::UpdateMovementSpeed(APawn* Pawn, float DistanceToTarget, FFollowTargetTaskMemory* Memory)
{
    ACharacter* Character = Cast<ACharacter>(Pawn);
    if (!Character) return;

    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (!MovementComp) return;

    EEnemyMovementState NewState = Memory->LastMovementState;
    if (DistanceToTarget > RunDistanceThreshold)
    {
        MovementComp->MaxWalkSpeed = RunSpeed;
        NewState = EEnemyMovementState::Running;
    }
    else if (DistanceToTarget < WalkDistanceThreshold)
    {
        MovementComp->MaxWalkSpeed = WalkSpeed;
        NewState = EEnemyMovementState::Walking;
    }

    if (NewState != Memory->LastMovementState)
    {
        if (NewState == EEnemyMovementState::Running)
        {
            UWarFunctionLibrary::AddGameplayTagToActorIfNone(Pawn, WarGameplayTags::Enemy_Status_Run);
        }
        else
        {
            UWarFunctionLibrary::RemoveGameplayTagToActorIfFound(Pawn, WarGameplayTags::Enemy_Status_Run);
        }
        Memory->LastMovementState = NewState;
    }
}

/**
 * @brief 指定距離に到達しているか判定する
 */
bool UBTTaskNode_FollowTarget::HasReachedStopDistance(float DistanceToTarget) const
{
    return DistanceToTarget <= StopFollowDistance;
}

/**
 * @brief タスク終了時の後処理
 *
 * タスク終了時にタスクメモリをリセットする。
 */
void UBTTaskNode_FollowTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, FFollowTargetTaskMemory* Memory)
{
    Memory->Reset();
}

