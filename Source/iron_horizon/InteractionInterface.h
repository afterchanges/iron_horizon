#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"


ENUM()
enum class EInteractableType : uint8 {
    Move UMETA(DisplayName = "Move"),
    Set UMETA(DisplayName = "Set"),
};

USTRUCT()
struct FInteractableData
{
    GENERATED_BODY()

    FInteractableData() :
        InteractableType(EInteractableType::Move),
        InteractableName(FText::GetEmpty()),
        InteractableAction(FText::GetEmpty())
    {};

    UPROPERTY(EditInstanceOnly)
    EInteractableType InteractableType;

    UPROPERTY(EditInstanceOnly)
    FText InteractableName;

    UPROPERTY(EditInstanceOnly)
    FText InteractableAction;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
    GENERATED_BODY()
};

class IRONHORIZON_API IInteractionInterface
{
    GENERATED_BODY()

public:
    // that receives the action
    virtual void BeginFocus();
    virtual void EndFocus();
    virtual void Interact();
    virtual void BeginInteract();
    virtual void EndInteract();

    FInteractableData InteractableData;
};
