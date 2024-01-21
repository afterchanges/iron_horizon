// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridManager.h"

// Sets default values
AHexGridManager::AHexGridManager()
{
	

}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay()
{
	Super::BeginPlay();

	HexGridLayout.SetNumZeroed(GridWidth);
	
	for (int32 i = 0; i < HexGridLayout.Num(); i++) {
		HexGridLayout[i].SetNumZeroed(GridHeight);
	}

	for (int32 y = 0; y < GridHeight; ++y) {
		for (int32 x = 0; x < GridWidth; ++x) {

			FVector Location = FVector((y % 2) * sqrt(3) / 2 * HexTileSize + x * sqrt(3) * HexTileSize, y * 1.5 * HexTileSize, 0.0f);
			
			TSubclassOf<AHexTile> TileToSpawn = GrassHexTile;
			AHexTile *NewTile = GetWorld()->SpawnActor<AHexTile>(TileToSpawn, Location, FRotator::ZeroRotator);

			NewTile->GridPositionIndex = FIntPoint(x, y);

			HexGridLayout[x][y] = NewTile;
        }
	}
}

