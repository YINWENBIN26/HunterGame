#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_ActivateAbilityByTag.generated.h"

// 前置声明
class UWarEnemyGameplayAbility;
class UAbilitySystemComponent;

/**
 * BT Task 节点：通过 Ability Tag 激活能力，并等待该能力结束后返回 Succeeded
 */
UCLASS()
class WAR_API UBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbilityByTag();

	// 在编辑器中设置需要激活的能力 Tag
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

	// 重写 ExecuteTask：激活能力
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 重写 AbortTask：任务中断时清理绑定
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 绑定的回调函数，当能力结束时调用
	UFUNCTION()
	void OnAbilityEnded(UWarEnemyGameplayAbility* EndedAbility);

private:
	// 缓存 BehaviorTreeComponent，便于在回调中结束任务
	UPROPERTY(Transient)
	UBehaviorTreeComponent* CachedOwnerComp;

	// 缓存当前激活的能力实例
	UPROPERTY(Transient)
	UWarEnemyGameplayAbility* ActivatedAbility;
};