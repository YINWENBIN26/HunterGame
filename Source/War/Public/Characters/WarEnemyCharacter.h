// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarCharacterBase.h"
#include "Component/UI/EnemyUIComponent.h"
#include "WarEnemyCharacter.generated.h"

class UWidgetComponent;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class WAR_API AWarEnemyCharacter : public AWarCharacterBase
{
	GENERATED_BODY()
public:
	AWarEnemyCharacter();
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent;}
	//~ Begin UEnemyInterface Interface.
	virtual bool IsEnemyHasTag(FGameplayTag TagToCheck);
	//~ End UEnemyInterface Interface.
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyTag;
	
	virtual void BeginPlay() override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController *NewController) override;
	//~ End APawn Interface

	//~ Begin IPawnCombatInterface Interfac
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interfac
	virtual  UPawnUIComponent* GetPawnUIComponent() const override;
	virtual  UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End IPawnUIInterface Interface
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	UEnemyCombatComponent* EnemyCombatComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;
private:
	void InitEnemyStartUpData();


};
