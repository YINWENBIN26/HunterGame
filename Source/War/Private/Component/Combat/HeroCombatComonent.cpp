// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Combat/HeroCombatComonent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarGameplayTags.h"
#include "Items/Weapon/WarHeroWeapon.h"
#include "Perception/AISense_Damage.h"

/**
 * @brief キャラクターが持っている武器の中から、指定タグに一致する武器を取得する
 */
AWarHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(const FGameplayTag InWeaponTag) const
{
    return Cast<AWarHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

/**
 * @brief 現在装備中の武器を取得する
 */
AWarHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
    return Cast<AWarHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

/**
 * @brief 現在装備中の武器のダメージレベルを取得する
 */
float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageLevel(float InLevel) const
{
    return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

/**
 * @brief ターゲットにヒットした際の処理
 *
 * ・同じターゲットに複数回ヒットしないように管理する
 * ・メレー攻撃のヒットイベントとヒットポーズのイベントを送信する
 * ・ダメージイベントを報告する
 */
void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
    // 既にヒット済みの場合は何もしない
    if (OverlappedActors.Contains(HitActor))
        return;
    OverlappedActors.AddUnique(HitActor);

    FGameplayEventData EventData;
    EventData.Instigator = GetOwningPawn();
    EventData.Target = HitActor;

    // ヒットイベントを送信
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        WarGameplayTags::Shared_Event_MeleeHit,
        EventData
    );
    // ヒットポーズイベントを送信
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        WarGameplayTags::Player_Event_HitPause,
        FGameplayEventData()
    );
    
    AActor* Instigator = GetOwner();

    // イベント位置とヒット位置（ここでは同じ位置）を取得
    FVector EventLocation = HitActor->GetActorLocation();
    FVector HitLocation = EventLocation;

    // AISenseダメージイベントを報告する
    UAISense_Damage::ReportDamageEvent(
        GetWorld(),
        HitActor,
        Instigator,
        0,
        EventLocation,
        HitLocation
    );
}

/**
 * @brief 武器がターゲットから引き抜かれた際の処理
 *
 * ヒットポーズイベントを送信する
 */
void UHeroCombatComponent::OnWeaponPulledFromTarget(AActor* InteractedActor)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        WarGameplayTags::Player_Event_HitPause,
        FGameplayEventData()
    );
}
