// Fill out your copyright notice in the Description page of Project Settings.


#include "WarTypes/WarCountDownAction.h"

void FWarCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}

void FWarCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountDownOutPut=EWarCountDownActionOutPutType::Cancel;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}
	if (ElapsedTimeSinceStart>=TotalCountDownTime)
	{
		CountDownOutPut=EWarCountDownActionOutPutType::Completed;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}
	if (ElapsedInterval<UpdateInterval)
	{
		//还没达到触发更新的间隔时间
		ElapsedInterval+=Response.ElapsedTime();
	}
	else
	{
		//倒计时更新逻辑
		ElapsedTimeSinceStart+=UpdateInterval>0.f?UpdateInterval:Response.ElapsedTime();
		OutRemainTime=TotalCountDownTime-ElapsedTimeSinceStart;

		CountDownOutPut=EWarCountDownActionOutPutType::Updated;
		Response.TriggerLink(ExecutionFunction,OutputLink,CallbackTarget);
		ElapsedInterval=0.f;
	}
}
