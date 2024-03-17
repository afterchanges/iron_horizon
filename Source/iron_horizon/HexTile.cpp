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

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMaterialAsset(TEXT("/iron_horizon/Content/Static/Materials/HighlightMaterial.uasset"));
    UE_LOG(LogTemp, Warning, TEXT("HighlightMaterialAsset.Succeeded() = %d"), HighlightMaterialAsset.Succeeded());
    if (HighlightMaterialAsset.Succeeded()) {
        HighlightMaterial = HighlightMaterialAsset.Object;
    }

    TileMesh->SetGenerateOverlapEvents(true);
    TileMesh->SetNotifyRigidBodyCollision(true);
    TileMesh->OnBeginCursorOver.AddDynamic(this, &AHexTile::OnBeginCursorOver);
    TileMesh->OnEndCursorOver.AddDynamic(this, &AHexTile::OnEndCursorOver);
}

void AHexTile::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent) {
    DefaultMaterial = TileMesh->GetMaterial(0);

    TileMesh->SetMaterial(0, HighlightMaterial);
}

void AHexTile::OnEndCursorOver(UPrimitiveComponent* TouchedComponent) {
    TileMesh->SetMaterial(0, DefaultMaterial);
}
