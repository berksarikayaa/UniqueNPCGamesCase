#include "HotbarWidget.h"
#include "HotbarItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UHotbarWidget::UpdateHotbar(const TArray<FHotbarItem>& Items)
{
	// Slotları ve sayıları tutan diziler
	TArray<UImage*> Slots = { Slot1, Slot2, Slot3, Slot4, Slot5 };
	TArray<UTextBlock*> SlotCounts = { Slot1Count, Slot2Count, Slot3Count, Slot4Count, Slot5Count };

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i])
		{
			// Eğer item geçerli bir index ise, slotu görünür yap
			if (Items.IsValidIndex(i))
			{
				const FHotbarItem& CurrentItem = Items[i];

				// Slot simgesini güncelle
				if (CurrentItem.ItemImage)
				{
					Slots[i]->SetBrushFromTexture(CurrentItem.ItemImage);  // Item görseli
				}

				// Sayıyı göster
				if (SlotCounts.IsValidIndex(i))
				{
					SlotCounts[i]->SetText(FText::AsNumber(CurrentItem.Count));  // Item sayısı
				}

				Slots[i]->SetVisibility(ESlateVisibility::Visible);  // Slot görünür
			}
			else
			{
				// Boş slot görselini ve sayısını göster
				Slots[i]->SetBrushFromTexture(EmptySlotTexture);  // Boş slot görseli
				if (SlotCounts.IsValidIndex(i))
				{
					SlotCounts[i]->SetText(FText::FromString(TEXT("")));  // Sayıyı gizle
				}
				Slots[i]->SetVisibility(ESlateVisibility::Visible);  // Slot her zaman görünür
			}
		}
	}
}





void UHotbarWidget::ShowHotbar()
{
	// Hotbar'ı görünür yapıyoruz
	SetVisibility(ESlateVisibility::Visible);
	
}

UTexture2D* EmptySlotTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/EmptySlotImage"));  // Gri boş slot görseli

// HotbarWidget.cpp
void UHotbarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Başlangıçta tüm slotları boş (gri) yapalım
	TArray<UImage*> Slots = { Slot1, Slot2, Slot3, Slot4, Slot5 };

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i])
		{
			// Her slotu gri (boş) olarak ayarlayalım
			Slots[i]->SetBrushFromTexture(EmptySlotTexture);  // Boş görseli ayarlıyoruz
			Slots[i]->SetVisibility(ESlateVisibility::Visible);  // Slotu görünür yapıyoruz
		}
	}
}
