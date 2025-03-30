// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Combat/PawnCombatComponent.h"

#include "Components/BoxComponent.h"

/**
 * @brief 生成された武器を登録する
 *
 * 指定タグが既に登録されていないことを確認し、武器をマップに登録する。
 * また、武器のヒットおよび引き抜かれたイベントをバインドし、必要に応じて装備中武器として設定する。
 */
void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarWeaponBase* InWeaponToRegister,
                                                 bool bRegisterAsEquippedWeapon)
{
    checkf(!ChararcterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named %s has already been added"), *InWeaponTagToRegister.ToString());
    check(InWeaponToRegister);

    // 武器をマップに登録
    ChararcterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
    
    // 武器のヒットイベントと引き抜かれたイベントをバインドする
    InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
    InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTarget);
    
    // 装備中の武器として登録する場合、タグをセットする
    if (bRegisterAsEquippedWeapon)
    {
        CurrentEquippedWeaponTag = InWeaponTagToRegister;
    }
}

/**
 * @brief 指定されたタグに一致する武器を取得する
 */
AWarWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
    if (ChararcterCarriedWeaponMap.Contains(InWeaponTagToGet))
    {
         if (AWarWeaponBase* const* FoundWeapon = ChararcterCarriedWeaponMap.Find(InWeaponTagToGet))
         {
             return *FoundWeapon;
         }
    }
    return nullptr;
}

/**
 * @brief 現在装備中の武器を取得する
 */
AWarWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
    if (!CurrentEquippedWeaponTag.IsValid())
    {
        return nullptr;
    }
    return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

/**
 * @brief 指定された武器の衝突判定を有効/無効に切り替える
 *
 * CurrentEquippedWeaponに対して、衝突が有効ならQueryOnlyに、
 * 無効ならNoCollisionに設定し、重複ヒット対象リストをクリアする。
 */
void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
    if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
    {
        AWarWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
        check(WeaponToToggle);
        if (bShouldEnable)
        {
            WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
        else
        {
            WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            OverlappedActors.Empty();
        }
    }
}

/**
 * @brief 武器でターゲットにヒットした際の処理（未実装）
 */
void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
    // TODO: ヒット時の処理を実装する
}

/**
 * @brief 武器がターゲットから引き抜かれた際の処理（未実装）
 */
void UPawnCombatComponent::OnWeaponPulledFromTarget(AActor* InteractedActor)
{
    // TODO: 引き抜かれた時の処理を実装する
}
