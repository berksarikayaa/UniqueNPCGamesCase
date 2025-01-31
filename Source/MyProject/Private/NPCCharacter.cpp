#include "NPCCharacter.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "HotbarItem.h"
#include "NPCSpawner.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyProject/MyProjectCharacter.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Varsayılan değerler
	NPCMessage = TEXT("Merhaba, ben bir NPC'yim!");
	NPCAnimation = nullptr;
	bTaskCompleted = false;
	MovementSpeed = 300.0f;
	NPCState = ENPCState::Spawning;
}

void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->SetAvoidanceEnabled(true); // AI kaçınmayı aç
	GetCharacterMovement()->AvoidanceWeight = 1.0f;   // Çarpışmayı minimuma indir

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // NPC'ler birbirine çarpmasın

	DetermineNextItemToGive();
	RandomizeStats();
	NPCAnimInstance = GetMesh()->GetAnimInstance();
	SpawnLocation = GetActorLocation();
	MoveToTarget();
}


void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NPCState == ENPCState::Leaving && HasReachedSpawnLocation())
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn noktasına ulaştı, yok ediliyor!"));
		Despawn();
	}



	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		float CurrentSpeed = GetVelocity().Size();
		bool bShouldMove = (CurrentSpeed > 10.0f); // Eğer hız 10'dan büyükse yürüyordur

		// Blueprint içinde "Should Move" değişkenini güncelle
		UAnimBlueprintGeneratedClass* AnimBPClass = Cast<UAnimBlueprintGeneratedClass>(AnimInstance->GetClass());
		if (AnimBPClass)
		{
			FProperty* ShouldMoveProperty = AnimBPClass->FindPropertyByName(FName("Should Move"));
			if (ShouldMoveProperty)
			{
				bool* ShouldMovePtr = ShouldMoveProperty->ContainerPtrToValuePtr<bool>(AnimInstance);
				if (ShouldMovePtr)
				{
					*ShouldMovePtr = bShouldMove;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[NPC] Güncel Hız: %.2f, Should Move: %s"), CurrentSpeed, bShouldMove ? TEXT("True") : TEXT("False"));
	}

	UpdateAnimation();

}

void ANPCCharacter::UpdateAnimation()
{
	if (!NPCAnimInstance) return;

	float CurrentSpeed = GetVelocity().Size();
	bool bIsMoving = (CurrentSpeed > 10.0f); // Eğer hız 10'dan büyükse yürüyordur

	if (bIsMoving)
	{
		// Eğer şu an Idle animasyonu oynuyorsa, onu durdur
		if (NPCAnimInstance->Montage_IsPlaying(IdleAnimMontage))
		{
			NPCAnimInstance->Montage_Stop(0.2f, IdleAnimMontage);
		}

		// Eğer yürüyüş animasyonu oynatmıyorsa, başlat
		if (!NPCAnimInstance->Montage_IsPlaying(WalkAnimMontage))
		{
			NPCAnimInstance->Montage_Play(WalkAnimMontage, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("[NPC] Yürüme animasyonu başlatıldı!"));
		}
	}
	else
	{
		// Eğer şu an Yürüme animasyonu oynuyorsa, onu durdur
		if (NPCAnimInstance->Montage_IsPlaying(WalkAnimMontage))
		{
			NPCAnimInstance->Montage_Stop(0.2f, WalkAnimMontage);
		}

		// Eğer idle animasyonu oynatmıyorsa, başlat
		if (!NPCAnimInstance->Montage_IsPlaying(IdleAnimMontage))
		{
			NPCAnimInstance->Montage_Play(IdleAnimMontage, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("[NPC] Idle animasyonu başlatıldı!"));
		}
	}
}


void ANPCCharacter::CheckQueue()
{
	if (!WaitingQueue.Contains(this))
	{
		WaitingQueue.Add(this);
	}

	for (int32 i = 0; i < WaitingQueue.Num(); i++)
	{
		ANPCCharacter* NPC = WaitingQueue[i];
		if (NPC)
		{
			FVector NewPos = GetQueuePosition(i); // NPC'nin sıradaki doğru konumunu al
			NPC->MoveToQueuePosition(NewPos);    // Doğru şekilde parametre ile çağır

		}
	}
}

FVector ANPCCharacter::GetQueuePosition(int32 Index)
{
	FVector BaseLocation = TargetLocation;
	float OffsetDistance = 150.0f; // NPC'ler arasındaki mesafe

	return BaseLocation - FVector(0.0f, OffsetDistance * Index, 0.0f); // Y ekseni boyunca hizalama
}



void ANPCCharacter::MoveToQueuePosition(FVector NewPosition)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(NewPosition, 5.0f, true, true, true, true, nullptr, true);
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Sıraya gidiyor. Yeni Pozisyon: X=%.2f, Y=%.2f, Z=%.2f"), 
			   NewPosition.X, NewPosition.Y, NewPosition.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller bulunamadı! NPC sıraya hareket edemiyor."));
	}
}



void ANPCCharacter::MoveToTarget()
{
	if (TargetLocation.IsNearlyZero() || IsMovingToTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Zaten hareket halinde, tekrar yönlendirilmedi!"));
		return;
	}

	int32 QueueIndex = WaitingQueue.Find(this);
	FVector QueuePosition = GetQueuePosition(QueueIndex);

	if (FVector::Dist(CurrentTargetLocation, QueuePosition) < 10.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Zaten hedefe gidiyor, tekrar yönlendirme yapılmadı."));
		return;
	}

	CurrentTargetLocation = QueuePosition;
	IsMovingToTarget = true;  // NPC'yi hareket moduna al

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->MoveToLocation(CurrentTargetLocation, 10.0f, true, true, true, true, nullptr, true);
		UE_LOG(LogTemp, Warning, TEXT("[NPC] AI başarıyla sıradaki pozisyona yönlendirildi!"));
	}
}








void ANPCCharacter::StartInteraction()
{
	NPCState = ENPCState::Interacting;
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Etkileşim başladı!"));
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &ANPCCharacter::FinishInteraction, 2.0f, false);
}

void ANPCCharacter::FinishInteraction()
{
	ReturnToSpawn();
    
	if (WaitingQueue.Num() > 0)
	{
		ANPCCharacter* NextNPC = WaitingQueue[0];
		WaitingQueue.RemoveAt(0);
        
		for (int32 i = 0; i < WaitingQueue.Num(); i++)
		{
			FVector NewPos = GetQueuePosition(i); // Doğru şekilde yeni pozisyonu hesapla
			WaitingQueue[i]->MoveToQueuePosition(NewPos); // Pozisyonu fonksiyona gönder
		}
        
		if (NextNPC) NextNPC->StartInteraction();
	}
}


void ANPCCharacter::ReturnToSpawn()
{
	NPCState = ENPCState::Leaving;
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(SpawnLocation, 50.0f);
	}
}

bool ANPCCharacter::HasReachedSpawnLocation() const
{
	return FVector::Dist(GetActorLocation(), SpawnLocation) <= 100.0f;
}

void ANPCCharacter::Despawn()
{
	for (TActorIterator<ANPCSpawner> It(GetWorld()); It; ++It)
	{
		if (ANPCSpawner* NPCSpawner = *It)
		{
			NPCSpawner->DecreaseNPCCount();
			break;
		}
	}
	Destroy();
}


bool ANPCCharacter::HasReachedTarget()
{
	float Distance = FVector::Dist(GetActorLocation(), CurrentTargetLocation);
    
	if (Distance <= 10.0f)
	{
		IsMovingToTarget = false; // NPC artık hareket etmiyor
		return true;
	}

	return false;
}


void ANPCCharacter::Interact_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, *FString::Printf(TEXT("NPC Mesajı: %s"), *NPCMessage));
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Oyuncu ile etkileşime girdi!"));
	StartInteraction();
}

void ANPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPCCharacter::RandomizeStats()
{
	Agility = UKismetMathLibrary::RandomFloatInRange(0.5f, 2.0f);
	Experience = FMath::RandRange(0, 100);
	IsMerchant = FMath::RandBool();
}

void ANPCCharacter::UpdateNPCStatsInUI(UUserWidget* InDialogueWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("[UpdateNPCStatsInUI] Fonksiyon çağrıldı!"));

	if (!InDialogueWidget) 
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] DialogueWidget referansı NULL! UI güncellenemedi."));
		return;
	}

	// **Görev İtemi Metnini Güncelle**
	UTextBlock* TaskItemText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("TaskItemText"));
	if (TaskItemText)
	{
		TaskItemText->SetText(FText::FromString(FString::Printf(TEXT("Görev İçin: %s"), *TaskItem.ItemName)));
		UE_LOG(LogTemp, Warning, TEXT("[UpdateNPCStatsInUI] Görev Item Güncellendi: %s"), *TaskItem.ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] TaskItemText bulunamadı! UI içinde var mı?"));
	}
	if (!InDialogueWidget) 
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] DialogueWidget referansı NULL! UI güncellenemedi."));
		return;
	}

	UTextBlock* AgilityText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("AgilityText"));
	UTextBlock* ExperienceText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("ExperienceText"));
	UTextBlock* IsMerchantText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("IsMerchantText"));

	if (AgilityText)
	{
		AgilityText->SetText(FText::FromString(FString::Printf(TEXT("Hız: %.2f"), Agility)));
		UE_LOG(LogTemp, Warning, TEXT("[UpdateNPCStatsInUI] Agility güncellendi: %.2f"), Agility);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] AgilityText bulunamadı! UI içinde var mı?"));
	}

	if (ExperienceText)
	{
		ExperienceText->SetText(FText::FromString(FString::Printf(TEXT("Deneyim: %d"), Experience)));
		UE_LOG(LogTemp, Warning, TEXT("[UpdateNPCStatsInUI] Experience güncellendi: %d"), Experience);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] ExperienceText bulunamadı! UI içinde var mı?"));
	}

	if (IsMerchantText)
	{
		FString MerchantText = IsMerchant ? TEXT("Bu NPC bir tüccardır.") : TEXT("Bu NPC tüccar değildir.");
		IsMerchantText->SetText(FText::FromString(MerchantText));
		UE_LOG(LogTemp, Warning, TEXT("[UpdateNPCStatsInUI] IsMerchant güncellendi: %s"), *MerchantText);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateNPCStatsInUI] IsMerchantText bulunamadı! UI içinde var mı?"));
	}
}

void ANPCCharacter::PerformAction_Implementation(int32 ActionIndex)
{
	switch (ActionIndex)
	{
	case 0:
		PlayNPCAnimation();
		StartInteraction();
		break;
	case 1:
		CompleteTask();
		StartInteraction();
		break;
	case 2:
		TriggerNPCEvent();
		GiveItemToPlayer();
		StartInteraction();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Geçersiz Seçenek!"));
		break;
	}
}

void ANPCCharacter::DetermineNextItemToGive()
{
	TArray<FHotbarItem> AvailableItems;

	// **Örnek item'lar**
	FHotbarItem PotionItem;
	PotionItem.ItemName = TEXT("Potion");
	PotionItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/PotionImage"));

	FHotbarItem SwordItem;
	SwordItem.ItemName = TEXT("Sword");
	SwordItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/SwordImage"));

	FHotbarItem ShieldItem;
	ShieldItem.ItemName = TEXT("Shield");
	ShieldItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/ShieldImage"));

	FHotbarItem SpecialPotionItem;
	SpecialPotionItem.ItemName = TEXT("SpecialPotion");
	SpecialPotionItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/SpecialPotionImage"));

	FHotbarItem HealthPotionItem;
	HealthPotionItem.ItemName = TEXT("HealthPotion");
	HealthPotionItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/HealthPotionImage"));

	// **Item'ları diziye ekleyelim**
	AvailableItems.Add(PotionItem);
	AvailableItems.Add(SwordItem);
	AvailableItems.Add(ShieldItem);
	AvailableItems.Add(SpecialPotionItem);
	AvailableItems.Add(HealthPotionItem);

	// **Rastgele bir item belirle ve oyuncuya versin**
	if (AvailableItems.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableItems.Num() - 1);
		NextItemToGive = AvailableItems[RandomIndex];

		// **Aynı listeden rastgele bir görev itemi de belirle**
		int32 TaskIndex = FMath::RandRange(0, AvailableItems.Num() - 1);
		TaskItem = AvailableItems[TaskIndex];

		UE_LOG(LogTemp, Warning, TEXT("[DetermineNextItemToGive] NPC'nin vereceği item: %s"), *NextItemToGive.ItemName);
		UE_LOG(LogTemp, Warning, TEXT("[DetermineNextItemToGive] NPC'nin görev için istediği item: %s"), *TaskItem.ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DetermineNextItemToGive] Item listesi boş!"));
	}
}

void ANPCCharacter::GiveItemToPlayer()
{
	if (!NextItemToGive.ItemName.IsEmpty())
	{
		// **Oyuncuya item ver**
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		AMyProjectCharacter* PlayerCharacter = Cast<AMyProjectCharacter>(PlayerController->GetCharacter());

		if (PlayerCharacter)
		{
			PlayerCharacter->AddItemToHotbar(NextItemToGive.ItemName, NextItemToGive.ItemImage);
			UE_LOG(LogTemp, Warning, TEXT("[GiveItemToPlayer] NPC oyuncuya %s verdi!"), *NextItemToGive.ItemName);

			// **Yeni item belirle**
			DetermineNextItemToGive();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GiveItemToPlayer] NextItemToGive belirlenmemiş!"));
	}
}

void ANPCCharacter::PlayNPCAnimation()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC Animasyon Oynatıyor!"));
	if (NPCAnimation)
	{
		PlayAnimMontage(NPCAnimation);
	}
}

void ANPCCharacter::CompleteTask()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC Görevi Tamamlıyor!"));
	bTaskCompleted = true;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Görev Tamamlandı!"));
	StartInteraction();
}

void ANPCCharacter::TriggerNPCEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC Yeni Bir Event Tetikliyor!"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, NPCMessage);
	}
}

