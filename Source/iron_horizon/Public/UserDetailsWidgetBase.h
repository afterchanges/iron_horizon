#pragma once

#include "CoreMinimal.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "UserDetailsWidgetBase.generated.h"

class UUserAccountDetails;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserLogout, int32, UserID);

UCLASS(Abstract)
class IRON_HORIZON_API UUserDetailsWidgetBase : public UUserAccountWidgetBase
{
public:
    virtual void OnWidgetInitialized() override;
    
    UFUNCTION()
    virtual void PerformLogout();
    virtual void OnUserAccountSet() override;

    UPROPERTY(BlueprintAssignable, Category = "User Account")
    FUserLogout OnUserLogout;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UTextBlock* UserNameLabel = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UTextBlock* TaglineLabel = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
    UButton* LogoutAction = nullptr;

protected:
private:
    GENERATED_BODY()
};