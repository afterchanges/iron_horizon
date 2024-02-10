// © Phoenix-100

#include "RTSPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ARTSPawn::ARTSPawn()
{
	// Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SetRootComponent(SphereComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-60, 0, 0));
	SpringArm->TargetArmLength = 4000;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 6;
	SpringArm->CameraRotationLagSpeed = 6;
	SpringArm->bDoCollisionTest = false;
	CameraComponent->SetupAttachment(SpringArm);

}

// Called when the game starts or when spawned
void ARTSPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetZoom = SpringArm->TargetArmLength;
	TargetVector = GetActorLocation();
	FTimerHandle Handler;
	// Wait to make sure the viewport is initialized
	GetWorldTimerManager().SetTimer(Handler, this, &ARTSPawn::SetBoundaries, 0.1f, false, 0.2f);

	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &ARTSPawn::ViewportSizeChanged);
	
	DefaultArmLength = SpringArm->TargetArmLength;
	TargetRotation = DefaultRotation;

}

// Called every frame
void ARTSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MouseMovement(DeltaTime);

	MoveTo(DeltaTime);
	if (HeightAdjustment)
	{
		SetTargetHeight();
		AdjustHeight(DeltaTime);
	}

	if (GetActorRotation().Yaw != TargetRotation)
	{
		const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(),
																			 FRotator(GetActorRotation().Pitch,
																					  TargetRotation,
																					  GetActorRotation().Roll),
																			 DeltaTime,
																			 RotationSmoothness);

		SetActorRotation(CurrentTargetRotation, ETeleportType::None);
	}

	if (SpringArm->TargetArmLength != TargetZoom)
	{
		float CurrentTargetZoom = UKismetMathLibrary::FInterpTo(SpringArm->TargetArmLength,
																TargetZoom, DeltaTime,
																ZoomSmoothness);
		CurrentTargetZoom = UKismetMathLibrary::Clamp(CurrentTargetZoom, MinSpringArm, MaxSpringArm);
		SpringArm->TargetArmLength = CurrentTargetZoom;
	}
}

// Called to bind functionality to input
void ARTSPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the EnhancedInputComponent
	PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(XAxisAction, ETriggerEvent::Triggered, this, &ARTSPawn::XMovement);
	XBinding = &PEI->BindActionValue(XAxisAction);

	PEI->BindAction(YAxisAction, ETriggerEvent::Triggered, this, &ARTSPawn::YMovement);
	YBinding = &PEI->BindActionValue(YAxisAction);

	PEI->BindAction(RotationAxisAction, ETriggerEvent::Triggered, this, &ARTSPawn::RotateCamera);
	PEI->BindAction(ZoomAxisAction, ETriggerEvent::Triggered, this, &ARTSPawn::Zoom);
	PEI->BindAction(ResetCameraAction, ETriggerEvent::Triggered, this, &ARTSPawn::ResetCamera);
}

// Called when the game starts or when spawned
void ARTSPawn::ResetCamera()
{
	TargetRotation = DefaultRotation;
	TargetZoom=DefaultArmLength;
}

void ARTSPawn::XMovement(const FInputActionValue &Value)
{
	float AxisValue = Value.GetMagnitude();
	FVector Movement;
	Movement.X = AxisValue * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Y = 0;
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}

void ARTSPawn::YMovement(const FInputActionValue &Value)
{
	float AxisValue = Value.GetMagnitude();
	FVector Movement;
	Movement.X = 0;
	Movement.Y = AxisValue * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}

void ARTSPawn::MouseMovement(const float DeltaTime)
{
	const float XValue = XBinding->GetValue().Get<float>();
	const float YValue = YBinding->GetValue().Get<float>();

	if (XValue != 0 || YValue != 0)
	{
		return;
	}

	float DirectionX = 0;
	float DirectionY = 0;

	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	if (this->IsValidMousePosition(MousePosition, Top))
	{
		DirectionX = 1;
	}
	else if (IsValidMousePosition(MousePosition, Bottom))
	{
		DirectionX = -1;
	}

	if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Right))
	{
		DirectionY = 1;
	}
	else if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Left))
	{
		DirectionY = -1;
	}

	FVector Movement;
	Movement.X = DirectionX * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Y = DirectionY * MovementMultiplier * GetAdjustedDeltaTime();
	Movement.Z = 0;
	Movement = UKismetMathLibrary::TransformDirection(GetActorTransform(), Movement);
	TargetVector.X = TargetVector.X + Movement.X;
	TargetVector.Y = TargetVector.Y + Movement.Y;
}

void ARTSPawn::MoveTo(const float DeltaTime)
{
	const FVector CurrentTargetVector = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetVector, DeltaTime,
																	  MovementSmoothness);
	SetActorLocation(FVector(CurrentTargetVector.X, CurrentTargetVector.Y, GetActorLocation().Z));
}

void ARTSPawn::AdjustHeight(float DeltaTime)
{
	const FVector CurrentLocation = GetActorLocation();

	if (CurrentLocation.Z != TargetHeight)
	{
		const float HeightToSet = UKismetMathLibrary::FInterpTo(CurrentLocation.Z, TargetHeight,
																DeltaTime,
																HeightAdjustmentSmoothness);
		SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, HeightToSet));
	}
}

void ARTSPawn::RotateCamera(const FInputActionValue &Value)
{
	float AxisValue = Value.GetMagnitude();
	int ZoomStep = (MaxSpringArm - MinSpringArm) / ZoomSteps;

	TargetRotation = TargetRotation + (AxisValue * RotationMultiplier * GetAdjustedDeltaTime());
}

void ARTSPawn::Zoom(const FInputActionValue &Value)
{
	float AxisValue = Value.GetMagnitude();
	if (AxisValue != 0)
	{
		TargetZoom = TargetZoom + (AxisValue * ZoomMultiplier);

		if (UseZoomSteps)
		{
			int ZoomStepSize = (MaxSpringArm - MinSpringArm) / ZoomSteps;
			float ZoomStep = SpringArm->TargetArmLength / ZoomStepSize;
			int Step = UKismetMathLibrary::Round(ZoomStep);
			if (AxisValue > 0)
			{
				Step++;
			}
			else if (AxisValue < 0)
			{
				Step--;
			}
			TargetZoom = Step * ZoomStepSize;
		}

		TargetZoom = UKismetMathLibrary::Clamp(TargetZoom, MinSpringArm, MaxSpringArm);
	}
}

bool ARTSPawn::IsValidMousePosition(const FVector2D Positions, const FVector4 Rules)
{
	const bool InputY = Positions.Y >= Rules.X && Positions.Y <= Rules.Y;
	const bool InputX = Positions.X >= Rules.Z && Positions.X <= Rules.W;
	return InputX && InputY;
}

void ARTSPawn::SetBoundaries()
{
	const FVector2D Result = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	Top = FVector4(0, 0, 0, 0);
	Bottom = FVector4(0, 0, 0, 0);
	Left = FVector4(0, 0, 0, 0);
	Right = FVector4(0, 0, 0, 0);

	Top.Y = (MovementZoneInPercent / 100.f) * Result.Y;
	Top.W = Result.X;

	Bottom.X = (1 - (MovementZoneInPercent / 100.f)) * Result.Y;
	Bottom.Y = Result.Y;
	Bottom.W = Result.X;

	Left.Y = (MovementZoneInPercent / 100.f) * Result.X;
	Left.W = Result.Y;

	Right.X = (1 - (MovementZoneInPercent / 100.f)) * Result.X;
	Right.Y = Result.X;
	Right.W = Result.Y;

	StartHeight = GetActorLocation().Z;
	PreviousHeight = GetActorLocation().Z;

	if (IgnoreBoundaries)
	{
		Top.X = -Deactivate;
		Top.Z = -Deactivate;
		Top.W = Deactivate;

		Bottom.Y = Deactivate;
		Bottom.Z = -Deactivate;
		Bottom.W = Deactivate;

		Left.X = -Deactivate;
		Left.Z = -Deactivate;
		Left.W = Deactivate;

		Right.Y = Deactivate;
		Right.Z = -Deactivate;
		Right.W = Deactivate;
	}
}

void ARTSPawn::SetTargetHeight()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();

	if (TranceFromCamera)
	{
		Start = CameraComponent->GetComponentLocation();
		End = CameraComponent->GetComponentLocation();
	}

	Start.Z = 999999;
	End.Z = -999999;

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(TraceChannel);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, TraceChannel);

	if (UKismetMathLibrary::Abs(PreviousHeight - HitResult.Location.Z) >= Threshold)
	{
		TargetHeight = HitResult.Location.Z;
		PreviousHeight = HitResult.Location.Z;
	}
	else if (UKismetMathLibrary::Abs(HitResult.Location.Z - StartHeight) < Threshold)
	{
		TargetHeight = StartHeight;
	}
}

float ARTSPawn::GetAdjustedDeltaTime() const
{
	return GetWorld()->GetDeltaSeconds() * 100;
}

void ARTSPawn::ViewportSizeChanged(FViewport *ViewPort, uint32 val)
{
	SetBoundaries();
}
