#include "HexGridManager.h"
#include <__ranges/size.h>
#include <cmath>
#include <iostream>
#include <random>
#include <unordered_map>
#include "PerlinNoise.hpp"
#include "TIntPointHash.h"
#include "astar.hpp"

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
    {HexTileType::WATER, {0.0f, 0.0f}},  {HexTileType::GRASS, {0.0f, 0.5f}},
    {HexTileType::FOREST, {0.5f, 1.0f}}, {HexTileType::MOUNTAIN, {1.0f, 2.0f}},
    {HexTileType::DESERT, {0.0f, 0.5f}}, {HexTileType::CITY, {0.0f, 0.5f}}
};

HexTileType getTileTypeByHeight(float h) {
    if (h < 1.0f) {
        return HexTileType::WATER;
    } else if (h < 1.5f) {
        return HexTileType::GRASS;
    } else if (h < 1.8f) {
        return HexTileType::CITY;
    } else if (h < 2.0f) {
        return HexTileType::FOREST;
    } else if (h < 2.5f) {
        return HexTileType::MOUNTAIN;
    } else {
        return HexTileType::DESERT;
    }
}

void AHexTile::SetTileType(HexTileType NewType) {
    TileType = NewType;
}

HexTileType AHexTile::GetTileType() const {
    if (TileType < HexTileType::DEFAULT || TileType >= HexTileType::MAX) {
        UE_LOG(LogTemp, Error, TEXT("Invalid TileType"));
        return HexTileType::DEFAULT;
    }
    return TileType;
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

AHexTile *AHexGridManager::GetTileAtPosition(const FIntPoint &GridPositionIndex
) {
    if (GridPositionIndex.X < 0 || GridPositionIndex.X >= HexGridLayout.Num() ||
        GridPositionIndex.Y < 0 ||
        GridPositionIndex.Y >= HexGridLayout[GridPositionIndex.X].Num()) {
        return nullptr;
    }
    return HexGridLayout[GridPositionIndex.X][GridPositionIndex.Y];
}

void AHexGridManager::generateCities(int numCities) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disWidth(0, GridWidth - 1);
    std::uniform_int_distribution<> disHeight(0, GridHeight - 1);

    for (int i = 0; i < numCities; ++i) {
        int x, y;
        do {
            x = disWidth(gen);
            y = disHeight(gen);
        } while (x >= HexGridLayout.Num() || y >= HexGridLayout[x].Num() ||
                 !HexGridLayout[x][y] ||
                 HexGridLayout[x][y]->GetTileType() == HexTileType::CITY);

        if (HexGridLayout[x][y]) {
            HexGridLayout[x][y]->SetTileType(HexTileType::CITY);
        }
    }
}

TArray<FIntPoint> AHexGridManager::determineCities() {
    TArray<FIntPoint> cities;
    for (int32 x = 0; x < GridWidth; ++x) {
        for (int32 y = 0; y < GridHeight; ++y) {
            if (x < HexGridLayout.Num() && y < HexGridLayout[x].Num() &&
                HexGridLayout[x][y] &&
                HexGridLayout[x][y]->GetTileType() == HexTileType::CITY) {
                cities.Add(FIntPoint(x, y));
            }
        }
    }
    return cities;
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay() {
    std::map<HexTileType, TSubclassOf<AHexTile>> TileTypeMap = {
        {HexTileType::GRASS, GrassHexTile},
        {HexTileType::WATER, WaterHexTile},
        {HexTileType::FOREST, ForestHexTile},
        {HexTileType::MOUNTAIN, MountainHexTile},
        {HexTileType::DESERT, DesertHexTile},
        {HexTileType::CITY, CityHexTile},
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

            UE_LOG(
                LogTemp, Warning, TEXT("Tile at (%d, %d) is of type %d"), x, y,
                (int32)spawnTileType
            );

            FVector Location = FVector(
                (y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize,
                y * 1.5 * HexTileSize, newTileHeightAdjusted * 200.0f
            );

            TSubclassOf<AHexTile> TileToSpawn = TileTypeMap[spawnTileType];

            if (TileToSpawn) {
                UWorld *World = GetWorld();
                if (World) {
                    AHexTile *NewTile = World->SpawnActor<AHexTile>(
                        TileToSpawn, Location, FRotator::ZeroRotator
                    );
                    if (NewTile) {
                        NewTile->GridPositionIndex = FIntPoint(x, y);
                        HexGridLayout[x][y] = NewTile;
                    }
                } else {
                    UE_LOG(
                        LogTemp, Warning,
                        TEXT("Failed to spawn actor at (%d, %d)"), x, y
                    );
                }
            } else {
                UE_LOG(
                    LogTemp, Warning,
                    TEXT("No class specified for tile type %d"),
                    (int32)spawnTileType
                );
            }
        }
    }

    TArray<FIntPoint> cities;
    bool allCitiesConnected = false;
    while (!allCitiesConnected) {
        generateCities(5);  // Generate 5 cities
        cities = determineCities();
        allCitiesConnected = true;

        for (size_t i = 0; i < cities.Num(); ++i) {
            for (size_t j = i + 1; j < cities.Num(); ++j) {
                if (!HexGridAStar(
                         HexGridLayout[cities[i].X][cities[i].Y],
                         HexGridLayout[cities[j].X][cities[j].Y], this
                    )
                         .Num()) {
                    allCitiesConnected = false;
                    break;
                }
            }
            if (!allCitiesConnected) {
                break;
            }
        }

        if (allCitiesConnected) {
            for (FIntPoint city : cities) {
                AHexTile *tile = HexGridLayout[city.X][city.Y];
                tile->SetTileType(HexTileType::CITY);
            }
        }
    }
}
