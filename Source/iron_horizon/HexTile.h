// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

class UStaticMeshComponent;

UENUM()
enum class HexTileType : uint8 {
    INVALID,
    GRASS,
    FOREST,
    MOUNTAIN,
    WATER,
    DESERT,
    MAX UMETA(Hidden)
};

UCLASS()

class IRON_HORIZON_API AHexTile : public AActor {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HexTile")
    FIntPoint GridPositionIndex;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HexTile")
    FIntPoint NormalizedGridPositionIndex;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
    HexTileType TileType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
    UStaticMeshComponent *TileMesh;

public:
    AHexTile();
};