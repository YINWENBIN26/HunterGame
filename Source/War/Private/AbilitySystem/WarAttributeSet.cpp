// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarGameplayTags.h"
#include "WarFunctionLibrary.h"
#include "Component/UI/HeroUIComponent.h"
#include "Component/UI/PawnUIComponent.h"
#include "Interface/UI/PawnUIInterface.h"

UWarAttributeSet::UWarAttributeSet()
{

}

/**
 * @brief ゲームプレイエフェクト実行後に属性値を更新し、UIに反映する
 */
void UWarAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	check(CachedPawnUIInterface.IsValid());
	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	// ヘルス属性の更新処理
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float NewHealth = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		SetHealth(NewHealth);
		PawnUIComponent->OnCurrentHealthPercentChanged.Broadcast(GetHealth() / GetMaxHealth());
	}

	// レイジ属性の更新処理と状態タグの設定
	if (Data.EvaluatedData.Attribute == GetRageAttribute())
	{
		const float NewRage = FMath::Clamp(GetRage(), 0.f, GetMaxRage());
		SetRage(NewRage);
		if (GetRage() == GetMaxRage())
		{
			UWarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarGameplayTags::Player_Status_Rage_Full);
		}
		else if (GetRage() == 0.f)
		{
			UWarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarGameplayTags::Player_Status_Rage_None);
		}
		else
		{
			UWarFunctionLibrary::RemoveGameplayTagToActorIfFound(Data.Target.GetAvatarActor(), WarGameplayTags::Player_Status_Rage_Full);
			UWarFunctionLibrary::RemoveGameplayTagToActorIfFound(Data.Target.GetAvatarActor(), WarGameplayTags::Player_Status_Rage_None);
		}
		if (CachedPawnUIInterface->GetHeroUIComponent())
		{
			CachedPawnUIInterface->GetHeroUIComponent()->OnCurrentRagePercentChanged.Broadcast(GetRage() / GetMaxRage());
		}
	}

	// 被ダメージ属性の更新処理と死亡判定
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetHealth();
		const float DamageDone = GetDamageTaken();
		const float NewHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		SetHealth(NewHealth);
		const FString DebugString = FString::Printf(TEXT("Old Health: %f, Damage Done: %f, NewCurrentHealth: %f"), OldHealth, DamageDone, NewHealth);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugString);
		PawnUIComponent->OnCurrentHealthPercentChanged.Broadcast(GetHealth() / GetMaxHealth());
		if (GetHealth() <= 0.f)
		{
			UWarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarGameplayTags::Shared_Status_Dead);
		}
	}
}

