// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAR_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	

protected:

	template<class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value, "'T'Template 必须来自 APawn");
		return CastChecked<T>(GetOwner());
	}

	APawn*  GetOwningPawn() const{return GetOwningPawn<APawn>();}

	template<class T>
	T* GetOwningController()const
	{
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value, "'T'Template 必须来自 AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
