// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemBase.h"
#include "UserInterface/Inventory/InventoryTooltip.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Items/ItemDataStructs.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    switch (ItemReference->ItemQuality)
    {
        case EItemQuality::Common:
            ItemBorder->SetBrushColor(FLinearColor::White);
            break;
        case EItemQuality::Quality:
            ItemBorder->SetBrushColor(FLinearColor::Blue);
            break;
        default: ;
    }
    ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);

    if (ItemReference->NumericData.bIsStackable)
    {
        ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
    }
    else
    {
        ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UInventoryItemSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (ToolTipClass)
    {
        UInventoryTooltip* ToolTip = CreateWidget<UInventoryTooltip>(GetWorld(), ToolTipClass);
        ToolTip->InventorySlotBeingHovered = this;

        SetToolTip(ToolTip);
    }
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}