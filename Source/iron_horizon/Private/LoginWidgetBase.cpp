#include "iron_horizon/Public/LoginWidgetBase.h"
#include "iron_horizon/IronHorizonAccessLayer.h"

void ULoginWidgetBase::OnWidgetInitialized()
{
    Super::OnWidgetInitialized();
    LoginAction->OnPressed.AddDynamic(this, &ULoginWidgetBase::PerformLogin);
}

void ULoginWidgetBase::PerformLogin()
{
    if (Dal)
    {
        FString UserName = UserNameInput->GetText().ToString();
        FString Password = PasswordInput->GetText().ToString();

        int32 UserID = Dal->TestUserLogin(UserName, Password);

        if (UserID != -1)
        {
            Clean();
            OnUserLogon.Broadcast(Dal->GetUserAccountDetails(UserID));
        }
        else
        {
            PasswordInput->SetText(FText::GetEmpty());
            ErrorText->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void ULoginWidgetBase::Clean()
{
    UserNameInput->SetText(FText::GetEmpty());
    PasswordInput->SetText(FText::GetEmpty());
    ErrorText->SetVisibility(ESlateVisibility::Hidden);
    UserNameInput->SetFocus();
}

void ULoginWidgetBase::SetErrorMessage(FText InErrorMessage)
{
    ErrorMessage = InErrorMessage;
}