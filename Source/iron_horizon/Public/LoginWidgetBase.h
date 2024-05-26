#pragma once

#include "CoreMinimal.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidgetBase.generated.h"

class UIronHorizonAccessLayer;
class UUserAccountDetails;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserLogon, UUserAccountDetails*, UserDetails);

UCLASS(Abstract)
class IRON_HORIZON_API ULoginWidgetBase : public UUserAccountWidgetBase
{
public:
    virtual void OnWidgetInitialized() override;

    UPROPERTY(BlueprintAssignable, Category = "User Account")
    FUserLogon OnUserLogon;
    
    UFUNCTION()
    virtual void PerformLogin();

    UFUNCTION()
    void Clean();

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UEditableTextBox* UserNameInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UEditableTextBox* PasswordInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UButton* LoginAction = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UTextBlock* ErrorText = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Messages")
    FText ErrorMessage;

    UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetErrorMessage(FText InErrorMessage);
    
protected:
private:
    GENERATED_BODY()
};