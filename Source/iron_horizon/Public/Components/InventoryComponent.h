// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Interfaces/InteractionInterface.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;
class UDataTable;

USTRUCT(BlueprintType)
struct FItemAddResult {
  GENERATED_BODY()

  FItemAddResult() : ActualAmountAdded(0) {}

  int32 ActualAmountAdded;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IRON_HORIZON_API UInventoryComponent : public UActorComponent,
                                             public IInteractionInterface {
  GENERATED_BODY()

public:
  FOnInventoryUpdated OnInventoryUpdated;

  UInventoryComponent();

  UFUNCTION(Category = "Inventory")
  UItemBase *FindMatchingItem(UItemBase *ItemIn) const;
  UFUNCTION(Category = "Inventory")
  UItemBase *FindNextItemByID(UItemBase *ItemIn) const;
  UFUNCTION(Category = "Inventory")
  UItemBase *FindNextPartialStack(UItemBase *ItemIn) const;

  UFUNCTION(Category = "Inventory")
  void RemoveSingleInstanceOfItem(UItemBase *ItemIn);
  UFUNCTION(Category = "Inventory")
  int32 RemoveAmountOfItem(UItemBase *ItemIn, int32 DesiredAmountToRemove);

  UFUNCTION(Category = "Inventory")
  void SplitExistingStack(UItemBase *ItemIn, const int32 AmountToSplit);

  UFUNCTION(Category = "Inventory")
  void AddNewItem(UItemBase *Item, const int32 AmountToAdd);

  UFUNCTION(Category = "Inventory")
  FORCEINLINE float GetInventoryTotalWeight() const {
    return InventoryTotalWeight;
  }
  UFUNCTION(Category = "Inventory")
  FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; }
  UFUNCTION(Category = "Inventory")
  FORCEINLINE float GetWeightCapacity() const {
    return InventoryWeightCapacity;
  }
  UFUNCTION(Category = "Inventory")
  FORCEINLINE TArray<UItemBase *> GetInventoryContents() const {
    return InventoryContents;
  }
  UFUNCTION(Category = "Inventory")
  FORCEINLINE UItemBase *GetItemData() const { return ItemReference; }

  UFUNCTION(Category = "Inventory")
  FORCEINLINE void SetSlotsCapacity(int32 NewSlotsCapacity) {
    InventorySlotsCapacity = NewSlotsCapacity;
  }
  UFUNCTION(Category = "Inventory")
  FORCEINLINE void SetWeightCapacity(float NewWeightCapacity) {
    InventoryWeightCapacity = NewWeightCapacity;
  }

  UPROPERTY(EditInstanceOnly, Category = "Item Initialization")
  UDataTable *ItemDataTable;

  UPROPERTY(VisibleAnywhere, Category = "Item Initialization")
  FName DesiredItemID;

  UPROPERTY(VisibleAnywhere, Category = "Item Reference")
  UItemBase *ItemReference;

  UPROPERTY(VisibleAnywhere, Category = "Interaction")
  FInteractableData InstanceInteractableData;

  UPROPERTY(EditInstanceOnly, Category = "Item Initialization")
  int32 ItemQuantity;

  UPROPERTY(VisibleAnywhere, Category = "Components")
  UStaticMeshComponent *AddMesh;

  void InitializeAdd(const TSubclassOf<UItemBase> BaseClass,
                     const int32 InQuantity);

  void InitializeDrop(UItemBase *ItemToDrop, const int32 InQuantity);

  void UpdateInteractableData();

  // UFUNCTION(Category = "Inventory")
  // bool CanAffordTile(UItemBase* Tile, int32 Quantity);

  // UFUNCTION(Category = "Inventory")
  // void DeductCost(UItemBase* Tile, int32 Quantity);

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere, Category = "Inventory")
  float InventoryTotalWeight;

  UPROPERTY(EditInstanceOnly, Category = "Inventory")
  int32 InventorySlotsCapacity;

  UPROPERTY(EditInstanceOnly, Category = "Inventory")
  float InventoryWeightCapacity;

  UPROPERTY(VisibleAnywhere, Category = "Inventory")
  TArray<TObjectPtr<UItemBase>> InventoryContents;

public:
  // Called every frame
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;
};
