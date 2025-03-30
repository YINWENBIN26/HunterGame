// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarEnumTypes.h"

class FRotateLatentAction : public FPendingLatentAction
{
public:
	FRotateLatentAction(AActor* InActorToRotate, ERotateTargetType InMode, float InRotationSpeed, float InTolerance, const FLatentActionInfo& LatentInfo);
	
	void SetTargetLocation(const FVector& InTargetLocation)
	{
		TargetLocation = InTargetLocation;
	}
	
	void SetTargetActor(AActor* InTargetActor)
	{
		TargetActor = InTargetActor;
	}
	
	void SetTargetRotation(const FRotator& InTargetRotation)
	{
		TargetRotation = InTargetRotation;
	}
	
	void CancelAction();
	
	virtual void UpdateOperation(FLatentResponse& Response) override;

private:
	bool bNeedToCancel;
	TWeakObjectPtr<AActor> ActorToRotate;
	ERotateTargetType RotateMode;
	// 回転速度
	float RotationSpeed;
	float Tolerance;

	// 各モードに応じたターゲットデータ
	FVector TargetLocation;                
	TWeakObjectPtr<AActor> TargetActor;     
	FRotator TargetRotation;              

	// ブループリント再開用パラメータ
	FName ExecutionFunction;              
	int32 OutputLink;                     
	FWeakObjectPtr CallbackTarget;         
};
