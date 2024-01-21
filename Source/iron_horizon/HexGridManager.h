// Fill out your copyright notice in the Description page of Project Settings.
#ifndef IRON_HORIZON_HEXGRIDMANAGER_H
#define IRON_HORIZON_HEXGRIDMANAGER_H
#pragma once

#include "HexTile.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

UCLASS()
class IRON_HORIZON_API AHexGridManager : public AActor
{
	GENERATED_BODY()

protected:
	TArray<TArray<AHexTile*>> HexGridLayout;
	UPROPERTY(EditAnywhere, Category = "HexGridLayout")
	int32 GridWidth;
	UPROPERTY(EditAnywhere, Category = "HexGridLayout")
	int32 GridHeight;

	UPROPERTY(EditAnywhere, Category = "HexGridManager")
	float HexTileSize;

	UPROPERTY(EditAnywhere, Category = "HexGridSetup")
	TSubclassOf<AHexTile> GrassHexTile;


public:	
	// Sets default values for this actor's properties
	AHexGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

#endif //IRON_HORIZON_HEXGRIDMANAGER_H