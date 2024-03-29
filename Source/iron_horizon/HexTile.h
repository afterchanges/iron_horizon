// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

class UStaticMeshComponent;

UENUM()
enum class HexTileType : uint8
{
	DEFAULT,
	GRASS,
	FOREST,
	MOUNTAIN,
	WATER,
	DESERT,
	CITY,
	MAX UMETA(Hidden)
};

UCLASS()
class IRON_HORIZON_API AHexTile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HexTile")
	FIntPoint GridPositionIndex;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HexTile")
    FIntPoint NormalizedGridPositionIndex;

	void SetColor(FLinearColor NewColor);

	void SetTileType(HexTileType NewType);

	HexTileType GetTileType() const;

protected:
	bool hasForest = false;
	bool isRiverStart = false;
	bool isRiverEnd = false;
	bool isRiverSegment = false;
	bool isRailroad = false;
	bool isCity = false;
	bool isWater = false;
	bool isBuildable = false;

	int prestige = 0;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
	HexTileType TileType;


protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HexTile")
	UStaticMeshComponent* TileMesh;
	
public:    
	AHexTile();

};