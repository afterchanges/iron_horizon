// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTile.h"
#include "HexGridManager.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <UObject/ConstructorHelpers.h> 
#include <Materials/MaterialInterface.h>
#include "UserInterface/Inventory/InventoryTooltip.h"
#include "UserInterface/MoneyWidget.h"
#include "Items/ItemBase.h"
#include "UserInterface/IronHorizonHUD.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHexTile::AHexTile() : TileType(HexTileType::DEFAULT) {
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));

    TileMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMaterialAsset(
        TEXT("/Game/Static/Materials/HighlightMaterial")
    );
    // UE_LOG(LogTemp, Warning, TEXT("HighlightMaterialAsset.Succeeded() = %d"),
    // HighlightMaterialAsset.Succeeded());
    if (HighlightMaterialAsset.Succeeded()) { HighlightMaterial = HighlightMaterialAsset.Object; }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RailwayMaterialAsset(
        TEXT("/Game/Static/Materials/RailwayMaterial")
    );
    if (RailwayMaterialAsset.Succeeded()) { RailwayMaterial = RailwayMaterialAsset.Object; }

    TileMesh->SetGenerateOverlapEvents(true);
    TileMesh->SetNotifyRigidBodyCollision(true);
    TileMesh->OnBeginCursorOver.AddDynamic(this, &AHexTile::OnBeginCursorOver);
    TileMesh->OnEndCursorOver.AddDynamic(this, &AHexTile::OnEndCursorOver);
}

void AHexTile::AddPrestige(int prestige_value) {
    prestige += prestige_value;
}

AHexTile *AHexTile::StartTile = nullptr;
AHexTile *AHexTile::EndTile = nullptr;

void AHexTile::OnBeginCursorOver(UPrimitiveComponent *TouchedComponent) {
    // UE_LOG(
    //     LogTemp,
    //     Warning,
    //     TEXT("Highlighted tile: %s"),
    //     GridPositionIndex.ToString().GetCharArray().GetData()
    // );
    DefaultMaterial = TileMesh->GetMaterial(0);

    TileMesh->SetMaterial(0, HighlightMaterial);
}

void AHexTile::OnEndCursorOver(UPrimitiveComponent *TouchedComponent) {
    TileMesh->SetMaterial(0, DefaultMaterial);
}

FString AHexTile::HexTileTypeToString(HexTileType Type) {
    switch (Type) {
        case HexTileType::DEFAULT:
            return "DEFAULT";
        case HexTileType::RAILWAY:
            return "RAILWAY";
        default:
            return "UNKNOWN";
    }
}

void AHexTile::SetTileType(HexTileType NewType) {
    TileType = NewType;
}

void AHexTile::ChangeToRailway()
{
    if (TileType == HexTileType::RAILWAY || TileType == HexTileType::CITY || TileType == HexTileType::WATER) {
        return;
    }
    // Get the player's current money
    AIronHorizonPlayerPawn* PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
    FString PlayerMoneyString = PlayerPawn->MoneyWidget->CurrentCurrency->GetText().ToString();
    int32 PlayerMoney = FCString::Atoi(*PlayerMoneyString);

    int32 RailwayCost;
    if (TileType == HexTileType::MOUNTAIN) {
        // tunnel railway
        RailwayCost = PlayerPawn->PlayerInventory->InventoryContents[1]->NumericData.Cost;
    } else {
        // surface railway
        RailwayCost = PlayerPawn->PlayerInventory->InventoryContents[0]->NumericData.Cost;
    }

    // Check if the player has enough money
    if (PlayerMoney < RailwayCost) {
        UE_LOG(LogTemp, Error, TEXT("Not enough money to build railway"));
        return;
    }

    // Deduct the cost of the railway from the player's money
    PlayerMoney -= RailwayCost;

    // Update the player's money
    PlayerPawn->UpdateMoney(-RailwayCost);

    // Change the tile type to railway
    TileType = HexTileType::RAILWAY;

    // Get the HexGridManager instance
    TSubclassOf<AActor> HexGridManagerClass = AHexGridManager::StaticClass();
    AHexGridManager* HexGridManagerInstance = Cast<AHexGridManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), HexGridManagerClass)
    );
    if (HexGridManagerInstance)
    {
        UClass* RailwayHexTileClass = HexGridManagerInstance->RailwayHexTile.Get();
        if (RailwayHexTileClass)
        {
            AHexTile* TempRailwayTile = RailwayHexTileClass->GetDefaultObject<AHexTile>();
            if (TempRailwayTile)
            {
                UStaticMeshComponent* RailwayMeshComponent = TempRailwayTile->TileMesh;
                if (RailwayMeshComponent && RailwayMeshComponent->GetStaticMesh())
                {
                    TileMesh->SetStaticMesh(RailwayMeshComponent->GetStaticMesh());
                    UE_LOG(LogTemp, Warning, TEXT("Tile mesh changed to Railway"));

                    // Optionally change the material if RailwayMaterial is set
                    // if (RailwayMaterial)
                    // {
                    //     UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(RailwayMaterial, this);
                    //     if (DynamicMaterial)
                    //     {
                    //         DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
                    //         TileMesh->SetMaterial(0, DynamicMaterial);
                    //     }
                    //     else
                    //     {
                    //         UE_LOG(LogTemp, Warning, TEXT("Failed to create dynamic material instance"));
                    //     }
                    // }
                    // else
                    // {
                    //     UE_LOG(LogTemp, Warning, TEXT("RailwayMaterial is not set"));
                    // }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to get static mesh from RailwayHexTile"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create default object for RailwayHexTile"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("RailwayHexTile class is not set in HexGridManager"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get HexGridManager"));
    }
}
