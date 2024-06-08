// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MoneyWidget.h"
#include "Components/TextBlock.h"


void UMoneyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CurrentCurrency)
    {
        FString CurrentCurrencyString = CurrentCurrency->GetText().ToString();
        CurrentMoneyAmount = FCString::Atoi(*CurrentCurrencyString);
    }
}

void UMoneyWidget::UpdateMoney(int32 NewMoneyAmount)
{
    if (CurrentMoneyAmount == 0 && CurrentCurrency)
    {
        FString CurrentCurrencyString = CurrentCurrency->GetText().ToString();
        CurrentMoneyAmount = FCString::Atoi(*CurrentCurrencyString);
    }

    CurrentMoneyAmount += NewMoneyAmount;
    CurrentCurrency->SetText(FText::FromString(FString::FromInt(CurrentMoneyAmount)));
}