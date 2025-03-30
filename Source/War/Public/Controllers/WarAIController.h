// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "WarAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;
/**
 * 
 */
UCLASS()
class WAR_API AWarAIController : public AAIController
{
	GENERATED_BODY()
public:
	// コンストラクタ：AI感知設定（視覚）とパスフォローコンポーネントの初期化、チームIDの設定
	AWarAIController(const FObjectInitializer& ObjectInitializer);
	
	//Begin IGenericTeamAgentInterface  Interface
	// 他のアクターに対するチーム態度を返す（敵か友好かを判断する）
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End IGenericTeamAgentInterface  Interface
	
	// AIの記憶時間（ターゲット保持時間）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MemoryTime = 5.0f;

protected:
	// ゲーム開始時の初期化処理
	virtual void BeginPlay() override;
	// 毎フレームの処理（ターゲット情報のチェック）
	virtual void Tick(float DeltaTime) override;
	
	// 敵感知コンポーネント（視覚による感知）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;
	
	// 視覚感知設定（パラメータ設定可能）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	// 敵感知更新時の処理
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	// ブループリントに回避設定を公開するためのフラグ
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config")
	bool bEnabledDetourCrowdAvoidance = true;
	
	// 回避品質設定（1～4の値）
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config", meta=(EditCondition="bEnabledDetourCrowdAvoidance", UIMin="1", UIMax="4"))
	int32 DetourCrowdAvoidanceQuality = 4;
	
	// 衝突クエリ範囲の設定
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config", meta=(EditCondition="bEnabledDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
	
	// 最後に感知した時間（内部管理用）
	float LastSenseTime = 0.f;
	
	// ターゲット喪失状態フラグ
	bool IsLostTarget = false;
};
