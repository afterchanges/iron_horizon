// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCameraController.generated.h"

UCLASS()
class IRON_HORIZON_API APlayerCameraController : public APlayerController {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraSpeed = 1000.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraZoomSpeed = 100.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraRotationSpeed = 100.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraMinZoom = 100.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraMaxZoom = 1000.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraZoom = 500.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraRotation = 0.0f;
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraPitch = 0.0f;

public:
    APlayerCameraController();

    UPROPERTY(EditAnywhere, Category = "Camera")
    class UInputMappingContext *PawnMappingContext;

    UPROPERTY(EditAnywhere, Category = "Camera movement")
    class UInputAction *MoveAction;

    UPROPERTY(EditAnywhere, Category = "Camera movement")
    class UInputAction *RotateAction;

    UPROPERTY(EditAnywhere, Category = "Camera movement")
    class UInputAction *SpringArmLengthAction;

    virtual void SetupInputComponent() override;

    void OnJKeyPressed();

    void OnPKeyPressed();
};
