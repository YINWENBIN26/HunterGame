// Valerii Diachenko All Rights Reserved.


#include "Widgets/WarWidgetBase.h"

#include "Interface/UI/PawnUIInterface.h"

void UWarWidgetBase::InitEnemyCreatedWidgets(AActor* OwningEnemyActor)
{
    if (const IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
    {
        UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();
        checkf(EnemyUIComponent, TEXT("Failed to extract an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel());
        BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
    }
}

void UWarWidgetBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (const IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
    {
        if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
        {
            BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
        }
    }
}