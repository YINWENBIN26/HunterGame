 // Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarEnemyCharacter.h"

#include "AbilitySystem/WarEnemyAbilitySystemComponent.h"
#include "Component/Combat/EnemyCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Widgets/WarWidgetBase.h"

AWarEnemyCharacter::AWarEnemyCharacter()
{
	WarAbilitySystemComponent = CreateDefaultSubobject<UWarEnemyAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;

	GetCharacterMovement()->bUseControllerDesiredRotation=false;
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,180.f,0.0f);
	GetCharacterMovement()->MaxWalkSpeed=400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking=1000.0f;

	EnemyCombatComponent=CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
	
	EnemyUIComponent=CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));
	EnemyHealthWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

/**
* @brief 指定タグが敵タグと一致するかチェックする
*/
bool AWarEnemyCharacter::IsEnemyHasTag(FGameplayTag TagToCheck)
{
	if (EnemyTag.IsValid())
	{
		return TagToCheck.MatchesTag(EnemyTag);
	}
	return false;
}

/**
 * @brief ゲーム開始時の初期化処理
 * - 敵ヘルスウィジェットの初期化
 * - AbilitySystemComponentに敵タグを追加する
 */
void AWarEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UWarWidgetBase* HealthWidget = Cast<UWarWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidgets(this);
	}
	if (EnemyTag.IsValid())
	{
		GetAbilitySystemComponent()->AddLooseGameplayTag(EnemyTag);
	}
}

void AWarEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}


UPawnCombatComponent* AWarEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}


UEnemyUIComponent* AWarEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

/**
 * @brief 敵の初期化データを非同期でロードし、AbilitySystemComponentに適用する
 */
void AWarEnemyCharacter::InitEnemyStartUpData()
{
	// 非同期ロード処理
	if (CharacterStartUpData.IsNull()) return;
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(WarAbilitySystemComponent);
				}
			}
		)
	);
}
