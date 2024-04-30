// Fill out your copyright notice in the Description page of Project Settings.
#include "UserInterface/IronHorizonHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"

AIronHorizonHUD::AIronHorizonHUD()
{
    bIsMenuVisible = false;
}

void AIronHorizonHUD::BeginPlay()
{
    Super::BeginPlay();
    
    if (MainMenuClass)
    {
        MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
        MainMenuWidget->AddToViewport(5);
        MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (InteractionWidgetClass)
    {
        InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
        InteractionWidget->AddToViewport(-1);
        InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AIronHorizonHUD::DisplayMenu()
{
    if (MainMenuWidget)
    {
        bIsMenuVisible = true;
        MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AIronHorizonHUD::HideMenu()
{
    if (MainMenuWidget)
    {
        bIsMenuVisible = false;
        MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AIronHorizonHUD::ShowInteractionWidget() const
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AIronHorizonHUD::HideInteractionWidget() const
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AIronHorizonHUD::UpdateInteractionWidget(const FInteractableData& InteractableData) const
{
    if (InteractionWidget)
    {
        if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
        {
            InteractionWidget->SetVisibility(ESlateVisibility::Visible);
        }
        InteractionWidget->UpdateWidget(&InteractableData);
    }
}

