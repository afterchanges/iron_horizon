// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
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
}