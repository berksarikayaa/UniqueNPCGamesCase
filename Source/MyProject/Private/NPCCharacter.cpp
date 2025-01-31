#include "NPCCharacter.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "HotbarItem.h"
#include "NPCSpawner.h"
#include "QueueArea.h"
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

	bHasLeftQueue = false;
	NPCMessage = TEXT("Merhaba, ben bir NPC'yim!");
	NPCAnimation = nullptr;
	bTaskCompleted = false;
	MovementSpeed = 300.0f;
	NPCState = ENPCState::Spawning;
	bIsWaitingInQueue = false;
	WalkAnimMontage = nullptr;
	IdleAnimMontage = nullptr;
	PreviousNPCInQueue = nullptr;
	NPCAnimInstance = nullptr;
	QueueArea = nullptr;
	DialogueWidget = nullptr;
	Agility = 1.0f;
	Experience = 0;
	IsMerchant = false;


}

void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->SetAvoidanceEnabled(true);
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 250.0f;
	GetCharacterMovement()->AvoidanceWeight = 0.2f;  

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	
	DetermineNextItemToGive();
	RandomizeStats();
	NPCAnimInstance = GetMesh()->GetAnimInstance();
	SpawnLocation = GetActorLocation();

	for (TActorIterator<AQueueArea> It(GetWorld()); It; ++It)
	{
		if (*It) 
		{
			QueueArea = *It;
			break; 
		}
	}

	if (QueueArea)
	{
		if (!QueueArea->QueueList.Contains(this))
		{
			QueueArea->QueueList.Add(this);
			QueueArea->ProcessQueue();
		}
	}
}

void ANPCCharacter::CheckQueueSystem()
{
	if (!QueueArea || bHasLeftQueue) return;

	if (!bIsWaitingInQueue && !QueueArea->QueueList.Contains(this))
	{
		QueueArea->QueueList.Add(this);
		QueueArea->ProcessQueue();
	}
}

void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NPCState == ENPCState::Leaving && HasReachedSpawnLocation())
	{
		Despawn();
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());

			if (DistanceToPlayer <= 1000.0f) 
			{
				FVector PlayerLocation = PlayerPawn->GetActorLocation();
				FVector NPCToPlayer = PlayerLocation - GetActorLocation();
				NPCToPlayer.Z = 0.0f;

				FRotator TargetRotation = NPCToPlayer.Rotation();
				FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
                
				SetActorRotation(SmoothRotation);
			}
		}
	}

	UpdateAnimation();
	if (IsInsideQueueArea()) 
	{
		CheckQueueSystem();
	}

}

bool ANPCCharacter::IsInsideQueueArea()
{
	return FVector::Dist(GetActorLocation(), QueueAreaCenter) <= QueueAreaRadius;
}

FVector ANPCCharacter::GetQueuePosition(int32 Index)
{
	if (!QueueArea) return GetActorLocation();

	FVector BaseLocation = QueueArea->GetActorLocation();
	float OffsetDistance = 200.0f;
	return BaseLocation + FVector(0.0f, -OffsetDistance * Index, 0.0f);
}

void ANPCCharacter::MoveToQueuePosition()
{
	if (!QueueArea) return; 

	int32 Index = QueueArea->QueueList.Find(this);
	FVector NewPosition = GetQueuePosition(Index); 

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(NewPosition, 5.0f, true, true, true, true, nullptr, true);
	}


}

bool ANPCCharacter::HasReachedTarget()
{
	float Distance = FVector::Dist(GetActorLocation(), CurrentTargetLocation);
    
	if (Distance <= 10.0f)
	{
		IsMovingToTarget = false; 
		return true;
	}

	return false;
}

void ANPCCharacter::StartInteraction()
{
	NPCState = ENPCState::Interacting;
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &ANPCCharacter::FinishInteraction, 2.0f, false);
}

void ANPCCharacter::FinishInteraction()
{
	if (!QueueArea) return;

	if (QueueArea->QueueList.Contains(this))
	{
		QueueArea->QueueList.Remove(this);
	}

	bHasLeftQueue = true; 
    
	ReturnToSpawn();
}

void ANPCCharacter::ReturnToSpawn()
{
	if (!QueueArea) return;

	NPCState = ENPCState::Leaving;

	if (QueueArea->QueueList.Contains(this))
	{
		QueueArea->QueueList.Remove(this);
	}

	QueueArea->ProcessQueue(); 

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	FVector AdjustedSpawnLocation = SpawnLocation;
	AdjustedSpawnLocation.Z += 10.0f;

	AIController->MoveToLocation(AdjustedSpawnLocation, 100.0f, true, true, true, true, nullptr, true);
}

bool ANPCCharacter::HasReachedSpawnLocation() const
{
	return FVector::Dist(GetActorLocation(), SpawnLocation) <= 150.0f;
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

void ANPCCharacter::Interact_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, *FString::Printf(TEXT("NPC Mesajı: %s"), *NPCMessage));
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
	if (!InDialogueWidget) 
	{
		return;
	}
	UTextBlock* TaskItemText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("TaskItemText"));
	if (TaskItemText)
	{
		TaskItemText->SetText(FText::FromString(FString::Printf(TEXT("Görev İçin: %s"), *TaskItem.ItemName)));
	}
	if (!InDialogueWidget) 
	{
		return;
	}
	UTextBlock* AgilityText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("AgilityText"));
	UTextBlock* ExperienceText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("ExperienceText"));
	UTextBlock* IsMerchantText = Cast<UTextBlock>(InDialogueWidget->GetWidgetFromName("IsMerchantText"));
	if (AgilityText)
	{
		AgilityText->SetText(FText::FromString(FString::Printf(TEXT("Hız: %.2f"), Agility)));
	}

	if (ExperienceText)
	{
		ExperienceText->SetText(FText::FromString(FString::Printf(TEXT("Deneyim: %d"), Experience)));
	}

	if (IsMerchantText)
	{
		FString MerchantText = IsMerchant ? TEXT("Bu NPC bir tüccardır.") : TEXT("Bu NPC tüccar değildir.");
		IsMerchantText->SetText(FText::FromString(MerchantText));
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
		break;
	}
}

void ANPCCharacter::DetermineNextItemToGive()
{
	TArray<FHotbarItem> AvailableItems;

	FHotbarItem PotionItem;
	PotionItem.ItemName = TEXT("Potion");
	PotionItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/PotionImage"));

	FHotbarItem SwordItem;
	SwordItem.ItemName = TEXT("Sword");
	SwordItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/SwordImage"));

	FHotbarItem ShieldItem;
	ShieldItem.ItemName = TEXT("Shield");
	ShieldItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/ShieldImage"));

	FHotbarItem SausageItem;
	SausageItem.ItemName = TEXT("Sausage");
	SausageItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/SausageImage"));

	FHotbarItem StrawberryItem;
	StrawberryItem.ItemName = TEXT("Strawberry");
	StrawberryItem.ItemImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/StrawberryImage"));

	AvailableItems.Add(PotionItem);
	AvailableItems.Add(SwordItem);
	AvailableItems.Add(ShieldItem);
	AvailableItems.Add(SausageItem);
	AvailableItems.Add(StrawberryItem);

	if (AvailableItems.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableItems.Num() - 1);
		NextItemToGive = AvailableItems[RandomIndex];

		int32 TaskIndex = FMath::RandRange(0, AvailableItems.Num() - 1);
		TaskItem = AvailableItems[TaskIndex];

	}

}

void ANPCCharacter::GiveItemToPlayer()
{
	if (!NextItemToGive.ItemName.IsEmpty())
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		AMyProjectCharacter* PlayerCharacter = Cast<AMyProjectCharacter>(PlayerController->GetCharacter());

		if (PlayerCharacter)
		{
			PlayerCharacter->AddItemToHotbar(NextItemToGive.ItemName, NextItemToGive.ItemImage);

			DetermineNextItemToGive();
		}
	}

}

void ANPCCharacter::PlayNPCAnimation()
{
	if (NPCAnimation)
	{
		PlayAnimMontage(NPCAnimation);
	}
}

void ANPCCharacter::CompleteTask()
{
	bTaskCompleted = true;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Görev Tamamlandı!"));
	StartInteraction();
}

void ANPCCharacter::TriggerNPCEvent()
{

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, NPCMessage);
	}
}

void ANPCCharacter::UpdateAnimation()
{
	if (!NPCAnimInstance) return;

	float CurrentSpeed = GetVelocity().Size();
	bool bIsMoving = (CurrentSpeed > 10.0f);

	if (NPCAnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	if (bIsMoving)
	{
		if (IdleAnimMontage && NPCAnimInstance->Montage_IsPlaying(IdleAnimMontage))
		{
			NPCAnimInstance->Montage_Stop(0.2f, IdleAnimMontage);
		}

		if (WalkAnimMontage && !NPCAnimInstance->Montage_IsPlaying(WalkAnimMontage))
		{
			NPCAnimInstance->Montage_Play(WalkAnimMontage, 1.0f);
		}
	}
	else
	{
		if (WalkAnimMontage && NPCAnimInstance->Montage_IsPlaying(WalkAnimMontage))
		{
			NPCAnimInstance->Montage_Stop(0.2f, WalkAnimMontage);
		}

		if (IdleAnimMontage && !NPCAnimInstance->Montage_IsPlaying(IdleAnimMontage) && !NPCAnimInstance->IsAnyMontagePlaying())
		{
			NPCAnimInstance->Montage_Play(IdleAnimMontage, 1.0f);
		}
	}
}

