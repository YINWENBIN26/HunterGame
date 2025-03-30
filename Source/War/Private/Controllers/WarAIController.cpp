// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarAIController.h"

#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/WarHeroCharacter.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

/**
 * @brief コンストラクタ
 * - AI感知設定（視覚）とパスフォローコンポーネントの初期化
 * - チームIDを設定する
 */
AWarAIController::AWarAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
    // 視覚センスの設定を作成
    AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfig_Sight");
    AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
    // BPで変更可能なパラメータ
    AISenseConfig_Sight->SightRadius = 5000.f;
    AISenseConfig_Sight->LoseSightRadius = 0.f;
    AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

    // 敵感知コンポーネントの作成と設定
    EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
    EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
    EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
    EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

    // チームIDを設定（ここでは1）
    AAIController::SetGenericTeamId(FGenericTeamId(1));
}

/**
 * @brief 他のアクターに対するチーム態度を決定する
 *
 * 他のアクターのコントローラーが属するチームIDと自分のチームIDを比較し、敵か友好かを判断する
 */
ETeamAttitude::Type AWarAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* PawnToCheck = Cast<const APawn>(&Other);
    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
    // 他のチームIDが自分より小さい場合、Hostile（敵）とする
    if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
    {
        return ETeamAttitude::Hostile;
    }
    return ETeamAttitude::Friendly;
}

/**
 * @brief BeginPlayでCrowdFollowingComponentの各種設定を行う
 *
 * 回避品質、グループ設定、衝突クエリ範囲などを設定して、AIの回避動作を調整する
 */
void AWarAIController::BeginPlay()
{
    Super::BeginPlay();
    // CrowdFollowingComponentを取得して設定
    if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
    {
        CrowdComp->SetCrowdSimulationState(bEnabledDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Enabled);
        // 回避品質を設定
        switch (DetourCrowdAvoidanceQuality)
        {
        case 1:
            CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
            break;
        case 2:
            CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
            break;
        case 3:
            CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
            break;
        case 4:
            CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
            break;
        default:
            break;
        }
        // 自身の回避グループと避けるグループを設定
        CrowdComp->SetAvoidanceGroup(1);
        CrowdComp->SetGroupsToAvoid(1);

        CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
    }
}

/**
 * @brief 毎フレーム呼ばれるTick処理
 *
 * ブラックボード内のターゲット情報をチェックし、記憶時間を超えた場合はターゲットを解除する
 */
void AWarAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return;
    }

    // ブラックボードにターゲットが設定され、かつターゲットが失われた状態なら記憶時間をチェック
    if (Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor"))) && IsLostTarget)
    {
        // 最後の感知時間から経過した時間がMemoryTimeを超えたらターゲットを解除
        if (LastSenseTime >= MemoryTime)
        {
            BlackboardComponent->SetValueAsObject(FName("TargetActor"), nullptr);

            // 警戒状態のタグを解除し、ターゲットロスト状態をリセットする
            if (APawn* OwningPawn = GetPawn())
            {
                UWarFunctionLibrary::RemoveGameplayTagToActorIfFound(OwningPawn, WarGameplayTags::Enemy_Wolf_Status_Alert);
                IsLostTarget = false;
            }
        }
    }
}

/**
 * @brief 敵感知更新時の処理
 *
 * 感知したアクターがヒーローキャラクターであれば、ブラックボードにターゲットを設定し、
 * 警戒状態タグを追加、感知時間をリセットする。感知に失敗した場合はターゲットロスト状態にする
 */
void AWarAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* OwningPawn = GetPawn();

    if (Stimulus.WasSuccessfullySensed() && Actor)
    {
        if (Cast<AWarHeroCharacter>(Actor))
        {
            // 感知したヒーローキャラクターをブラックボードに設定
            if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
            {
                BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
                UWarFunctionLibrary::AddGameplayTagToActorIfNone(OwningPawn, WarGameplayTags::Enemy_Wolf_Status_Alert);
                LastSenseTime = 0;
                IsLostTarget = false;
            }
        }
    }
    else
    {
        // 感知に失敗した場合、ターゲットロスト状態にする
        IsLostTarget = true;
    }
}
