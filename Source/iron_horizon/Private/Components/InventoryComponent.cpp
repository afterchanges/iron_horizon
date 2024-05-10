// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Engine/DataTable.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AddMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AddMesh"));
	AddMesh->SetSimulatePhysics(true);
	if (AActor* Owner = GetOwner())
{
    Owner->SetRootComponent(AddMesh);
}
}

void UInventoryComponent::InitializeAdd(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);

		ItemReference->ID = ItemData->ID;
		ItemReference->Quantity = InQuantity;
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->TextData = ItemData->TextData;
		ItemReference->AssetData = ItemData->AssetData;
		ItemReference->NumericData = ItemData->NumericData;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		AddMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void UInventoryComponent::InitializeDrop(UItemBase*ItemToDrop, const int32 InQuantity)
{
	if(ItemToDrop != nullptr) {
        ItemReference = ItemToDrop;
        InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
        ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
        AddMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

        UpdateInteractableData();
    }
}

void UInventoryComponent::UpdateInteractableData()
{
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;

	InteractableData = InstanceInteractableData;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeAdd(UItemBase::StaticClass(), ItemQuantity);
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		for (UItemBase* Item : InventoryContents)
		{
			if (Item->ID == ItemIn->ID)
			{
				return Item;
			}
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByPredicate([&ItemIn](const TObjectPtr<UItemBase>& InventoryItem) { return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack(); }))
	{
		return *Result;
	}
	return nullptr;
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemIn)
{
	InventoryContents.RemoveSingle(ItemIn);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ItemIn->GetItemSingleWeight() * ActualAmountToRemove;

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}


void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		// If the item is a copy or a pickup, we can just add it directly
		NewItem = Item;
		Item->ResetItemFlags();
	}
	else
	{
		// Used when splitting or dragging items from one inventory to another
		NewItem = Item->CreateItemCopy();
	}

	if (ItemDataTable && !NewItem->ID.IsNone())
    {
        const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(NewItem->ID, NewItem->ID.ToString());

        NewItem->ItemType = ItemData->ItemType;
		NewItem->TextData = ItemData->TextData;
		NewItem->NumericData = ItemData->NumericData;
    }

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}


// bool UInventoryComponent::CanAffordTile(UItemBase* Tile, int32 Quantity)
// {
//    int32 TotalCost = Tile->GetCost() * Quantity;
//
//    return PlayerScore >= TotalCost;
// }

// void UInventoryComponent::DeductCost(UItemBase* Tile, int32 Quantity)
// {
//     if (CanAffordTile(Tile, Quantity))
//     {
//         int32 TotalCost = Tile->GetCost() * Quantity;
//         PlayerScore -= TotalCost;
//     }
// }
