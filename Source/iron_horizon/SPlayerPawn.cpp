// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASPlayerPawn::ASPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(CameraComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

}

// Called when the game starts or when spawned
void ASPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	TargetLocation = GetActorLocation();
	TargetZoom = 3000.0f;

	const FRotator Rotation = SpringArmComponent->GetComponentRotation();
	TargetRotation = FRotator(Rotation.Pitch + -50, Rotation.Yaw, 0.0f);
}

void ASPlayerPawn::MoveForward(float AxisValue)
{
	if (AxisValue == 0.0f)
	{
		return;
	}
	TargetLocation += SpringArmComponent->GetForwardVector() * AxisValue * CameraMoveSpeed;
}

void ASPlayerPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		return;
	}
	TargetLocation += SpringArmComponent->GetRightVector() * AxisValue * CameraMoveSpeed;
}

void ASPlayerPawn::Zoom(float AxisValue)
{
	if (AxisValue == 0.0f)
	{
		return;
	}
	const float Zoom = AxisValue * 100.0f;
	TargetZoom = FMath::Clamp(Zoom + TargetZoom, CameraZoomMin, CameraZoomMax);
}

void ASPlayerPawn::RotateRight()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(FRotator(0.0f, -45, 0.0f), TargetRotation);
}

void ASPlayerPawn::RotateLeft()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(FRotator(0.0f, 45.0f, 0.0f), TargetRotation);
}

void ASPlayerPawn::EnableRotation()
{
	CanRotate = true;
}

void ASPlayerPawn::DisableRotation()
{
	CanRotate = false;
}

void ASPlayerPawn::RotateHorizontal(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
	if (CanRotate) {
		TargetRotation = UKismetMathLibrary::ComposeRotators(FRotator(0.0f, AxisValue, 0.0f), TargetRotation);
	}
}

void ASPlayerPawn::RotateVertical(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
	if (CanRotate) {
		TargetRotation = UKismetMathLibrary::ComposeRotators(FRotator(AxisValue, 0.0f, 0.0f), TargetRotation);
	}
}

// Called every frame
void ASPlayerPawn::Tick(float DeltaTime)
{
	CameraBounds();

	Super::Tick(DeltaTime);

	const FVector NewLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, CameraMoveSpeed);
	SetActorLocation(NewLocation);

	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetComponentRotation(), TargetRotation, DeltaTime, CameraRotateSpeed);
	SpringArmComponent->SetRelativeRotation(NewRotation);

	const float NewZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, CameraZoomSpeed);
	SpringArmComponent->TargetArmLength = NewZoom;


}

// Called to bind functionality to input
void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASPlayerPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &ASPlayerPawn::Zoom);
	PlayerInputComponent->BindAxis(TEXT("RotateHorizontal"), this, &ASPlayerPawn::RotateHorizontal);
	PlayerInputComponent->BindAxis(TEXT("RotateVertical"), this, &ASPlayerPawn::RotateVertical);

	PlayerInputComponent->BindAction(TEXT("RotateRight"), IE_Pressed, this, &ASPlayerPawn::RotateRight);
	PlayerInputComponent->BindAction(TEXT("RotateLeft"), IE_Pressed, this, &ASPlayerPawn::RotateLeft);
	PlayerInputComponent->BindAction(TEXT("EnableRotation"), IE_Pressed, this, &ASPlayerPawn::EnableRotation);
	PlayerInputComponent->BindAction(TEXT("DisableRotation"), IE_Pressed, this, &ASPlayerPawn::DisableRotation);


}

void ASPlayerPawn::CameraBounds() {
	float newPitch = TargetRotation.Pitch;
	if (newPitch > (CameraRotatePitchMin * -1)) {
		newPitch = (-1 * CameraRotatePitchMin);
	}
	else if (newPitch < (CameraRotatePitchMax * -1)) {
		newPitch = (-1 * CameraRotatePitchMax);
	}
	
	TargetRotation = FRotator(newPitch, TargetRotation.Yaw, 0.f);

	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0.f);
}