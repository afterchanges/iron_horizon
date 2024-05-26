#include "IronHorizonAccessLayer.h"
#include "PreparedStatementManager.h"
#include "Hash/Blake3.h"
#include "Public/UserAccountDetails.h"
#include "SqliteGameDB/Public/CustomLogging.h"

void UIronHorizonAccessLayer::Build(FString DatabaseFilePath, FGameDbConfig Config) {
    QueryManager->LoadStatements();
}

int32 UIronHorizonAccessLayer::TestUserLogin(FString UserName, FString Password) {
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_TestLogin)) {
        Statement->SetBindingValue(P_UserName, UserName);
        Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));
        FQueryResultField Result = Statement->ExecuteScalar();
        return (Result.IsNull() || Result.IntVal == 0) ? -1 : Result.IntVal;
    }
    return -1;
}

int32 UIronHorizonAccessLayer::CreateUserAccount(FString UserName, FString Password, FString TagLine) {
    FString TrimmedUserName = UserName.TrimStartAndEnd();
    FString TrimmedTagLine = TagLine.TrimStartAndEnd();
    if (TrimmedUserName.IsEmpty()) return -1;
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_CreateAccount)) {
        Statement->SetBindingValue(P_UserName, TrimmedUserName);
        Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));
        Statement->SetBindingValue(P_TagLine, TrimmedTagLine);
        FQueryResultField Result = Statement->ExecuteScalar();
        return Result.IsNull() ? -1 : Result.IntVal;
    }
    return -1;
}

bool UIronHorizonAccessLayer::EditUserAccount(int32 UserID, FString UserName, FString Tagline, FString Password) {
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_EditAccount)) {
        Statement->SetBindingValue(P_UserID, UserID);
        Statement->SetBindingValue(P_UserName, UserName.TrimStartAndEnd());
        Statement->SetBindingValue(P_TagLine, Tagline.TrimStartAndEnd());
        Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));
        FQueryResultField Result = Statement->ExecuteScalar();
        return Result.IsNull() ? false : true;
    }
    return false;
}

bool UIronHorizonAccessLayer::ChangeUserPassword(int32 UserID, FString CurrentPassword, FString NewPassword) {
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_ChangePassword)) {
        Statement->SetBindingValue(P_UserID, UserID);
        Statement->SetBindingValue(P_NewPasswordHash, HashPassword(NewPassword));
        Statement->SetBindingValue(P_CurrentPasswordHash, HashPassword(CurrentPassword));
        FQueryResultField Result = Statement->ExecuteScalar();
        return Result.IsNull() ? false : true;
    }
    return false;
}

bool UIronHorizonAccessLayer::DeleteUserAccount(int32 UserID, FString Password) {
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_DeleteAccount)) {
        Statement->SetBindingValue(P_UserID, UserID);
        Statement->SetBindingValue(P_CurrentPasswordHash, HashPassword(Password));
        FQueryResultField Result = Statement->ExecuteScalar();
        return Result.IsNull() ? false : true;
    }
    return false;
}

bool UIronHorizonAccessLayer::IsUserNameInUse(FString UserName, int32 UserID) {
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_IsUserNameInUse)) {
        Statement->SetBindingValue(P_UserName, UserName.TrimStartAndEnd());
        Statement->SetBindingValue(P_UserID, UserID);
        FQueryResultField Result = Statement->ExecuteScalar();
        return Result.IsNull() ? true : Result.BoolVal;
    }
    return true;
}

UUserAccountDetails* UIronHorizonAccessLayer::GetUserAccountDetails(int32 UserID) {
    UUserAccountDetails* UserAccount = nullptr;
    if (UDbStatement* Statement = QueryManager->FindStatement(Q_GetAccount)) {
        Statement->SetBindingValue(P_UserID, UserID);
        UserAccount = Statement->CreateObjectFromData<UUserAccountDetails>();
    }
    return UserAccount;
}

FString UIronHorizonAccessLayer::HashPassword(FString Password) {
    FBlake3 Hash;
    Hash.Update(*Password, Password.Len());
    TStringBuilder<70> HashHex;
    HashHex << Hash.Finalize();
    const FString HashedPw = FString(HashHex.ToString());
    return HashedPw;
}