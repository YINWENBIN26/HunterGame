// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "WarPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API AWarPlayerController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AWarPlayerController();
	//~ Begin IGenericTeamAgentInterface  Interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface  Interface
private:
	FGenericTeamId HeroTeamId;
	
};
