#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

// Hotbar item yapısı
USTRUCT(BlueprintType)
struct FHotbarItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsUsable;
};

UCLASS()
class YOURGAME_API AMainCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMainCharacter();

    // Interaction için trace fonksiyonu
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void PerformInteractionTrace();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // Hotbar özellikleri
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
    TArray<FHotbarItem> HotbarItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
    int32 MaxHotbarSlots = 6;

    // Interaction özellikleri
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionTraceRadius = 30.0f;

    // Blueprint'te implement edilecek event
    UFUNCTION(BlueprintImplementableEvent, Category = "Hotbar")
    void OnHotbarItemUsed(const FHotbarItem& UsedItem, int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Hotbar")
    void UseHotbarItem(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Hotbar")
    bool AddItemToHotbar(const FHotbarItem& NewItem);

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    
    // Input binding fonksiyonları
    void BindHotbarInputs();
    void InitializeHotbar();

    // Interaction için son tespit edilen aktör
    UPROPERTY()
    AActor* LastInteractableActor;
}; 