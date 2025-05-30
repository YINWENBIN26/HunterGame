// Valerii Diachenko All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate, float, NewPercent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WAR_API UPawnUIComponent : public UPawnExtensionComponentBase
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable)
    FOnPercentChangedDelegate OnCurrentHealthPercentChanged;
};