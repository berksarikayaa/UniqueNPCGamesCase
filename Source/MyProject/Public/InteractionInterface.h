#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class MYPROJECT_API IInteractionInterface
{
	GENERATED_BODY()

public:
	/** NPC ile oyuncunun etkileşime girmesi */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	/** NPC'nin bir aksiyon gerçekleştirmesi */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void PerformAction(int32 ActionIndex);

	/** UI güncelleme işlemi */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateUI();

	/** NPC'nin istatistik panelini gizleme */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void HideStats();
};
