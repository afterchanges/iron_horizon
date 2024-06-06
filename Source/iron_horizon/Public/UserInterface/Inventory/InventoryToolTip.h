// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryToolTip.generated.h"

class UInventoryItemSlot;
class UTextBlock;

UCLASS()
class IRON_HORIZON_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* InventorySlotBeingHovered;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackSizeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ProfitRatio;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Cost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackWeight;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeedFactor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStackSize;


protected:
	virtual void NativeConstruct() override;

};
