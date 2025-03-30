// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Hero/HeroGameplayAbility_TargetLock.h"

#include "EnhancedInputSubsystems.h"
#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/WarHeroCharacter.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "Controllers/WarPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widgets/WarWidgetBase.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * @brief ターゲットロック能力を起動する。
 */
void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMappingContext();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

/**
 * @brief ターゲットロック能力を終了する。
 */
void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUpLockOnTarget();
	ResetTargetLockMappingContext();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/**
 * @brief ターゲットロック中の毎フレーム処理。
 */
void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	// ターゲットまたはプレイヤーが無効（または死亡）ならロック解除
	if (!CurrentLockedActor ||
	    UWarFunctionLibrary::Native_DoesActorHaveTag(CurrentLockedActor, WarGameplayTags::Shared_Status_Dead) ||
		UWarFunctionLibrary::Native_DoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarGameplayTags::Shared_Status_Dead))
	{
		CancelLockOnTarget();
		return;
	}
	// ターゲットロックウィジェットの位置更新
	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation = !UWarFunctionLibrary::Native_DoesActorHaveTag(
		GetHeroCharacterFromActorInfo(), WarGameplayTags::Player_Status_Rolling);

	if (bShouldOverrideRotation)
	{
		// プレイヤーからターゲットへの方向を求める
		FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(), CurrentLockedActor->GetActorLocation());

		LookAtRotator -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

		// 回転の補間処理（カメラ回転）
		const FRotator CurrentControllerRotator = GetPlayerControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRotator = FMath::RInterpTo(CurrentControllerRotator, LookAtRotator, DeltaTime,
			TargetLockRotationInterpSpeed);
		GetPlayerControllerFromActorInfo()->SetControlRotation(FRotator(TargetRotator.Pitch, TargetRotator.Yaw, 0.f));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRotator.Yaw, 0.f));
	}
}

/**
 * @brief 入力に基づき、左右のターゲットを切り替える。
 */
void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	// 現在のターゲット周辺の左右ターゲットを取得
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);
	UE_LOG(LogTemp, Warning, TEXT("ActorsOnLeft:%d,ActorsOnRight:%d"), ActorsOnLeft.Num(), ActorsOnRight.Num());
	
	if (InSwitchDirectionTag == WarGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	if (NewTargetToLock)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewTargetToLock"));
		CurrentLockedActor = NewTargetToLock;
	}
}

/**
 * @brief ターゲットロックを試みる。
 */
void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	// 利用可能なターゲットを更新
	GetAvailableActorsToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelLockOnTarget();
		return;
	}

	// 最も近いターゲットを選択
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);
	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelLockOnTarget();
	}
}

/**
 * @brief Box Traceを使用して、利用可能なターゲット候補を取得する。
 */
void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult> BoxTraceHitResults;
	// Box Traceによるターゲット候補の追跡
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowDebugShapes ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHitResults,
		true
	);
	for (const FHitResult& HitResult : BoxTraceHitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

/**
 * @brief 利用可能なターゲットから最も近いものを取得する。
 */
AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = FLT_MAX;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

/**
 * @brief ターゲットロックウィジェットを表示する。
 */
void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!CurrentLockedWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("在bp中设置DrawTargetLockWidget"));
		CurrentLockedWidget = CreateWidget<UWarWidgetBase>(GetPlayerControllerFromActorInfo(), TargetLockWidgetClass);
		check(CurrentLockedWidget);
		CurrentLockedWidget->AddToViewport();
	}
}

/**
 * @brief ターゲットロックウィジェットの位置を更新する。
 */
void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!CurrentLockedWidget || !CurrentLockedActor)
	{
		CancelLockOnTarget();
		return;
	}
	// ワールド位置をウィジェット位置に変換
	FVector2d ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetPlayerControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true
	);
	// ウィジェットサイズが未設定なら取得
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		CurrentLockedWidget->WidgetTree->ForEachWidget(
			[this](class UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}

	ScreenPosition -= TargetLockWidgetSize / 2.f;
	CurrentLockedWidget->SetPositionInViewport(ScreenPosition, false);
}

/**
 * @brief ターゲットロック用の入力マッピングを有効にする。
 */
void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerControllerFromActorInfo()->GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

/**
 * @brief ターゲットロック用の入力マッピングを無効にする。
 */
void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetPlayerControllerFromActorInfo()) return;
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerControllerFromActorInfo()->GetLocalPlayer());
	check(Subsystem);
	Subsystem->RemoveMappingContext(TargetLockMappingContext);
}

/**
 * @brief ターゲットロックをキャンセルする。
 */
void UHeroGameplayAbility_TargetLock::CancelLockOnTarget()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

/**
 * @brief ターゲットロックの後処理を行う。
 */
void UHeroGameplayAbility_TargetLock::CleanUpLockOnTarget()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;

	if (CurrentLockedWidget)
	{
		CurrentLockedWidget->RemoveFromParent();
	}
	CurrentLockedWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
}

/**
 * @brief 現在のターゲット周辺の左右のターゲット候補を取得する。
 */
void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelLockOnTarget();
		return;
	}

	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("AvailableActorsToLock:%d"), AvailableActorsToLock.Num());

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;
		// プレイヤーから対象への方向を求め、左右を判定
		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("ActorsOnLeft:%d,ActorsOnRight:%d"), OutActorsOnLeft.Num(), OutActorsOnRight.Num());
}
