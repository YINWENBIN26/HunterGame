// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarPlayerController.h"

AWarPlayerController::AWarPlayerController()
{
	HeroTeamId=FGenericTeamId(0);
}

FGenericTeamId AWarPlayerController::GetGenericTeamId() const
{
	return HeroTeamId;
}
