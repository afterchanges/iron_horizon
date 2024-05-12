#include "RailroadSpline.h"
#include "Algo/Reverse.h"

// Sets default values
ARailroadSpline::ARailroadSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ThisRailroadSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RailroadSpline"));
	ThisRailroadSpline->SetupAttachment(GetRootComponent());

	ThisRailroadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RailroadMesh"));
	ThisRailroadMesh->SetupAttachment(ThisRailroadSpline);
}

void ARailroadSpline::SetRailroadSplinePoints(TArray<FVector3d> SplinePoints) {
	ThisSplinePoints = SplinePoints;
	ThisRailroadSpline->SetSplinePoints(ThisSplinePoints, ESplineCoordinateSpace::World, true);
	ThisRailroadSpline->SetClosedLoop(false);
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

	ThisRailroadMesh->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
}

void ARailroadSpline::OnEndMovementTimeline() {
	// Set the actor location to the end of the spline
	FVector Location = ThisRailroadSpline->GetLocationAtSplinePoint(ThisSplinePoints.Num() - 1, ESplineCoordinateSpace::World);
	SetActorLocation(Location);
	ThisRailroadSpline->ClearSplinePoints();
	Algo::Reverse(ThisSplinePoints);
	ThisRailroadSpline->SetSplinePoints(ThisSplinePoints, ESplineCoordinateSpace::World, true);
}

void ARailroadSpline::BeginMovement() {
	MovementTimeline = new FTimeline();
	FOnTimelineFloat ProcessFunction;
	ProcessFunction.BindUFunction(this, FName("ProcessMovementTimeline"));
	FOnTimelineEvent EndFunction;
	EndFunction.BindUFunction(this, FName("OnEndMovementTimeline"));
	MovementTimeline->AddInterpFloat(MovementCurve, ProcessFunction);
	MovementTimeline->SetTimelineFinishedFunc(EndFunction);
	MovementTimeline->SetLooping(false);
	MovementTimeline->PlayFromStart();
}