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

void UItemBase::Use() {
    // Placeholder for use function
}