// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Interaction/InteractionWidget.h"
#include "Interfaces/InteractionInterface.h"

void UInteractionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    KeyPressText->SetText(FText::FromString("Press")); 
    CurrentInteractionDuration = 0.0f;
}

void UInteractionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData)
{
    switch (InteractableData->InteractableType)
    {
        case EInteractableType::Pickup:
            KeyPressText->SetText(FText::FromString("Pickup"));
            InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

            if (InteractableData->Quantity < 2)
            {
                Quantity->SetVisibility(ESlateVisibility::Collapsed);
            }
            else
            {
                Quantity->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "Quantity", "x{0}"), InteractableData->Quantity));
                Quantity->SetVisibility(ESlateVisibility::Visible);
            }
            break;
        case EInteractableType::Move:
            KeyPressText->SetText(FText::FromString("Press"));
            break;
        case EInteractableType::Set:
            KeyPressText->SetText(FText::FromString("Set"));
            break;
        case EInteractableType::Use:
            KeyPressText->SetText(FText::FromString("Use"));
            break;
        case EInteractableType::Container:
            KeyPressText->SetText(FText::FromString("Open"));
            break;
        default: ;
    }
}

float UInteractionWidget::UpdateInteractionProgress()
{
    return CurrentInteractionDuration;
}