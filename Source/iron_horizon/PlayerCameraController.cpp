// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraController.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void APlayerCameraController::SetupInputComponent() {
    Super::SetupInputComponent();

    PawnMappingContext = NewObject<UInputMappingContext>(this);

    MoveAction = NewObject<UInputAction>(this);
    MoveAction->ValueType = EInputActionValueType::Axis3D;
    PawnMappingContext->MapKey(MoveAction, EKeys::W);
}