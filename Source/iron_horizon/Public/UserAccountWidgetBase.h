#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserAccountWidgetBase.generated.h"

class UUserAccountDetails;
class UIronHorizonAccessLayer;
/* */
UCLASS()
class IRON_HORIZON_API UUserAccountWidgetBase : public UUserWidget
{
public:
	virtual void NativeOnInitialized() override final;

	UFUNCTION()
	virtual void OnWidgetInitialized();

	UFUNCTION(BlueprintCallable)
	void SetUserDetails(UUserAccountDetails* InUserDetails);

	UFUNCTION()
	virtual void OnUserAccountSet();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	UPROPERTY()
	UIronHorizonAccessLayer* Dal = nullptr;

	UPROPERTY()
	UUserAccountDetails* CurrentUser = nullptr;

private:
	GENERATED_BODY()
};
