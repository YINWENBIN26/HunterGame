// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarCharacterAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Characters/WarCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @brief アニメーション初期化処理
 * Pawnオーナーを取得し、キャラクターとその移動コンポーネントをキャッシュする
 */
void UWarCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AWarCharacterBase>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

/**
 * @brief 毎フレーム呼ばれるアニメーション更新処理
 */
void UWarCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}
    
	// 移動速度の更新
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
    
	// キャラクターの移動方向を計算
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(
		OwningCharacter->GetVelocity(),
		OwningCharacter->GetActorRotation()
	);
}
