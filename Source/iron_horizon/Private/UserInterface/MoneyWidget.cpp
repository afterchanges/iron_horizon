// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MoneyWidget.h"

void UMoneyWidget::UpdateMoney(int32 NewMoneyAmount)
{
    CurrentMoneyAmount += NewMoneyAmount;
    CurrentCurrency->SetText(FText::AsNumber(CurrentMoneyAmount));
}