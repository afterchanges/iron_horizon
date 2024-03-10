// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "HexTile.h"
#include "InteractableInterface.h"
#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

USTRUCT()
struct FInteractionData {   // info about the interaction
    GENERATED_USTRUCT_BODY()

    FInteractionData() :
        CurrentInteractable(nullptr),
        LastInteractionTime(0.0f)
    {};

    UPROPERTY()
    AActor* CurrentInteractable;    // for checking if the interactable is still in focus

    UPROPERTY()
    float LastInteractionTime;
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

    UPROPERTY(VisibleAnywhere, Category = "Interaction")
    TScriptInterface<IInteractionInterface> TargetInteractable;

    float InteractionCheckFrequency;

    FTimerHandle InteractionCheckTimer;

    FInteractionData InteractionData;

    // that acting on
    void FoundNewInteractable(AActor* NewInteractable);
    void NoInteractableFound();
    void Interact();

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
        // Sets default values for this actor's properties
	AHexGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};