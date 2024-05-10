// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/MainMenu.h"
#include "../HexTile.h"
#include "../IronHorizonPlayerPawn.h"
#include "../PlayerCameraController.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

void UMainMenu::NativeConstruct() {
  Super::NativeConstruct();

  PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetOwningPlayerPawn());

  SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMainMenu::NativeOnInitialized() { Super::NativeOnInitialized(); }

bool UMainMenu::NativeOnDrop(const FGeometry &InGeometry,
                             const FDragDropEvent &InDragDropEvent,
                             UDragDropOperation *InOperation) {
  const UItemDragDropOperation *ItemDragDrop =
      Cast<UItemDragDropOperation>(InOperation);
  if (ItemDragDrop->SourceItem && PlayerPawn) {
    // Get the item type from the source item
    EItemType ItemType = ItemDragDrop->SourceItem->ItemType;

    APlayerController *PC = GetWorld()->GetFirstPlayerController();
    if (PC) {
      UE_LOG(LogTemp, Warning, TEXT("PlayerController found"));
      // Cast to APlayerCameraController
      APlayerCameraController *PCC = Cast<APlayerCameraController>(PC);
      if (PCC) {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCameraController found"));
        // Get the current mouse position from the drag drop event
        FVector2D MousePosition = InDragDropEvent.GetScreenSpacePosition();
        // Convert the mouse screen position to a world space ray
        FCollisionQueryParams TraceParams;
        FHitResult HitResult;
        if (PCC->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility,
                                              TraceParams, HitResult)) {
          // Check if the hit actor is a hex tile
          AHexTile *HexTile = Cast<AHexTile>(HitResult.GetActor());
          if (HexTile) {
            // Check the tile type
            HexTileType TileType = HexTile->GetTileType();
            // Check if the item type and tile type are compatible
            if ((ItemType == EItemType::Tunnel_Railway &&
                 TileType == HexTileType::MOUNTAIN) ||
                (ItemType == EItemType::Surface_Railway &&
                 TileType != HexTileType::WATER)) {
              PlayerPawn->DropItem(ItemDragDrop->SourceItem,
                                   ItemDragDrop->SourceItem->Quantity);
              HexTile->ChangeToRailway();
            }
          }
        }
      }
    }
    return true;
  }
  return false;
}