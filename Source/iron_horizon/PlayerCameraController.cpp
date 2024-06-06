// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraController.h"
#include "InputAction.h"
#include "EngineUtils.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "HexTile.h"
#include "IronHorizonPlayerPawn.h"
#include "Engine/Engine.h"
#include "HexGridManager.h"
#include "Kismet/GameplayStatics.h"

static void MapKey(
    UInputMappingContext *InputMappingContext,
    UInputAction *InputAction,
    FKey Key,
    bool bNegate = false,
    bool bSwizzle = false,
    EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ
) {
    FEnhancedActionKeyMapping &MoveActionMapping = InputMappingContext->MapKey(InputAction, Key);
    UObject *Outer = InputMappingContext->GetOuter();

    if (bNegate) {
        UInputModifierNegate *Negate = NewObject<UInputModifierNegate>(Outer);
        MoveActionMapping.Modifiers.Add(Negate);
    }

    if (bSwizzle) {
        UInputModifierSwizzleAxis *Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
        Swizzle->Order = SwizzleOrder;
        MoveActionMapping.Modifiers.Add(Swizzle);
    }

    UE_LOG(LogTemp, Warning, TEXT("Mapped key %s"), *Key.ToString());
}

void APlayerCameraController::SetupInputComponent() {
    Super::SetupInputComponent();

    PawnMappingContext = NewObject<UInputMappingContext>(this);

    MoveAction = NewObject<UInputAction>(this);
    MoveAction->ValueType = EInputActionValueType::Axis3D;
    MapKey(PawnMappingContext, MoveAction, EKeys::W);
    MapKey(PawnMappingContext, MoveAction, EKeys::S, true);
    MapKey(PawnMappingContext, MoveAction, EKeys::A, true, true);
    MapKey(PawnMappingContext, MoveAction, EKeys::D, false, true);
    MapKey(
        PawnMappingContext, MoveAction, EKeys::SpaceBar, false, true,
        EInputAxisSwizzle::ZYX
    );
    MapKey(
        PawnMappingContext, MoveAction, EKeys::LeftShift, true, true,
        EInputAxisSwizzle::ZYX
    );

    RotateAction = NewObject<UInputAction>(this);
    RotateAction->ValueType = EInputActionValueType::Axis3D;
    MapKey(
        PawnMappingContext, RotateAction, EKeys::E, false, true,
        EInputAxisSwizzle::ZXY
    );
    MapKey(
        PawnMappingContext, RotateAction, EKeys::Q, true, true,
        EInputAxisSwizzle::ZXY
    );

    SpringArmLengthAction = NewObject<UInputAction>(this);
    SpringArmLengthAction->ValueType = EInputActionValueType::Axis1D;
    MapKey(PawnMappingContext, SpringArmLengthAction, EKeys::MouseWheelAxis);

    // Create a new action for the J key press
    UInputAction *JKeyPressAction = NewObject<UInputAction>(this);
    JKeyPressAction->ValueType = EInputActionValueType::Axis1D;
    MapKey(PawnMappingContext, JKeyPressAction, EKeys::J);

    // Bind the action to the OnJKeyPressed function
    InputComponent->BindKey(EKeys::J, IE_Pressed, this, &APlayerCameraController::OnJKeyPressed);

    // Create a new action for the P key press
    UInputAction *PKeyPressAction = NewObject<UInputAction>(this);
    PKeyPressAction->ValueType = EInputActionValueType::Axis1D;
    MapKey(PawnMappingContext, PKeyPressAction, EKeys::P);

    // Bind the action to the OnPKeyPressed function
    InputComponent->BindKey(EKeys::P, IE_Pressed, this, &APlayerCameraController::OnPKeyPressed);

    UInputAction *TKeyPressAction = NewObject<UInputAction>(this);
    TKeyPressAction->ValueType = EInputActionValueType::Axis1D;
    MapKey(PawnMappingContext, TKeyPressAction, EKeys::T);

    InputComponent->BindKey(EKeys::T, IE_Pressed, this, &APlayerCameraController::OnTKeyPressed);
}

APlayerCameraController::APlayerCameraController() {
    bEnableMouseOverEvents = true;
}

void APlayerCameraController::OnJKeyPressed() {
    UE_LOG(LogTemp, Warning, TEXT("J key pressed"));

    // Get the current mouse position
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY)) {
        // Convert the mouse screen position to a world space ray
        FCollisionQueryParams TraceParams;
        FHitResult HitResult;
        if (GetHitResultAtScreenPosition(
                FVector2D(MouseX, MouseY), ECC_Visibility, TraceParams, HitResult
            )) {
            // Check if the hit actor is a hex tile
            AHexTile *HexTile = Cast<AHexTile>(HitResult.GetActor());
            if (HexTile) {
                // Change the tile color and type
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("J MOUSE POSITION: %s"),
                    *HitResult.ImpactPoint.ToString()
                );
                HexTile->ChangeToRailway();
                TSubclassOf<AActor> HexGridManagerClass = AHexGridManager::StaticClass();
                AHexGridManager* HexGridManagerInstance = Cast<AHexGridManager>(
                    UGameplayStatics::GetActorOfClass(GetWorld(), HexGridManagerClass)
                );

                if (HexGridManagerInstance) {
                    HexGridManagerInstance->AddNewRailroadTile(HexTile);
                }
            }
        }
    }
}

void APlayerCameraController::OnPKeyPressed() {
    UE_LOG(LogTemp, Warning, TEXT("P key pressed"));

    // Get the current mouse position
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY)) {
        // Convert the mouse screen position to a world space ray
        FCollisionQueryParams TraceParams;
        FHitResult HitResult;
        if (GetHitResultAtScreenPosition(
                FVector2D(MouseX, MouseY), ECC_Visibility, TraceParams, HitResult
            )) {
            // Check if the hit actor is a hex tile
            AHexTile *HexTile = Cast<AHexTile>(HitResult.GetActor());
            if (HexTile) {
                UE_LOG(LogTemp, Warning, TEXT("HexTile hit"));

                // Check if it's the first or second tile selected
                if (!AHexTile::StartTile) {
                    AHexTile::StartTile = HexTile;
                    UE_LOG(LogTemp, Warning, TEXT("StartTile set"));
                } else if (!AHexTile::EndTile) {
                    AHexTile::EndTile = HexTile;
                    UE_LOG(LogTemp, Warning, TEXT("EndTile set"));

                    // Get the hex grid manager instance
                    AHexGridManager *HexGridManager = nullptr;
                    for (TActorIterator<AHexGridManager> It(GetWorld()); It; ++It) {
                        HexGridManager = *It;
                        break;
                    }

                    if (!HexGridManager) {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to find HexGridManager instance"));
                        return;
                    }

                    UE_LOG(LogTemp, Warning, TEXT("HexGridManager instance obtained"));
                    if (HexGridManager) {
                        UE_LOG(LogTemp, Warning, TEXT("HexGridManager instance obtained"));

                        // Call the A* algorithm function
                        TArray<AHexTile*> Path =
                            HexGridManager->HexGridAStar(AHexTile::StartTile, AHexTile::EndTile);
                        UE_LOG(LogTemp, Warning, TEXT("Shortest path length: %d"), Path.Num());
                        // Iterate over the tiles in the path and change their type
                        for (auto Tile : Path) {
                            if (Tile) { Tile->ChangeToRailway(); }
                        }

                        // Reset the start and end tiles
                        AHexTile::StartTile = nullptr;
                        AHexTile::EndTile = nullptr;
                    } else {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to get HexGridManager instance"));
                    }
                }
            } else {
                UE_LOG(LogTemp, Warning, TEXT("No HexTile hit"));
            }
        } else {
            UE_LOG(LogTemp, Warning, TEXT("No hit result at screen position"));
        }
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get mouse position"));
    }
}

void APlayerCameraController::OnTKeyPressed() {
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY)) {
        // Convert the mouse screen position to a world space ray
        FCollisionQueryParams TraceParams;
        FHitResult HitResult;
        if (GetHitResultAtScreenPosition(
                FVector2D(MouseX, MouseY), ECC_Visibility, TraceParams, HitResult
            )) {
            // Check if the hit actor is a hex tile
            AHexTile *HexTile = Cast<AHexTile>(HitResult.GetActor());
            if (HexTile) {
                TSubclassOf<AActor> HexGridManagerClass = AHexGridManager::StaticClass();
                AHexGridManager* HexGridManagerInstance = Cast<AHexGridManager>(
                    UGameplayStatics::GetActorOfClass(GetWorld(), HexGridManagerClass)
                );

                if (HexGridManagerInstance) {
                    HexGridManagerInstance->AddRouteEndpoint(HexTile);
                }
            }
        }
    }
}

void APlayerCameraController::BeginPlay() {
    Super::BeginPlay();

    // Enable the mouse for the player controller
    this->bShowMouseCursor = true;
    this->bEnableClickEvents = true;
    this->bEnableMouseOverEvents = true;
}