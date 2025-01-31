#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameFramework/Character.h"
#include "HotbarWidget.h"
#include "HotbarItem.h"  
#include "Containers/Array.h"

#include "MyProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

public:
	AMyProjectCharacter();

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* HotbarMappingContext;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Interact with NPCs or objects */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FString> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseHotbarSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseHotbarSlot2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseHotbarSlot3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseHotbarSlot4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseHotbarSlot5Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotbar")
	TArray<FHotbarItem> HotbarItems;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HotbarWidgetClass;

	UHotbarWidget* HotbarWidget;
	
	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void AddItemToHotbar(const FString& ItemName, UTexture2D* ItemImage);

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void UseHotbarItem(int32 SlotIndex);

	void UpdateOptionButton3Text(const FString& ItemName);

	void SetLastReceivedItem(const FString& ItemName);

	void ShowDialogue(AActor* InteractedNPC);

	FString GetLastReceivedItem() const;

private:
    FString LastReceivedItem;  

	UPROPERTY()
	UUserWidget* CurrentDialogueWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DialogueWidgetClass;

	UPROPERTY()
	AActor* CurrentInteractedNPC;

	UPROPERTY()
	UButton* OptionButton1;

	UPROPERTY()
	UButton* OptionButton2;

	UPROPERTY()
	UButton* OptionButton3;

	void BindDialogueButtons();

	void CloseDialogue();
	
	UFUNCTION()
	void OnOption1Selected();

	UFUNCTION()
	void OnOption2Selected();

	UFUNCTION()
	void OnOption3Selected();

	UFUNCTION()
	void OnDialogueOptionSelected();

protected:
	virtual void BeginPlay() override;
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

};
