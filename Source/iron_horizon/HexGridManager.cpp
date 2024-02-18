// Fill out your copyright notice in the Description page of Project Settings.
#include "HexGridManager.h"
#include <cmath>
#include <iostream>
#include <random>
#include <unordered_map>
#include "PerlinNoise.hpp"

// Sets default values
AHexGridManager::AHexGridManager() {
}

float GenerateRandomFloat(float left_border, float right_border) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(left_border, right_border);
    return dis(gen);
}

std::unordered_map<HexTileType, std::pair<float, float>> TileHeightRanges = {
    {HexTileType::WATER, {0.0f, 0.0f}},
    {HexTileType::GRASS, {0.0f, 0.5f}},
    {HexTileType::FOREST, {0.5f, 1.0f}},
    {HexTileType::MOUNTAIN, {1.0f, 2.0f}},
    {HexTileType::DESERT, {0.0f, 0.5f}}
};

HexTileType getTileTypeByHeight(float h) {
    if (h < 1.0f) {
        return HexTileType::WATER;
    } else if (h < 1.5f) {
        return HexTileType::GRASS;
    } else if (h < 2.0f) {
        return HexTileType::FOREST;
    } else if (h < 2.5f) {
        return HexTileType::MOUNTAIN;
    } else {
        return HexTileType::DESERT;
    }
}

std::vector<std::vector<float>> generatePerlinNoise(int32 width, int32 height) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    uint32_t random_uint = dis(gen);

    const siv::PerlinNoise::seed_type seed = random_uint;

    const siv::PerlinNoise perlin{seed};

    std::vector<std::vector<float>> noise_map;

    noise_map.resize(width);
    for (int32 i = 0; i < width; i++) {
        noise_map[i].resize(height);
    }

    for (int32 y = 0; y < height; y++) {
        for (int32 x = 0; x < width; x++) {
            noise_map[x][y] =
                perlin.octave2D_01(
                    (x * 0.05), (y * 0.05), std::sqrt(width * height)
                ) *
                3;
        }
    }

    return noise_map;
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay() {
    std::map<HexTileType, TSubclassOf<AHexTile>> TileTypeMap = {
        {HexTileType::GRASS, GrassHexTile},
        {HexTileType::WATER, WaterHexTile},
        {HexTileType::FOREST, ForestHexTile},
        {HexTileType::MOUNTAIN, MountainHexTile},
        {HexTileType::DESERT, DesertHexTile},
    };

    Super::BeginPlay();

    HexGridLayout.SetNumZeroed(GridWidth);

    for (int32 i = 0; i < HexGridLayout.Num(); i++) {
        HexGridLayout[i].SetNumZeroed(GridHeight);
    }

    std::vector<std::vector<float>> noise_map =
        generatePerlinNoise(GridWidth, GridHeight);

    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            HexTileType spawnTileType = getTileTypeByHeight(noise_map[x][y]);
            float newTileHeightAdjusted =
                GenerateRandomFloat(0.0f, 1.0f) *
                    (TileHeightRanges[spawnTileType].second -
                     TileHeightRanges[spawnTileType].first) +
                TileHeightRanges[spawnTileType].first;

            // UE_LOG(
            //     LogTemp, Warning, TEXT("Tile at (%d, %d) is of type %d"), x,
            //     y, (int32)spawnTileType
            // );

            FVector Location = FVector(
                (y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize,
                y * 1.5 * HexTileSize, newTileHeightAdjusted * 200.0f
            );

            TSubclassOf<AHexTile> TileToSpawn = TileTypeMap[spawnTileType];

            AHexTile *NewTile = GetWorld()->SpawnActor<AHexTile>(
                TileToSpawn, Location, FRotator::ZeroRotator
            );

            NewTile->GridPositionIndex = FIntPoint(x, y);

            HexGridLayout[x][y] = NewTile;
        }
    }
}
