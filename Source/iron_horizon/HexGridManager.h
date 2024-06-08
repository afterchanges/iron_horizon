// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "HexTile.h"
#include "RailroadSpline.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

class IronHorizonCity {
public:
    int city_id;
    TSet<IronHorizonCity *> connected_cities;
    AHexTile *this_tile;
};

class IronHorizonRailroadTile {
public:
    TSet<IronHorizonCity *> connected_cities;
    AHexTile *this_tile;
};

UCLASS()

class IRON_HORIZON_API AHexGridManager : public AActor {
    GENERATED_BODY()

protected:
    TArray<TArray<AHexTile *>> HexGridLayout;
    TMap<FIntVector, AHexTile *> HexGridLayoutAxial;
    TMap<FIntVector, IronHorizonRailroadTile *> RailroadTiles;
    TMap<FIntVector, IronHorizonCity *> Cities;
    TArray<ARailroadSpline *> RailroadSplines;

    UPROPERTY(EditAnywhere, Category = "HexGridLayout")
    int32 GridWidth;
    UPROPERTY(EditAnywhere, Category = "HexGridLayout")
    int32 GridHeight;
    UPROPERTY(EditAnywhere, Category = "HexGridManager")
    float HexTileSize;

public:
    AHexGridManager();

    TArray<AHexTile*> generateCities(int numCities);
    TArray<FIntPoint> determineCities();
    bool allCitiesConnected();
    TArray<AHexTile *> HexGridAStar(AHexTile *StartTile, AHexTile *EndTile);

    void generateHexGrid();
    AHexTile *GetTileAtPosition(const FIntPoint &GridPositionIndex);
    TArray<AHexTile *> GetNeighbors(const FIntPoint &GridPositionIndex);
    TArray<AHexTile *> GetPassableNeighbors(const FIntVector& CubeCoordinates);
    TArray<AHexTile *> GetNeighborsOnRingOfRadius(AHexTile *, int32 radius);
    AHexTile* GetTileAtCubeCoordinates(const FIntVector& CubeCoordinates);

    float GetTilePrestige(const FIntVector &GridPositionIndex);
    void SetTilesPrestige();

    void AddNewRailroadTile(AHexTile *NewTile);
    void AddNewCityConnection(AHexTile *city_1, AHexTile *city_2);
    void AddRouteEndpoint(AHexTile *city_1);

    AHexTile *current_city_1 = nullptr;
    AHexTile *current_city_2 = nullptr;

public:
    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> GrassHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> WaterHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> ForestHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> ForestHexTile2;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> ForestHexTile3;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> ForestHexTile4;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> MountainHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> DesertHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> CityHexTile;

    UPROPERTY(EditAnywhere, Category = "HexGridSetup")
    TSubclassOf<AHexTile> RailwayHexTile;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};