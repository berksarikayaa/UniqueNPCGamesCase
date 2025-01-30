#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatWidget.generated.h"

class UTextBlock;

UCLASS()
class MYPROJECT_API UStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateStats(float Yas, int32 Para, bool Acgozluluk);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* YasText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ParaText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AcgozlulukText;
};
