// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

class UStaticMeshComponent;

UENUM()
enum class HexTileType : uint8
{
	INVALID,
	GRASS,
	FOREST,
	MOUNTAIN,
	WATER,
	DESERT,
	MAX UMETA(Hidden)
};

UCLASS()
class IRON_HORIZON_API AHexTile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HexTile")
	FIntPoint GridPositionIndex;
	std::vector<int> CubicGridPositionIndex;

	bool isForest = false;
	bool isRiverStart = false;
	bool isRiverEnd = false;
	bool isRiverSegment = false;
	bool isRailroad = false;
	bool isCity = false;
	bool isWater = false;
	bool isBuildable = false;

	bool wasVisitedByBiomeBFS = false;

	int biome_id = -1;
	int prestige = 0;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
	HexTileType TileType;


protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
	UStaticMeshComponent* TileMesh;
	
public:	
	AHexTile();
};