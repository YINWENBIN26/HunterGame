#include "AI/Task/BTTask_RotatorToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotatorToFaceTarget::UBTTask_RotatorToFaceTarget()
{
	NodeName = TEXT("Native Rotate to Face Target Actor");
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;

	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	InTargetToFaceKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(ThisClass,InTargetToFaceKey),AActor::StaticClass());
}

/**
 * @brief ブラックボードからターゲットキーの情報を解決する
 */
void UBTTask_RotatorToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

/**
 * @brief このタスクが使用するインスタンスメモリのサイズを返す
 */
uint16 UBTTask_RotatorToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotatorToFaceTargetTaskMemory);
}

FString UBTTask_RotatorToFaceTarget::GetStaticDescription() const
{	
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision: %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

/**
 * @brief タスク実行開始時に、ターゲットとPawnの情報を設定する
 */
EBTNodeResult::Type UBTTask_RotatorToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// ブラックボードからターゲットアクターを取得
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	// AIのPawnを取得
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	// タスク用メモリに情報を設定
	FRotatorToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	// 有効な情報がなければ失敗
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	// 既に十分な角度精度に達していれば成功
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

/**
 * @brief 毎フレーム、Pawnをターゲットの方向にスムーズに回転させる
 */
void UBTTask_RotatorToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotatorToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);

	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	// 目標の角度精度に達していればタスクを終了
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		// ターゲットの方向を向く回転を計算して補間する
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

/**
 * @brief Pawnがターゲットに対して十分な角度精度に達しているかを判定する
 */
bool UBTTask_RotatorToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();
	const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);
	return AngleDiff <= AnglePrecision;
}
