#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractionInterface.h"
#include "QueueArea.h"
class AQueueArea; 
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
	void CheckQueueSystem();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* WalkAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* IdleAnimMontage;
	
	FVector GetQueuePosition(int32 Index);
	void MoveToQueuePosition();
	void StartInteraction();
	void FinishInteraction();
	void ReturnToSpawn();
	bool HasReachedSpawnLocation() const;
	bool HasReachedTarget();
	void Despawn();
	FHotbarItem TaskItem;


	bool bInteracted = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NPC", meta=(AllowPrivateAccess = "true"))
	ENPCState NPCState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	TArray<ANPCCharacter*> WaitingQueue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	ANPCCharacter* PreviousNPCInQueue;  

	FTimerHandle InteractionTimerHandle;
	FTimerHandle DespawnTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* NPCAnimInstance;

	bool bHasLeftQueue;
	
	bool bIsWaitingInQueue;
	
	ANPCCharacter();
	virtual void Tick(float DeltaTime) override;
	bool IsInsideQueueArea();
	void UpdateAnimation();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FVector CurrentTargetLocation;
	void RandomizeStats();

	bool IsMovingToTarget = false;
	
	void UpdateNPCStatsInUI(UUserWidget* InDialogueWidget);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	float Agility;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	int32 Experience; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Stats")
	bool IsMerchant;

	void PlayNPCAnimation();

	void CompleteTask();

	void TriggerNPCEvent();

	void DetermineNextItemToGive();

	FHotbarItem NextItemToGive;
	
	UPROPERTY()
	AQueueArea* QueueArea; 


	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void PerformAction_Implementation(int32 ActionIndex) override;
	
	void GiveItemToPlayer();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString NPCMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	UAnimMontage* NPCAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	bool bTaskCompleted;

	FVector QueueAreaCenter; 
	float QueueAreaRadius = 300.0f; 
	
	UPROPERTY()
	UUserWidget* DialogueWidget;

};
