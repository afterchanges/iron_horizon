#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "RailroadSpline.generated.h"

UCLASS()
class IRON_HORIZON_API ARailroadSpline : public AActor
{
    GENERATED_BODY()
    
public:    
    ARailroadSpline();

    UPROPERTY(EditAnywhere, Category = "Spline")
    TArray<FVector> ThisSplinePoints;

    UPROPERTY(EditAnywhere, Category = "Spline")
    USplineComponent* ThisRailroadSpline;

    UPROPERTY(EditAnywhere, Category = "Spline")
    UStaticMesh* ThisRailroadMesh;

    UPROPERTY(EditAnywhere, Category = "Movement")
    UCurveFloat* MovementCurve;

    FTimeline* MovementTimeline;

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void ProcessMovementTimeline(float Value);

    UFUNCTION()
    void OnEndMovementTimeline();

    UFUNCTION()
    void BeginMovement();

    void SetRailroadSplinePoints(TArray<FVector3d> SplinePoints);

private:
    void CreateSplineMesh();
};
