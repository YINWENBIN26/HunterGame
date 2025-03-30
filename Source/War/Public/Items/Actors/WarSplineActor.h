// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "WarSplineActor.generated.h"


UCLASS()
class WAR_API AWarSplineActor : public AActor
{
	GENERATED_BODY()
    
public:    
	AWarSplineActor();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:    
	// Splineコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline")
	USplineComponent* SplineComp;

	// 制御点配列
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TArray<FVector> ControlPoints;

	// 最大許容回転角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	float MaxRotationAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	float DistanceStep;

	// Splineコンポーネントを取得する
	UFUNCTION(BlueprintCallable, Category = "Spline")
	USplineComponent* GetSplineComp() const { return SplineComp; }

private:
	// 制御点に基づきSplineを生成する（必要な中間点を挿入）
	void GenerateSpline();
};

