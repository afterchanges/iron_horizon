#pragma once

#include "CoreMinimal.h"
#include "DbBase.h"
#include "IronHorizonAccessLayer.generated.h"

class UDbEquipmentComponent;
class UUserAccountDetails;

UCLASS()
class IRON_HORIZON_API UIronHorizonAccessLayer : public UDbBase
{
public:

    int32 TestUserLogin(FString UserName, FString Password);
    int32 CreateUserAccount(FString UserName, FString Password, FString TagLine);
    bool EditUserAccount(int32 UserID, FString UserName, FString Tagline, FString Password);
    bool ChangeUserPassword(int32 UserID, FString CurrentPassword, FString NewPassword);
    bool DeleteUserAccount(int32 UserID, FString Password);
    bool IsUserNameInUse(FString UserName, int32 UserID = 0);
    UUserAccountDetails* GetUserAccountDetails(int32 UserID);

protected:
    virtual void Build(FString DatabaseFilePath, FGameDbConfig Config) override;
    virtual void TearDown() override;

private:
    FString HashPassword(FString Password);

    const FString Q_CreateAccount = TEXT("CreateAccount");
    const FString Q_EditAccount = TEXT("EditAccount");
    const FString Q_ChangePassword = TEXT("ChangePassword");
    const FString Q_ListAccounts = TEXT("ListAccounts");
    const FString Q_DeleteAccount = TEXT("DeleteAccount");
    const FString Q_TestLogin = TEXT("TestLogin");
    const FString Q_GetAccount = TEXT("GetAccount");
    const FString Q_IsUserNameInUse = TEXT("IsUserNameInUse");
    const FString P_LevelName = TEXT("@LevelName");
    const FString P_InstanceID = TEXT("@InstanceID");
    const FString P_EquipmentID = TEXT("@EquipmentID");
    const FString P_LevelID = TEXT("@LevelID");
    const FString P_Quantity = TEXT("@Quantity");
    const FString P_UserID = TEXT("@UserID");
    const FString P_UserName = TEXT("@UserName");
    const FString P_PasswordHash = TEXT("@PasswordHash");
    const FString P_TagLine = TEXT("@TagLine");
    const FString P_NewPasswordHash = TEXT("@NewPasswordHash");
    const FString P_CurrentPasswordHash = TEXT("@CurrentPasswordHash");
    const FString P_UserNameFilter = TEXT("@UserNameFilter");

    GENERATED_BODY()
};