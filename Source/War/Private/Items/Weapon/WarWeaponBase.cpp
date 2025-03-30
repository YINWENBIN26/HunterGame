// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/WarWeaponBase.h"

#include "WarFunctionLibrary.h"
#include "Components/BoxComponent.h"

/**
 * @brief コンストラクタ
 * - 武器のメッシュと衝突ボックスを作成・初期化する
 * - 衝突イベント（BeginOverlap, EndOverlap）のデリゲートをバインドする
 */
AWarWeaponBase::AWarWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // 武器メッシュの作成とルートコンポーネントへの設定
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);

    // 衝突ボックスの作成と武器メッシュにアタッチ
    WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
    WeaponCollisionBox->SetupAttachment(GetRootComponent());
    WeaponCollisionBox->SetBoxExtent(FVector(20.f));
    WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
    WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}

/**
 * @brief 衝突ボックスに他のアクターが重なり始めたときの処理
 *
 * - 武器の所有Pawnを取得し、OtherActorがPawnかつ敵であれば、OnWeaponHitTargetイベントを実行する
 */
void AWarWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResul)
{
    // 武器の所有Pawn（Instigator）を取得
    APawn* WeaponOwningPawn = GetInstigator<APawn>();
    checkf(WeaponOwningPawn, TEXT("Forgot to assign an Instigator as the owning pawn of the weapon: %s"), *GetName());
    
    if (APawn* HitPawn = Cast<APawn>(OtherActor))
    {
        // 他のPawnが敵かどうかを判定し、敵であればヒットイベントを実行する
        if (UWarFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
        {
            OnWeaponHitTarget.ExecuteIfBound(OtherActor);
        }
    }
}

/**
 * @brief 衝突ボックスから他のアクターが離れたときの処理
 *
 * - 武器の所有Pawnを取得し、OtherActorがPawnかつ敵であれば、OnWeaponPulledFromTargetイベントを実行する
 */
void AWarWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 武器の所有Pawn（Instigator）を取得
    APawn* WeaponOwningPawn = GetInstigator<APawn>();
    checkf(WeaponOwningPawn, TEXT("Forgot to assign an Instigator as the owning pawn of the weapon: %s"), *GetName());

    // 他のアクターがPawnの場合
    if (APawn* HitPawn = Cast<APawn>(OtherActor))
    {
        // 他のPawnが敵であれば、武器がターゲットから引き抜かれたイベントを実行する
        if (UWarFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
        {
            OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
        }
    }
}
