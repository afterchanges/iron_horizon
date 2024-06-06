// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Components/InventoryComponent.h"
#include "../IronHorizonPlayerPawn.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void UInventoryPanel::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetOwningPlayerPawn());

    if (PlayerPawn)
    {
        InventoryReference = PlayerPawn->GetInventory();
        if (InventoryReference)
        {
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
            SetInfoText();
    }
}
}

void UInventoryPanel::SetInfoText() const
{
    const FString WeightInfoValue{ FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + "/" + FString::SanitizeFloat(InventoryReference->GetWeightCapacity()) };

    const FString CapacityInfoValue{ FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/" + FString::FromInt(InventoryReference->GetSlotsCapacity()) };

    WeightInfo->SetText(FText::FromString(WeightInfoValue));
    CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

void UInventoryPanel::RefreshInventory()
{       
    if (InventoryReference && InventorySlotClass) {
        InventoryPanel->ClearChildren();
        for (UItemBase* const& Item : InventoryReference->GetInventoryContents())
        {
            UInventoryItemSlot* Slot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
            Slot->SetItemReference(Item);
            InventoryPanel->AddChildToWrapBox(Slot);
        }
        UE_LOG(LogTemp, Warning, TEXT("Inventory refreshed"));
        SetInfoText();
    }
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    // cast operation to item drag drop, ensure player is valid, call drop item
}


