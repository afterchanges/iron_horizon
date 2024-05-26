#include "iron_horizon/Public/CreateUserWidgetBase.h"
#include "UserAccountDetails.h"
#include "iron_horizon/IronHorizonAccessLayer.h"

void UCreateUserWidgetBase::OnWidgetInitialized()
{
    Super::OnWidgetInitialized();
    CreateAccountAction->OnPressed.AddDynamic(this, &UCreateUserWidgetBase::CreateUserAccount);
}

void UCreateUserWidgetBase::CreateUserAccount()
{
    if (Dal)
    {
        FString UserName = UserNameInput->GetText().ToString().TrimStartAndEnd();
        FString Tagline = TaglineInput->GetText().ToString().TrimStartAndEnd();
        FString Password = PasswordInput->GetText().ToString();
        FString ConfirmPassword = ConfirmPasswordInput->GetText().ToString();

        if (Dal->IsUserNameInUse(UserName, 0))
        {
            ErrorText->SetText(NSLOCTEXT("UserAccounts", "NameInUse", "Error: The selected name is already taken."));
            ErrorText->SetVisibility(ESlateVisibility::Visible);
            return;
        }

        if (Password != ConfirmPassword)
        {
            ErrorText->SetText(NSLOCTEXT("UserAccounts", "PasswordConfirmFail",
                                         "Error: The entered password, and password confirmation do not match."));
            ErrorText->SetVisibility(ESlateVisibility::Visible);
            return;
        }

        int32 NewUserID = Dal->CreateUserAccount(UserName, Password, Tagline);
        if (NewUserID != -1)
        {
            Clean();
            OnAccountCreated.Broadcast(Dal->GetUserAccountDetails(NewUserID));
        }
    }
}

void UCreateUserWidgetBase::Clean()
{
    UserNameInput->SetText(FText::GetEmpty());
    TaglineInput->SetText(FText::GetEmpty());
    PasswordInput->SetText(FText::GetEmpty());
    ConfirmPasswordInput->SetText(FText::GetEmpty());
    ErrorText->SetVisibility(ESlateVisibility::Hidden);
    UserNameInput->SetFocus();
}