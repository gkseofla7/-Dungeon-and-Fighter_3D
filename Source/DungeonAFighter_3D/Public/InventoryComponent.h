// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONAFIGHTER_3D_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;

	bool AddItem(class UItem* Item);
	bool RemoveItem(class UItem* Item);
	//Instanced는 이 클래스의 인스턴스가 생성될때 이 프로퍼티에
	//기본으로 할당된 오브젝트 고유 사본을 쥐어줌
	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UItem*> DefaultItems;


	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
		TArray<class UItem*> Items;
};
