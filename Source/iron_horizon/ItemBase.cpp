#include "ItemBase.h"
#include "ItemDataStructs.h"

UItemBase::UItemBase() {
}

UItemBase* UItemBase::CreateItemCopy() const {
    UItemBase* NewItem = NewObject<UItemBase>(StaticClass());

    NewItem->ID = this->ID;
    NewItem->ItemType = this->ItemType;
    NewItem->TextData = this->TextData;
    NewItem->AssetData = this->AssetData;
    NewItem->NumericData = this->NumericData;
    NewItem->Quantity = this->Quantity;

    return NewItem;
}

void UItemBase::SetQuantity(int32 NewQuantity) {
    if (NewQuantity != Quantity) {
        Quantity = FMath::Clamp(NewQuantity, 0, NumericData.IsStackable ? NumericData.MaxStackSize : 1);
    }
}

void UItemBase::Use() {
    // Placeholder for use function
}