// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AHexTile;

UENUM()
enum class EInteractableType : uint8 {
    Pickup UMETA(DisplayName = "Pickup"),
    Move UMETA(DisplayName = "Move"),
    Set UMETA(DisplayName = "Set"),
    Use UMETA(DisplayName = "Use"),
    Container UMETA(DisplayName = "Container")
};

USTRUCT()
struct FInteractableData
{
    GENERATED_USTRUCT_BODY()

    FInteractableData() :
        InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
        Action(FText::GetEmpty()),
        Quantity(0),
        InteractionDuration(0.0f)
    {};

    UPROPERTY(EditInstanceOnly)
    EInteractableType InteractableType;

    UPROPERTY(EditInstanceOnly)
    FText Name;

    UPROPERTY(EditInstanceOnly)
    FText Action;

    UPROPERTY(EditInstanceOnly)
    int32 Quantity;

    UPROPERTY(EditInstanceOnly)
    float InteractionDuration;
    
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IRON_HORIZON_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// that receives the action
    virtual void BeginFocus() = 0;
    virtual void EndFocus() = 0;
    virtual void Interact(AHexTile *HexTile) = 0;
    virtual void BeginInteract() = 0;
    virtual void EndInteract() = 0;
    virtual void UpdateInteractionWidget() = 0;
    virtual void FoundInteractable() = 0;
    virtual void NoInteractableFound() = 0;

    FInteractableData InteractableData;
	
};
