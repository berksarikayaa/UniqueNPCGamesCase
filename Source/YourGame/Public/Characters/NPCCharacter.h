#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "NPCCharacter.generated.h"

UCLASS()
class YOURGAME_API ANPCCharacter : public ACharacter, public IInteractionInterface
{
    GENERATED_BODY()

public:
    ANPCCharacter();

    virtual void Interact(AActor* Interactor) override;
    virtual void HandleOption1() override;
    virtual void HandleOption2() override;
    virtual void HandleOption3() override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Properties")
    float RandomProperty1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Properties")
    int32 RandomProperty2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Properties")
    bool RandomProperty3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Movement")
    FVector TargetLocation;

private:
    void InitializeRandomProperties();
    void MoveToTarget();
}; 