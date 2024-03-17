#pragma once

#include "CoreMinimal.h"
#include "Items/ItemDataStructs.h"
#include "ItemBase.generated.h"

UCLASS()
class IRON_HORIZON_API UItemBase : public UObject {
    GENERATED_BODY()

public:

	int32 Quantity;

    UPROPERTY(VisibleAnywhere, Category = "Item Data", meta = (UIMin=1, UIMax=100))
    FName ID;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemTextData TextData;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemAssetData AssetData;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemNumericData NumericData;

    UItemBase();

    UFUNCTION(Category = "Item")
    UItemBase* CreateItemCopy() const;

    UFUNCTION(Category = "Item")
    FORCEINLINE void SetQuantity(int32 NewQuantity) {
        if (NewQuantity != Quantity) {
            Quantity = FMath::Clamp(NewQuantity, 0, NumericData.bIsStackable ? NumericData.MaxStackSize : 1);
        }
    }

    UFUNCTION(Category = "Item")
    FORCEINLINE float GetItemStackWeight() const { return NumericData.Weight * Quantity; }
    
    UFUNCTION(Category = "Item")
    FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; }
    
    UFUNCTION(Category = "Item")
    FORCEINLINE bool IsFullItemStack() const {return Quantity == NumericData.MaxStackSize;}

    UFUNCTION(Category = "Item")
    virtual void Use();

protected:

    bool operator==(const FName& OtherID) const {
        return ID == OtherID;
    }

};