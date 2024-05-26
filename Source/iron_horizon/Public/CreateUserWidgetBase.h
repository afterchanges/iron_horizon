#pragma once

#include "CoreMinimal.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Blueprint/UserWidget.h"
#include "CreateUserWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserCreation, UUserAccountDetails*, UserDetails);

UCLASS(Abstract)
class IRON_HORIZON_API UCreateUserWidgetBase : public UUserAccountWidgetBase
{
public:
    virtual void OnWidgetInitialized() override;

    UPROPERTY(BlueprintAssignable, Category = "User Account")
    FUserCreation OnAccountCreated;

    UFUNCTION()
    virtual void CreateUserAccount();

    UFUNCTION()
    void Clean();
    
    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UEditableTextBox* UserNameInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UMultiLineEditableTextBox* TaglineInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UEditableTextBox* PasswordInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UEditableTextBox* ConfirmPasswordInput = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UButton* CreateAccountAction = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UTextBlock* ErrorText = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Messages")
    FText ErrorMessage;

protected:
private:
    GENERATED_BODY()
};