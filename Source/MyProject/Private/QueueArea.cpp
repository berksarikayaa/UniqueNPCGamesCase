#include "QueueArea.h"
#include "Components/BoxComponent.h"
#include "NPCCharacter.h"

AQueueArea::AQueueArea()
{
    PrimaryActorTick.bCanEverTick = false;

    QueueTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("QueueTrigger"));
    RootComponent = QueueTrigger;
    QueueTrigger->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f)); 

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
    if (NPC && !QueueList.Contains(NPC)) 
    {
        QueueList.Add(NPC);
        ProcessQueue(); 
    }
}


void AQueueArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ANPCCharacter* NPC = Cast<ANPCCharacter>(OtherActor);
    if (NPC && QueueList.Contains(NPC))
    {
        QueueList.Remove(NPC);
        ProcessQueue(); 
    }
}

void AQueueArea::ProcessQueue()
{
    if (QueueList.Num() == 0) return;

    for (int32 i = 0; i < QueueList.Num(); i++)
    {
        ANPCCharacter* NPC = QueueList[i];
        if (NPC)
        {
            if (i == 0) 
            {
                NPC->MoveToQueuePosition();
            }
            else
            {
                NPC->MoveToQueuePosition();
            }
        }
    }
}

FVector AQueueArea::GetQueuePosition(int32 Index)
{
    FVector BaseLocation = GetActorLocation();
    float OffsetDistance = 200.0f; 

    return BaseLocation + FVector(0.0f, -OffsetDistance * Index, 0.0f); 
}
