// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
public:
	// ヒットしたターゲットに対してメレー攻撃ヒットイベントを送信する（攻撃対象の登録とイベント送信）
	virtual void OnHitTargetActor(AActor* HitActor) override;
	// ターゲットから武器が引き抜かれたときの処理（スーパークラスの処理を呼び出す）
	virtual void OnWeaponPulledFromTarget(AActor* InteractedActor) override;
};
