// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/MainMenu.h"
#include "../IronHorizonPlayerPawn.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetOwningPlayer());
}

void UMainMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    // cast operation to item drag drop, ensure player is valid, call drop item
}

