// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemies/WarEnemyKingGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"

/**
 * @brief 指定アクターの周辺から、特定タグを持つ敵アクターを収集する。
 */
TArray<AActor*> UWarEnemyKingGameplayAbility::GetAllEnemiesHasTag(AActor* Owner, FGameplayTag EnemyTag, FVector BoxExtent)
{
	TArray<AActor*> OutActors;
	if (!Owner) return OutActors;

	UWorld* World = Owner->GetWorld();
	if (!World) return OutActors;

	FVector Origin = Owner->GetActorLocation();

	TArray<AActor*> OverlappingActors;
	const TArray<AActor*> IgnoreActors = { Owner };
	const ECollisionChannel CollisionChannel = ECC_Pawn;

	// ボックス領域内のアクター衝突検出
	bool bHit = UKismetSystemLibrary::BoxOverlapActors(
		World,
		Origin,
		BoxExtent, 
		{ UEngineTypes::ConvertToObjectType(CollisionChannel) },
		nullptr, 
		IgnoreActors, 
		OverlappingActors
	);

	if (bHit)
	{
		for (AActor* Actor : OverlappingActors)
		{
			if (Actor)
			{
				UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
				if (ASC && ASC->HasMatchingGameplayTag(EnemyTag))
				{
					OutActors.Add(Actor);
				}
			}
		}
	}
	return OutActors;
}

/**
 * @brief 対象アクター群のブラックボードに、キングアクターとして自身を設定する。
 */
void UWarEnemyKingGameplayAbility::SetKingOfActors(const TArray<AActor*>& TargetActors)
{
	AActor* OwningActor = GetOwningActorFromActorInfo();
	if (!OwningActor)
	{
		return;
	}

	for (AActor* TargetActor : TargetActors)
	{
		if (!TargetActor)
		{
			continue;
		}
		
		APawn* TargetPawn = Cast<APawn>(TargetActor);
		if (!TargetPawn)
		{
			continue;
		}
		
		AController* TargetController = TargetPawn->GetController();
		if (!TargetController)
		{
			continue;
		}
		
		AAIController* TargetAIController = Cast<AAIController>(TargetController);
		if (!TargetAIController)
		{
			continue;
		}
		
		UBlackboardComponent* BlackboardComp = TargetAIController->GetBlackboardComponent();
		if (!BlackboardComp)
		{
			continue;
		}
		
		BlackboardComp->SetValueAsObject(TEXT("KingOfActor"), OwningActor);
	}
}