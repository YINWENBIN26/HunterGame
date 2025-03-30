// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_CircleAround.h"
#include "AIController.h"
#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UUBTTask_CircleAround::UUBTTask_CircleAround(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("Circle Around Target ");
	bNotifyTick = true;
}

EBTNodeResult::Type UUBTTask_CircleAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	// キャラクターの初期移動速度を設定
	if (ACharacter* MyCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetOwner()))
	{
		if (UCharacterMovementComponent* MovementComponent = MyCharacter->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = MoveSpeed;
		}
	}

	// タスクメモリの初期化
	FCircleAroundMemory* Memory = CastInstanceNodeMemory<FCircleAroundMemory>(NodeMemory);
	Memory->TimeSinceClose = 0.f;

	return EBTNodeResult::InProgress;
}

void UUBTTask_CircleAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 必要なコンポーネントの取得
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// タスクメモリの取得
	FCircleAroundMemory* Memory = CastInstanceNodeMemory<FCircleAroundMemory>(NodeMemory);

	// 移動パラメータの計算（前方ベクトルと必要な速度を取得）
	FVector DesiredForward;
	float RequiredMoveSpeed;
	CalculateMovementParameters(OwningPawn, TargetActor->GetActorLocation(), DeltaSeconds, Memory, DesiredForward, RequiredMoveSpeed);

	// Pawnの回転を平滑に更新
	const FVector UpVector = FVector::UpVector;
	const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromXZ(DesiredForward, UpVector);
	const FRotator CurrentRotation = OwningPawn->GetActorRotation();
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpSpeed);
	OwningPawn->SetActorRotation(NewRotation);

	// 前方への移動入力
	const FVector Forward = OwningPawn->GetActorForwardVector();
	if (ACharacter* MyChar = Cast<ACharacter>(OwningPawn))
	{
		if (UCharacterMovementComponent* MovementComponent = MyChar->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = RequiredMoveSpeed;
		}
		MyChar->AddMovementInput(Forward, RequiredMoveSpeed * DeltaSeconds);
	}
}

void UUBTTask_CircleAround::CalculateMovementParameters(APawn* Pawn, const FVector& TargetLocation, float DeltaSeconds, FCircleAroundMemory* Memory, FVector& OutDesiredForward, float& OutRequiredMoveSpeed) const
{
	// Pawnとターゲット間の距離と方向を計算
	const FVector PawnLocation = Pawn->GetActorLocation();
	FVector ToPawn = PawnLocation - TargetLocation;
	const float CurrentDistance = ToPawn.Size();

	// 水平面上のベクトルに変換
	ToPawn.Z = 0.f;
	const FVector UpVector = FVector::UpVector;
	
	// 逆時計回りの接線方向を計算
	const FVector TangentDir = FVector::CrossProduct(UpVector, ToPawn).GetSafeNormal();

	// 希望半径との差分と誤差判定
	const float DistanceDiff = CurrentDistance - DesiredCircleRadius;
	const bool bWithinRange = FMath::Abs(DistanceDiff) <= AcceptableRadiusError;

	// デフォルトは接線方向
	FVector MoveDirection = TangentDir;
	const FVector OutwardDir = ToPawn.GetSafeNormal();

	// ターゲットに近い場合、時間を累積
	if (CurrentDistance < DesiredCircleRadius)
	{
		Memory->TimeSinceClose += DeltaSeconds;
	}
	else
	{
		Memory->TimeSinceClose = 0.f;
	}
	const bool bShouldAdjustOutward = (Memory->TimeSinceClose > AdjustOutwardDelay);
	float RequiredSpeed = MoveSpeed;

	// 許容範囲外なら径方向の補正を行う
	if (!bWithinRange)
	{
		FVector AdjustDir = FVector::ZeroVector;
		if (DistanceDiff < 0.f)  // 近すぎる場合
		{
			AdjustDir = bShouldAdjustOutward ? OutwardDir : FVector::ZeroVector;
			RequiredSpeed = MoveSpeed + AdjustDistanceSpeed;
		}
		else  // 遠すぎる場合
		{
			AdjustDir = -OutwardDir;
			RequiredSpeed = MoveSpeed + AdjustDistanceSpeed;
		}
		// 接線方向と径方向を加重平均
		MoveDirection = (TangentDir * WeightTangent + AdjustDir * WeightRadial).GetSafeNormal();
	}
	OutDesiredForward = MoveDirection;
	OutRequiredMoveSpeed = RequiredSpeed;
}