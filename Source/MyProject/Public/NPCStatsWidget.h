// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UNPCStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NPC Stats")
	void SetNPCStats(float InYas, int32 InPara, bool InAcgozluluk);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "NPC Stats")
	float Yas;

	UPROPERTY(BlueprintReadWrite, Category = "NPC Stats")
	int32 Para;

	UPROPERTY(BlueprintReadWrite, Category = "NPC Stats")
	bool Acgozluluk;
};