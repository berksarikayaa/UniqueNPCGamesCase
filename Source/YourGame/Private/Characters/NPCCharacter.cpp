#include "Characters/NPCCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ANPCCharacter::ANPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANPCCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitializeRandomProperties();
}

void ANPCCharacter::InitializeRandomProperties()
{
    RandomProperty1 = FMath::RandRange(0.0f, 100.0f);
    RandomProperty2 = FMath::RandRange(0, 100);
    RandomProperty3 = FMath::RandBool();
}

void ANPCCharacter::Interact(AActor* Interactor)
{
    // Burada UI widget'ını açacak blueprint implementasyonu için event dispatch edilebilir
    // Blueprint'te UI açılıp seçenekler gösterilebilir
}

void ANPCCharacter::HandleOption1()
{
    // Option 1 davranışı - Blueprint'te implement edilebilir
}

void ANPCCharacter::HandleOption2()
{
    // Option 2 davranışı - Blueprint'te implement edilebilir
}

void ANPCCharacter::HandleOption3()
{
    // Option 3 davranışı - Blueprint'te implement edilebilir
}

void ANPCCharacter::MoveToTarget()
{
    // AI Controller kullanarak hedefe hareket implementasyonu
} 