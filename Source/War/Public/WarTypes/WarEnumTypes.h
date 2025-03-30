#pragma once

UENUM()
enum class EWarConfirmType:uint8
{
	Yes,
	No 
};
UENUM()
enum class EWarValidType:uint8
{
	Valid,
	Invalid,
};
UENUM()
enum class EWarSuccessType:uint8
{
	Successful,
	Failed,
};
UENUM()
enum class EWarCountDownActionInputType:uint8
{
	Start,
	Cancel,
};
UENUM()
enum class EWarCountDownActionOutPutType:uint8
{
	Updated,
	Completed,
	Cancel,
};
UENUM()
enum class ERotateTargetType: uint8
{
	Location,
	Rotation,
	Actor
};
UENUM(BlueprintType)
enum class EWarInputMode : uint8
{
	GameOnly,
	UIOnly
};