#include "HexGridManager.h"
#include "Math/UnrealMathUtility.h"
#include "PerlinNoise.hpp"
#include "TIntPointHash.h"
// #include "astar.hpp"
#include "Containers/Map.h"

// Sets default values
AHexGridManager::AHexGridManager() {
}

TMap<HexTileType, TArray<float>> TileHeightRanges = {
    {HexTileType::WATER, {0.0f, 0.0f}},
    {HexTileType::GRASS, {0.0f, 0.5f}},
    {HexTileType::FOREST, {0.5f, 1.0f}},
    {HexTileType::MOUNTAIN, {1.0f, 2.0f}},
    {HexTileType::DESERT, {0.0f, 0.5f}}
};

enum class heightTerrainGroup : uint8 {
    waterHeightGroup,
    shorelineHeightGroup,
    plainsHeightGroup,
    hillsHeightGroup,
    mouninousHeightGroup
};

TMap<heightTerrainGroup, TArray<float>>
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

TMap<heightTerrainGroup, std::pair<float, float>>
    heightFeaturesRanges = {
        {heightTerrainGroup::waterHeightGroup, {0.0f, 0.0f}},
        {heightTerrainGroup::shorelineHeightGroup, {0.0f, 0.1f}},
        {heightTerrainGroup::plainsHeightGroup, {0.1f, 1.5f}},
        {heightTerrainGroup::hillsHeightGroup, {1.5f, 2.5f}},
        {heightTerrainGroup::mouninousHeightGroup, {2.5f, 3.0f}}};



TMap<heightTerrainGroup, TArray<HexTileType>>
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
    
TArray<TArray<float>> generatePerlinNoise(
    int32 width,
    int32 height,
    TFunction<float(float x)> functionModifier
) {
    uint32_t random_uint = FMath::Rand();

    const siv::PerlinNoise::seed_type seed = random_uint;
    const siv::PerlinNoise perlin{seed};

    TArray<TArray<float>> noise_map;
    noise_map.Init(TArray<float>(), width);
    for (int32 i = 0; i < width; i++) {
        noise_map[i].Init(0.f, height);
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
    for (int i = 0; i < numCities; ++i) {
        int x, y;
        do {
            x = FMath::RandRange(0, GridWidth - 1);
            y = FMath::RandRange(0, GridHeight - 1);
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

float redistributeHeights(float x) {
    // UE_LOG(LogTemp, Warning, TEXT("Redistributing height %f, got %f"), x, 134.018 * x * x * x * x * x - 388.378 * x * x * x * x + 427.395 * x * x * x - 217.915 * x * x + 52.8798 * x - 5);
    return 134.018 * x * x * x * x * x - 388.378 * x * x * x * x +
           427.395 * x * x * x - 217.915 * x * x + 52.8798 * x - 5;
}

bool AHexGridManager::floodFillStep(FIntPoint current_tile, int current_biome_id, TArray<TArray<FIntPoint>> &biome_tiles) {
    int column_offset = current_tile.X % 2 == 0 ? -1 : 0;
    bool no_tiles_converted = true;
    TArray<FIntPoint> tile_neighbors = {
        {current_tile.X - 1, current_tile.Y},
        {current_tile.X + 1, current_tile.Y},
        {current_tile.X + column_offset + 1, current_tile.Y - 1},
        {current_tile.X + column_offset, current_tile.Y - 1},
        {current_tile.X + column_offset + 1, current_tile.Y + 1},
        {current_tile.X + column_offset, current_tile.Y + 1}
    };

    for (FIntPoint neighbor : tile_neighbors) {
        if (neighbor.X >= 0 && neighbor.X < HexGridBiomesIDs.Num() &&
            neighbor.Y >= 0 && neighbor.Y < HexGridBiomesIDs[neighbor.X].Num() &&
            HexGridBiomesIDs[neighbor.X][neighbor.Y] == 0) {
            HexGridBiomesIDs[neighbor.X][neighbor.Y] = current_biome_id;
            biome_tiles[current_biome_id - 1].Add(neighbor);
            no_tiles_converted = false;
        }
    }

    return no_tiles_converted;
}

bool AHexGridManager::floodFillManager(
    TArray<TArray<FIntPoint>> &biome_center_points,
    TArray<TArray<FIntPoint>> &biome_tiles
) {
    bool all_tiles_covered = true;
    while (!all_tiles_covered) {
        for (int i = 0; i < biome_center_points.Num(); ++i) {
            bool this_biome_covered = true;
            for (int j = 0; j < biome_tiles[i].Num(); ++j) {
                if (!floodFillStep(biome_tiles[i][j], i + 1, biome_tiles)) {
                    this_biome_covered = true;
                };
            }
            if (!this_biome_covered) {
                all_tiles_covered = false;
            }
        }
    }
    return all_tiles_covered;
    
}

void AHexGridManager::distributeBiomes() {
    TArray<TArray<FIntPoint>> biome_center_points;
    TArray<TArray<FIntPoint>> biome_tiles;
    int x, y;
    for (int i = 0; i < NumBiomes; ++i) {
        x = FMath::RandRange(0, GridWidth - 1);
        y = FMath::RandRange(0, GridHeight - 1);
        biome_center_points.Add({x, y});
        HexGridBiomesIDs[x][y] = i + 1;
        biome_tiles[i].Add({x, y});
    }

    floodFillManager(biome_center_points, biome_tiles);
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay() {
    TMap<HexTileType, TSubclassOf<AHexTile>> TileTypeMap = {
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

    TArray<TArray<float>> noise_height_map = generatePerlinNoise(
        GridWidth, GridHeight, redistributeHeights
    );

    // std::vector<std::vector<float>> forest_feature_map =
    //     generatePerlinNoise(GridWidth, GridHeight, redistributeHeights);

    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            HexTileType spawnTileType =
                terrainTilesByHeight[heightTileGroupDecider(noise_height_map[x][y])][0];
            float newTileHeightAdjusted =
                FMath::RandRange(0.f, 1.f) *
                    (TileHeightRanges[spawnTileType][1] -
                     TileHeightRanges[spawnTileType][0]) +
                TileHeightRanges[spawnTileType][0];

            newTileHeightAdjusted = noise_height_map[x][y] >= 0.0f
                                       ? noise_height_map[x][y]
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
                // if (!HexGridAStar(
                //          HexGridLayout[cities[i].X][cities[i].Y],
                //          HexGridLayout[cities[j].X][cities[j].Y], this
                //     )
                //          .Num()) {
                //     allCitiesConnected = false;
                //     break;
                // }
                continue;
            }
            if (!allCitiesConnected) {
                break;
            }
        }

        if (allCitiesConnected) {
            for (FIntPoint city : cities) {
                AHexTile *tile = HexGridLayout[city.X][city.Y];
                if (tile) {
                    tile->Destroy();  // Despawn the current tile

                    // Spawn a new tile of CITY class
                    FVector Location = tile->GetActorLocation();
                    UWorld *World = GetWorld();
                    if (World) {
                        AHexTile *NewTile = World->SpawnActor<AHexTile>(
                            CityHexTile, Location, FRotator::ZeroRotator
                        );
                        if (NewTile) {
                            NewTile->GridPositionIndex = FIntPoint(city.X, city.Y);
                            HexGridLayout[city.X][city.Y] = NewTile;
                            UE_LOG(
                                LogTemp, Warning, TEXT("City at (%d, %d)"), city.X, city.Y
                            );
                        }
                    }
                }
            }
        }
    }
}

void AHexGridManager::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (GetWorld()->GetTimeSeconds() - InteractionData.LastInteractionCheckTime >
        InteractionData.InteractionCheckFrequency) {
        PerformInteractionCheck();
    }
}

void AHexGridManager::PerformInteractionCheck() {
    
}

void AHexGridManager::FoundInteractable(AActor *NewInteractable) {
    
}

void AHexGridManager::NoInteractableFound() {
    
}


