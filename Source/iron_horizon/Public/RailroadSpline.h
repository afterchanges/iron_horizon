// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"


#include "RailroadSpline.generated.h"

UCLASS()
class IRON_HORIZON_API ARailroadSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "Spline")
	TArray<FVector> ThisSplinePoints;

	UPROPERTY(EditAnywhere, Category = "Spline")
	USplineComponent* ThisRailroadSpline;
	float ThisSplineLength;

	UPROPERTY(EditAnywhere, Category = "Spline")
	UStaticMeshComponent* ThisRailroadMesh;

	UPROPERTY(EditAnywhere, Category = "Movement")
	UCurveFloat* MovementCurve;

	FTimeline* MovementTimeline;

	ARailroadSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ProcessMovementTimeline(float Value);

	UFUNCTION()
	void OnEndMovementTimeline();

	UFUNCTION()
	void BeginMovement();

	void SetRailroadSplinePoints(TArray<FVector3d> SplinePoints);

};
