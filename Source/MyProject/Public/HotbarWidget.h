#pragma once

#include "CoreMinimal.h"
#include "HotbarItem.h"  // HotbarItem struct'ını dahil ettik
#include "Blueprint/UserWidget.h"
#include "HotbarWidget.generated.h"

UCLASS()
class MYPROJECT_API UHotbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Hotbar'daki slotların UI'daki referansları
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
	class UTextBlock* Slot1Count;  // 1. slot için sayıyı gösterecek TextBlock

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot2Count;  // 2. slot için sayıyı gösterecek TextBlock

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot3Count;  // 3. slot için sayıyı gösterecek TextBlock

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot4Count;  // 4. slot için sayıyı gösterecek TextBlock

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot5Count;  // 5. slot için sayıyı gösterecek TextBlock

	
	// Hotbar'ın item'ları
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	TArray<FHotbarItem> HotbarItems;

	// UpdateHotbar fonksiyonu ve diğer fonksiyonlar
	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void UpdateHotbar(const TArray<FHotbarItem>& Items);

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void ShowHotbar();
	void NativeConstruct();

	// Boş slot için görsel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	UTexture2D* EmptySlotTexture;
};
