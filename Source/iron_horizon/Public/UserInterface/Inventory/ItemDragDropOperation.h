// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "CoreMinimal.h"
#include "ItemDragDropOperation.generated.h"

class UItemBase;
class UInventoryComponent;

/**
 *
 */
UCLASS()
class IRON_HORIZON_API UItemDragDropOperation : public UDragDropOperation {
  GENERATED_BODY()
public:
  UPROPERTY()
  UItemBase *SourceItem;

  UPROPERTY()
  UInventoryComponent *SourceInventory;
};