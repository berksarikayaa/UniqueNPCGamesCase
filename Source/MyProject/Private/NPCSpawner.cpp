#include "NPCSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"

ANPCSpawner::ANPCSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    SpawnInterval = 5.0f;  

    static ConstructorHelpers::FClassFinder<ANPCCharacter> NPCBlueprint(TEXT("/Game/Blueprints/BP_NPCCharacter.BP_NPCCharacter_C"));
    if (NPCBlueprint.Succeeded())
    {
        NPCClass = NPCBlueprint.Class;
    }

}

void ANPCSpawner::BeginPlay()
{
    Super::BeginPlay();

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
        return;
    }

    if (NPCClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        FRotator SpawnRotation = FRotator::ZeroRotator;

        ANPCCharacter* SpawnedNPC = GetWorld()->SpawnActor<ANPCCharacter>(NPCClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedNPC)
        {
            CurrentNPCCount++;
        }
    }
}

void ANPCSpawner::DecreaseNPCCount()
{
    if (CurrentNPCCount > 0)
    {
        CurrentNPCCount--;
    }
}