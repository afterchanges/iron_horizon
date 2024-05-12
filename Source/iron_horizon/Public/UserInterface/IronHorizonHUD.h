// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interaction/InteractionWidget.h"
#include "MainMenu.h"
#include "IronHorizonHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
class UMainMenu;

UCLASS()

class IRON_HORIZON_API AIronHorizonHUD : public AHUD {
  GENERATED_BODY()
public:
  UPROPERTY(EditDefaultsOnly, Category = "Widgets")
  TSubclassOf<class UMainMenu> MainMenuClass;

  UPROPERTY(EditDefaultsOnly, Category = "Widgets")
  TSubclassOf<class UInteractionWidget> InteractionWidgetClass;

  bool bIsMenuVisible;

  AIronHorizonHUD();

  void DisplayMenu();
  void HideMenu();
  void ToggleMenu();

  void ShowInteractionWidget() const;
  void HideInteractionWidget() const;
  void UpdateInteractionWidget(const FInteractableData *InteractableData) const;

protected:
  UPROPERTY()
  UMainMenu *MainMenuWidget;

  UPROPERTY()
  UInteractionWidget *InteractionWidget;

  virtual void BeginPlay() override;
};
