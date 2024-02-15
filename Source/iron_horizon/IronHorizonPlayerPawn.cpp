// Fill out your copyright notice in the Description page of Project Settings.

#include "IronHorizonPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCameraController.h"

// Sets default values
AIronHorizonPlayerPawn::AIronHorizonPlayerPawn() {
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

    Movement =
        CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent")
        );
    MoveScale = 3.0f;
    RotateScale = 60.0f;
}

// Called to bind functionality to input
void AIronHorizonPlayerPawn::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent
) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent *EIController =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);
    APlayerCameraController *FPController =
        Cast<APlayerCameraController>(Controller);
    check(EIController);
    check(FPController);

    EIController->BindAction(
        FPController->MoveAction, ETriggerEvent::Triggered, this,
        &AIronHorizonPlayerPawn::Move
    );

    EIController->BindAction(
        FPController->RotateAction, ETriggerEvent::Triggered, this,
        &AIronHorizonPlayerPawn::Rotate
    );

    ULocalPlayer *LocalPlayer = Cast<ULocalPlayer>(FPController->Player);
    check(LocalPlayer);

    UEnhancedInputLocalPlayerSubsystem *Subsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(Subsystem);
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(FPController->PawnMappingContext, 0);
}

void AIronHorizonPlayerPawn::Move(const FInputActionValue &ActionValue) {
    UE_LOG(LogTemp, Warning, TEXT("Move called"));
    FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
	Input *= MoveScale;
    AddMovementInput(GetActorRotation().RotateVector(Input), MoveScale);
}

void AIronHorizonPlayerPawn::Rotate(const FInputActionValue &ActionValue) {
    UE_LOG(LogTemp, Warning, TEXT("Rotate called"));
    FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
	UE_LOG(LogTemp, Warning, TEXT("Input: %f, %f, %f"), ActionValue[0], ActionValue[1], ActionValue[2]);
    Input *= GetWorld()->GetDeltaSeconds() * RotateScale;
    Input += GetActorRotation();
    SetActorRotation(Input);
}