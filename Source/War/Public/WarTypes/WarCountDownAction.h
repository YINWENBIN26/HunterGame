// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarEnumTypes.h"

/**
 * 
 */
class FWarCountDownAction:public  FPendingLatentAction
{
public:
	FWarCountDownAction(float InTotalCountDownTime,float InUpdateInterval,float& InOutRemainTime,EWarCountDownActionOutPutType InCountDownOutput,const FLatentActionInfo& LatentActionInfo)
	:bNeedToCancel(false),TotalCountDownTime(InTotalCountDownTime),UpdateInterval(InUpdateInterval),OutRemainTime(InOutRemainTime),CountDownOutPut(InCountDownOutput),ExecutionFunction(LatentActionInfo.ExecutionFunction)
	,OutputLink(LatentActionInfo.Linkage),CallbackTarget(LatentActionInfo.CallbackTarget),ElapsedInterval(0.f),ElapsedTimeSinceStart(0.f)
	{
		
	}
	void CancelAction();
	virtual void UpdateOperation(FLatentResponse& Response) override;

private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainTime;
	EWarCountDownActionOutPutType CountDownOutPut;
	FName ExecutionFunction;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
	int32 OutputLink;

};
