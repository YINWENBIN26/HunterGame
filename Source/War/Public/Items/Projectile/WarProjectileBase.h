// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "WarProjectileBase.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EProjectileDamagePolicy:uint8
{
	OnHit,
	OnBeginOverlap,
};
UCLASS()
class WAR_API AWarProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UBoxComponent* ProjectileCollisionBox;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	EProjectileDamagePolicy ProjectileDamagePolicy=EProjectileDamagePolicy::OnHit;
	UPROPERTY(BlueprintReadOnly,Category="Projectile",meta=(ExposeOnSpawn="true"))
	FGameplayEffectSpecHandle ProjectileDamageSpecHandle;
	UFUNCTION()
	virtual  void OnprojectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;
//可以在BP中设置
	UFUNCTION(BlueprintImplementableEvent,meta =(DisplayName="On Spawn Projectile Hit FX"))
	void BP_OnSpawnProjectileFX(const FVector& HitLocation);
private:
	void HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayload);

	TArray<AActor*> OverlayppedActors;
};
