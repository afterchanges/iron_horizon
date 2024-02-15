// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
// Sets default values
APlayerCameraPawn::APlayerCameraPawn() {
    // Set this pawn to call Tick() every frame.  You can turn this off to
    // improve performance if you don't need it.

    SphereComponent =
        CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SetRootComponent(SphereComponent);

    SpringArmComponent =
        CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(SphereComponent);

    CameraComponent =
        CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(
        SpringArmComponent, USpringArmComponent::SocketName
    );
}

// Called to bind functionality to input
void APlayerCameraPawn::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent
) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
