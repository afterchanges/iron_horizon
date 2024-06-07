// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MoneyWidget.generated.h"

UCLASS()
class IRON_HORIZON_API UMoneyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Money")
    void UpdateMoney(int32 NewMoneyAmount);

protected:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* CurrentCurrency;

private:
	int32 CurrentMoneyAmount = 0;
};
