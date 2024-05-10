// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryTooltip.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Components/TextBlock.h"

FString ItemTypeToString(EItemType ItemType)
{
    switch (ItemType)
    {
    case EItemType::Surface_Railway:
        return "Surface Railway";
    case EItemType::Tunnel_Railway:
        return "Tunnel Railway";
    default:
        return "Unknown";
    }
}

void UInventoryTooltip::NativeConstruct()
{
    Super::NativeConstruct();

    const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();
    

    switch (ItemBeingHovered->ItemType) {
        case EItemType::Surface_Railway: 
            ItemType->SetText(FText::FromString("Surface Railway"));
            ItemDescription->SetText(FText::FromString("A railway that is built on the surface."));
            break;  
        case EItemType::Tunnel_Railway: 
            ItemType->SetText(FText::FromString("Tunnel Railway"));
            ItemDescription->SetText(FText::FromString("A railway that is built underground."));
            break;
        default:
            ItemType->SetText(ItemBeingHovered->TextData.Name);
            ItemDescription->SetText(ItemBeingHovered->TextData.Description);
            break;
    }
        ItemName->SetText(ItemBeingHovered->TextData.Name);
        Cost->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.Cost));
        ProfitRatio->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ProfitRatio));
        UsageText->SetText(ItemBeingHovered->TextData.UsageText);
        StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));
        SpeedFactor->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.SpeedFactor));

        const FString WeightInfo = { "Weight: " + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };

        StackWeight->SetText(FText::FromString(WeightInfo));

        if (ItemBeingHovered->NumericData.bIsStackable) {
            const FString StackInfo = { "Max stack size: " + FString::FromInt(ItemBeingHovered->NumericData.MaxStackSize) };
            MaxStackSize->SetText(FText::FromString(StackInfo));
        } else {
            MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
        }
}