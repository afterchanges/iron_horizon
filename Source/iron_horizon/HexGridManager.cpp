#include "HexGridManager.h"
#include "Containers/Map.h"
#include "Math/UnrealMathUtility.h"
#include "PerlinNoise.hpp"
#include "RailroadSpline.h"
#include "TIntPointHash.h"
#include "astar.hpp"

// Sets default values
AHexGridManager::AHexGridManager() {
}

TMap<HexTileType, TArray<float>> TileHeightRanges = {
    {HexTileType::WATER,    {0.0f, 0.0f}},
    {HexTileType::GRASS,    {0.0f, 0.5f}},
    {HexTileType::FOREST,   {0.5f, 1.0f}},
    {HexTileType::MOUNTAIN, {1.0f, 2.0f}},
    {HexTileType::DESERT,   {0.0f, 0.5f}}
};

//! order is the same as on unit circle (from right, anti-clockwise)
TArray<FIntPoint> AxialNeighbors = {
    FIntPoint(1, 0),
    FIntPoint(1, -1),
    FIntPoint(0, -1),
    FIntPoint(-1, 0),
    FIntPoint(-1, 1),
    FIntPoint(0, 1)};

TArray<float> PrestigeRangeInfluenceModifier = {1.0f, 0.7f, 0.5f, 0.2f};

TMap<HexTileType, float> TilePrestige = {
    {HexTileType::WATER,    6.0f },
    {HexTileType::GRASS,    4.0f },
    {HexTileType::FOREST,   7.0f },
    {HexTileType::MOUNTAIN, 10.0f},
    {HexTileType::DESERT,   2.0f }
};

enum class heightTerrainGroup : uint8 {
    waterHeightGroup,
    shorelineHeightGroup,
    plainsHeightGroup,
    hillsHeightGroup,
    mouninousHeightGroup
};

TMap<heightTerrainGroup, TArray<float>> heightFeaturesThresholds = {
    {heightTerrainGroup::waterHeightGroup,     {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
    {heightTerrainGroup::shorelineHeightGroup, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
    {heightTerrainGroup::plainsHeightGroup,    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
    {heightTerrainGroup::hillsHeightGroup,     {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
    {heightTerrainGroup::mouninousHeightGroup, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}
};

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

TMap<heightTerrainGroup, std::pair<float, float>> heightFeaturesRanges = {
    {heightTerrainGroup::waterHeightGroup,     {0.0f, 0.0f}},
    {heightTerrainGroup::shorelineHeightGroup, {0.0f, 0.1f}},
    {heightTerrainGroup::plainsHeightGroup,    {0.1f, 1.5f}},
    {heightTerrainGroup::hillsHeightGroup,     {1.5f, 2.5f}},
    {heightTerrainGroup::mouninousHeightGroup, {2.5f, 3.0f}}
};

TMap<heightTerrainGroup, TArray<HexTileType>> terrainTilesByHeight = {
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
     }}
};

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

HexTileType AHexTile::GetTileType() const {
    if (TileType < HexTileType::DEFAULT || TileType >= HexTileType::MAX) {
        UE_LOG(LogTemp, Error, TEXT("Invalid TileType"));
        return HexTileType::DEFAULT;
    }
    return TileType;
}

TArray<TArray<float>>
generatePerlinNoise(int32 width, int32 height, TFunction<float(float x)> functionModifier) {
    uint32_t random_uint = FMath::Rand();

    const siv::PerlinNoise::seed_type seed = random_uint;
    const siv::PerlinNoise perlin{seed};

    TArray<TArray<float>> noise_map;
    noise_map.Init(TArray<float>(), width);
    for (int32 i = 0; i < width; i++) { noise_map[i].Init(0.f, height); }

    for (int32 y = 0; y < height; y++) {
        for (int32 x = 0; x < width; x++) {
            noise_map[x][y] = functionModifier(
                perlin.octave2D_01((x * 0.05), (y * 0.05), std::sqrt(width * height))
            );
        }
    }
    return noise_map;
}

AHexTile *AHexGridManager::GetTileAtPosition(const FIntPoint &GridPositionIndex) {
    if (GridPositionIndex.X < 0 || GridPositionIndex.X >= HexGridLayout.Num() ||
        GridPositionIndex.Y < 0 ||
        GridPositionIndex.Y >= HexGridLayout[GridPositionIndex.X].Num()) {
        return nullptr;
    }
    return HexGridLayout[GridPositionIndex.X][GridPositionIndex.Y];
}

AHexTile* AHexGridManager::GetTileAtCubeCoordinates(const FIntVector& CubeCoordinates) {
    AHexTile** TilePtr = HexGridLayoutAxial.Find(CubeCoordinates);
    return TilePtr ? *TilePtr : nullptr;
}


TArray<AHexTile*> AHexGridManager::generateCities(int numCities) {
    TArray<AHexTile*> cities;
    for (int i = 0; i < numCities; ++i) {
        int x, y;
        do {
            x = FMath::RandRange(0, GridWidth - 1);
            y = FMath::RandRange(0, GridHeight - 1);
        } while (x >= HexGridLayout.Num() || y >= HexGridLayout[x].Num() || !HexGridLayout[x][y] ||
                 HexGridLayout[x][y]->GetTileType() == HexTileType::CITY);

        if (HexGridLayout[x][y]) { cities.Add(HexGridLayout[x][y]); }
    }
    return cities;
}

TArray<FIntPoint> AHexGridManager::determineCities() {
    TArray<FIntPoint> cities;
    for (int32 x = 0; x < GridWidth; ++x) {
        for (int32 y = 0; y < GridHeight; ++y) {
            if (x < HexGridLayout.Num() && y < HexGridLayout[x].Num() && HexGridLayout[x][y] &&
                HexGridLayout[x][y]->GetTileType() == HexTileType::CITY) {
                cities.Add(FIntPoint(x, y));
            }
        }
    }
    return cities;
}

float AHexGridManager::GetTilePrestige(const FIntVector &GridPositionIndex) {
    return TilePrestige[HexGridLayoutAxial[GridPositionIndex]->GetTileType()];
}

void AHexGridManager::SetTilesPrestige() {
    for (int32 x = 0; x < GridWidth; ++x) {
        for (int32 y = 0; y < GridHeight; ++y) {
            AHexTile *current_tile = HexGridLayout[x][y];
            for (int32 radius = 0; radius < 1; radius++) {
                for (FIntPoint &neighbor : AxialNeighbors) {
                    const FIntVector neighbor_cube_coords = FIntVector(
                        current_tile->CubeCoordinates.X + neighbor.X,
                        current_tile->CubeCoordinates.Y + neighbor.Y,
                        current_tile->CubeCoordinates.Z - neighbor.X - neighbor.Y
                    );
                    if (HexGridLayoutAxial.Contains(neighbor_cube_coords)) {
                        // UE_LOG(
                        //     LogTemp,
                        //     Warning,
                        //     TEXT("Neighbor at (%d, %d) has prestige value of (%f)"),
                        //     neighbor_cube_coords.X,
                        //     neighbor_cube_coords.Y,
                        //     GetTilePrestige(neighbor_cube_coords)
                        // );
                        current_tile->prestige += GetTilePrestige(neighbor_cube_coords) *
                                                  PrestigeRangeInfluenceModifier[radius];
                    }
                }
            }
            // UE_LOG(
            //     LogTemp,
            //     Warning,
            //     TEXT("Tile at (%d, %d) has prestige %f"),
            //     x,
            //     y,
            //     current_tile->prestige
            // );
        }
    }
}

void setTlieCubeCoordinates(AHexTile *tile) {
    int x = tile->GridPositionIndex.X;
    int y = tile->GridPositionIndex.Y;
    tile->CubeCoordinates = FIntVector(x, y, -x - y);
}

float redistributeHeights(float x) {
    // UE_LOG(LogTemp, Warning, TEXT("Redistributing height %f, got %f"), x,
    // 134.018 * x * x * x * x * x - 388.378 * x * x * x * x + 427.395 * x * x * x
    // - 217.915 * x * x + 52.8798 * x - 5);
    return 134.018 * x * x * x * x * x - 388.378 * x * x * x * x + 427.395 * x * x * x -
           217.915 * x * x + 52.8798 * x - 5;
}

void AHexGridManager::AddNewRailroadTile(AHexTile *NewTile) {
    IronHorizonRailroadTile *NewRailroadTile = new IronHorizonRailroadTile();
    NewRailroadTile->this_tile = NewTile;
    for (auto neighbor : GetNeighbors(NewTile->GridPositionIndex)) {
        if (neighbor->GetTileType() == HexTileType::CITY) {
            NewRailroadTile->connected_cities.Add(Cities[neighbor->CubeCoordinates]);
        }
        if (neighbor->GetTileType() == HexTileType::RAILWAY) {
            NewRailroadTile->connected_cities.Append(
                RailroadTiles[neighbor->CubeCoordinates]->connected_cities
            );
        }
    }

    RailroadTiles.Add(NewTile->CubeCoordinates, NewRailroadTile);
    for (auto city_1 : NewRailroadTile->connected_cities) {
        for (auto city_2 : NewRailroadTile->connected_cities) {
            if (city_1 != city_2 && !city_1->connected_cities.Contains(city_2)) {
                city_1->connected_cities.Add(city_2);
                city_2->connected_cities.Add(city_1);
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Connecting city %d to city %d"),
                    city_1->this_tile->GridPositionIndex.X,
                    city_2->this_tile->GridPositionIndex.X
                );
                AddNewCityConnection(
                    HexGridLayoutAxial[city_1->this_tile->CubeCoordinates],
                    HexGridLayoutAxial[city_2->this_tile->CubeCoordinates]
                );
            }
        }
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("New railroad tile at (%d, %d) has %d connected cities"),
        NewTile->GridPositionIndex.X,
        NewTile->GridPositionIndex.Y,
        NewRailroadTile->connected_cities.Num()
    );
}

void AHexGridManager::AddNewCityConnection(AHexTile *city_1, AHexTile *city_2) {
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Adding connection between city %d and city %d"),
        city_1->GridPositionIndex.X,
        city_2->GridPositionIndex.X);
    if (city_1->GetTileType() != HexTileType::CITY || city_2->GetTileType() != HexTileType::CITY) {
        UE_LOG(LogTemp, Warning, TEXT("type of city_1: %d"), (int32)city_1->GetTileType());
        UE_LOG(LogTemp, Warning, TEXT("type of city_2: %d"), (int32)city_2->GetTileType());
        UE_LOG(LogTemp, Warning, TEXT("Not a city"));
        return;
    }
    TArray<AHexTile *> path = HexGridAStar(city_1, city_2);
    if (path.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("No path found"));
        return;
    }

    TArray<FVector3d> path_central_points;
    for (auto point : path) { 
        path_central_points.Add(point->GetActorLocation()); 
        path_central_points.Last().Z += 500.0f;
    }
    
    ARailroadSpline *NewSpline =
        GetWorld()->SpawnActor<ARailroadSpline>(ARailroadSpline::StaticClass());
    NewSpline->SetRailroadSplinePoints(path_central_points);
    NewSpline->BeginMovement();
    RailroadSplines.Add(NewSpline);
}

void AHexGridManager::AddRouteEndpoint(AHexTile *city_1) {
    if (city_1->GetTileType() != HexTileType::CITY) {
        UE_LOG(LogTemp, Warning, TEXT("Not a city"));
        return;
    }
    if (current_city_1 == nullptr) {
        current_city_1 = city_1;
        UE_LOG(LogTemp, Warning, TEXT("City 1 set"));
    } else if (current_city_2 == nullptr) {
        current_city_2 = city_1;
        UE_LOG(LogTemp, Warning, TEXT("City 2 set"));
        AddNewCityConnection(current_city_1, current_city_2);
        current_city_1 = nullptr;
        current_city_2 = nullptr;
    }
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay() {
    TMap<HexTileType, TSubclassOf<AHexTile>> TileTypeMap = {
        {HexTileType::GRASS,    GrassHexTile   },
        {HexTileType::WATER,    WaterHexTile   },
        {HexTileType::FOREST,   ForestHexTile  },
        {HexTileType::MOUNTAIN, MountainHexTile},
        {HexTileType::DESERT,   DesertHexTile  },
        {HexTileType::CITY,     CityHexTile    },
    };

    Super::BeginPlay();

    HexGridLayout.SetNumZeroed(GridWidth);

    for (int32 i = 0; i < HexGridLayout.Num(); i++) { HexGridLayout[i].SetNumZeroed(GridHeight); }

    TArray<TArray<float>> noise_height_map =
        generatePerlinNoise(GridWidth, GridHeight, redistributeHeights);

    // std::vector<std::vector<float>> forest_feature_map =
    //     generatePerlinNoise(GridWidth, GridHeight, redistributeHeights);

    for (int32 y = 0; y < GridHeight; ++y) {
        for (int32 x = 0; x < GridWidth; ++x) {
            HexTileType spawnTileType =
                terrainTilesByHeight[heightTileGroupDecider(noise_height_map[x][y])][0];
            float newTileHeightAdjusted =
                FMath::RandRange(0.f, 1.f) *
                    (TileHeightRanges[spawnTileType][1] - TileHeightRanges[spawnTileType][0]) +
                TileHeightRanges[spawnTileType][0];

            newTileHeightAdjusted = noise_height_map[x][y] >= 0.0f ? noise_height_map[x][y] : 0.0f;

            // UE_LOG(
            //     LogTemp, Warning, TEXT("Tile at (%d, %d) is of type %d"), x, y,
            //     (int32)spawnTileType
            // );

            FVector Location = FVector(
                (y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize,
                y * 1.5 * HexTileSize,
                newTileHeightAdjusted * 500.0f
            );

            TSubclassOf<AHexTile> TileToSpawn = TileTypeMap[spawnTileType];

            if (TileToSpawn) {
                UWorld *World = GetWorld();
                if (World) {
                    AHexTile *NewTile =
                        World->SpawnActor<AHexTile>(TileToSpawn, Location, FRotator::ZeroRotator);
                    if (NewTile) {
                        NewTile->GridPositionIndex = FIntPoint(x, y);

                        NewTile->CubeCoordinates =
                            FIntVector(x - (y - (y & 1)) / 2, y, -(x - (y - (y & 1)) / 2) - y);
                        HexGridLayout[x][y] = NewTile;
                        HexGridLayoutAxial.Add(NewTile->CubeCoordinates, NewTile);
                    }
                } else {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor at (%d, %d)"), x, y);
                }
            } else {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("No class specified for tile type %d"),
                    (int32)spawnTileType
                );
            }
        }
    }

    SetTilesPrestige();

    TArray<AHexTile*> cities;
    bool allCitiesConnected = false;
    while (!allCitiesConnected) {
        cities = generateCities(5);  // Generate 5 cities
        // cities = determineCities();
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
            if (!allCitiesConnected) { break; }
        }

        if (allCitiesConnected) {
            for (AHexTile* city : cities) {
                UE_LOG(LogTemp, Warning, TEXT("Trying to spawn a city at (%d, %d)"), city->GridPositionIndex.X, city->GridPositionIndex.Y);
                if (city) {
                    city->Destroy();  // Despawn the current tile

                    // Spawn a new tile of CITY class
                    FVector Location = city->GetActorLocation();
                    UWorld *World = GetWorld();
                    if (World) {
                        AHexTile *NewTile = World->SpawnActor<AHexTile>(
                            CityHexTile, Location, FRotator::ZeroRotator
                        );
                        if (NewTile) {
                            NewTile->GridPositionIndex = FIntPoint(city->GridPositionIndex.X, city->GridPositionIndex.Y);
                            NewTile->CubeCoordinates = city->CubeCoordinates;
                            UE_LOG(LogTemp, Warning, TEXT("Tile type is %d"), (int32)NewTile->GetTileType());
                            NewTile->SetTileType(HexTileType::CITY);
                            UE_LOG(LogTemp, Warning, TEXT("New tile type is %d"), (int32)NewTile->GetTileType());
                            HexGridLayout[city->GridPositionIndex.X][city->GridPositionIndex.Y] = NewTile;
                            UE_LOG(LogTemp, Warning, TEXT("City at (%d, %d)"), city->GridPositionIndex.X, city->GridPositionIndex.Y);
                            HexGridLayoutAxial.Remove(city->CubeCoordinates);
                            HexGridLayoutAxial.Add(NewTile->CubeCoordinates, NewTile);
                        }
                        IronHorizonCity *NewCity = new IronHorizonCity();
                        NewCity->this_tile = NewTile;
                        NewCity->connected_cities = TSet<IronHorizonCity *>();
                        NewCity->connected_cities.Add(NewCity);
                        Cities.Add(NewTile->CubeCoordinates, NewCity);
                        UE_LOG(LogTemp, Warning, TEXT("Added city to map"));
                    }
                }
            }
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Number of cities: %d"), Cities.Num());
    }
}