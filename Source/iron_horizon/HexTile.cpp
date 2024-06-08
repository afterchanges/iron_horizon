// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTile.h"
#include "HexGridManager.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <UObject/ConstructorHelpers.h> 
#include <Materials/MaterialInterface.h>
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
    if (TileType == HexTileType::RAILWAY) {
        TileMesh->SetMaterial(0, RailwayMaterial);
    } else {
        TileMesh->SetMaterial(0, DefaultMaterial);
    }
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
    if (TileType == HexTileType::RAILWAY) { return; }
    
    // Change the tile type to railway
    TileType = HexTileType::RAILWAY;

    // Get the HexGridManager instance
    TSubclassOf<AActor> HexGridManagerClass = AHexGridManager::StaticClass();
    AHexGridManager* HexGridManagerInstance = Cast<AHexGridManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), HexGridManagerClass)
    );
    if (HexGridManagerInstance)
    {
        // Get the RailwayHexTile class from the HexGridManager
        UClass* RailwayHexTileClass = HexGridManagerInstance->RailwayHexTile.Get();
        if (RailwayHexTileClass)
        {
            // Create a temporary instance to access the static mesh
            AHexTile* TempRailwayTile = RailwayHexTileClass->GetDefaultObject<AHexTile>();
            if (TempRailwayTile)
            {
                UStaticMeshComponent* RailwayMeshComponent = TempRailwayTile->TileMesh;
                if (RailwayMeshComponent && RailwayMeshComponent->GetStaticMesh())
                {
                    // Change the tile's static mesh to the railway mesh
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
