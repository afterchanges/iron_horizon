#pragma once

#include "CoreMinimal.h"
#include "Items/ItemDataStructs.h"
#include "../IronHorizonPlayerPawn.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

UCLASS()
class IRON_HORIZON_API UItemBase : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    UInventoryComponent* OwningInventory;

    UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

    UPROPERTY(EditAnywhere, Category = "Item")
    FName ID;

    UPROPERTY(EditAnywhere, Category = "Item")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemTextData TextData;

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemAssetData AssetData;

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemNumericData NumericData;

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemStatistics ItemStatistics;


    UItemBase();

    UFUNCTION(Category = "Item")
    UItemBase* CreateItemCopy() const;

    UFUNCTION(Category = "Item")
    void SetQuantity(const int32 NewQuantity);

    UFUNCTION(Category = "Item")
    FORCEINLINE float GetItemStackWeight() const { return NumericData.Weight * Quantity; }
    
    UFUNCTION(Category = "Item")
    FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; }
    
    UFUNCTION(Category = "Item")
    FORCEINLINE bool IsFullItemStack() const {return Quantity == NumericData.MaxStackSize;}

    UFUNCTION(Category = "Item")
    virtual void Use(AIronHorizonPlayerPawn* PlayerPawn);

    bool bIsPickup;
    bool bIsCopy;

    void ResetItemFlags();

protected:

    bool operator==(const FName& OtherID) const {
        return this->ID == OtherID;
    }

};