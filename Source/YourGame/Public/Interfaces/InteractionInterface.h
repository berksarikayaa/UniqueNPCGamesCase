#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
    GENERATED_BODY()
};

class YOURGAME_API IInteractionInterface
{
    GENERATED_BODY()

public:
    // NPC ile etkileşime geçildiğinde çağrılacak fonksiyon
    virtual void Interact(AActor* Interactor) = 0;
    
    // Etkileşim seçenekleri için fonksiyonlar
    virtual void HandleOption1() = 0;
    virtual void HandleOption2() = 0;
    virtual void HandleOption3() = 0;
}; 