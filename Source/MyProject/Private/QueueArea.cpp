#include "QueueArea.h"
#include "Components/BoxComponent.h"
#include "NPCCharacter.h"
#include "AIController.h"

AQueueArea::AQueueArea()
{
    PrimaryActorTick.bCanEverTick = false;

    QueueTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("QueueTrigger"));
    RootComponent = QueueTrigger;
    QueueTrigger->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f)); // Alanın boyutları

    QueueTrigger->OnComponentBeginOverlap.AddDynamic(this, &AQueueArea::OnOverlapBegin);
    QueueTrigger->OnComponentEndOverlap.AddDynamic(this, &AQueueArea::OnOverlapEnd);
}

void AQueueArea::BeginPlay()
{
    Super::BeginPlay();
}

void AQueueArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                bool bFromSweep, const FHitResult & SweepResult)
{
    ANPCCharacter* NPC = Cast<ANPCCharacter>(OtherActor);
    if (NPC && !QueueList.Contains(NPC)) // NPC zaten eklenmemişse sıraya ekleyelim
    {
        QueueList.Add(NPC);
        UE_LOG(LogTemp, Warning, TEXT("[QueueArea] NPC sıraya eklendi: %s"), *NPC->GetName());

        ProcessQueue(); // Sıradaki NPC'yi yönlendir
    }
}

void AQueueArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ANPCCharacter* NPC = Cast<ANPCCharacter>(OtherActor);
    if (NPC && QueueList.Contains(NPC))
    {
        QueueList.Remove(NPC);
        UE_LOG(LogTemp, Warning, TEXT("[QueueArea] NPC sıradan çıktı: %s"), *NPC->GetName());
        ProcessQueue(); // Kalan NPC'leri yeniden hizala
    }
}

void AQueueArea::ProcessQueue()
{
    if (QueueList.Num() == 0) return; 

    for (int32 i = 0; i < QueueList.Num(); i++)
    {
        FVector NewPos = GetQueuePosition(i);
        ANPCCharacter* NPC = QueueList[i];

        if (NPC)
        {
            if (i == 0)
            {
                NPC->MoveToTarget();
                UE_LOG(LogTemp, Warning, TEXT("[QueueArea] Ön sıradaki NPC hedefe yönlendirildi!"));
            }
            else
            {
                NPC->MoveToQueuePosition(NewPos);
                UE_LOG(LogTemp, Warning, TEXT("[QueueArea] NPC sıraya geçti: X=%.2f, Y=%.2f, Z=%.2f"), 
                        NewPos.X, NewPos.Y, NewPos.Z);
            }
        }
    }
}



FVector AQueueArea::GetQueuePosition(int32 Index)
{
    FVector BaseLocation = GetActorLocation();
    float OffsetDistance = 150.0f; // NPC'ler arasındaki mesafe

    return BaseLocation - FVector(0.0f, OffsetDistance * Index, 0.0f); // Y ekseni boyunca hizalama
}
