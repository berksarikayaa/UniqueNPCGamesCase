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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void PerformAction(int32 ActionIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateUI();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void HideStats();
};
