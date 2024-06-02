// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class IRON_HORIZON_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
	void Login();
protected:
	virtual void Init() override;
private:
	class IOnlineSubsystem* onlineSubsystem;
	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;

	void LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};
