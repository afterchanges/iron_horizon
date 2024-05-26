#include "iron_horizon/Public/EditUserWidgetBase.h"
#include "iron_horizon/IronHorizonAccessLayer.h"
#include "iron_horizon/Public/UserAccountDetails.h"

void UEditUserWidgetBase::OnWidgetInitialized()
{
    Super::OnWidgetInitialized();
    SaveChangesAction->OnPressed.AddDynamic(this, &UEditUserWidgetBase::SaveChanges);
}

void UEditUserWidgetBase::SaveChanges()
{
    if (Dal && CurrentUser)
    {
        FString UserName = UserNameInput->GetText().ToString();

        if (Dal->IsUserNameInUse(UserName, CurrentUser->UserID))
        {
            ErrorText->SetText(NSLOCTEXT("UserAccounts", "NameInUse", "Error: The selected name is already taken."));
            ErrorText->SetVisibility(ESlateVisibility::Visible);
            SuccessText->SetVisibility(ESlateVisibility::Hidden);
            return;
        }

        bool Result = Dal->EditUserAccount(CurrentUser->UserID,
                                           UserName,
                                           TaglineInput->GetText().ToString(),
                                           PasswordInput->GetText().ToString());

        if (Result)
        {
            CurrentUser = Dal->GetUserAccountDetails(CurrentUser->UserID);
            OnUserEdit.Broadcast(CurrentUser);
            PasswordInput->SetText(FText::GetEmpty());
            SuccessText->SetVisibility(ESlateVisibility::Visible);
            ErrorText->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            ErrorText->SetText(NSLOCTEXT("UserAccounts", "BadPassword", "Error: The password is incorrect."));
            ErrorText->SetVisibility(ESlateVisibility::Visible);
            SuccessText->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UEditUserWidgetBase::OnUserAccountSet()
{
    Super::OnUserAccountSet();
    UserNameInput->SetText(FText::FromString(CurrentUser->UserName));
    TaglineInput->SetText(FText::FromString(CurrentUser->TagLine));
}

void UEditUserWidgetBase::SetErrorMessage(FText InErrorMessage)
{
    ErrorMessage = InErrorMessage;
}

void UEditUserWidgetBase::SetSuccessMessage(FText InSuccessMessage)
{
    SuccessMessage = InSuccessMessage;
}