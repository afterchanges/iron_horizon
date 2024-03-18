// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "HexTile.h"
#include "IronHorizonPlayerPawn.h"

static void MapKey(
    UInputMappingContext *InputMappingContext,
    UInputAction *InputAction,
    FKey Key,
    bool bNegate = false,
    bool bSwizzle = false,
    EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ
) {
    FEnhancedActionKeyMapping &MoveActionMapping =
        InputMappingContext->MapKey(InputAction, Key);
    UObject *Outer = InputMappingContext->GetOuter();

    if (bNegate) {
        UInputModifierNegate *Negate = NewObject<UInputModifierNegate>(Outer);
        MoveActionMapping.Modifiers.Add(Negate);
    }

    if (bSwizzle) {
        UInputModifierSwizzleAxis *Swizzle =
            NewObject<UInputModifierSwizzleAxis>(Outer);
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
    UInputAction* JKeyPressAction = NewObject<UInputAction>(this);
    JKeyPressAction->ValueType = EInputActionValueType::Axis1D;
    MapKey(PawnMappingContext, JKeyPressAction, EKeys::J);

    // Bind the action to the OnJKeyPressed function
    InputComponent->BindKey(EKeys::J, IE_Pressed, this, &APlayerCameraController::OnJKeyPressed);
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
        if (GetHitResultAtScreenPosition(FVector2D(MouseX, MouseY), ECC_Visibility, TraceParams, HitResult)) {
            // Check if the hit actor is a hex tile
            AHexTile* HexTile = Cast<AHexTile>(HitResult.GetActor());
            if (HexTile) {
                // Change the tile color and type
                HexTile->ChangeToRailway();
            }
        }
    }
}