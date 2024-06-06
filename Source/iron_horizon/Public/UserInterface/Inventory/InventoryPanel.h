// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UWrapBox;
class UTextBlock;
class AIronHorizonPlayerPawn;
class UInventoryComponent;
class UInventoryItemSlot;
/**
 * 
 */
UCLASS()
class IRON_HORIZON_API UInventoryPanel : public UUserWidget
{
public:
	GENERATED_BODY()

	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	AIronHorizonPlayerPawn* PlayerPawn;
	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;
protected:
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};