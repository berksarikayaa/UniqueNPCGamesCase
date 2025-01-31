#include "MyProjectCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HotbarItem.h"
#include "InputActionValue.h"
#include "NPCCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "HotbarItem.h"

#include "Components/TextBlock.h"
#include "Engine/LocalPlayer.h"
#include "Public/InteractionInterface.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();


	
	if (DialogueWidgetClass)
	{

		CurrentDialogueWidget = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);

	}


	if (HotbarWidgetClass == nullptr)
	{
		return;
	}


	UUserWidget* TempWidget = CreateWidget<UUserWidget>(GetWorld(), HotbarWidgetClass);
	HotbarWidget = Cast<UHotbarWidget>(TempWidget);

	if (HotbarWidget)
	{
		HotbarWidget->AddToViewport();
        
	}


	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(HotbarMappingContext, 0);
		}

	}
	

}

void AMyProjectCharacter::UpdateOptionButton3Text(const FString& ItemName)
{
    if (CurrentDialogueWidget)
    {
        UTextBlock* OptionButton3Text = Cast<UTextBlock>(CurrentDialogueWidget->GetWidgetFromName("OptionButton3Text"));

        if (OptionButton3Text)
        {
            OptionButton3Text->SetText(FText::FromString(ItemName));
        }
    }
}

void AMyProjectCharacter::BindDialogueButtons()
{
    if (!CurrentDialogueWidget)
    {
        return;
    }

    OptionButton1 = Cast<UButton>(CurrentDialogueWidget->GetWidgetFromName("OptionButton1"));
    OptionButton2 = Cast<UButton>(CurrentDialogueWidget->GetWidgetFromName("OptionButton2"));
    OptionButton3 = Cast<UButton>(CurrentDialogueWidget->GetWidgetFromName("OptionButton3"));

    if (OptionButton1)
    {
        if (!OptionButton1->OnClicked.IsBound()) {
            OptionButton1->OnClicked.AddDynamic(this, &AMyProjectCharacter::OnOption1Selected);
        }
    }

    if (OptionButton2)
    {
        if (!OptionButton2->OnClicked.IsBound()) {
            OptionButton2->OnClicked.AddDynamic(this, &AMyProjectCharacter::OnOption2Selected);
        }
    }


    if (OptionButton3)
    {
        if (!OptionButton3->OnClicked.IsBound()) {
            OptionButton3->OnClicked.AddDynamic(this, &AMyProjectCharacter::OnOption3Selected);
        }
    }

}

void AMyProjectCharacter::OnDialogueOptionSelected()
{
	if (OptionButton1 && OptionButton1->IsPressed())
	{
		OnOption1Selected();
	}
	else if (OptionButton2 && OptionButton2->IsPressed())
	{
		OnOption2Selected();
	}
	else if (OptionButton3 && OptionButton3->IsPressed())
	{
		OnOption3Selected();
	}
}

void AMyProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(UseHotbarSlot1Action, ETriggerEvent::Started, this, &AMyProjectCharacter::UseHotbarItem, 0);
		EnhancedInputComponent->BindAction(UseHotbarSlot2Action, ETriggerEvent::Started, this, &AMyProjectCharacter::UseHotbarItem, 1);
		EnhancedInputComponent->BindAction(UseHotbarSlot3Action, ETriggerEvent::Started, this, &AMyProjectCharacter::UseHotbarItem, 2);
		EnhancedInputComponent->BindAction(UseHotbarSlot4Action, ETriggerEvent::Started, this, &AMyProjectCharacter::UseHotbarItem, 3);
		EnhancedInputComponent->BindAction(UseHotbarSlot5Action, ETriggerEvent::Started, this, &AMyProjectCharacter::UseHotbarItem, 4);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Look);
	}

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyProjectCharacter::Interact);
}

void AMyProjectCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMyProjectCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyProjectCharacter::CloseDialogue()
{
	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->RemoveFromParent();
		CurrentDialogueWidget = nullptr;
	
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = false;

			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
		}

	}
}

void AMyProjectCharacter::Interact()
{

	if (CurrentDialogueWidget)
	{
		CloseDialogue();
		return;
	}

	FVector Start = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + (ForwardVector * 500.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			ShowDialogue(HitActor);
		}
	}
}

void AMyProjectCharacter::AddItemToHotbar(const FString& ItemName, UTexture2D* ItemImage)
{
	const int32 MaxHotbarSize = 5;

	for (FHotbarItem& ExistingItem : HotbarItems)
	{
		if (ExistingItem.ItemName == ItemName)  
		{
			ExistingItem.Count += 1;
			HotbarWidget->UpdateHotbar(HotbarItems); 
			return;
		}
	}

	if (HotbarItems.Num() < MaxHotbarSize)
	{
		FHotbarItem NewItem;
		NewItem.ItemName = ItemName;
		NewItem.ItemImage = ItemImage;
		NewItem.Count = 1;  

		HotbarItems.Add(NewItem);  

		HotbarWidget->UpdateHotbar(HotbarItems); 
	}

}

void AMyProjectCharacter::ShowDialogue(AActor* InteractedNPC)
{
	CurrentInteractedNPC = InteractedNPC;

	if (!CurrentDialogueWidget && DialogueWidgetClass)
	{
		CurrentDialogueWidget = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
	}

	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->AddToViewport();

		BindDialogueButtons();

		ANPCCharacter* NPC = Cast<ANPCCharacter>(CurrentInteractedNPC);
		if (NPC)
		{
			NPC->UpdateNPCStatsInUI(CurrentDialogueWidget);
			FString NextItem = NPC->NextItemToGive.ItemName;
			UpdateOptionButton3Text(NextItem);
		}

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
		}
	}
}

FString AMyProjectCharacter::GetLastReceivedItem() const
{
    return LastReceivedItem;
}

void AMyProjectCharacter::SetLastReceivedItem(const FString& ItemName)
{
	LastReceivedItem = ItemName;
}

void AMyProjectCharacter::OnOption1Selected()
{

	if (CurrentInteractedNPC)
	{
		if (CurrentInteractedNPC->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			IInteractionInterface::Execute_PerformAction(CurrentInteractedNPC, 0); 
		}

	}

	CloseDialogue(); 
}

void AMyProjectCharacter::OnOption2Selected()
{

	if (CurrentInteractedNPC)
	{
		ANPCCharacter* NPC = Cast<ANPCCharacter>(CurrentInteractedNPC);
		if (NPC)
		{
			FString RequiredItem = NPC->TaskItem.ItemName;

			bool bHasRequiredItem = false;
			for (const FHotbarItem& Item : HotbarItems)
			{
				if (Item.ItemName == RequiredItem && Item.Count > 0)
				{
					bHasRequiredItem = true;
					break;
				}
			}
			if (bHasRequiredItem)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Görev Tamamlandı!"));

				for (FHotbarItem& Item : HotbarItems)
				{
					if (Item.ItemName == RequiredItem)
					{
						Item.Count -= 1;
						break;
					}
				}

				if (HotbarWidget)
				{
					HotbarWidget->UpdateHotbar(HotbarItems);
				}
				NPC->ReturnToSpawn();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Görev Tamamlanamadı: Gerekli item yok!"));
			}
		}
	}

	CloseDialogue();
	
}

void AMyProjectCharacter::OnOption3Selected()
{

	if (CurrentInteractedNPC)
	{
		if (CurrentInteractedNPC->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			IInteractionInterface::Execute_PerformAction(CurrentInteractedNPC, 2);
		}
	}

	CloseDialogue(); 
}

void AMyProjectCharacter::UseHotbarItem(int32 SlotIndex)
{
	if (HotbarItems.IsValidIndex(SlotIndex) && HotbarItems[SlotIndex].Count > 0) 
	{
		FHotbarItem& UsedItem = HotbarItems[SlotIndex]; 

		if (UsedItem.Count > 1)
		{
			UsedItem.Count -= 1; 
		}
		else
		{
			UsedItem.Count = 0;
		}
		if (HotbarWidget)
		{
			HotbarWidget->UpdateHotbar(HotbarItems);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Kullanılan item: %s"), *UsedItem.ItemName));

	}

}