// Fill out your copyright notice in the Description page of Project Settings.
#include <random>
#include "HexGridManager.h"

// Sets default values
AHexGridManager::AHexGridManager() {
}

float GenerateRandomFloat(float left_border, float right_border) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(left_border, right_border);
    return dis(gen);
}

#include <unordered_map>

// Define the minimum and maximum height for each tile type
std::unordered_map<HexTileType, std::pair<float, float>> TileHeightRanges = {
    {HexTileType::WATER, {0.0f, 0.0f}},
    {HexTileType::GRASS, {0.0f, 0.5f}},
    {HexTileType::FOREST, {0.5f, 1.0f}},
    {HexTileType::MOUNTAIN, {1.0f, 2.0f}},
    {HexTileType::DESERT, {0.0f, 0.5f}}};

HexTileType getTileTypeByHeight(float h) {
    if (h < 0.0f) {
        return HexTileType::WATER;
    } else if (h < 0.5f) {
        return HexTileType::GRASS;
    } else if (h < 1.0f) {
        return HexTileType::FOREST;
    } else if (h < 1.5f) {
        return HexTileType::MOUNTAIN;
    } else {
        return HexTileType::DESERT;
    }
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay() {
    Super::BeginPlay();

    HexGridLayout.SetNumZeroed(GridWidth);

    for (int32 i = 0; i < HexGridLayout.Num(); i++) {
        HexGridLayout[i].SetNumZeroed(GridHeight);
    }

    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            float randomised_height = GenerateRandomFloat(-1.0f, 2.0f);
            HexTileType spawnTileType = getTileTypeByHeight(randomised_height);
            float newTileHeightAdjusted =
                GenerateRandomFloat(0.0f, 1.0f) *
                    (TileHeightRanges[spawnTileType].second -
                     TileHeightRanges[spawnTileType].first) +
                TileHeightRanges[spawnTileType].first;

            FVector Location = FVector(
                (y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize,
                y * 1.5 * HexTileSize, newTileHeightAdjusted * 100.0f
            );

            TSubclassOf<AHexTile> TileToSpawn = GrassHexTile;
            AHexTile *NewTile = GetWorld()->SpawnActor<AHexTile>(
                TileToSpawn, Location, FRotator::ZeroRotator
            );

            NewTile->GridPositionIndex = FIntPoint(x, y);

            HexGridLayout[x][y] = NewTile;
        }
    }
}
