#include "HotbarWidget.h"
#include "HotbarItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UHotbarWidget::UpdateHotbar(const TArray<FHotbarItem>& Items)
{
	TArray<UImage*> Slots = { Slot1, Slot2, Slot3, Slot4, Slot5 };
	TArray<UTextBlock*> SlotCounts = { Slot1Count, Slot2Count, Slot3Count, Slot4Count, Slot5Count };

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i])
		{
			
			if (Items.IsValidIndex(i))
			{
				const FHotbarItem& CurrentItem = Items[i];

				
				if (CurrentItem.ItemImage)
				{
					Slots[i]->SetBrushFromTexture(CurrentItem.ItemImage);
				}

		
				if (SlotCounts.IsValidIndex(i))
				{
					SlotCounts[i]->SetText(FText::AsNumber(CurrentItem.Count)); 
				}

				Slots[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				Slots[i]->SetBrushFromTexture(EmptySlotTexture);  
				if (SlotCounts.IsValidIndex(i))
				{
					SlotCounts[i]->SetText(FText::FromString(TEXT("")));  
				}
				Slots[i]->SetVisibility(ESlateVisibility::Visible);  
			}
		}
	}
}





void UHotbarWidget::ShowHotbar()
{
	SetVisibility(ESlateVisibility::Visible);
	
}

UTexture2D* EmptySlotTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/EmptySlotImage"));  

void UHotbarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UImage*> Slots = { Slot1, Slot2, Slot3, Slot4, Slot5 };

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i])
		{
			Slots[i]->SetBrushFromTexture(EmptySlotTexture);  
			Slots[i]->SetVisibility(ESlateVisibility::Visible);  
		}
	}
}
