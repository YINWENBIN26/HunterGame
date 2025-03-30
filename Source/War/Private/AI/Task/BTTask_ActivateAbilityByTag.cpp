#include "AI/Task/BTTask_ActivateAbilityByTag.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "WarFunctionLibrary.h"
#include "AbilitySystem/WarEnemyAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Enemies/WarEnemyGameplayAbility.h"

UBTTask_ActivateAbilityByTag ::UBTTask_ActivateAbilityByTag()
{
    NodeName = TEXT("Activate Ability By Tag");
    CachedOwnerComp = nullptr;
    ActivatedAbility = nullptr;
}



/**
 * @brief 指定されたAbilityTagに一致する能力を起動し、BTタスクを進行中にする
 */
EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedOwnerComp = &OwnerComp;
    
    // AIが制御するPawnを取得
    APawn* ControlledPawn = CachedOwnerComp->GetAIOwner()->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }
    UAbilitySystemComponent* ASC = UWarFunctionLibrary::Native_GetWarAbilitySystemComponentFromActor(ControlledPawn);
    TArray<FGameplayAbilitySpec*> MatchingSpecs = Cast<UWarEnemyAbilitySystemComponent>(ASC)->GetAbilitySpecFromTag(AbilityTag);

    // 一致する能力がなければ失敗
    if (MatchingSpecs.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }
    FGameplayAbilitySpec* Spec = MatchingSpecs[0];
    
    if (!ASC->TryActivateAbility(Spec->Handle, /*InputPressed=*/true))
    {
        return EBTNodeResult::Failed;
    }

    // 起動した能力インスタンスを取得し、OnAbilityEndedイベントに動的バインドする
    ActivatedAbility = Cast<UWarEnemyGameplayAbility>(Spec->GetPrimaryInstance());
    if (ActivatedAbility)
    {
        UE_LOG(LogTemp, Warning, TEXT("%p 绑定了回调函数"), ActivatedAbility->GetActorInfo().AvatarActor.Get());
        ActivatedAbility->OnAbilityEndedEvent.AddDynamic(this, &UBTTask_ActivateAbilityByTag::OnAbilityEnded);
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

/**
 * @brief タスク中止時に、起動した能力のOnAbilityEndedイベントからコールバックを解除する
 */
EBTNodeResult::Type UBTTask_ActivateAbilityByTag::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AActor* OwingActor = OwnerComp.GetAIOwner()->GetPawn();
    if (OwingActor)
    {
        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwingActor);
        if (ASC)
        {
            // 指定のAbilityTagに一致する能力を取得する
            TArray<FGameplayAbilitySpec*> MatchingSpecs = Cast<UWarEnemyAbilitySystemComponent>(ASC)->GetAbilitySpecFromTag(AbilityTag);
            for (FGameplayAbilitySpec* Spec : MatchingSpecs)
            {
                // 各Specの全インスタンスからコールバックを解除する
                for (UGameplayAbility* Instance : Spec->GetAbilityInstances())
                {
                    if (UWarEnemyGameplayAbility* WarAbility = Cast<UWarEnemyGameplayAbility>(Instance))
                    {
                        WarAbility->OnAbilityEndedEvent.RemoveDynamic(this, &UBTTask_ActivateAbilityByTag::OnAbilityEnded);
                    }
                }
            }
        }
    }
    return EBTNodeResult::Aborted;
}

/**
 * @brief 能力終了時に呼ばれるコールバック。BTタスクを完了させる
 */
void UBTTask_ActivateAbilityByTag::OnAbilityEnded(UWarEnemyGameplayAbility* EndedAbility)
{
    UE_LOG(LogTemp, Warning, TEXT("%p 解绑了回调函数"), EndedAbility->GetActorInfo().AvatarActor.Get());
    if (EndedAbility)
    {
        EndedAbility->OnAbilityEndedEvent.RemoveDynamic(this, &UBTTask_ActivateAbilityByTag::OnAbilityEnded);
    }

    // 能力終了後、BTタスクを成功として完了させる
    AActor* AvatarActor = EndedAbility->GetActorInfo().AvatarActor.Get();
    if (AvatarActor)
    {
        AAIController* AIController = Cast<AAIController>(AvatarActor->GetInstigatorController());
        if (AIController)
        {
            UBehaviorTreeComponent* BTComp = AIController->FindComponentByClass<UBehaviorTreeComponent>();
            if (BTComp)
            {
                FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
            }
        }
    }
}
