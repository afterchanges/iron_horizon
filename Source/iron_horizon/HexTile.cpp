// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTile.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <UObject/ConstructorHelpers.h> 

// Sets default values
AHexTile::AHexTile() : TileType(HexTileType::DEFAULT) {
    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    TileMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMaterialAsset(TEXT("/Content/Static/Materials/HighlightMaterial.uasset"));
    UE_LOG(LogTemp, Warning, TEXT("HighlightMaterialAsset.Succeeded() = %d"), HighlightMaterialAsset.Succeeded());
    if (HighlightMaterialAsset.Succeeded()) {
        HighlightMaterial = HighlightMaterialAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RailwayMaterialAsset(TEXT("/Content/Static/Materials/RailwayMaterial.uasset"));
    if (RailwayMaterialAsset.Succeeded()) {
        RailwayMaterial = RailwayMaterialAsset.Object;
    }

    TileMesh->SetGenerateOverlapEvents(true);
    TileMesh->SetNotifyRigidBodyCollision(true);
    TileMesh->OnBeginCursorOver.AddDynamic(this, &AHexTile::OnBeginCursorOver);
    TileMesh->OnEndCursorOver.AddDynamic(this, &AHexTile::OnEndCursorOver);
}

void AHexTile::BeginPlay() {
    Super::BeginPlay();

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    if (PlayerController) {
        EnableInput(PlayerController);
    }
}

void AHexTile::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent) {
    DefaultMaterial = TileMesh->GetMaterial(0);

    TileMesh->SetMaterial(0, HighlightMaterial);
}

void AHexTile::OnEndCursorOver(UPrimitiveComponent* TouchedComponent) {
    TileMesh->SetMaterial(0, DefaultMaterial);
}

void AHexTile::ChangeToRailway() {
    // Change the tile type
    TileType = HexTileType::RAILWAY;

    // Change the tile color to red
    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(RailwayMaterial, this);
    DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
    TileMesh->SetMaterial(0, DynamicMaterial);
}