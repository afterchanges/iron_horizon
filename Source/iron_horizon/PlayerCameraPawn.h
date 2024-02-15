// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerCameraPawn.generated.h"

UCLASS()

class IRON_HORIZON_API APlayerCameraPawn : public APawn {
    GENERATED_BODY()
public:
    // Called every frame
    APlayerCameraPawn();
    virtual void SetupPlayerInputComponent(
        UInputComponent *PlayerInputComponent
    ) override;

    // Called to bind functionality to input
    UPROPERTY(EditAnywhere, Category = "Camera")
    class UCameraComponent *CameraComponent;
    UPROPERTY(EditAnywhere, Category = "Camera")
    class USpringArmComponent *SpringArmComponent;
    UPROPERTY(EditAnywhere, Category = "Camera")
    class USphereComponent *SphereComponent;
};
