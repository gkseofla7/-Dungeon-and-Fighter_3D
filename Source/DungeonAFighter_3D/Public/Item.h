// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */

//Abstract는 사용자가 이 클래스 액터를 월드에 추가하지 못하도록 함
//BlueprintType는 이 클래스를 블루프린트에서 변수로 사용할 수 있는 유형으로 노출시킴
///Blueprintable : 해당 클래스가 블루프린트 생성시 부모 클래스로써 노출시키는 지정자, 즉 이 지정자가
//붙어 있는 C++클래스로 부터 Blueprint 파생클래스를 생성시켜 확장시킬 수 있음
//EdintlineNew : 언리얼 에디터 프로퍼티 창에서 생성할 수 있음
//DefaultToInstanced : 이 클래스의 모든 인스턴스는 instanced로 간주함
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DUNGEONAFIGHTER_3D_API UItem : public UObject
{
	GENERATED_BODY()
	

public:
	UItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
		class UWorld* World;
	//DefaultsOnly는 클래스별 기본값에 해당하기에 인스턴스들이 공통으로 가져야 하는 값
	//InstanceOnly는 인스턴스된 객체별로 다르게 가질 수 있는 값에 해당
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText UseActionText;//유저에게 보여지는 모든 테긋트는 FText클래스 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		class UStaticMesh* PickupMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		class UTexture2D* Thumbnail;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText ItemDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
		FText ItemDescriptioni;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta =(ClampMin=  0.0))
		float Weight;//ClampMin은 값 범위 지정

	UPROPERTY()
		class UInventoryComponent* OwningInventory;

	virtual void Use(class ADFGhostKnight* Character) PURE_VIRTUAL(UItem, );
	////블루프린트 자체 내에 구현하는 가상함수같은 것
	//UFUNCTION(BlueprintImplementableEvent)
		//void OnUse(class ADFGhostKnight* Character);
	
};
