#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCCharacter.h"
#include "NPCSpawner.generated.h"

UCLASS()
class MYPROJECT_API ANPCSpawner : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ANPCSpawner();
	virtual void Tick(float DeltaTime) override;  

	UFUNCTION()
	void SpawnNPC();
	void DecreaseNPCCount();

	int32 CurrentNPCCount = 0;
	const int32 MaxNPCCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	TSubclassOf<ANPCCharacter> NPCClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	float SpawnInterval;

private:
	FTimerHandle SpawnTimerHandle;
};
