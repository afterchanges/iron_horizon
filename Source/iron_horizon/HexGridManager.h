// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include <map>
#include "HexTile.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

UCLASS()

class IRON_HORIZON_API AHexGridManager : public AActor {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "HexGridLayout")
    int32 GridWidth;
    UPROPERTY(EditAnywhere, Category = "HexGridLayout")
    int32 GridHeight;

    UPROPERTY(EditAnywhere, Category = "HexGridManager")
    float HexTileSize;

public:
    TArray<TArray<AHexTile *>> HexGridLayout;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> GrassHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> WaterHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> ForestHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> MountainHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> DesertHexTile;
    // Sets default values for this actor's properties
    AHexGridManager();

    std::vector<std::vector<float>> height_noise_map;
    std::vector<std::vector<float>> moisture_noise_map;
    std::vector<std::vector<float>> temperature_noise_map;
    std::vector<std::vector<bool>> river_noise_map;

    void riverPathfinding(int x, int y);
    void createRivers();
    void generateNoiseMaps();
    void generateHexGrid();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};