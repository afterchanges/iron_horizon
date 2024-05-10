// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemBase.h"
#include "UserInterface/Inventory/InventoryTooltip.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Items/ItemDataStructs.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UserInterface/Inventory/DragItemVisual.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "../PlayerCameraController.h"
#include "../HexTile.h"

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
    FReply Reply =  Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    if (DragItemVisualClass) {
        UWorld* World = GetWorld();
        if (World) {
            const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, TSubclassOf<UUserWidget>(DragItemVisualClass));
            DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
            DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
            DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));

            UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
            DragItemOperation->SourceItem = ItemReference;
            DragItemOperation->SourceInventory = ItemReference->OwningInventory;

            DragItemOperation->DefaultDragVisual = DragVisual;
            DragItemOperation->Pivot = EDragPivot::TopLeft;

            OutOperation = DragItemOperation;
        }
    }
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

     APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // Cast to APlayerCameraController
        APlayerCameraController* PCC = Cast<APlayerCameraController>(PC);
        if (PCC)
        {
            // Call the function
            PCC->OnJKeyPressed();
        }
    }

    return true;
}