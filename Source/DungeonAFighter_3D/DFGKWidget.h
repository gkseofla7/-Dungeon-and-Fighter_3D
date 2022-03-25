// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DFGKWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UDFGKWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindHp(class UStatComponent* StatComp);
	void UpdateHp();
private:
	TWeakObjectPtr<class UStatComponent> CurrentStatComp;
	UPROPERTY(meta = (BindWidget))//알아서 찾아줌
	class UProgressBar* PB_HpBar;
};
