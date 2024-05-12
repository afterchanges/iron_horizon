// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "HexTile.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

UCLASS()
class IRON_HORIZON_API AHexGridManager : public AActor {
  GENERATED_BODY()

protected:
	TArray<TArray<AHexTile*>> HexGridLayout;
    TMap<FIntVector, AHexTile*> HexGridLayoutAxial;
    
    UPROPERTY(EditAnywhere, Category = "HexGridLayout")
	int32 GridWidth;
	UPROPERTY(EditAnywhere, Category = "HexGridLayout")
	int32 GridHeight;
	UPROPERTY(EditAnywhere, Category = "HexGridManager")
	float HexTileSize;

public: 
        AHexGridManager();

        void generateCities(int numCities);
        TArray<FIntPoint> determineCities();
        bool allCitiesConnected();
        TArray<FIntPoint> HexGridAStar(AHexTile *StartTile, AHexTile *EndTile,
                                 AHexGridManager *hexGridManagerInstance);

        void generateHexGrid();
        AHexTile* GetTileAtPosition(const FIntPoint &GridPositionIndex);
        TArray<AHexTile *> GetNeighbors(const FIntPoint &GridPositionIndex);
        TArray<AHexTile *> GetNeighborsOnRingOfRadius(AHexTile*, int32 radius);

        float GetTilePrestige(const FIntVector &GridPositionIndex);
        void SetTilesPrestige();

public:
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
  UPROPERTY(EditAnywhere, Category = "HexGridSetup")
  TSubclassOf<AHexTile> CityHexTile;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
};