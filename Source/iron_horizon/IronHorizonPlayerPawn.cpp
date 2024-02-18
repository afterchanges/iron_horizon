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
}

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

void AIronHorizonPlayerPawn::UpdateCameraPosition() {
    APlayerController *PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController) {
        FVector2D MousePosition;
        FVector2D ViewportSize;
        int32 ViewportSizeX, ViewportSizeY;
        PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
        ViewportSize.X = ViewportSizeX;
        ViewportSize.Y = ViewportSizeY;

        if (PlayerController->GetMousePosition(
                MousePosition.X, MousePosition.Y
            )) {
            FVector NewLocation = GetActorLocation();
            float BorderSize = 550.0f;

            // Check if the mouse is at the edge of the screen and move the
            // camera accordingly
            float MoveSpeed = 100.0f;
            if (MousePosition.X <= BorderSize) {
                NewLocation -= GetActorRightVector() * MoveSpeed;
            } else if (MousePosition.X >= ViewportSize.X - BorderSize) {
                NewLocation += GetActorRightVector() * MoveSpeed;
            }

            if (MousePosition.Y <= BorderSize) {
                NewLocation += GetActorForwardVector() * MoveSpeed;
            } else if (MousePosition.Y >= ViewportSize.Y - BorderSize) {
                NewLocation -= GetActorForwardVector() * MoveSpeed;
            }

            SetActorLocation(NewLocation);
        }
    }
}

void AIronHorizonPlayerPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    UpdateCameraPosition();
}

void AIronHorizonPlayerPawn::Move(const FInputActionValue &ActionValue) {
    UE_LOG(LogTemp, Warning, TEXT("Move called"));
    FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
    AddMovementInput(GetActorRotation().RotateVector(Input), 2000.0f, true);
}

void AIronHorizonPlayerPawn::Rotate(const FInputActionValue &ActionValue) {
    UE_LOG(LogTemp, Warning, TEXT("Rotate called"));
    FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
    UE_LOG(
        LogTemp, Warning, TEXT("Input: %f, %f, %f"), ActionValue[0],
        ActionValue[1], ActionValue[2]
    );
    Input *= GetWorld()->GetDeltaSeconds() * RotateScale;
    Input += GetActorRotation();
    SetActorRotation(Input);
}

void AIronHorizonPlayerPawn::UpdateSpringArmLength(
    const FInputActionValue &ActionValue
) {
    UE_LOG(
        LogTemp, Warning, TEXT("UpdateSpringArmLength called %f"),
        ActionValue[0]
    );
    SpringArmComponent->TargetArmLength += ActionValue[0] *
                                           GetWorld()->GetDeltaSeconds() *
                                           SpringArmLengthScale * -1.0f;
    SpringArmComponent->TargetArmLength = FMath::Clamp(
        SpringArmComponent->TargetArmLength, MinSpringArmLength,
        MaxSpringArmLength
    );

    UE_LOG(
        LogTemp, Warning, TEXT("SpringArmComponent->TargetArmLength: %f"),
        SpringArmComponent->TargetArmLength
    );
}
