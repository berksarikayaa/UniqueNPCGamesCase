#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HotbarItem.h"
#include "NPCCharacter.generated.h"

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Spawning UMETA(DisplayName = "Spawning"),
	MovingToTarget UMETA(DisplayName = "MovingToTarget"),
	Waiting UMETA(DisplayName = "Waiting"),
	Interacting UMETA(DisplayName = "Interacting"),
	Leaving UMETA(DisplayName = "Leaving")
};

UCLASS()
class MYPROJECT_API ANPCCharacter : public ACharacter, public IInteractionInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* WalkAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* IdleAnimMontage;
	
	void ReturnToSpawn(FVector SpawnPoint);
	void CheckQueue();
	FVector GetQueuePosition(int32 Index);
	void MoveToQueuePosition(FVector NewPosition);
	void CheckAIController();
	void SetNPCState(ENPCState NewState);
	void StartInteraction();
	void FinishInteraction();
	void ReturnToSpawn();
	bool HasReachedTargetLocation() const;
	bool HasReachedSpawnLocation() const;
	bool HasReachedTarget();
	void Despawn();
	FHotbarItem TaskItem;


	bool bInteracted = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NPC", meta=(AllowPrivateAccess = "true"))
	ENPCState NPCState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	TArray<ANPCCharacter*> WaitingQueue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	ANPCCharacter* PreviousNPCInQueue;  // Önündeki NPC

	// **Timer için Handle Tanımla**
	FTimerHandle InteractionTimerHandle;
	FTimerHandle DespawnTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* NPCAnimInstance;


	
	ANPCCharacter();
	virtual void Tick(float DeltaTime) override;
	void UpdateAnimation();
	void MoveToTarget();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FVector CurrentTargetLocation;
	void RandomizeStats();

	bool IsMovingToTarget = false;
	
	void UpdateNPCStatsInUI(UUserWidget* InDialogueWidget);
	// **NPC'nin rastgele statları**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	float Agility;  // **Hız (Float)**

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	int32 Experience;  // **Deneyim Puanı (Int32)**

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	bool IsMerchant;  // **Tüccar mı? (Boolean)**

	// **NPC'nin animasyonunu oynatır**
	void PlayNPCAnimation();

	// **NPC'nin görev tamamlama fonksiyonu**
	void CompleteTask();

	// **NPC'nin event tetikleme fonksiyonu**
	void TriggerNPCEvent();

	// **NPC'ye rastgele bir item belirleme**
	void DetermineNextItemToGive();

	// **NPC'nin oyuncuya vereceği item**
	FHotbarItem NextItemToGive;

	// IInteractionInterface fonksiyonları
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void PerformAction_Implementation(int32 ActionIndex) override;
	
	// **Oyuncuya item verme fonksiyonu**
	void GiveItemToPlayer();
	// NPC için diyalog widget'ı
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString NPCMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	UAnimMontage* NPCAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	bool bTaskCompleted;


	UPROPERTY()
	UUserWidget* DialogueWidget;

};
