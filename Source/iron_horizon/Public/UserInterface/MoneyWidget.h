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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* CurrentCurrency;

    int32 CurrentMoneyAmount = 0;
};