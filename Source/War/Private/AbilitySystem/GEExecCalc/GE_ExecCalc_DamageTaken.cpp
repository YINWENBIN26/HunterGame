// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GEExecCalc/GE_ExecCalc_DamageTaken.h"

#include "WarGameplayTags.h"
#include "AbilitySystem/WarAttributeSet.h"

struct FWarDamageCapture {
	// 攻撃力、防御力、被ダメージのキャプチャ定義
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	
	FWarDamageCapture()
	{
		// ソースから攻撃力、ターゲットから防御力と被ダメージをキャプチャ
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarAttributeSet, DamageTaken, Target, false);
	}
};

/**
 * @brief FWarDamageCapture のシングルトンを取得
 */
static const FWarDamageCapture& GetWarDamageCapture()
{
	static FWarDamageCapture WarDamageCapture;
	return WarDamageCapture;
}

UGE_ExecCalc_DamageTaken::UGE_ExecCalc_DamageTaken()
{
	// キャプチャ対象の属性を追加
	RelevantAttributesToCapture.Add(GetWarDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarDamageCapture().DamageTakenDef);
}

/**
 * @brief ダメージ計算を実行し、最終ダメージを出力に反映する
 */
void UGE_ExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// エフェクト仕様を取得
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// ソースとターゲットのタグを取得
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;

	// 攻撃力属性のキャプチャ値を計算
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarDamageCapture().AttackPowerDef, EvaluateParams, SourceAttackPower);

	// セットバイコーラータグから基本ダメージとコンボ回数を取得
	for (const TPair<FGameplayTag, float>& TagMagnitudes : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitudes.Key.MatchesTagExact(WarGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitudes.Value;
		}
		if (TagMagnitudes.Key.MatchesTagExact(WarGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount = TagMagnitudes.Value;
		}
		if (TagMagnitudes.Key.MatchesTagExact(WarGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitudes.Value;
		}
	}

	float TargetDefensePower = 0.f;
	// 防御力属性のキャプチャ値を計算
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarDamageCapture().DefensePowerDef, EvaluateParams, TargetDefensePower);

	// 軽攻撃コンボによるダメージ増加
	if (UsedLightAttackComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05f + 1.f;
		BaseDamage *= DamageIncreasePercentLight;
	}
	// 重攻撃コンボによるダメージ増加
	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = (UsedHeavyAttackComboCount - 1) * 0.15f + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
	}

	UE_LOG(LogTemp, Warning, TEXT("FinalDamageDone%s"), *EffectSpec.GetContext().GetInstigator()->GetName());
	// 最終ダメージ計算：基本ダメージ * 攻撃力 / 防御力
	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	if (FinalDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone)
		);
	}
}