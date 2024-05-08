// Fill out your copyright notice in the Description page of Project Settings.

#include "IronHorizonPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "UserInterface/IronHorizonHUD.h"
#include "Components/InventoryComponent.h"
#include "DrawDebugHelpers.h"
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

    // Bind the input mapping context to the player controller

    PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(Mesh);

    InteractionCheckFrequency = 0.1f;

    PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
    PlayerInventory->SetSlotsCapacity(20);
    PlayerInventory->SetWeightCapacity(100.0f);

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

    InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &APlayerCameraController::ToggleMenu);

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

    if (IsInteracting()) {
        PerformInteractionCheck();
    }
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

void AIronHorizonPlayerPawn::BeginPlay() {
    Super::BeginPlay();
    
    HUD = Cast<AIronHorizonHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

    InteractableData = InstanceInteractableData;

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    if (PlayerController) {
        EnableInput(PlayerController);
        PlayerController->bShowMouseCursor = true; 
        PlayerController->bEnableClickEvents = true; 
        PlayerController->bEnableMouseOverEvents = true;
    }
}

void AIronHorizonPlayerPawn::Interact() {
    Interact(this);
}

void AIronHorizonPlayerPawn::BeginInteract() {
    // Verify nothing has changed with the interactable state since beginning interaction
    PerformInteractionCheck();
    if (InteractionData.CurrentInteractable) {
        if (IsValid(TargetInteractable.GetObject())) {
            TargetInteractable->BeginInteract();
        }
        if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f)) {
            Interact();
        } else {
            GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &AIronHorizonPlayerPawn::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
        }
    }
}

void AIronHorizonPlayerPawn::EndInteract() {
    GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    if (IsValid(TargetInteractable.GetObject())) {
        TargetInteractable->EndInteract();
    }
}

void AIronHorizonPlayerPawn::Interact(AIronHorizonPlayerPawn* PlayerPawn) {
    GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    if (IsValid(TargetInteractable.GetObject())) {
        TargetInteractable->Interact(this);
    }
}

void AIronHorizonPlayerPawn::FoundInteractable(AIronHorizonPlayerPawn* NewInteractable) {
    if (IsInteracting()) {
        EndInteract();
    }
    if (InteractionData.CurrentInteractable) {
        TargetInteractable = InteractionData.CurrentInteractable;
        TargetInteractable->EndFocus();
    }
    InteractionData.CurrentInteractable = NewInteractable;
    TargetInteractable = NewInteractable;

    HUD->UpdateInteractionWidget(TargetInteractable->InteractableData);
    TargetInteractable->BeginFocus();
}

void AIronHorizonPlayerPawn::NoInteractableFound() {
    if (IsInteracting()) {
        GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
    }
    if (InteractionData.CurrentInteractable) {
        if (IsValid(TargetInteractable.GetObject())) {
            TargetInteractable->EndFocus();
        }
        HUD->HideInteractionWidget();

        InteractionData.CurrentInteractable = nullptr;
        TargetInteractable = nullptr;
    }
}

void AIronHorizonPlayerPawn::PerformInteractionCheck() {
    if (IsValid(InteractionData.CurrentInteractable)) {
        if (InteractionData.CurrentInteractable->IsInteractable()) {
            if (InteractionData.CurrentInteractable->IsInInteractionRange(this)) {
                return;
            }
        }
    }

    TArray<AActor*> OverlappingActors;
    SphereComponent->GetOverlappingActors(OverlappingActors, AInteractableActor::StaticClass());

    AInteractableActor* NewInteractable = nullptr;
    for (AActor* Actor : OverlappingActors) {
        AInteractableActor* Interactable = Cast<AInteractableActor>(Actor);
        if (Interactable && Interactable->IsInteractable() && Interactable->IsInInteractionRange(this)) {
            NewInteractable = Interactable;
            break;
        }
    }

    if (NewInteractable) {
        FoundInteractable(NewInteractable);
    } else {
        NoInteractableFound();
    }
}

void AIronHorizonPlayerPawn::UpdateInteractionWidget() const {
    if (IsValid(TargetInteractable.GetObject())) {
        HUD->UpdateInteractionWidget(TargetInteractable->InteractableData);
    }
}

void AIronHorizonPlayerPawn::ToggleMenu() {
    HUD->ToggleMenu();
}