#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"  
#include "UObject/NoExportTypes.h"
#include "HotbarItem.generated.h"  

USTRUCT(BlueprintType)
struct FHotbarItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	UTexture2D* ItemImage;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	int32 Count;  

	FHotbarItem()
	{
		ItemName = TEXT("Default Item");
		ItemImage = nullptr;
		Count = 1;  
	}
};
