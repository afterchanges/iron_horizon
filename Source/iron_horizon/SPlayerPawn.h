// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SPlayerPawn.generated.h"

class USceneComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class IRON_HORIZON_API ASPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASPlayerPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveForward(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION()
	void Zoom(float AxisValue);

	UFUNCTION()
	void RotateRight();

	UFUNCTION()
	void RotateLeft();

	UFUNCTION()
	void EnableRotation();

	UFUNCTION()
	void DisableRotation();

	UFUNCTION()
	void RotateHorizontal(float AxisValue);

	UFUNCTION()
	void RotateVertical(float AxisValue);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraMoveSpeed = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraZoomSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraZoomMin = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraZoomMax = 3000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraRotateSpeed = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraRotatePitchMin = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraRotatePitchMax = 80.0f;

private:	
	UFUNCTION()
	void CameraBounds();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY() 
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float TargetZoom;

	UPROPERTY()
	bool CanRotate;

};
