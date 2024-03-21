// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "HexTile.h"
#include <map>

#include "CoreMinimal.h"
#include "Interfaces/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

USTRUCT()
struct FInteractionData {
    GENERATED_USTRUCT_BODY()

    FInteractionData() :
        CurrentInteractable(nullptr), 
        LastInteractionCheckTime(0.0f),
        InteractionCheckFrequency(0.01f)
    {};

    UPROPERTY()
    AActor* CurrentInteractable;

    UPROPERTY()
    float LastInteractionCheckTime;

    UPROPERTY()
    float InteractionCheckFrequency;
};

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

    virtual void Tick(float DeltaTime) override;

public: 
        AHexGridManager();
        void generateCities(int numCities);
        TArray<FIntPoint> determineCities();
        bool allCitiesConnected();
        void generateHexGrid();
        AHexTile* GetTileAtPosition(const FIntPoint &GridPositionIndex);
        TArray<AHexTile *> GetNeighbors(const FIntPoint &GridPositionIndex);

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


        /** Interaction. */
        void SetHighlight(bool bHighlight);

        void PerformInteractionCheck();

        void FoundInteractable(AActor* NewInteractable);

        void NoInteractableFound();

        bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

        UPROPERTY(VisibleAnywhere, Category = "Interaction")
        TScriptInterface<IInteractionInterface> TargetInteractable;

        UPROPERTY(VisibleAnywhere, Category = "Interaction")
        TScriptInterface<IInteractionInterface> CurrentInteractable;

        FTimerHandle TimerHandle_Interaction;

        FInteractionData InteractionData;
        

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};