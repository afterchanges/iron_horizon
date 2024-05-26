#include "iron_horizon/Public/UserDetailsWidgetBase.h"
#include "iron_horizon/Public/UserAccountDetails.h"

void UUserDetailsWidgetBase::OnWidgetInitialized()
{
    Super::OnWidgetInitialized();
    LogoutAction->OnPressed.AddDynamic(this, &UUserDetailsWidgetBase::PerformLogout);
}

void UUserDetailsWidgetBase::PerformLogout()
{
    OnUserLogout.Broadcast(CurrentUser->UserID);
    CurrentUser = nullptr;
    UserNameLabel->SetText(FText::GetEmpty());
    TaglineLabel->SetText(FText::GetEmpty());
}

void UUserDetailsWidgetBase::OnUserAccountSet()
{
    Super::OnUserAccountSet();
    UserNameLabel->SetText(FText::FromString(CurrentUser->UserName));
    TaglineLabel->SetText(FText::FromString(CurrentUser->TagLine));
}