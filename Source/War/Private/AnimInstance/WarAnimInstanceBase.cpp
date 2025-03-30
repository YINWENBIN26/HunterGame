// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarAnimInstanceBase.h"

#include "WarFunctionLibrary.h"

bool UWarAnimInstanceBase::DoseOwnerHaveTag(FGameplayTag TagToCheak) const
{
	if (APawn* OwningPawn= TryGetPawnOwner())
	{
		return  UWarFunctionLibrary::Native_DoesActorHaveTag(OwningPawn,TagToCheak);
	}
	return false;
}
