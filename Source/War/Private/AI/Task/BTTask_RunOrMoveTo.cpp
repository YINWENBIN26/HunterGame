// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_RunOrMoveTo.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "GameFramework/MovementComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Actors/WarSplineActor.h"
#include "Navigation/PathFollowingComponent.h"


UBTTask_RunOrMoveTo::UBTTask_RunOrMoveTo(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Run Or Move To  ");
    bNotifyTick = true;
}

/**
 * @brief タスクインスタンス用のメモリサイズを返す
 */
uint16 UBTTask_RunOrMoveTo::GetInstanceMemorySize() const
{
    return sizeof(FRunOrMoveToMemory);
}

EBTNodeResult::Type UBTTask_RunOrMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // タスク用メモリの初期化
    FRunOrMoveToMemory* Memory = CastInstanceNodeMemory<FRunOrMoveToMemory>(NodeMemory);
    if (!Memory)
    {
        return EBTNodeResult::Failed;
    }
    Memory->Reset();

    // ブラックボードからターゲット位置を取得
    FVector TargetPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);

    // AIコントローラーとPawnを取得
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }
    
    // メモリに各種情報を保存
    Memory->AIController   = AIController;
    Memory->OwningPawn     = Pawn;
    Memory->TargetLocation = TargetPos;
    if (!Memory->IsValid())
    {
        return EBTNodeResult::Failed;
    }

    // 目標までの距離が極端に短い場合、移動不要としてタスク成功とする
    if (FVector::Dist(Pawn->GetActorLocation(), TargetPos) < 30.f)
    {
        Memory->Reset();
        return EBTNodeResult::Succeeded;
    }

    // Pawnの前方とターゲット方向の角度を計算
    float AngleDeg = ComputeAngleBetween(Pawn, TargetPos);
    UE_LOG(LogTemp, Display, TEXT("Angle: %f"), AngleDeg);
    
    // 角度が閾値より大きい場合、Spline移動を使用する
    Memory->bUseSpline = (AngleDeg > AngleThreshold);

    // Spline移動モードの場合はSplineのセットアップ、そうでなければ直接移動を実行
    if (Memory->bUseSpline)
    {
        return SetupSplineMovement(Memory);
    }
    else
    {
        FAIMoveRequest MoveReq;
        MoveReq.SetGoalLocation(TargetPos);
        MoveReq.SetAcceptanceRadius(50.f);
        AIController->MoveTo(MoveReq);
        return EBTNodeResult::InProgress;
    }
}

/**
 * @brief Pawnの前向きと目標方向の角度を計算（度単位）
 */
float UBTTask_RunOrMoveTo::ComputeAngleBetween(APawn* Pawn, const FVector& TargetPos) const
{
    FVector Forward = Pawn->GetActorForwardVector();
    FVector ToTarget = TargetPos - Pawn->GetActorLocation();
    Forward.Z = 0.f;
    ToTarget.Z = 0.f;
    float DotVal = FVector::DotProduct(Forward.GetSafeNormal(), ToTarget.GetSafeNormal());
    DotVal = FMath::Clamp(DotVal, -1.f, 1.f);
    return FMath::RadiansToDegrees(FMath::Acos(DotVal));
}

/**
 * @brief Spline移動モードの初期設定
 *
 * Splineアクターを生成し、Splineの制御点を設定する。
 */
EBTNodeResult::Type UBTTask_RunOrMoveTo::SetupSplineMovement(FRunOrMoveToMemory* Memory)
{
    APawn* Pawn = Memory->OwningPawn.Get();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }
    UWorld* World = Pawn->GetWorld();
    if (!World)
    {
        return EBTNodeResult::Failed;
    }
    
    // Splineアクターをスポーンする
    AWarSplineActor* SplineActor = World->SpawnActor<AWarSplineActor>(
        AWarSplineActor::StaticClass(),
        Pawn->GetActorLocation(),
        Pawn->GetActorRotation()
    );
    if (!SplineActor)
    {
        return EBTNodeResult::Failed;
    }

    Memory->SplineActor = SplineActor;
    Memory->CurrentDistance = 0.f;
    Memory->bReachedDestination = false;

    if (USplineComponent* SplineComp = SplineActor->GetSplineComp())
    {
        SplineComp->ClearSplinePoints(false);
        SetupSplinePoints(Pawn, Memory->TargetLocation, SplineComp);
        SplineComp->UpdateSpline();
    }
    return EBTNodeResult::InProgress;
}

/**
 * @brief Splineの制御点を設定する
 *
 * 始点、中間点、終点を計算し、Splineに設定する。
 */
void UBTTask_RunOrMoveTo::SetupSplinePoints(APawn* OwningPawn, const FVector& TargetPos, USplineComponent* SplineComp)
{
    // 始点位置の取得（キャラクターのメッシュ位置優先）
    FVector StartPos = OwningPawn->GetActorLocation();
    if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
    {
        if (USkeletalMeshComponent* MeshComp = Character->GetMesh())
        {
            StartPos = MeshComp->GetComponentLocation();
        }
    }
    FVector EndPos = TargetPos;
    FVector StartDirection = OwningPawn->GetActorForwardVector().GetSafeNormal();

    // 移動速度に基づくオフセット距離の計算
    float OffsetDistance = RunSpeed;
    if (UMovementComponent* MoveCom = OwningPawn->GetMovementComponent())
    {
        if (RunSpeed < MoveCom->Velocity.Size())
        {
            OffsetDistance = MoveCom->Velocity.Size();
        }
    }
    FVector DesiredDirection = (EndPos - StartPos).GetSafeNormal();
    float AngleDiff = FMath::Acos(FVector::DotProduct(StartDirection, DesiredDirection));
    float OffsetMultiplier = FMath::Sin(AngleDiff * 0.5f);
    FVector PerpOffset = (DesiredDirection - (FVector::DotProduct(DesiredDirection, StartDirection) * StartDirection)).GetSafeNormal();
    FVector MidPos = StartPos + StartDirection * OffsetDistance + PerpOffset * (OffsetDistance * OffsetMultiplier);

    // Splineに3つの制御点を設定する
    SplineComp->AddSplinePoint(StartPos, ESplineCoordinateSpace::World, false);
    SplineComp->AddSplinePoint(MidPos, ESplineCoordinateSpace::World, false);
    SplineComp->AddSplinePoint(EndPos, ESplineCoordinateSpace::World, false);
    SplineComp->SetSplinePointType(0, ESplinePointType::Curve, false);
    SplineComp->SetSplinePointType(1, ESplinePointType::Curve, false);
    SplineComp->SetSplinePointType(2, ESplinePointType::Curve, false);
    FVector StartTangent = StartDirection * OffsetDistance;
    SplineComp->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
}

/**
 * @brief 毎フレームのタスク処理
 *
 * Spline移動モードの場合はSpline上の位置と回転を更新し、直接移動モードの場合はMoveToの結果を監視する。
 */
void UBTTask_RunOrMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FRunOrMoveToMemory* Memory = CastInstanceNodeMemory<FRunOrMoveToMemory>(NodeMemory);
    if (!Memory || !Memory->IsValid())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AAIController* AIController = Memory->AIController.Get();
    APawn* OwningPawn = Memory->OwningPawn.Get();
    if (!AIController || !OwningPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 目的地に到達していればタスク終了
    if (Memory->bReachedDestination)
    {
        OnTaskFinished(OwnerComp, Memory);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Spline移動モードの場合
    if (Memory->bUseSpline && Memory->SplineActor.IsValid())
    {
        UpdateSplineMovement(OwningPawn, Memory, DeltaSeconds, OwnerComp);
    }
    else
    {
        // 直接移動モードの場合：MoveToの状態を確認して終了を判断する
        if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
        {
            float Dist = FVector::Dist(OwningPawn->GetActorLocation(), Memory->TargetLocation);
            if (Dist < 200.f)
            {
                OnTaskFinished(OwnerComp, Memory);
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
            else
            {
                OnTaskFinished(OwnerComp, Memory);
                FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
            }
        }
    }
}

/**
 * @brief Spline移動モードの更新処理
 *
 * 現在の速度に基づいてSpline上の進行距離を更新し、Pawnの位置と回転を補間で調整する。
 */
void UBTTask_RunOrMoveTo::UpdateSplineMovement(APawn* OwningPawn, FRunOrMoveToMemory* Memory, float DeltaSeconds, UBehaviorTreeComponent& OwnerComp)
{
    AWarSplineActor* SplineActor = Cast<AWarSplineActor>(Memory->SplineActor.Get());
    if (!SplineActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    USplineComponent* SplineComp = SplineActor->GetSplineComp();
    if (!SplineComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 移動コンポーネントから現在の速度を取得し、進行距離を更新
    if (UMovementComponent* MoveCom = OwningPawn->GetMovementComponent())
    {
        float CurrentSpeed = MoveCom->Velocity.Size();
        Memory->CurrentDistance += CurrentSpeed * DeltaSeconds;
    }
    float SplineLength = SplineComp->GetSplineLength();
    if (Memory->CurrentDistance >= SplineLength)
    {
        Memory->CurrentDistance = SplineLength;
    }

    // Spline上の現在位置と回転を取得
    FVector SplinePos = SplineComp->GetLocationAtDistanceAlongSpline(Memory->CurrentDistance, ESplineCoordinateSpace::World);
    FRotator SplineRot = SplineComp->GetRotationAtDistanceAlongSpline(Memory->CurrentDistance, ESplineCoordinateSpace::World);
    float DistToEnd = FVector::Dist(SplinePos, Memory->TargetLocation);
    if (DistToEnd < 50.f || Memory->CurrentDistance >= SplineLength)
    {
        Memory->bReachedDestination = true;
        OnTaskFinished(OwnerComp, Memory);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 回転補間でPawnの向きをSplineに合わせる
    FRotator CurrentRot = OwningPawn->GetActorRotation();
    FRotator DesiredRot = FMath::RInterpTo(CurrentRot, SplineRot, DeltaSeconds, RotationInterpSpeed);
    OwningPawn->SetActorRotation(DesiredRot);
    
    // キャラクターの最大歩行速度を更新し、前進入力を追加する
    if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
    {
        if (Character->GetCharacterMovement())
        {
            Character->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
        }
    }
    OwningPawn->AddMovementInput(OwningPawn->GetActorForwardVector(), 1.0f);
}

/**
 * @brief タスク終了時の後処理
 *
 * Splineアクターが存在すれば破棄し、メモリをリセットする。
 */
void UBTTask_RunOrMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, FRunOrMoveToMemory* Memory)
{
    if (Memory->SplineActor.IsValid())
    {
        if (AActor* SplineActor = Memory->SplineActor.Get())
        {
            SplineActor->Destroy();
        }
    }
    Memory->Reset();
}