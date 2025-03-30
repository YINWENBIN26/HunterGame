// Fill out your copyright notice in the Description page of Project Settings.


#include "WarGameInstance.h"

#include "MoviePlayer.h"

TSoftObjectPtr<UWorld> UWarGameInstance::GetGameLevelByTag(FGameplayTag InTag)
{
	for (const FWarriorGameLevelSet& GameLevelSet : GameLevelSets)
	{
		if(!GameLevelSet.IsValid()) continue;
 
		if (GameLevelSet.LevelTag == InTag)
		{
			return GameLevelSet.Level;
		}
	}
 
	return TSoftObjectPtr<UWorld>();
}
void UWarGameInstance::Init()
{
	Super::Init();
 
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ThisClass::OnDestinationWorldLoaded);
}
 
void UWarGameInstance::OnPreLoadMap(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
 
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}
 
void UWarGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();
}