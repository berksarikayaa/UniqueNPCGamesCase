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

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

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
		UE_LOG(LogTemp, Warning, TEXT("DialogueWidgetClass başarıyla ayarlandı: %s"), *DialogueWidgetClass->GetName());

		CurrentDialogueWidget = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);

		if (CurrentDialogueWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentDialogueWidget başarıyla oluşturuldu!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CreateWidget başarısız oldu!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidgetClass nullptr! Widget sınıfı atanmadı."));
	}

	if (HotbarWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HotbarWidgetClass nullptr! Widget blueprint atanmamış olabilir."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HotbarWidgetClass başarıyla ayarlandı: %s"), *HotbarWidgetClass->GetName());

	// Widget'ı oluşturup, UHotbarWidget olarak cast edelim.
	UUserWidget* TempWidget = CreateWidget<UUserWidget>(GetWorld(), HotbarWidgetClass);
	HotbarWidget = Cast<UHotbarWidget>(TempWidget);

	if (HotbarWidget)
	{
		HotbarWidget->AddToViewport();
        
		UE_LOG(LogTemp, Warning, TEXT("HotbarWidget başarıyla oluşturuldu ve ekrana eklendi."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HotbarWidget oluşturulamadı! Tip dönüşümü başarısız oldu."));
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(HotbarMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("Hotbar Input Mapping Context yüklendi!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Enhanced Input Subsystem bulunamadı!"));
		}
	}
	

}

void AMyProjectCharacter::UpdateOptionButton3Text(const FString& ItemName)
{
    if (CurrentDialogueWidget)
    {
        // **OptionButton3'ün içindeki TextBlock'u elle bul**
        UTextBlock* OptionButton3Text = Cast<UTextBlock>(CurrentDialogueWidget->GetWidgetFromName("OptionButton3Text"));

        if (OptionButton3Text)
        {
            OptionButton3Text->SetText(FText::FromString(ItemName));
            UE_LOG(LogTemp, Warning, TEXT("3. Butonun metni güncellendi: %s"), *ItemName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("OptionButton3Text bulunamadı! Blueprint içinde 'OptionButton3Text' isminde bir TextBlock var mı?"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Diyalog ekranı açık değil!"));
    }
}

void AMyProjectCharacter::BindDialogueButtons()
{
    if (!CurrentDialogueWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("BindDialogueButtons: CurrentDialogueWidget nullptr!"));
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
        UE_LOG(LogTemp, Warning, TEXT("OptionButton1 başarıyla bulundu ve bağlandı!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OptionButton1 bulunamadı!"));
    }

    if (OptionButton2)
    {
        if (!OptionButton2->OnClicked.IsBound()) {
            OptionButton2->OnClicked.AddDynamic(this, &AMyProjectCharacter::OnOption2Selected);
        }
        UE_LOG(LogTemp, Warning, TEXT("OptionButton2 başarıyla bulundu ve bağlandı!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OptionButton2 bulunamadı!"));
    }

    if (OptionButton3)
    {
        if (!OptionButton3->OnClicked.IsBound()) {
            OptionButton3->OnClicked.AddDynamic(this, &AMyProjectCharacter::OnOption3Selected);
        }
        UE_LOG(LogTemp, Warning, TEXT("OptionButton3 başarıyla bulundu ve bağlandı!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OptionButton3 bulunamadı! UI'da doğru isimlendirilmiş mi?"));
    }
}

void AMyProjectCharacter::OnDialogueOptionSelected()
{
	// Hangi butona tıklandığını kontrol et
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
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// Interact tuşunu bağlama
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
	
		// Fare imlecini gizle ve giriş modunu eski haline getir
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = false;

			// Giriş modunu oyun moduna döndür
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
		}

		UE_LOG(LogTemp, Warning, TEXT("Diyalog kapatıldı."));
	}
}

void AMyProjectCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact fonksiyonu çağrıldı!"));

	// Eğer bir diyalog widget'ı zaten açık ise, kapat
	if (CurrentDialogueWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentDialogueWidget mevcut, CloseDialogue çağrılıyor."));
		CloseDialogue();
		return;
	}

	// LineTrace ile bir aktörle çarpışmayı kontrol et
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

		// Çarpılan aktörün geçerli olup olmadığını kontrol et
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC ile etkileşim kuruldu: %s"), *HitActor->GetName());

			// Diyalog aç
			ShowDialogue(HitActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Etkileşimde bulunulan aktör bir NPC değil."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hiçbir şeyle etkileşim kurulamadı."));
	}
}

void AMyProjectCharacter::AddItemToHotbar(const FString& ItemName, UTexture2D* ItemImage)
{
	const int32 MaxHotbarSize = 5;

	// Item ekleme işlemine başlamadan önce boş slot var mı diye kontrol edelim
	for (FHotbarItem& ExistingItem : HotbarItems)
	{
		if (ExistingItem.ItemName == ItemName)  // Aynı item varsa, count'u arttır
		{
			ExistingItem.Count += 1;  // Item sayısını arttır
			HotbarWidget->UpdateHotbar(HotbarItems);  // UI'yi güncelle
			return;
		}
	}

	// Eğer item bulunamazsa, yeni item ekle
	if (HotbarItems.Num() < MaxHotbarSize)
	{
		FHotbarItem NewItem;
		NewItem.ItemName = ItemName;
		NewItem.ItemImage = ItemImage;
		NewItem.Count = 1;  // Yeni eklenen item'ın sayısı 1 olacak

		HotbarItems.Add(NewItem);  // Yeni item ekle

		HotbarWidget->UpdateHotbar(HotbarItems);  // UI'yi güncelle
		UE_LOG(LogTemp, Warning, TEXT("%s hotbar'a eklendi!"), *ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hotbar dolu! Yeni eşya eklenemiyor."));
	}
}

void AMyProjectCharacter::ShowDialogue(AActor* InteractedNPC)
{
	UE_LOG(LogTemp, Warning, TEXT("[ShowDialogue] Çağrıldı!"));

	CurrentInteractedNPC = InteractedNPC;

	if (!CurrentDialogueWidget && DialogueWidgetClass)
	{
		CurrentDialogueWidget = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		if (CurrentDialogueWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ShowDialogue] CurrentDialogueWidget başarıyla oluşturuldu!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ShowDialogue] CreateWidget başarısız oldu!"));
			return;
		}
	}

	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->AddToViewport();

		// **Butonları bağla**
		BindDialogueButtons();

		// **NPC'nin belirlediği item'i al ve butonu güncelle**
		ANPCCharacter* NPC = Cast<ANPCCharacter>(CurrentInteractedNPC);
		if (NPC)
		{
			NPC->UpdateNPCStatsInUI(CurrentDialogueWidget);
			FString NextItem = NPC->NextItemToGive.ItemName;
			UE_LOG(LogTemp, Warning, TEXT("[ShowDialogue] 3. Butonun metni güncelleniyor: %s"), *NextItem);
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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ShowDialogue] CurrentDialogueWidget oluşturulamadı!"));
	}
}

FString AMyProjectCharacter::GetLastReceivedItem() const
{
    return LastReceivedItem;
}

void AMyProjectCharacter::SetLastReceivedItem(const FString& ItemName)
{
	LastReceivedItem = ItemName;
	UE_LOG(LogTemp, Warning, TEXT("LastReceivedItem güncellendi: %s"), *LastReceivedItem);
}

void AMyProjectCharacter::OnOption1Selected()
{
	UE_LOG(LogTemp, Warning, TEXT("Buton 1'e basıldı!"));

	// Eğer bir NPC ile etkileşimde bulunuluyorsa, animasyon oynat
	if (CurrentInteractedNPC)
	{
		// Arayüzü uygulayan bir aktör olup olmadığını kontrol et
		if (CurrentInteractedNPC->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			// PerformAction arayüz fonksiyonunu çağır
			IInteractionInterface::Execute_PerformAction(CurrentInteractedNPC, 0); // ActionIndex = 0
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentInteractedNPC arayüzü uygulamıyor!"));
		}
	}

	CloseDialogue(); // Widget'i kapat
}

void AMyProjectCharacter::OnOption2Selected()
{
	UE_LOG(LogTemp, Warning, TEXT("Buton 2'ye basıldı!"));

	// Eğer bir NPC ile etkileşimde bulunuluyorsa
	if (CurrentInteractedNPC)
	{
		ANPCCharacter* NPC = Cast<ANPCCharacter>(CurrentInteractedNPC);
		if (NPC)
		{
			FString RequiredItem = NPC->TaskItem.ItemName;

			// **Oyuncunun envanterinde bu item var mı kontrol et**
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
				UE_LOG(LogTemp, Warning, TEXT("[OnOption2Selected] Görev Tamamlandı!"));
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Görev Tamamlandı!"));

				// **Görevi tamamlayınca item azalt**
				for (FHotbarItem& Item : HotbarItems)
				{
					if (Item.ItemName == RequiredItem)
					{
						Item.Count -= 1;  // 1 tane düşür
						break;
					}
				}

				// **UI Güncellemesi**
				if (HotbarWidget)
				{
					HotbarWidget->UpdateHotbar(HotbarItems);
				}
				NPC->ReturnToSpawn();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OnOption2Selected] Görev Tamamlanamadı: %s bulunamadı!"), *RequiredItem);
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Görev Tamamlanamadı: Gerekli item yok!"));
			}
		}
	}

	CloseDialogue(); // Diyalog penceresini kapat
	
}

void AMyProjectCharacter::OnOption3Selected()
{
	UE_LOG(LogTemp, Warning, TEXT("Buton 3'e basıldı!"));

	// Eğer bir NPC ile etkileşimde bulunuluyorsa, yeni bir seviye yükle
	if (CurrentInteractedNPC)
	{
		// Arayüzü uygulayan bir aktör olup olmadığını kontrol et
		if (CurrentInteractedNPC->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			// PerformAction arayüz fonksiyonunu çağır
			IInteractionInterface::Execute_PerformAction(CurrentInteractedNPC, 2); // ActionIndex = 2
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentInteractedNPC arayüzü uygulamıyor!"));
		}
	}

	CloseDialogue(); // Widget'i kapat
}

void AMyProjectCharacter::UseHotbarItem(int32 SlotIndex)
{
	if (HotbarItems.IsValidIndex(SlotIndex) && HotbarItems[SlotIndex].Count > 0)  // Eğer geçerli öğe varsa
	{
		FHotbarItem& UsedItem = HotbarItems[SlotIndex];  // Tıklanan item

		if (UsedItem.Count > 1)
		{
			UsedItem.Count -= 1;  // Sadece 1 tanesini kullanıyoruz
		}
		else
		{
			// Eğer item sayısı 1 ise, item tamamen kullanılır ve sayıyı sıfırla
			UsedItem.Count = 0;
		}

		// UI Güncellemesi
		if (HotbarWidget)
		{
			HotbarWidget->UpdateHotbar(HotbarItems);  // UI'yi güncelle
		}

		// Öğenin ismini ekrana yazdırıyoruz
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Kullanılan item: %s"), *UsedItem.ItemName));

		UE_LOG(LogTemp, Warning, TEXT("%s kullanıldı! Slot güncellendi."), *UsedItem.ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Geçersiz Hotbar slotu seçildi veya zaten boş!"));
	}
}