// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class IRON_HORIZON_API UStartMainMenu : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	class UEOSGameInstance* gameInstance;
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* loginButton;

	UFUNCTION()
	void loginButtonClicked();
};
