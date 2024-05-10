// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/MainMenu.h"
#include "../IronHorizonPlayerPawn.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Items/ItemBase.h"
#include "../PlayerCameraController.h"
#include "../HexTile.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetOwningPlayerPawn());

    SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMainMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}


bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (ItemDragDrop->SourceItem && PlayerPawn)
    {
        PlayerPawn->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController found"));
            // Cast to APlayerCameraController
            APlayerCameraController* PCC = Cast<APlayerCameraController>(PC);
            if (PCC)
            {
                UE_LOG(LogTemp, Warning, TEXT("PlayerCameraController found"));
                // Get the current mouse position from the drag drop event
                FVector2D MousePosition = InDragDropEvent.GetScreenSpacePosition();
                // Convert the mouse screen position to a world space ray
                FCollisionQueryParams TraceParams;
                FHitResult HitResult;
                if (PCC->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, TraceParams, HitResult)) {
                    // Check if the hit actor is a hex tile
                    AHexTile* HexTile = Cast<AHexTile>(HitResult.GetActor());
                    if (HexTile) {
                        // Change the tile color and type
                        HexTile->ChangeToRailway();
                    }
                }
            }
        }
        return true;
    }
    return false;
}