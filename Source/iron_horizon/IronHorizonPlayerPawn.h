// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Components/InventoryComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "IronHorizonPlayerPawn.generated.h"

class UStaticMeshComponent;
class AIronHorizonHUD;

USTRUCT()
struct FInteractionData {
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f) {};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};


UCLASS()
class IRON_HORIZON_API AIronHorizonPlayerPawn : public APawn, public IInteractionInterface{
    GENERATED_BODY()
public:
    AIronHorizonPlayerPawn();
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    virtual void SetupPlayerInputComponent(
        class UInputComponent *PlayerInputComponent
    ) override;

    UPROPERTY(EditAnywhere)
    class USphereComponent *SphereComponent;

    /** Spring arm to hold camera, attached to the root. */
    UPROPERTY(EditAnywhere)
    class USpringArmComponent *SpringArmComponent;

    /** Camera attached to spring arm to provide pawn's view. */
    UPROPERTY(EditAnywhere)
    class UCameraComponent *CameraComponent;

    /** Use floating pawn movement to smooth out motion. */
    UPROPERTY(EditAnywhere)
    class UFloatingPawnMovement *Movement;

    /** Scale to apply to location input. */
    UPROPERTY(EditAnywhere, Category = "Camera movement")
    float MoveScale = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Camera movement")
	float RotateScale = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Camera movement")
	float SpringArmLength = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera movement")
	float SpringArmLengthScale = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera movement")
	float MinSpringArmLength = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera movement")
	float MaxSpringArmLength = 5000.0f;


    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float FastBorderSizeThreshold = 0.05f;
    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float MediumBorderSizeThreshold = 0.1f;
    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float SlowBorderSizeThreshold = 0.2f;
    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float FastSpeed = 5000.0f;
    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float MediumSpeed = 2000.0f;
    UPROPERTY(EditAnywhere, Category = "Movement on cursor")
    float SlowSpeed = 500.0f;

    /** Handle input to update location. */
    float determineMouseMovementSpeedOnAxis(float MousePosition, float ViewportSize);

    void Move(const struct FInputActionValue &ActionValue);

	void Rotate(const struct FInputActionValue &ActionValue);

    void UpdateSpringArmLength(const struct FInputActionValue &ActionValue);

    void UpdateCameraPosition();

    // INTERACTION INTERFACE & INVENTORY INTERFACE

    UPROPERTY()
	AIronHorizonHUD* HUD;
    UPROPERTY(VisibleAnywhere, Category = "PlayerPawn | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;
	UPROPERTY(VisibleAnywhere, Category = "PlayerPawn | Inventory")
	UInventoryComponent* PlayerInventory;
    UPROPERTY(EditInstanceOnly, Category = "PlayerPawn")
    FInteractableData InstanceInteractableData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPawn")
    UStaticMeshComponent* Mesh;

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); }
	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; }

	float InteractionCheckFrequency;

	FInteractionData InteractionData;
    FTimerHandle TimerHandle_Interaction;

	virtual void BeginPlay() override;

	void UpdateInteractionWidget() const;

    void ToggleMenu();



};
