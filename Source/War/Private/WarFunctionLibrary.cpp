// Fill out your copyright notice in the Description page of Project Settings.


#include "WarFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "Interface/Combat/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarGameplayTags.h"
#include "WarGameInstance.h"
#include "WarTypes/RotateLatentAction.h"
#include "WarTypes/WarCountDownAction.h"

/**
 * @brief アクターから UWarAbilitySystemComponent を取得する
 */
UWarAbilitySystemComponent* UWarFunctionLibrary::Native_GetWarAbilitySystemComponentFromActor(AActor* InActor)
{
	check(InActor);
	return CastChecked<UWarAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

/**
 * @brief アクターに指定タグが存在しなければ追加する
 */
void UWarFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarAbilitySystemComponent* ASC = Native_GetWarAbilitySystemComponentFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

/**
 * @brief 複数のアクターに指定タグを追加する
 */
void UWarFunctionLibrary::AddGameplayTagToActors(const TArray<AActor*>& Actors, FGameplayTag TagToAdd)
{
	for (AActor* Actor : Actors)
	{
		UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(TagToAdd);
		}
	}
}

/**
 * @brief アクターに指定タグが存在すれば削除する
 */
void UWarFunctionLibrary::RemoveGameplayTagToActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarAbilitySystemComponent* ASC = Native_GetWarAbilitySystemComponentFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

/**
 * @brief 複数のアクターから指定タグを削除する
 */
void UWarFunctionLibrary::RemoveGameplayTagFromActors(const TArray<AActor*>& Actors, FGameplayTag TagToRemove)
{
	for (AActor* Actor : Actors)
	{
		UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(TagToRemove);
		}
	}
}

/**
 * @brief アクターが指定タグを持っているか判定する
 */
bool UWarFunctionLibrary::Native_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarAbilitySystemComponent* ASC = Native_GetWarAbilitySystemComponentFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

/**
 * @brief アクターが指定タグを持っているかを EWarConfirmType で返す
 */
void UWarFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarConfirmType& OutConfirmType)
{
	OutConfirmType = Native_DoesActorHaveTag(InActor, TagToCheck) ? EWarConfirmType::Yes : EWarConfirmType::No;
}

/**
 * @brief アクターから PawnCombatComponent を取得する
 */
UPawnCombatComponent* UWarFunctionLibrary::Native_GetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	if (const IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}

/**
 * @brief アクターから PawnCombatComponent を取得し、有効性を返す
 */
UPawnCombatComponent* UWarFunctionLibrary::BP_GetPawnCombatComponent(AActor* InActor, EWarValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = Native_GetPawnCombatComponentFromActor(InActor);
	OutValidType = CombatComponent ? EWarValidType::Valid : EWarValidType::Invalid;
	return CombatComponent;
}

/**
 * @brief 指定された2つの Pawn の敵対関係を判定する
 */
bool UWarFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

/**
 * @brief FScalableFloat の指定レベルでの値を取得する
 */
float UWarFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

/**
 * @brief 攻撃者と被攻撃者の位置関係からヒット反応の方向タグを計算する
 *
 * 被攻撃者の前方ベクトルと攻撃者との相対方向から角度を算出し、
 * 角度に応じた方向タグを返す
 */
FGameplayTag UWarFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);
	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	// 前方ベクトルとの内積から角度を計算
	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	// 外積で左右を判定し、角度の符号を反転する
	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.0f;
	}

	// 角度に応じて適切なヒット反応方向タグを返す
	if (OutAngleDifference > -45.f && OutAngleDifference <= 45.f)
	{
		return WarGameplayTags::Shared_Status_MeleeHit_Front;
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return WarGameplayTags::Shared_Status_MeleeHit_Left;
	}
	else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
	{
		return WarGameplayTags::Shared_Status_MeleeHit_Back;
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return WarGameplayTags::Shared_Status_MeleeHit_Right;
	}
	return WarGameplayTags::Shared_Status_MeleeHit_Front;
}

/**
 * @brief ゲームプレイエフェクトをターゲットに適用する
 */
bool UWarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarAbilitySystemComponent* SourceASC = Native_GetWarAbilitySystemComponentFromActor(InInstigator);
	UWarAbilitySystemComponent* TargetASC = Native_GetWarAbilitySystemComponentFromActor(InTargetActor);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

/**
 * @brief 複数のアクターにゲームプレイエフェクトを適用する
 */
bool UWarFunctionLibrary::ApplyGameplayEffectToActors(AActor* InInstigator, const TArray<AActor*>& TargetActors, const FGameplayEffectSpecHandle& InSpecHandle)
{
	bool bAllAppliedSuccessfully = true;
	for (AActor* TargetActor : TargetActors)
	{
		// 既存の ApplyGameplayEffectSpecHandleToTargetActor を呼び出す
		if (!ApplyGameplayEffectSpecHandleToTargetActor(InInstigator, TargetActor, InSpecHandle))
		{
			bAllAppliedSuccessfully = false;
		}
	}
	return bAllAppliedSuccessfully;
}

/**
 * @brief アクターを目標位置に向けてスムーズに回転させる
 */
void UWarFunctionLibrary::RotateToFaceTargetLatent_ByLocation(UObject* WorldContextObject, AActor* ActorToRotate, FVector TargetLocation, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FRotateLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FRotateLatentAction* NewAction = new FRotateLatentAction(ActorToRotate, ERotateTargetType::Location, RotationSpeed, Tolerance, LatentInfo);
			NewAction->SetTargetLocation(TargetLocation);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}

/**
 * @brief アクターを目標アクターに向けてスムーズに回転させる
 */
void UWarFunctionLibrary::RotateToFaceTargetLatent_ByActor(UObject* WorldContextObject, AActor* ActorToRotate, AActor* TargetActor, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FRotateLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FRotateLatentAction* NewAction = new FRotateLatentAction(ActorToRotate, ERotateTargetType::Actor, RotationSpeed, Tolerance, LatentInfo);
			NewAction->SetTargetActor(TargetActor);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}

/**
 * @brief アクターを指定の回転に向けてスムーズに回転させる
 */
void UWarFunctionLibrary::RotateToFaceTargetLatent_ByRotation(UObject* WorldContextObject, AActor* ActorToRotate, FRotator TargetRotation, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FRotateLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FRotateLatentAction* NewAction = new FRotateLatentAction(ActorToRotate, ERotateTargetType::Rotation, RotationSpeed, Tolerance, LatentInfo);
			NewAction->SetTargetRotation(TargetRotation);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}

/**
 * @brief カウントダウン処理を実行する
 *
 * 指定時間内で更新間隔ごとに残り時間を更新し、入力に応じて開始またはキャンセルする
 */
void UWarFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterVal, float& OutRemainingTime, EWarCountDownActionInputType CountDownInput, EWarCountDownActionOutPutType& CountDownOutput, FLatentActionInfo LatentActionInfo)
{
	UWorld* World = nullptr;
	if (GEngine)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	if (!World)
	{
		return;
	}
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FWarCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarCountDownAction>(LatentActionInfo.CallbackTarget, LatentActionInfo.UUID);
	if (CountDownInput == EWarCountDownActionInputType::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentActionInfo.CallbackTarget, LatentActionInfo.UUID, new FWarCountDownAction(TotalTime, UpdateInterVal, OutRemainingTime, CountDownOutput, LatentActionInfo));
		}
	}
	if (CountDownInput == EWarCountDownActionInputType::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}

/**
 * @brief アクター配列をターゲットに対して左右に分割する
 *
 * ターゲットの位置・方向に基づいて、入力アクター群を左右に分類する
 */
void UWarFunctionLibrary::DiffMoveDirectionArray(const TArray<AActor*>& Actors, const AActor* TargetActor, TArray<AActor*>& OutLeftActors, TArray<AActor*>& OutRightActors)
{
	OutLeftActors.Empty();
	OutRightActors.Empty();
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is null."));
		return;
	}

	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator TargetRotation = TargetActor->GetActorRotation();
	FVector ForwardVector = TargetRotation.Vector();
	FVector RightVector = UKismetMathLibrary::GetRightVector(TargetRotation);

	// 一時的に左右のアクター情報を格納する構造体
	struct FActorInfo
	{
		AActor* Actor;
		float Distance;
		float SideDistance; // 分界線からの距離
	};

	TArray<FActorInfo> LeftTemp;
	TArray<FActorInfo> RightTemp;

	for (AActor* Actor : Actors)
	{
		if (!Actor || Actor == TargetActor)
			continue;

		FVector ActorLocation = Actor->GetActorLocation();
		FVector Direction = ActorLocation - TargetLocation;
		float Distance = Direction.Size();

		// 右方向ベクトルとの内積で左右を判定
		float DotProduct = FVector::DotProduct(Direction, RightVector);

		FActorInfo ActorInfo;
		ActorInfo.Actor = Actor;
		ActorInfo.Distance = Distance;
		ActorInfo.SideDistance = FMath::Abs(DotProduct); // 絶対値で距離を算出

		if (DotProduct > 0)
		{
			RightTemp.Add(ActorInfo);
		}
		else
		{
			LeftTemp.Add(ActorInfo);
		}
	}

	// 距離の昇順にソート
	auto SortByDistance = [](const FActorInfo& A, const FActorInfo& B) -> bool
	{
		return A.Distance < B.Distance;
	};

	LeftTemp.Sort(SortByDistance);
	RightTemp.Sort(SortByDistance);

	int32 LeftCount = LeftTemp.Num();
	int32 RightCount = RightTemp.Num();
	int32 TotalActors = LeftCount + RightCount;

	// 期待する左右のアクター数を計算
	int32 DesiredLeft = TotalActors / 2;
	int32 DesiredRight = TotalActors / 2;

	// 奇数の場合は左側に1つ多く許容する
	if (TotalActors % 2 != 0)
	{
		if (LeftCount > RightCount)
			DesiredLeft += 1;
		else
			DesiredRight += 1;
	}

	// 実際の数に合わせて調整
	DesiredLeft = FMath::Min(DesiredLeft, LeftCount + RightCount);
	DesiredRight = FMath::Min(DesiredRight, LeftCount + RightCount);

	// 初期割り当て
	int32 FinalLeft = FMath::Min(DesiredLeft, LeftCount);
	int32 FinalRight = FMath::Min(DesiredRight, RightCount);

	// 初期割り当てされたアクターを追加
	for (int32 i = 0; i < FinalLeft; ++i)
	{
		OutLeftActors.Add(LeftTemp[i].Actor);
		if (GEngine)
		{
			FString DebugMessage = FString::Printf(TEXT("%s が左側に追加されました"), *LeftTemp[i].Actor->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
		}
	}

	for (int32 i = 0; i < FinalRight; ++i)
	{
		OutRightActors.Add(RightTemp[i].Actor);
		if (GEngine)
		{
			FString DebugMessage = FString::Printf(TEXT("%s が右側に追加されました"), *RightTemp[i].Actor->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, DebugMessage);
		}
	}

	// 現在の左右の差を計算
	int32 CurrentDifference = FMath::Abs(FinalLeft - FinalRight);

	// 差が1より大きい場合、調整を行う
	while (CurrentDifference > 1)
	{
		if (FinalLeft > FinalRight)
		{
			// 左側から分界線に近いアクターを右側へ移動
			TArray<FActorInfo> RemainingLeft = LeftTemp;
			for (AActor* Actor : OutLeftActors)
			{
				RemainingLeft.RemoveAll([Actor](const FActorInfo& Info) -> bool
				{
					return Info.Actor == Actor;
				});
			}

			if (RemainingLeft.Num() == 0)
				break;

			// 分界線からの距離が近い順にソート
			RemainingLeft.Sort([](const FActorInfo& A, const FActorInfo& B) -> bool
			{
				return A.SideDistance < B.SideDistance;
			});

			AActor* ActorToMove = RemainingLeft[0].Actor;
			OutLeftActors.Remove(ActorToMove);
			OutRightActors.Add(ActorToMove);
			FinalLeft--;
			FinalRight++;
		}
		else
		{
			// 右側から分界線に近いアクターを左側へ移動
			TArray<FActorInfo> RemainingRight = RightTemp;
			for (AActor* Actor : OutRightActors)
			{
				RemainingRight.RemoveAll([Actor](const FActorInfo& Info) -> bool
				{
					return Info.Actor == Actor;
				});
			}

			if (RemainingRight.Num() == 0)
				break;

			RemainingRight.Sort([](const FActorInfo& A, const FActorInfo& B) -> bool
			{
				return A.SideDistance < B.SideDistance;
			});

			AActor* ActorToMove = RemainingRight[0].Actor;
			OutRightActors.Remove(ActorToMove);
			OutLeftActors.Add(ActorToMove);
			FinalRight--;
			FinalLeft++;
		}

		CurrentDifference = FMath::Abs(FinalLeft - FinalRight);
	}
}

UWarGameInstance* UWarFunctionLibrary::GetWarGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameInstance<UWarGameInstance>();
		}
	}
 
	return nullptr;
}
void UWarFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;
 
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
	}
 
	if (!PlayerController)
	{
		return;
	}
 
	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;
 
	switch (InInputMode)
	{
	case EWarInputMode::GameOnly:
         
		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;
 
		break;
 
	case EWarInputMode::UIOnly:
 
		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;
 
		break;
 
	default:
		break;
	}
}




