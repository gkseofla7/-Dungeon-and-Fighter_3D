// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGKWidget.h"
#include "StatComponent.h"
#include "Components/ProgressBar.h"

void UDFGKWidget::BindHp(class UStatComponent* StatComp)
{
	CurrentStatComp = StatComp;
	StatComp->OnHpChanged.AddUObject(this, &UDFGKWidget::UpdateHp);

	
}

void UDFGKWidget::UpdateHp()
{
	if(CurrentStatComp.IsValid())
		PB_HpBar->SetPercent(CurrentStatComp->GetHpRatio());
}
