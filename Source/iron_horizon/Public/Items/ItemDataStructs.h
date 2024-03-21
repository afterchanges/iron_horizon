#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemType : uint8 {
    Surface_Railway UMETA(DisplayName = "Surface Railway"),
    Tunnel_Railway UMETA(DisplayName = "Tunnel Railway"),
};

USTRUCT()
struct FItemTextData {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FText Name;

    UPROPERTY(EditAnywhere)
    FText Description;

    UPROPERTY(EditAnywhere)
    FText InteractionText;

    UPROPERTY(EditAnywhere)
    FText UseText;
};

USTRUCT()
struct FItemAssetData {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere)
    UStaticMesh* Mesh;
};

USTRUCT()
struct FItemNumericData {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    float SpeedCoefficient;

    UPROPERTY(EditAnywhere)
    float Cost;

    UPROPERTY(EditAnywhere)
    float Weight;

    UPROPERTY(EditAnywhere)
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere)
    bool bIsStackable;
};

USTRUCT()
struct FItemData : public FTableRowBase {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FName ID;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemTextData TextData;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemAssetData AssetData;

    UPROPERTY(EditAnywhere, Category = "Item Data")
    FItemNumericData NumericData;

};
