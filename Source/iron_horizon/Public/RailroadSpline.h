#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"
#include "UserInterface/MoneyWidget.h"
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

    UPROPERTY(VisibleAnywhere, Category = "Spline")
    UStaticMeshComponent* MovingMesh;

    UPROPERTY(EditAnywhere, Category = "Movement")
    UCurveFloat* MovementCurve;

    FTimeline MovementTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UMoneyWidget* MoneyWidget;

    void UpdateMoney(int32 PlayerMoney);

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    void CreateMovementCurve();

    UFUNCTION()
    void ProcessMovementTimeline(float Value);

    UFUNCTION()
    void OnEndMovementTimeline();

    void SetRailroadSplinePoints(TArray<FVector3d> SplinePoints);

    void BeginMovement();

private:
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MovementSpeed = 20.0f;
    bool bMovingForward = true;
    void CreateSplineMesh();
};