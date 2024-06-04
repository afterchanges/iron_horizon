#include "RailroadSpline.h"
#include "Components/SplineMeshComponent.h"
#include "Algo/Reverse.h"

// Sets default values
ARailroadSpline::ARailroadSpline()
{
    UE_LOG(LogTemp, Warning, TEXT("RailroadSpline Constructor called"));
    PrimaryActorTick.bCanEverTick = true;

    ThisRailroadSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RailroadSpline"));
    RootComponent = ThisRailroadSpline;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Static/3D_objects/RailroadStuff/train.train"));
    if (MeshAsset.Succeeded())
    {
        ThisRailroadMesh = MeshAsset.Object;
	}
}

void ARailroadSpline::SetRailroadSplinePoints(TArray<FVector3d> SplinePoints)
{
    ThisSplinePoints = SplinePoints;
    ThisRailroadSpline->ClearSplinePoints(false);

    for (const FVector3d& Point : ThisSplinePoints)
    {
        ThisRailroadSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
    }

    ThisRailroadSpline->UpdateSpline();
    ThisRailroadSpline->SetClosedLoop(false);

    CreateSplineMesh();
}

void ARailroadSpline::BeginPlay()
{
    Super::BeginPlay();
}

void ARailroadSpline::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MovementTimeline != nullptr && MovementTimeline->IsPlaying())
    {
        MovementTimeline->TickTimeline(DeltaTime);
    }
}

void ARailroadSpline::ProcessMovementTimeline(float Value)
{
    FVector CurrentSplineLocation = ThisRailroadSpline->GetLocationAtDistanceAlongSpline(ThisRailroadSpline->GetSplineLength() * Value, ESplineCoordinateSpace::World);
    FRotator CurrentSplineRotation = ThisRailroadSpline->GetRotationAtDistanceAlongSpline(ThisRailroadSpline->GetSplineLength() * Value, ESplineCoordinateSpace::World);

    ThisRailroadSpline->SetWorldLocation(CurrentSplineLocation);
	ThisRailroadSpline->SetWorldRotation(CurrentSplineRotation);
}

void ARailroadSpline::OnEndMovementTimeline()
{
    FVector Location = ThisRailroadSpline->GetLocationAtSplinePoint(ThisSplinePoints.Num() - 1, ESplineCoordinateSpace::World);
    SetActorLocation(Location);
    ThisRailroadSpline->ClearSplinePoints();
    Algo::Reverse(ThisSplinePoints);
    ThisRailroadSpline->SetSplinePoints(ThisSplinePoints, ESplineCoordinateSpace::World, true);
}

void ARailroadSpline::BeginMovement()
{
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

void ARailroadSpline::CreateSplineMesh()
{
    const int32 NumPoints = ThisRailroadSpline->GetNumberOfSplinePoints();
    
    for (int32 i = 0; i < NumPoints - 1; ++i)
    {
        USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), *FString::Printf(TEXT("SplineMesh_%d"), i));
        SplineMesh->SetStaticMesh(ThisRailroadMesh);
        SplineMesh->SetMobility(EComponentMobility::Movable);
        SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
        SplineMesh->AttachToComponent(ThisRailroadSpline, FAttachmentTransformRules::KeepRelativeTransform);
        SplineMesh->RegisterComponentWithWorld(GetWorld());

        FVector StartPos, StartTangent, EndPos, EndTangent;
        ThisRailroadSpline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTangent, ESplineCoordinateSpace::Local);
        ThisRailroadSpline->GetLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

        SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
    }
}
