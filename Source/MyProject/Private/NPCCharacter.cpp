#include "NPCCharacter.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "HotbarItem.h"
#include "NPCSpawner.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

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

	UE_LOG(LogTemp, Warning, TEXT("[NPC] BeginPlay Çalıştı!"));


	UE_LOG(LogTemp, Warning, TEXT("[NPC] 5 saniye sonra ReturnToSpawn() çağrılacak!"));
	
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] AI Controller atandı ve çalışıyor!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("[NPC] AI Controller atandı ve çalışıyor!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller atanamadı! NPC hareket edemez."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[NPC] AI Controller atanamadı!"));
	}

	GetCharacterMovement()->SetAvoidanceEnabled(true);
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
	
	DetermineNextItemToGive();
	RandomizeStats();

	UE_LOG(LogTemp, Warning, TEXT("[NPC] Rastgele Stats Atandı -> Agility: %.2f, Experience: %d, IsMerchant: %s"),
		Agility, Experience, IsMerchant ? TEXT("Evet") : TEXT("Hayır"));

	SpawnLocation = GetActorLocation();
	MoveToTarget();

}

void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NPCState == ENPCState::Leaving) 
	{
		if (HasReachedSpawnLocation()) 
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn noktasına ulaştı, yok ediliyor!"));
			Despawn();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn noktasına gitmeye çalışıyor..."));
			
		}
	}
}





void ANPCCharacter::MoveToTarget()
{
	if (TargetLocation.IsNearlyZero())
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] MoveToTarget çağrıldı ama TargetLocation tanımlı değil!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[NPC] MoveToTarget Çağrıldı! Hedef Nokta: %s"), *TargetLocation.ToString());

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetLocation, 50.0f, true, true, true, true);

		if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPC] AI Controller başarıyla hedefe yönlendirildi!"));
		}
		else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPC] AI Controller zaten hedefteydi!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller hedefe yönlendirmeyi başaramadı!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller BULUNAMADI! NPC HAREKET EDEMİYOR!"));
	}
}






void ANPCCharacter::CheckQueue()
{
	UE_LOG(LogTemp, Warning, TEXT("[NPC] CheckQueue() fonksiyonu çağrıldı."));

	if (WaitingQueue.Contains(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] NPC zaten sırada, tekrar eklenmiyor."));
		return;
	}

	if (WaitingQueue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] En öndeki NPC, etkileşime başlıyor!"));
		StartInteraction();
	}
	else
	{
		WaitingQueue.Add(this);
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Kuyruğa eklendi, bekliyor. Mevcut Kuyruk Uzunluğu: %d"), WaitingQueue.Num());
	}
}





void ANPCCharacter::StartInteraction()
{
	NPCState = ENPCState::Interacting;

	UE_LOG(LogTemp, Warning, TEXT("[NPC] Etkileşim başladı!"));

	GetWorldTimerManager().SetTimer(
		InteractionTimerHandle,
		this,
		&ANPCCharacter::FinishInteraction,
		2.0f,
		false
	);
}

void ANPCCharacter::FinishInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Etkileşim tamamlandı, kuyruktaki bir sonraki NPC çağrılıyor!"));

	// **NPC spawn noktasına dönsün**
	ReturnToSpawn();

	// **Sıradaki NPC'yi sıradan çıkar ve başlat**
	if (WaitingQueue.Num() > 0)
	{
		ANPCCharacter* NextNPC = WaitingQueue[0];
		WaitingQueue.RemoveAt(0);

		if (NextNPC)
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPC] Sıradaki NPC etkileşime başlıyor!"));
			NextNPC->StartInteraction();
		}
	}
}


void ANPCCharacter::ReturnToSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("[NPC] ReturnToSpawn() fonksiyonu ÇAĞRILDI!"));

	NPCState = ENPCState::Leaving;

	// **AI Controller kontrolü**
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller bulunamadı! NPC hareket edemez."));
		return;
	}

	// **Hareket yeteneğini açıyoruz**
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// **Hedef noktanın doğruluğunu kontrol edelim**
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn Noktası: X=%.2f, Y=%.2f, Z=%.2f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);

	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(SpawnLocation, 50.0f, true, true, true, true, nullptr, true);

	if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] AI Controller başarıyla spawn noktasına yönlendirildi!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPC] AI Controller spawn noktasına yönlendirmeyi başaramadı!"));
	}
}

bool ANPCCharacter::HasReachedSpawnLocation() const
{
	float Distance = FVector::Dist(GetActorLocation(), SpawnLocation);
    
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn noktasına kalan mesafe: %.2f"), Distance);

	if (Distance <= 100.0f)  // 100 birimlik hassasiyet
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPC] Spawn noktasına ulaşıldı!"));
		return true;
	}

	return false;
}

void ANPCCharacter::Despawn()
{
	UE_LOG(LogTemp, Warning, TEXT("[NPC] Yok oluyor!"));

	// **NPCSpawner'ı sahnede bul**
	for (TActorIterator<ANPCSpawner> It(GetWorld()); It; ++It)
	{
		ANPCSpawner* NPCSpawner = *It;
		if (NPCSpawner)
		{
			NPCSpawner->DecreaseNPCCount();
			break;  // İlk bulduğumuz spawner ile işlem yapıyoruz
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[NPC] NPC Destroy() çağrılıyor!"));
	Destroy(); // **Gerçekten yok olmasını sağlıyoruz**
}



void ANPCCharacter::Interact_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, *FString::Printf(TEXT("NPC Mesajı: %s"), *NPCMessage));

	UE_LOG(LogTemp, Warning, TEXT("[NPC] Oyuncu ile etkileşime girdi! Doğrudan spawn noktasına dönecek."));

	// NPC doğrudan spawn noktasına dönsün
	ReturnToSpawn();
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

	// **Rastgele item seç**
	if (AvailableItems.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableItems.Num() - 1);
		NextItemToGive = AvailableItems[RandomIndex];
		UE_LOG(LogTemp, Warning, TEXT("[DetermineNextItemToGive] NPC'nin vereceği item: %s"), *NextItemToGive.ItemName);
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
}

void ANPCCharacter::TriggerNPCEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC Yeni Bir Event Tetikliyor!"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, NPCMessage);
	}
}


