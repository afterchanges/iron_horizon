// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UItemBase* UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemIn)
{
	InventoryContents.RemoveSingle(ItemIn);
	OnInventoryUpdated.Broadcast();

	return ItemIn;
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ItemIn->GetItemSingleWeight() * ActualAmountToRemove;

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
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
