// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gamemodes/WarGameModeBase.h"
#include "WarSurivialGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class WAR_API AWarSurivialGameModeBase : public AWarGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
