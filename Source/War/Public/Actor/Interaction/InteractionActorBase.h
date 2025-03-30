// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "InteractionActorBase.generated.h"

UCLASS()
class WAR_API AInteractionActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionActorBase();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag InteractionTag;

};
