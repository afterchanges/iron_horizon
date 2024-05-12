// Fill out your copyright notice in the Description page of Project Settings.


#include "RailroadSpline.h"

// Sets default values
ARailroadSpline::ARailroadSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ThisRailroadSpline = CreateDefaultSubobject<SplineComponent>(TEXT("RailroadSpline"));
	SplineComponent->SetAttachment(GetRootComponent());

	ThisRailroadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RailroadMesh"));
	ThisRailroadMesh->SetAttachment(ThisRailroadSpline);
}

void ARailroadSpline::SetRailroadSplinePoints(TArray<FVector3d> SplinePoints) {
	ThisSplinePoints = SplinePoints;
	ThisRailroadSpline->SetSplinePoints(ThisSplinePoints);
	ThisRailroadSpline->SetClosedLoop(false);
	ThisRailroadSpline->UpdateSpline();
	ThisSplineLength = ThisRailroadSpline->GetSplineLength();
}
// Called when the game starts or when spawned
void ARailroadSpline::BeginPlay()
{
	
	
}

// Called every frame
void ARailroadSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementTimeline != nullptr && MovementTimeline->IsPlaying()) {
		MovementTimeline->TickTimeline(DeltaTime);
	}

}

void ARailroadSpline::ProcessMovementTimeline(float Value) {
	FVector CurrentSplineLocation = ThisRailroadSpline->GetLocationAtDistanceAlongSpline(ThisSplineLength * Value, ESplineCoordinateSpace::World);
	FRotator CurrentSplineRotation = ThisRailroadSpline->GetRotationAtDistanceAlongSpline(ThisSplineLength * Value, ESplineCoordinateSpace::World);

	MeshComponent->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
}

void ARailroadSpline::OnEndMovementTimeline() {
	// Set the actor location to the end of the spline
	FVector Location = ThisRailroadSpline->GetLocationAtSplinePoint(ThisSplinePoints.Num() - 1, ESplineCoordinateSpace::World);
	SetActorLocation(Location);
}

void ARailroadSpline::BeginMovement() {
	MovementTimeline = NewObject<UTimelineComponent>(this, FName("MovementTimeline"));
	MovementTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	this->BlueprintCreatedComponents.Add(MovementTimeline);
	MovementTimeline->SetNetAddressable();
	MovementTimeline->SetPropertySetObject(this);

	MovementTimeline->AddInterpFloat(MovementCurve, FOnTimelineFloat(), FName("ProcessMovementTimeline"));
	MovementTimeline->SetTimelineFinishedFunc(FOnTimelineEvent(), FName("OnEndMovementTimeline"));

	MovementTimeline->SetTimelineLength(ThisSplineLength);
	MovementTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	MovementTimeline->PlayFromStart();
}