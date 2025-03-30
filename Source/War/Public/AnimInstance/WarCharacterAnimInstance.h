// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/WarAnimInstanceBase.h"
#include "WarCharacterAnimInstance.generated.h"

class AWarCharacterBase;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class WAR_API UWarCharacterAnimInstance : public UWarAnimInstanceBase
{
	GENERATED_BODY()
public:
	// アニメーション初期化処理：Pawnオーナーを取得し、キャラクターと移動コンポーネントをキャッシュする
	virtual void NativeInitializeAnimation() override;
	// アニメーション更新処理：地上速度、加速度の有無、移動方向を更新する
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	// キャラクターオーナーへの参照
	TObjectPtr<AWarCharacterBase> OwningCharacter;

	UPROPERTY()
	// キャラクターの移動コンポーネントへの参照
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	// 要修改 移动速度
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	// 加速度の存在フラグ（水平）
	bool bHasAcceleration;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	// キャラクターの移動方向
	float LocomotionDirection;
};
