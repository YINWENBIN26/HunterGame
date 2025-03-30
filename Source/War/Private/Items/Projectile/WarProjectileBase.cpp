// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Projectile/WarProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "WarFunctionLibrary.h"
#include "WarGameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"

/**
 * @brief コンストラクタ
 * プロジェクタイルのコンポーネント（衝突ボックス、Niagara、移動コンポーネント）の初期化と設定を行う
 */
AWarProjectileBase::AWarProjectileBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // プロジェクタイル移動コンポーネントの作成と初期値設定
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
    ProjectileMovementComponent->InitialSpeed = 700.f;
    ProjectileMovementComponent->MaxSpeed = 900.f;
    ProjectileMovementComponent->Velocity = FVector(1.f, 0.f, 0.f);
    ProjectileMovementComponent->ProjectileGravityScale = 0.f;
    
    // 衝突ボックスの作成と設定
    ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>("ProjectileCollisionBox");
    SetRootComponent(ProjectileCollisionBox);
    ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnprojectileHit);
    ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnOverlapBegin);

    // Niagaraコンポーネントの作成とアタッチ
    ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Projectil=eNiagaraComponent"));
    ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
    
    // ライフスパンの設定
    InitialLifeSpan = 4.f;
}

/**
 * @brief ゲーム開始時の初期化処理
 *
 * プロジェクタイルダメージポリシーに基づいて、衝突ボックスの反応設定を変更する
 */
void AWarProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
    {
        ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

/**
 * @brief プロジェクタイルが何かにヒットしたときの処理
 *
 * ヒットした位置でエフェクトをスポーンし、ヒット対象が敵の場合はダメージ処理を行いプロジェクタイルを破壊する
 */
void AWarProjectileBase::OnprojectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // ヒット位置でエフェクトを生成する（BPで実装）
    BP_OnSpawnProjectileFX(Hit.ImpactPoint);

    // ヒットしたアクターをPawnとして取得
    APawn* HitPawn = Cast<APawn>(OtherActor);
    // 味方に攻撃している場合は破壊して終了
    if (HitPawn != nullptr && !UWarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
    {
        Destroy();
        return;
    }

    /*
    bool IsVaildBlock = false;
    const bool bIsPlayerBlocking = UWarFunctionLibrary::Native_DoesActorHaveTag(HitPawn, WarGameplayTags::Player_Ability_Defense);
    if (bIsPlayerBlocking)
    {
        // TODO: ブロック処理を実装する
    }
    */

    // イベントデータの作成
    FGameplayEventData Data;
    Data.Instigator = this;
    Data.Target = HitPawn;

    // ダメージ処理を呼び出し、プロジェクタイルを破壊する
    HandleApplyProjectileDamage(HitPawn, Data);
    Destroy();
}

/**
 * @brief プロジェクタイルの衝突オーバーラップ開始時の処理
 *
 * 同じアクターへの複数回の処理を防ぎ、敵であればダメージ処理を実行する
 */
void AWarProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 既に処理済みの場合は何もしない
    if (OverlayppedActors.Contains(OtherActor))
    {
        return;
    }
    OverlayppedActors.AddUnique(OtherActor);
    if (APawn* HitPawn = Cast<APawn>(OtherActor))
    {
        FGameplayEventData Data;
        Data.Instigator = this;
        Data.Target = HitPawn;
        // 敵であればダメージ処理を実行
        if (UWarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
        {
            HandleApplyProjectileDamage(HitPawn, Data);
        }
    }
}

/**
 * @brief プロジェクタイルダメージ処理を実行する
 *
 * ProjectileDamageSpecHandleが有効であることを確認し、ダメージエフェクトを適用後にヒット反応イベントを送信する
 */
void AWarProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
    checkf(ProjectileDamageSpecHandle.IsValid(), TEXT("ProjectileDamageSpecHandle is invalid"));
    const bool bWasApplied = UWarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageSpecHandle);

    if (bWasApplied)
    {
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            InHitPawn,
            WarGameplayTags::Shared_Event_HitReact,
            InPayload
        );
    }
}
