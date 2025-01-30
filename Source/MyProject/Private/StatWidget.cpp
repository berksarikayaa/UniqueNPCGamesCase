#include "StatWidget.h"
#include "Components/TextBlock.h"

void UStatWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStatWidget::UpdateStats(float Yas, int32 Para, bool Acgozluluk)
{
	if (!YasText || !ParaText || !AcgozlulukText)
	{
		UE_LOG(LogTemp, Error, TEXT("StatWidget içinde TextBlock bileşenleri bulunamadı!"));
		return;
	}

	YasText->SetText(FText::FromString(FString::Printf(TEXT("Yaş: %.0f"), Yas)));
	ParaText->SetText(FText::FromString(FString::Printf(TEXT("Para: %d"), Para)));
	AcgozlulukText->SetText(FText::FromString(Acgozluluk ? TEXT("Açgözlü") : TEXT("Açgözlü Değil")));
}
