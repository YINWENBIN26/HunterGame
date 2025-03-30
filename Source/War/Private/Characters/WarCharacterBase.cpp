// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarCharacterBase.h"

#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "AbilitySystem/WarAttributeSet.h"
#include "Component/UI/EnemyUIComponent.h"
#include "MotionWarpingComponent.h"
#include "Component/UI/HeroUIComponent.h"

// Sets default values
AWarCharacterBase::AWarCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetMesh()->bReceivesDecals = false;


	WarAttributeSet = CreateDefaultSubobject<UWarAttributeSet>(TEXT("WarriorAttributeSet"));

	MotionWarpingComponent=CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

/**
 * @brief Ability System Component を返す
 */
UAbilitySystemComponent* AWarCharacterBase::GetAbilitySystemComponent() const
{
	return WarAbilitySystemComponent;
}


UPawnCombatComponent* AWarCharacterBase::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* AWarCharacterBase::GetPawnUIComponent() const
{
	return nullptr;
}

/**
 * @brief キャラクターがコントローラーに Possessed されたときの処理
 *
 * Ability System Component の初期化と、スタートアップデータの確認を行う
 */
void AWarCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (WarAbilitySystemComponent)
	{
		WarAbilitySystemComponent->InitAbilityActorInfo(this, this);
		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}
