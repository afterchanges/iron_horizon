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

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SetRootComponent(SphereComponent);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(SphereComponent);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void AIronHorizonPlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent *EIController = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    APlayerCameraController *FPController = Cast<APlayerCameraController>(Controller);
    check(EIController);
    check(FPController);

    EIController->BindAction(
        FPController->MoveAction, ETriggerEvent::Triggered, this, &AIronHorizonPlayerPawn::Move
    );
    EIController->BindAction(
        FPController->RotateAction, ETriggerEvent::Triggered, this, &AIronHorizonPlayerPawn::Rotate
    );
    EIController->BindAction(
        FPController->SpringArmLengthAction, ETriggerEvent::Triggered, this,
        &AIronHorizonPlayerPawn::UpdateSpringArmLength
    );

    ULocalPlayer *LocalPlayer = Cast<ULocalPlayer>(FPController->Player);
    check(LocalPlayer);

    UEnhancedInputLocalPlayerSubsystem *Subsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(Subsystem);
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(FPController->PawnMappingContext, 0);
}

FVector2D LastMousePosition;

float AIronHorizonPlayerPawn::determineMouseMovementSpeedOnAxis(
    float MousePosition,
    float ViewportSize
) {
    float newPositionDifference = 0.0f;
    if (MousePosition >= FastBorderSizeThreshold * ViewportSize &&
        MousePosition <= (1.0f - FastBorderSizeThreshold) * ViewportSize) {
        if (MousePosition >= MediumBorderSizeThreshold * ViewportSize &&
            MousePosition <= (1.0f - MediumBorderSizeThreshold) * ViewportSize) {
            if (MousePosition >= SlowBorderSizeThreshold * ViewportSize &&
                MousePosition <= (1.0f - SlowBorderSizeThreshold) * ViewportSize) {
            } else {
                newPositionDifference +=
                    SlowSpeed * FMath::Sign(SlowBorderSizeThreshold * ViewportSize - MousePosition);
                // UE_LOG(LogTemp, Warning, TEXT("SlowSpeed"));
            }
        } else {
            newPositionDifference +=
                MediumSpeed * FMath::Sign(MediumBorderSizeThreshold * ViewportSize - MousePosition);
            // UE_LOG(LogTemp, Warning, TEXT("MediumSpeed"));
        }
    } else {
        newPositionDifference +=
            FastSpeed * FMath::Sign(FastBorderSizeThreshold * ViewportSize - MousePosition);
        // UE_LOG(LogTemp, Warning, TEXT("FastSpeed"));
    }

    return newPositionDifference;
}

void AIronHorizonPlayerPawn::UpdateCameraPosition() {
    APlayerController *PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController) {
        FVector2D MousePosition;
        FVector2D ViewportSize;
        int32 ViewportSizeX, ViewportSizeY;
        PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
        ViewportSize.X = ViewportSizeX;
        ViewportSize.Y = ViewportSizeY;

        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y)) {
            FVector NewLocation = GetActorLocation();
            NewLocation.Y -= GetWorld()->GetDeltaSeconds() * determineMouseMovementSpeedOnAxis(
                MousePosition.X, ViewportSize.X
            );
            NewLocation.X += GetWorld()->GetDeltaSeconds() * determineMouseMovementSpeedOnAxis(
                MousePosition.Y, ViewportSize.Y
            );

            SetActorLocation(NewLocation);
        }
    }
}

void AIronHorizonPlayerPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    UpdateCameraPosition();
}

void AIronHorizonPlayerPawn::Move(const FInputActionValue &ActionValue) {
    // UE_LOG(LogTemp, Warning, TEXT("Move called"));
    FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
    AddMovementInput(GetActorRotation().RotateVector(Input), 2000.0f, true);
}

void AIronHorizonPlayerPawn::Rotate(const FInputActionValue &ActionValue) {
    // UE_LOG(LogTemp, Warning, TEXT("Rotate called"));
    FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
    // UE_LOG(
    //     LogTemp, Warning, TEXT("Input: %f, %f, %f"), ActionValue[0], ActionValue[1], ActionValue[2]
    // );
    Input *= GetWorld()->GetDeltaSeconds() * RotateScale;
    Input += GetActorRotation();
    SetActorRotation(Input);
}

void AIronHorizonPlayerPawn::UpdateSpringArmLength(const FInputActionValue &ActionValue) {
    // UE_LOG(LogTemp, Warning, TEXT("UpdateSpringArmLength called %f"), ActionValue[0]);
    SpringArmComponent->TargetArmLength +=
        ActionValue[0] * GetWorld()->GetDeltaSeconds() * SpringArmLengthScale * -1.0f;
    SpringArmComponent->TargetArmLength =
        FMath::Clamp(SpringArmComponent->TargetArmLength, MinSpringArmLength, MaxSpringArmLength);

    // UE_LOG(
    //     LogTemp, Warning, TEXT("SpringArmComponent->TargetArmLength: %f"),
    //     SpringArmComponent->TargetArmLength
    // );
}

void AIronHorizonPlayerPawn::BeginFocus() {
   
}

void AIronHorizonPlayerPawn::EndFocus() {
}

void AIronHorizonPlayerPawn::BeginInteract() {
}

void AIronHorizonPlayerPawn::EndInteract() {
}

void AIronHorizonPlayerPawn::Interact(AIronHorizonPlayerPawn *PlayerPawn) {
}