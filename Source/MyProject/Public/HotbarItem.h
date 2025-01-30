#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"  // Görsel türü için gerekli
#include "UObject/NoExportTypes.h"
#include "HotbarItem.generated.h"  // Unreal'in gerekli makrosu

USTRUCT(BlueprintType)
struct FHotbarItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	UTexture2D* ItemImage;  // Görseli Texture2D olarak tutacağız

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	int32 Count;  // Item'ın stack sayısı

	FHotbarItem()
	{
		ItemName = TEXT("Default Item");
		ItemImage = nullptr;
		Count = 1;  // Başlangıçta 1 olarak ayarlıyoruz
	}
};
