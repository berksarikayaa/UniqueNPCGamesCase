#pragma once

#include "CoreMinimal.h"
#include "HotbarItem.h"  
#include "Blueprint/UserWidget.h"
#include "HotbarWidget.generated.h"

UCLASS()
class MYPROJECT_API UHotbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* Slot1;

	UPROPERTY(meta = (BindWidget))
	class UImage* Slot2;

	UPROPERTY(meta = (BindWidget))
	class UImage* Slot3;

	UPROPERTY(meta = (BindWidget))
	class UImage* Slot4;

	UPROPERTY(meta = (BindWidget))
	class UImage* Slot5;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot1Count;  

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot2Count; 

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot3Count;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot4Count;  

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot5Count; 

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	TArray<FHotbarItem> HotbarItems;

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void UpdateHotbar(const TArray<FHotbarItem>& Items);

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void ShowHotbar();
	void NativeConstruct();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	UTexture2D* EmptySlotTexture;
};
