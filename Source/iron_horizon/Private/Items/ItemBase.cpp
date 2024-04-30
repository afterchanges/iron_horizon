// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"
#include "Items/ItemDataStructs.h"

UItemBase::UItemBase() {
}

UItemBase* UItemBase::CreateItemCopy() const {
    UItemBase* NewItem = NewObject<UItemBase>(StaticClass());

    NewItem->ID = this->ID;
    NewItem->Quantity = this->Quantity;
    NewItem->ItemType = this->ItemType;
    NewItem->TextData = this->TextData;
    NewItem->AssetData = this->AssetData;
    NewItem->NumericData = this->NumericData;

    return NewItem;
}

void UItemBase::Use(AHexTile* HexTile) {
    // Placeholder for use function
}

void UItemBase::SetQuantity(const int32 NewQuantity) {
    if (NewQuantity != Quantity) {
        Quantity = FMath::Clamp(NewQuantity, 0, NumericData.bIsStackable ? NumericData.MaxStackSize : 1);

        // if (OwningInventory) {
        //    if (Quantity <= 0) {
        //        OwningInventory->RemoveItem(this);
        //    } 
        // }
    }
}