// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "WarGameInstance.generated.h"
USTRUCT(BlueprintType)
struct FWarriorGameLevelSet
{
	GENERATED_BODY()
 
	UPROPERTY(EditDefaultsOnly,meta = (Categories = "GameData.Level"))
	FGameplayTag LevelTag;
 
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;
 
	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
 
 
};
/**
 * 
 */
UCLASS()
class WAR_API UWarGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;

	virtual void OnPreLoadMap(const FString& MapName);
	virtual void OnDestinationWorldLoaded(UWorld* LoadedWorld);
 
public:
	UFUNCTION(BlueprintPure, meta =(GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag);

	void Init() override;
};
