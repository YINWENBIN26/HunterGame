// Fill out your copyright notice in the Description page of Project Settings.


#include "WarTypes/RotateLatentAction.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

/**
 * @brief 遅延回転アクションのコンストラクタ
 *
 * 対象アクター、回転モード、速度、許容誤差、ラテントアクション情報を設定する
 */
FRotateLatentAction::FRotateLatentAction(AActor* InActorToRotate, ERotateTargetType InMode, float InRotationSpeed, float InTolerance, const FLatentActionInfo& LatentInfo)
    : bNeedToCancel(false)
    , ActorToRotate(InActorToRotate)
    , RotateMode(InMode)
    , RotationSpeed(InRotationSpeed)
    , Tolerance(InTolerance)
    , ExecutionFunction(LatentInfo.ExecutionFunction)
    , OutputLink(LatentInfo.Linkage)
    , CallbackTarget(LatentInfo.CallbackTarget)
{
}

/**
 * @brief アクションのキャンセル処理
 */
void FRotateLatentAction::CancelAction()
{
    bNeedToCancel = true;
}

/**
 * @brief 遅延回転アクションの更新処理
 *
 * 対象アクターの現在の回転と目標回転を補間し、許容誤差内に収まればアクションを完了する
 */
void FRotateLatentAction::UpdateOperation(FLatentResponse& Response)
{
    // キャンセルされたか、対象が無効ならアクションを終了
    if (bNeedToCancel || !ActorToRotate.IsValid())
    {
        Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
        return;
    }

    AActor* MyActor = ActorToRotate.Get();
    FRotator CurrentRotation = MyActor->GetActorRotation();
    FRotator DesiredRotation = CurrentRotation; 

    // 回転モードに応じた目標回転を計算
    switch (RotateMode)
    {
    case ERotateTargetType::Location:
    {
        FVector Direction = TargetLocation - MyActor->GetActorLocation();
        if (Direction.IsNearlyZero())
        {
            return;
        }
        DesiredRotation = Direction.Rotation();
    }
    break;
    case ERotateTargetType::Actor:
    {
        if (!TargetActor.IsValid())
        {
            Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
            return;
        }
        AActor* Target = TargetActor.Get();
        FVector Direction = Target->GetActorLocation() - MyActor->GetActorLocation();
        if (Direction.IsNearlyZero())
        {
            Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
            return;
        }
        DesiredRotation = Direction.Rotation();
    }
    break;
    case ERotateTargetType::Rotation:
    {
        DesiredRotation = TargetRotation;
    }
    break;
    default:
        break;
    }

    float DeltaTime = Response.ElapsedTime();
    // 現在の回転から目標回転へ滑らかに補間
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, RotationSpeed);
    MyActor->SetActorRotation(NewRotation);
    
    float YawDiff = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(NewRotation, DesiredRotation).Yaw);
    if (YawDiff <= Tolerance)
    {
        // 目標回転に最終設定してアクションを終了
        MyActor->SetActorRotation(DesiredRotation);
        Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
    }
}
