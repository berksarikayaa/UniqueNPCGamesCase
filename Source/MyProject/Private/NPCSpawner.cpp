#include "NPCSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"

ANPCSpawner::ANPCSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    SpawnInterval = 5.0f;  // **Her 5 saniyede bir yeni NPC spawn eder**

    // **Blueprint'ten NPC Sınıfını Yükleme**
    static ConstructorHelpers::FClassFinder<ANPCCharacter> NPCBlueprint(TEXT("/Game/Blueprints/BP_NPCCharacter.BP_NPCCharacter_C"));
    if (NPCBlueprint.Succeeded())
    {
        NPCClass = NPCBlueprint.Class;
        UE_LOG(LogTemp, Warning, TEXT("[NPCSpawner] NPCClass Constructor'da Başarıyla Yüklendi!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[NPCSpawner] BP_NPCCharacter BULUNAMADI! Yolu Kontrol Et!"));
    }
}

void ANPCSpawner::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("[NPCSpawner] Spawner Başlatıldı!"));

    // **Belirli aralıklarla NPC spawn etmek için zamanlayıcıyı başlat**
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ANPCSpawner::SpawnNPC, SpawnInterval, true);
}

void ANPCSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ANPCSpawner::SpawnNPC()
{
    if (CurrentNPCCount >= MaxNPCCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("[NPCSpawner] Maksimum NPC sayısına ulaşıldı, yeni NPC spawn edilmeyecek!"));
        return;
    }

    if (NPCClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        FRotator SpawnRotation = FRotator::ZeroRotator;

        // Eğer sınıfta SpawnLocation tanımlıysa, burayı değiştirmeye gerek yok!
        ANPCCharacter* SpawnedNPC = GetWorld()->SpawnActor<ANPCCharacter>(NPCClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedNPC)
        {
            CurrentNPCCount++;
            UE_LOG(LogTemp, Warning, TEXT("[NPCSpawner] Yeni NPC spawn edildi! Şu anki NPC sayısı: %d"), CurrentNPCCount);
        }
    }
}


void ANPCSpawner::DecreaseNPCCount()
{
    if (CurrentNPCCount > 0)
    {
        CurrentNPCCount--;
        UE_LOG(LogTemp, Warning, TEXT("[NPCSpawner] Bir NPC yok oldu! Yeni NPC sayısı: %d"), CurrentNPCCount);
    }
}