// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
class ANPCCharacter;
#include "QueueArea.generated.h"

UCLASS()
class MYPROJECT_API AQueueArea : public AActor
{
	GENERATED_BODY()
    
public:	
	AQueueArea();

protected:
	virtual void BeginPlay() override;

public:
	TArray<ANPCCharacter*> QueueList; // NPC'leri sıraya almak için liste

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Queue")
	UBoxComponent* QueueTrigger;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult & SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ProcessQueue();

	UFUNCTION()
	FVector GetQueuePosition(int32 Index);
};