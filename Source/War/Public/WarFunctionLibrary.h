// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarTypes/WarEnumTypes.h"

#include "WarFunctionLibrary.generated.h"

class UWarGameInstance;
struct FScalableFloat;
struct FGameplayEffectSpecHandle;
class UWarAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class WAR_API UWarFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// アクターから UWarAbilitySystemComponent を取得する
	static UWarAbilitySystemComponent* Native_GetWarAbilitySystemComponentFromActor(AActor* InActor);

	// アクターに指定タグが存在しなければ追加する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	// 複数のアクターに指定タグを追加する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static void AddGameplayTagToActors(const TArray<AActor*>& Actors, FGameplayTag TagToAdd);

	// アクターに指定タグが存在すれば削除する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static void RemoveGameplayTagToActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	// 複数のアクターから指定タグを削除する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static void RemoveGameplayTagFromActors(const TArray<AActor*>& Actors, FGameplayTag TagToRemove);

	// アクターが指定タグを持っているか判定する（bool返却）
	static bool Native_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	// アクターが指定タグを持っているかを EWarConfirmType で返す
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarConfirmType& OutConfirmType);

	// アクターから PawnCombatComponent を取得する
	static UPawnCombatComponent* Native_GetPawnCombatComponentFromActor(AActor* InActor);

	// アクターから PawnCombatComponent を取得し、有効性を返す
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponent(AActor* InActor, EWarValidType& OutValidType);

	// 指定された2つの Pawn の敵対関係を判定する
	UFUNCTION(BlueprintPure, Category = "War|FuctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	// FScalableFloat の指定レベルでの値を取得する
	UFUNCTION(BlueprintPure, Category = "War|FuctionLibrary", meta = (CompactNodeTitle = "Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel);

	// 攻撃者と被攻撃者の位置関係からヒット反応の方向タグを計算する
	UFUNCTION(BlueprintPure, Category = "War|FuctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	// ゲームプレイエフェクトをターゲットに適用する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	// 複数のアクターにゲームプレイエフェクトを適用する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary")
	static bool ApplyGameplayEffectToActors(AActor* InInstigator, const TArray<AActor*>& TargetActors, const FGameplayEffectSpecHandle& InSpecHandle);

	// アクターを目標位置に向けてスムーズに回転させる
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"), Category = "War|FuctionLibrary")
	static void RotateToFaceTargetLatent_ByLocation(UObject* WorldContextObject, AActor* ActorToRotate, FVector TargetLocation, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo);

	// アクターを目標アクターに向けてスムーズに回転させる
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"), Category = "War|FuctionLibrary")
	static void RotateToFaceTargetLatent_ByActor(UObject* WorldContextObject, AActor* ActorToRotate, AActor* TargetActor, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo);

	// アクターを指定の回転に向けてスムーズに回転させる
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"), Category = "War|FuctionLibrary")
	static void RotateToFaceTargetLatent_ByRotation(UObject* WorldContextObject, AActor* ActorToRotate, FRotator TargetRotation, float RotationSpeed, float Tolerance, FLatentActionInfo LatentInfo);

	// カウントダウン処理を実行する
	UFUNCTION(BlueprintCallable, Category = "War|FuctionLibrary", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentActionInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput", TotalTime = "1.0", UpdateInterVal = "0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterVal, float& OutRemainingTime, EWarCountDownActionInputType CountDownInput, UPARAM(DisplayName = "Output") EWarCountDownActionOutPutType& CountDownOutput, FLatentActionInfo LatentActionInfo);

	// ターゲットに対して、アクター配列を左右に分割する
	UFUNCTION(BlueprintPure, Category = "War|FuctionLibrary")
	static void DiffMoveDirectionArray(const TArray<AActor*>& Actors, const AActor* TargetActor, TArray<AActor*>& OutLeftActors, TArray<AActor*>& OutRightActors);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static UWarGameInstance* GetWarGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject,EWarInputMode InInputMode);
	
};
