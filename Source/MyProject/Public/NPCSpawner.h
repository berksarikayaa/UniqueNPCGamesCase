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
	virtual void Tick(float DeltaTime) override;  // **Bu satırın olduğundan emin ol!**

	// **NPC Spawn fonksiyonu**
	UFUNCTION()
	void SpawnNPC();
	void DecreaseNPCCount();

	int32 CurrentNPCCount = 0;
	const int32 MaxNPCCount = 4;

	// **Spawn edilecek NPC'nin Blueprint sınıfı**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	TSubclassOf<ANPCCharacter> NPCClass;

	// **NPC'nin spawn olacağı konum**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	FVector SpawnLocation;

	// **NPC'nin gitmesi gereken hedef konum**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	FVector TargetLocation;

	// **NPC'lerin spawn süresi**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawner")
	float SpawnInterval;

private:
	// **NPC spawn etmek için zamanlayıcı**
	FTimerHandle SpawnTimerHandle;
};
