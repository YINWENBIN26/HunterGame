// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarWeaponBase.generated.h"

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate,AActor*);
UCLASS()
class WAR_API AWarWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWarWeaponBase();
	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<UBoxComponent> WeaponCollisionBox;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent*OverlappedComponent, AActor *OtherActor
                                           ,UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const
                                           FHitResult &SweepResul);
	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent*OverlappedComponent, AActor *OtherActor,
                                          UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

public:
	FORCEINLINE UBoxComponent *GetWeaponCollisionBox() const { return WeaponCollisionBox; }	

};


