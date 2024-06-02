// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMainMenu.h"
#include "EOSGameInstance.h"
#include "Components/Button.h"

void UStartMainMenu::NativeConstruct() {
    Super::NativeConstruct();
    gameInstance = GetGameInstance<UEOSGameInstance>();

    loginButton->OnClicked.AddDynamic(this, &UStartMainMenu::loginButtonClicked);
}

void UStartMainMenu::loginButtonClicked() {
    if (gameInstance) {
        gameInstance->Login();
    }
}