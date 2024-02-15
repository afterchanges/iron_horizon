// Fill out your copyright notice in the Description page of Project Settings.
#include "HexGridManager.h"
#include <cmath>
#include <functional>
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
    {HexTileType::DESERT, {0.0f, 0.5f}}};

enum class heightTerrainGroup : uint8 {
    waterHeightGroup,
    shorelineHeightGroup,
    plainsHeightGroup,
    hillsHeightGroup,
    mouninousHeightGroup
};

std::unordered_map<heightTerrainGroup, std::vector<float>>
    heightFeaturesThresholds = {
        {heightTerrainGroup::waterHeightGroup, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {heightTerrainGroup::shorelineHeightGroup,
         {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {heightTerrainGroup::plainsHeightGroup, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {heightTerrainGroup::hillsHeightGroup, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {heightTerrainGroup::mouninousHeightGroup,
         {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}};

heightTerrainGroup heightTileGroupDecider(float h) {
    if (h < 0) {
        return heightTerrainGroup::waterHeightGroup;
    } else if (h < 0.1) {
        return heightTerrainGroup::shorelineHeightGroup;
    } else if (h < 1.5) {
        return heightTerrainGroup::plainsHeightGroup;
    } else if (h < 2.5) {
        return heightTerrainGroup::hillsHeightGroup;
    } else {
        return heightTerrainGroup::mouninousHeightGroup;
    }
}

std::unordered_map<heightTerrainGroup, std::pair<float, float>>
    heightFeaturesRanges = {
        {heightTerrainGroup::waterHeightGroup, {0.0f, 0.0f}},
        {heightTerrainGroup::shorelineHeightGroup, {0.0f, 0.1f}},
        {heightTerrainGroup::plainsHeightGroup, {0.1f, 1.5f}},
        {heightTerrainGroup::hillsHeightGroup, {1.5f, 2.5f}},
        {heightTerrainGroup::mouninousHeightGroup, {2.5f, 3.0f}}};



std::unordered_map<heightTerrainGroup, std::vector<HexTileType>>
    terrainTilesByHeight = {
        {heightTerrainGroup::waterHeightGroup,
         {
             HexTileType::WATER,
         }},
        {heightTerrainGroup::shorelineHeightGroup,
         {
             HexTileType::DESERT,
         }},
        {heightTerrainGroup::plainsHeightGroup,
         {
             HexTileType::GRASS,
         }},
        {heightTerrainGroup::hillsHeightGroup,
         {
             HexTileType::FOREST,
         }},
        {heightTerrainGroup::mouninousHeightGroup,
         {
             HexTileType::MOUNTAIN,
         }}};

enum class climateTerrainGroup : uint8 {
    equatorialClimate,
    tropicalClimate,
    subtropicalClimate,
    mediterranianClimate,
    continentalClimate,
    oceanicClimate,
    polarClimate,
    highlandClimate
};

HexTileType tileTypeDecider(float h) {
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

std::vector<std::vector<float>> generatePerlinNoise(
    int32 width,
    int32 height,
    std::function<float(float)> functionModifier
) {
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
            noise_map[x][y] = functionModifier(perlin.octave2D_01(
                    (x * 0.05), (y * 0.05), std::sqrt(width * height)
                ));
        }
    }
    return noise_map;
}

float redistributeHeights(float x) {
    UE_LOG(LogTemp, Warning, TEXT("Redistributing height %f, got %f"), x, 134.018 * x * x * x * x * x - 388.378 * x * x * x * x + 427.395 * x * x * x - 217.915 * x * x + 52.8798 * x - 5);
    return 134.018 * x * x * x * x * x - 388.378 * x * x * x * x +
           427.395 * x * x * x - 217.915 * x * x + 52.8798 * x - 5;
}

void AHexGridManager::generateNoiseMaps() {
    height_noise_map =
        generatePerlinNoise(GridWidth, GridHeight, redistributeHeights);

    moisture_noise_map =
        generatePerlinNoise(GridWidth, GridHeight, [](float x) { return x; });

    temperature_noise_map =
        generatePerlinNoise(GridWidth, GridHeight, [](float x) { return x; });
    
    river_noise_map.resize(GridWidth);
    for (int32 i = 0; i < GridWidth; i++) {
        river_noise_map[i].resize(GridHeight, false);
    }
}

void biomeBFS(int tile_count, int current_biome_id, int x, int y){};

void assignBiomes(auto HexGridLayout, int32 GridWidth, int32 GridHeight) {
    int biome_count = 0;
    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            if (!(HexGridLayout[x][y]->wasVisitedByBiomeBFS)) {
                biomeBFS(0, biome_count, x, y);
                biome_count++;
            }
        }
    }
}

void AHexGridManager::riverPathfinding(int x, int y) {
    std::vector<std::pair<int, int>> HexDirections = {
        {0, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}};
    auto current_height = height_noise_map[x][y];
    bool found_next_segment = false;
    river_noise_map[x][y] = true;
    if (current_height < 0.0f) {
        return;
    }

    std::pair<int, int> next_river_segment;
    float next_river_segment_height = 1000.0f;
    UE_LOG(LogTemp, Warning, TEXT("River pathfinding at (%d, %d)"), x, y);
    for (int i = 0; i < 6; i++) {
        int new_x = x + HexDirections[i].first;
        int new_y = y + HexDirections[i].second;
        if (new_x >= 0 && new_x < GridWidth && new_y >= 0 &&
            new_y < GridHeight) {
            if (river_noise_map[new_x][new_y]) {
                continue;
            }
            if (height_noise_map[new_x][new_y] < current_height &&
                height_noise_map[new_x][new_y] < next_river_segment_height) {
                next_river_segment = std::make_pair(new_x, new_y);
                found_next_segment = true;
                next_river_segment_height = height_noise_map[new_x][new_y];
            }
        }
    }

    if (found_next_segment) {
        riverPathfinding(
            next_river_segment.first, next_river_segment.second
        );
    } else {
        return;
        // HexGridLayout[x][y]->isRiverEnd = true;
    }
};

void AHexGridManager::createRivers() {
    int river_count = sqrt(sqrt(GridWidth * GridHeight));
    for (int i = 0; i < river_count; i++) {
        int x = rand() % GridWidth;
        int y = rand() % GridHeight;
        while (height_noise_map[x][y] < 0.0f || river_noise_map[x][y]) {
            x = rand() % GridWidth;
            y = rand() % GridHeight;
        }
        height_noise_map[x][y] -= 0.1f;
        riverPathfinding(x, y);
    }
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

    generateNoiseMaps();
    // createRivers();

    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            HexTileType spawnTileType =
                terrainTilesByHeight[heightTileGroupDecider(height_noise_map[x][y])][0];
            float newTileHeightAdjusted =
                GenerateRandomFloat(0.0f, 1.0f) *
                    (TileHeightRanges[spawnTileType].second -
                     TileHeightRanges[spawnTileType].first) +
                TileHeightRanges[spawnTileType].first;

            if (HexGridLayout[x][y]->isRiverSegment) {
                spawnTileType = HexTileType::WATER;
                newTileHeightAdjusted = height_noise_map[x][y] >= 0.0f
                                           ? height_noise_map[x][y]
                                           : 0.0f;
            }

            newTileHeightAdjusted = height_noise_map[x][y] >= 0.0f
                                       ? height_noise_map[x][y]
                                       : 0.0f;

            // UE_LOG(
            //     LogTemp, Warning, TEXT("Tile at (%d, %d) is of type %d"), x, y,
            //     (int32)spawnTileType
            // );

            FVector Location = FVector(
                (y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize,
                y * 1.5 * HexTileSize, newTileHeightAdjusted * 500.0f
            );

            TSubclassOf<AHexTile> TileToSpawn = TileTypeMap[spawnTileType];

            AHexTile *NewTile = GetWorld()->SpawnActor<AHexTile>(
                TileToSpawn, Location, FRotator::ZeroRotator
            );

            NewTile->GridPositionIndex = FIntPoint(x, y);
            NewTile->isWater = heightTileGroupDecider(height_noise_map[x][y]) == heightTerrainGroup::waterHeightGroup ? true : false;
            NewTile->TileType = spawnTileType;

            HexGridLayout[x][y] = NewTile;
        }
    }
}