#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../HexTile.h"
#include "Interfaces/InteractionInterface.h"
#include "InterfaceTestActor.generated.h"

UCLASS()
class IRON_HORIZON_API AInterfaceTestActor : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:

    AInterfaceTestActor();

protected:
    
    UPROPERTY(EditAnywhere, Category = "Test Actor")
    UStaticMeshComponent* TileMesh;

    UPROPERTY(EditInstanceOnly, Category = "Test Actor")
    FInteractableData InstanceInteractableData;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AHexTile* HexTile) override;

};