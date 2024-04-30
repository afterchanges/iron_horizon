// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTile.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <UObject/ConstructorHelpers.h> 
#include "UserInterface/IronHorizonHUD.h"

// Sets default values
AHexTile::AHexTile() : TileType(HexTileType::DEFAULT) {
    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    TileMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMaterialAsset(TEXT("/Game/Static/Materials/HighlightMaterial"));
    // UE_LOG(LogTemp, Warning, TEXT("HighlightMaterialAsset.Succeeded() = %d"), HighlightMaterialAsset.Succeeded());
    if (HighlightMaterialAsset.Succeeded()) {
        HighlightMaterial = HighlightMaterialAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RailwayMaterialAsset(TEXT("/Game/Static/Materials/RailwayMaterial"));
    if (RailwayMaterialAsset.Succeeded()) {
        RailwayMaterial = RailwayMaterialAsset.Object;
    }

    TileMesh->SetGenerateOverlapEvents(true);
    TileMesh->SetNotifyRigidBodyCollision(true);
    TileMesh->OnBeginCursorOver.AddDynamic(this, &AHexTile::OnBeginCursorOver);
    TileMesh->OnEndCursorOver.AddDynamic(this, &AHexTile::OnEndCursorOver);

    PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(TileMesh);

    InteractionCheckFrequency = 0.1f;
}

AHexTile* AHexTile::StartTile = nullptr;
AHexTile* AHexTile::EndTile = nullptr;

void AHexTile::BeginPlay() {
    Super::BeginPlay();

    HUD = Cast<AIronHorizonHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

    InteractableData = InstanceInteractableData;

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    if (PlayerController) {
        EnableInput(PlayerController);
        PlayerController->bShowMouseCursor = true; 
        PlayerController->bEnableClickEvents = true; 
        PlayerController->bEnableMouseOverEvents = true;
    }
}

void AHexTile::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent) {
    UE_LOG(LogTemp, Warning, TEXT("Highlighted tile: %s"), GridPositionIndex.ToString().GetCharArray().GetData());
    DefaultMaterial = TileMesh->GetMaterial(0);

    TileMesh->SetMaterial(0, HighlightMaterial);
}

void AHexTile::OnEndCursorOver(UPrimitiveComponent* TouchedComponent) {
    if (TileType == HexTileType::RAILWAY) {
        TileMesh->SetMaterial(0, RailwayMaterial);
    } else {
        TileMesh->SetMaterial(0, DefaultMaterial);
    }
}

FString AHexTile::HexTileTypeToString(HexTileType Type) {
    switch (Type) {
        case HexTileType::DEFAULT:
            return "DEFAULT";
        case HexTileType::RAILWAY:
            return "RAILWAY";
        default:
            return "UNKNOWN";
    }
}

void AHexTile::ChangeToRailway() {
    if (TileType == HexTileType::RAILWAY) {
        return;
    }
    // Change the tile type to railway
    TileType = HexTileType::RAILWAY;

    // Change the tile color to red
    if (RailwayMaterial) {
        UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(RailwayMaterial, this);
        if (DynamicMaterial) {
            DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
            TileMesh->SetMaterial(0, DynamicMaterial);
        } else {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create dynamic material instance"));
        }
    } else {
        UE_LOG(LogTemp, Warning, TEXT("RailwayMaterial is not set"));
    }
}

void AHexTile::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (IsInteracting()) {
        PerformInteractionCheck();
    }
}

void AHexTile::Interact() {
    Interact(this);
}

void AHexTile::BeginInteract() {
    // Verify nothing has changed with the interactable state since beginning interaction
    PerformInteractionCheck();
    if (InteractionData.CurrentInteractable) {
        if (IsValid(TargetInteractable.GetObject())) {
            TargetInteractable->BeginInteract();
        }
        if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f)) {
            Interact();
        } else {
            GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &AHexTile::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
        }
    }
}

void AHexTile::EndInteract() {
    GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    if (IsValid(TargetInteractable.GetObject())) {
        TargetInteractable->EndInteract();
    }
}

void AHexTile::Interact(AHexTile* HexTile) {
    GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    if (IsValid(TargetInteractable.GetObject())) {
        TargetInteractable->Interact(this);
    }
}

void AHexTile::FoundInteractable(AHexTile* NewInteractable) {
    if (IsInteracting()) {
        EndInteract();
    }
    if (InteractionData.CurrentInteractable) {
        TargetInteractable = InteractionData.CurrentInteractable;
        TargetInteractable->EndFocus();
    }
    InteractionData.CurrentInteractable = NewInteractable;
    TargetInteractable = NewInteractable;

    HUD->UpdateInteractionWidget(TargetInteractable->InteractableData);
    TargetInteractable->BeginFocus();
}

void AHexTile::NoInteractableFound() {
    if (IsInteracting()) {
        GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    }
    if (InteractionData.CurrentInteractable) {
        if (IsValid(TargetInteractable.GetObject())) {
            TargetInteractable->EndFocus();
        }
        HUD->HideInteractionWidget();

        InteractionData.CurrentInteractable = nullptr;
        TargetInteractable = nullptr;
    }
}

void AHexTile::PerformInteractionCheck() {
    FVector2D MousePosition;
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

    FHitResult TraceHitResult;
    PlayerController->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, true, TraceHitResult);

    AHexTile* HitTile = Cast<AHexTile>(TraceHitResult.GetActor());
    if (HitTile) {
        if (HitTile->InteractableData.bIsInteractable) {
            if (HitTile != InteractionData.CurrentInteractable) {
                FoundInteractable(HitTile);
            }
        } else {
            NoInteractableFound();
        }
    } else {
        NoInteractableFound();
    }
}