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

    MovingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovingMesh"));
    MovingMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Static/3D_objects/RailroadStuff/train.train"));
    if (MeshAsset.Succeeded())
    {
        ThisRailroadMesh = MeshAsset.Object;
        MovingMesh->SetStaticMesh(ThisRailroadMesh);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load static mesh"));
    }

    CreateMovementCurve();
}

void ARailroadSpline::CreateMovementCurve()
{
    MovementCurve = NewObject<UCurveFloat>(this, FName("MovementCurve"));

    if (MovementCurve)
    {
        MovementCurve->FloatCurve.AddKey(0.0f, 0.0f);
        float TotalTimeToTravel = ThisRailroadSpline->GetSplineLength() / MovementSpeed;
        MovementCurve->FloatCurve.AddKey(TotalTimeToTravel, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("MovementCurve created and initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create MovementCurve"));
    }
}

void ARailroadSpline::SetRailroadSplinePoints(TArray<FVector3d> SplinePoints)
{
    UE_LOG(LogTemp, Warning, TEXT("Setting Railroad Spline Points"));
    ThisSplinePoints.Empty();
    ThisRailroadSpline->ClearSplinePoints(false);

    for (const FVector3d& Point : SplinePoints)
    {
        ThisRailroadSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
        ThisSplinePoints.Add(Point);
        UE_LOG(LogTemp, Warning, TEXT("Added Spline Point: %s"), *Point.ToString());
    }

    ThisRailroadSpline->UpdateSpline();
    ThisRailroadSpline->SetClosedLoop(false);
    // CreateSplineMesh();
}

void ARailroadSpline::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay called"));

    if (MovementCurve)
    {
        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("ProcessMovementTimeline"));
        MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

        FOnTimelineEventStatic TimelineFinishedCallback;
        TimelineFinishedCallback.BindUFunction(this, FName("OnEndMovementTimeline"));
        MovementTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
    }
}

void ARailroadSpline::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    MovementTimeline.TickTimeline(DeltaTime);
}

void ARailroadSpline::ProcessMovementTimeline(float Value)
{
    FVector CurrentSplineLocation = ThisRailroadSpline->GetLocationAtDistanceAlongSpline(ThisRailroadSpline->GetSplineLength() * Value, ESplineCoordinateSpace::World);
    FRotator CurrentSplineRotation = ThisRailroadSpline->GetRotationAtDistanceAlongSpline(ThisRailroadSpline->GetSplineLength() * Value, ESplineCoordinateSpace::World);
    
    if (MovingMesh)
    {
        MovingMesh->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
        // UE_LOG(LogTemp, Warning, TEXT("Moving Mesh to Location: %s Rotation: %s"), *CurrentSplineLocation.ToString(), *CurrentSplineRotation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MovingMesh is null"));
    }
}

void ARailroadSpline::OnEndMovementTimeline()
{
    UE_LOG(LogTemp, Warning, TEXT("Movement Timeline Finished"));

    // Toggle the direction
    bMovingForward = !bMovingForward;

    // Reverse the timeline direction and play again
    if (bMovingForward)
    {
        MovementTimeline.PlayFromStart();
    }
    else
    {
        MovementTimeline.ReverseFromEnd();
    }

    // Update the money
    AIronHorizonPlayerPawn* PlayerPawn = Cast<AIronHorizonPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerPawn && PlayerPawn->MoneyWidget)
    {
        PlayerPawn->UpdateMoney(RoutePrestige / ThisRailroadSpline->GetSplineLength() * 100);
        UE_LOG(LogTemp, Warning, TEXT("Updated money by %f"), RoutePrestige / ThisRailroadSpline->GetSplineLength() * 100);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to update money"));
    }
}

void ARailroadSpline::BeginMovement()
{
    if (MovementCurve)
    {
        UE_LOG(LogTemp, Warning, TEXT("Beginning Movement"));
        MovementTimeline.PlayFromStart();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MovementCurve is null"));
    }
}

void ARailroadSpline::CreateSplineMesh()
{
    const int32 NumPoints = ThisRailroadSpline->GetNumberOfSplinePoints();
    UE_LOG(LogTemp, Warning, TEXT("Creating Spline Mesh. Num Points: %d"), NumPoints);

    for (int32 i = 0; i < NumPoints - 1; ++i)
    {
        USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), *FString::Printf(TEXT("SplineMesh_%d"), i));
        if (SplineMesh)
        {
            SplineMesh->SetStaticMesh(ThisRailroadMesh);
            SplineMesh->SetMobility(EComponentMobility::Movable);
            SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
            SplineMesh->AttachToComponent(ThisRailroadSpline, FAttachmentTransformRules::KeepRelativeTransform);
            SplineMesh->RegisterComponentWithWorld(GetWorld());

            FVector StartPos, StartTangent, EndPos, EndTangent;
            ThisRailroadSpline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTangent, ESplineCoordinateSpace::Local);
            ThisRailroadSpline->GetLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

            SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);

            UE_LOG(LogTemp, Warning, TEXT("Spline Mesh Created: StartPos: %s, EndPos: %s"), *StartPos.ToString(), *EndPos.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create SplineMeshComponent"));
        }
    }
}