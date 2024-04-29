// Fill out your copyright notice in the Description page of Project Settings.

#include "../HexTile.h"
#include "UserInterface/MainMenu.h"
#include "../HexTile.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerPawn = Cast<AHexTile>(GetOwningPlayerPawn());
}

void UMainMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    return true;
}

