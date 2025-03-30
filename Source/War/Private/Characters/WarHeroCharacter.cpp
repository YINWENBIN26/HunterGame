// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarHeroCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WarGameplayTags.h"
#include "Component/Input/WarInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "AbilitySystem/WarAbilitySystemComponent.h"
#include "AbilitySystem/WarHeroAbilitySystemComponent.h"
#include "Component/Combat/HeroCombatComonent.h"
#include "Component/UI/HeroUIComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"

/**
 * @brief コンストラクタ：各コンポーネントの生成と初期設定を行う
 */
AWarHeroCharacter::AWarHeroCharacter()
{
	// Ability System Component の生成（ヒーロー用）
	WarAbilitySystemComponent = CreateDefaultSubobject<UWarHeroAbilitySystemComponent>(TEXT("WarHeroAbilitySystemComponent"));
	// ヒーローUIコンポーネントの生成
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
	// カプセルサイズの初期化
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// コントローラー回転無効化設定
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// カメラブーム（カメラアーム）の生成と設定
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 55.0f, 65.0f);

	// フォローカメラの生成と設定
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// キャラクターの回転処理（移動方向に合わせる）※重要処理
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.f, 500.f, 0.f);
	MoveComp->MaxWalkSpeed = 400.f;
	MoveComp->BrakingDecelerationWalking = 2000.f;

	// ヒーローコンバットコンポーネントの生成
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

/**
 * @brief PawnCombatComponent を返す
 */
UPawnCombatComponent* AWarHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

/**
 * @brief PawnUIComponent を返す
 */
UPawnUIComponent* AWarHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

/**
 * @brief ヒーローUIコンポーネント を返す
 */
UHeroUIComponent* AWarHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

/**
 * @brief 移動入力処理
 * 入力軸に基づいて前方・右方向の移動を追加する
 */
void AWarHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	// 入力軸の取得
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	// コントローラーの回転を取得し、ヨー軸の回転のみを使用
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// 前方・右方向のベクトル計算
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 入力に応じた移動追加
	AddMovementInput(ForwardDirection, InputAxisVector.Y);
	AddMovementInput(RightDirection, InputAxisVector.X);
}

/**
 * @brief 視点操作入力処理
 * 入力軸に基づきカメラのヨー・ピッチを更新する
 */
void AWarHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	// 入力軸の取得
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	// ヨー回転の更新
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	// ピッチ回転の更新
	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

/**
 * @brief ターゲット切り替え入力（トリガー時）処理
 * 入力値を保持する
 */
void AWarHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("SwitchDirection: %f"), SwitchDirection);
}

/**
 * @brief ターゲット切り替え入力（完了時）処理
 * 入力に応じてターゲット切り替えイベントを送信する
 */
void AWarHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,
		SwitchDirection.X > 0.f ? WarGameplayTags::Player_Event_SwitchTarget_Right : WarGameplayTags::Player_Event_SwitchTarget_Left,
		Data
	);
}

/**
 * @brief アビリティ入力（押下）処理
 * 入力タグに基づいてアビリティの押下処理を実行する
 */
void AWarHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	Cast<UWarHeroAbilitySystemComponent>(WarAbilitySystemComponent)->OnAbilityInputPressed(InInputTag);
}

/**
 * @brief アビリティ入力（離す）処理
 * 入力タグに基づいてアビリティの離す処理を実行する
 */
void AWarHeroCharacter::Input_AbilityInputRelease(FGameplayTag InInputTag)
{
	Cast<UWarHeroAbilitySystemComponent>(WarAbilitySystemComponent)->OnAbilityInputReleased(InInputTag);
}

void AWarHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 起動データが設定されている場合、同期的に読み込みアビリティを付与する
	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase *LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			UE_LOG(LogTemp, Log, TEXT("CharacterStartUpData.LoadSynchronous"));
			LoadedData->GiveToAbilitySystemComponent(WarAbilitySystemComponent);
		}
	}
}

void AWarHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * @brief プレイヤー入力バインド処理
 * 入力コンフィグを設定し、各入力イベントに対応する処理をバインドする
 */
void AWarHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("InputConfigDataAsset is nullptr!"));
	ULocalPlayer *LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem *Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UWarInputComponent *WarInputComponent = CastChecked<UWarInputComponent>(PlayerInputComponent);
	WarInputComponent->BindNativeInputAction(InputConfigDataAsset, WarGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &AWarHeroCharacter::Input_Move);
	WarInputComponent->BindNativeInputAction(InputConfigDataAsset, WarGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &AWarHeroCharacter::Input_Look);
	WarInputComponent->BindNativeInputAction(InputConfigDataAsset, WarGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &AWarHeroCharacter::Input_SwitchTargetTriggered);
	WarInputComponent->BindNativeInputAction(InputConfigDataAsset, WarGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &AWarHeroCharacter::Input_SwitchTargetCompleted);
	WarInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &AWarHeroCharacter::Input_AbilityInputPressed, &AWarHeroCharacter::Input_AbilityInputRelease);
}

