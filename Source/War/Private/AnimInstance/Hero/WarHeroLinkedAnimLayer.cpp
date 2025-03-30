// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/Hero/WarHeroLinkedAnimLayer.h"

#include "AnimInstance/Hero/WarHeroAnimInstance.h"

UWarHeroAnimInstance* UWarHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return  Cast<UWarHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
