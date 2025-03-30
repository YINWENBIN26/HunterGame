// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Actors/WarSplineActor.h"

AWarSplineActor::AWarSplineActor()
{
	PrimaryActorTick.bCanEverTick = false; 
	
	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	RootComponent = SplineComp;

	// デフォルトパラメータ
	MaxRotationAngle = 10.0f;    // 最大回転角度
	DistanceStep    = 300.0f;    // サンプリング間隔
}

void AWarSplineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateSpline();
}

void AWarSplineActor::GenerateSpline()
{
	// 既存のSplineポイントをクリアする
	SplineComp->ClearSplinePoints(false);

	// 制御点が2点未満の場合、デフォルトの開始点と終了点を使用する
	if (ControlPoints.Num() < 2)
	{
		FVector Start = GetActorLocation();
		FVector End   = Start + GetActorForwardVector() * 1000.0f;
		ControlPoints.Empty();
		ControlPoints.Add(Start);
		ControlPoints.Add(End);
	}

	// 制御点からSplineポイントを追加する
	for (const FVector& Point : ControlPoints)
	{
		SplineComp->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
	}
	SplineComp->UpdateSpline();

	// サンプリングと簡易細分化により新たな制御点を生成する
	TArray<FVector> RefinedPoints;
	float SplineLength = SplineComp->GetSplineLength();

	// 始点を追加する
	RefinedPoints.Add(SplineComp->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World));

	// DistanceStep間隔でサンプリングする
	for (float d = DistanceStep; d <= SplineLength; d += DistanceStep)
	{
		// 前のサンプリング点の距離を取得
		float PrevDistance = d - DistanceStep;
		// 前後の接線方向を取得して正規化
		FVector PrevTangent = SplineComp->GetTangentAtDistanceAlongSpline(PrevDistance, ESplineCoordinateSpace::World).GetSafeNormal();
		FVector CurrTangent = SplineComp->GetTangentAtDistanceAlongSpline(d, ESplineCoordinateSpace::World).GetSafeNormal();

		// 接線間の角度差（度）を計算する
		float DotVal = FMath::Clamp(FVector::DotProduct(PrevTangent, CurrTangent), -1.0f, 1.0f);
		float AngleDiff = FMath::RadiansToDegrees(acosf(DotVal));

		// 角度差が設定値を超えた場合、中間点を1回だけ挿入する
		if (AngleDiff > MaxRotationAngle)
		{
			float MidDistance = PrevDistance + (d - PrevDistance) * 0.5f;
			FVector MidPoint = SplineComp->GetLocationAtDistanceAlongSpline(MidDistance, ESplineCoordinateSpace::World);
			RefinedPoints.Add(MidPoint);
		}
		
		// 現在のサンプリング点を追加する
		FVector CurrPoint = SplineComp->GetLocationAtDistanceAlongSpline(d, ESplineCoordinateSpace::World);
		RefinedPoints.Add(CurrPoint);
	}
	
	// 最後の点が含まれていない場合、末尾点を追加する
	FVector EndPoint = SplineComp->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::World);
	if (!EndPoint.Equals(RefinedPoints.Last(), 1.0f))
	{
		RefinedPoints.Add(EndPoint);
	}

	// 細分化後の点を用いてSplineを再構築する
	SplineComp->ClearSplinePoints(false);
	for (const FVector& Pt : RefinedPoints)
	{
		SplineComp->AddSplinePoint(Pt, ESplineCoordinateSpace::World, false);
	}
	SplineComp->UpdateSpline();
}
