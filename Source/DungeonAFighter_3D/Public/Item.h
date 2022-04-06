// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */

//Abstract�� ����ڰ� �� Ŭ���� ���͸� ���忡 �߰����� ���ϵ��� ��
//BlueprintType�� �� Ŭ������ �������Ʈ���� ������ ����� �� �ִ� �������� �����Ŵ
///Blueprintable : �ش� Ŭ������ �������Ʈ ������ �θ� Ŭ�����ν� �����Ű�� ������, �� �� �����ڰ�
//�پ� �ִ� C++Ŭ������ ���� Blueprint �Ļ�Ŭ������ �������� Ȯ���ų �� ����
//EdintlineNew : �𸮾� ������ ������Ƽ â���� ������ �� ����
//DefaultToInstanced : �� Ŭ������ ��� �ν��Ͻ��� instanced�� ������
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DUNGEONAFIGHTER_3D_API UItem : public UObject
{
	GENERATED_BODY()
	

public:
	UItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
		class UWorld* World;
	//DefaultsOnly�� Ŭ������ �⺻���� �ش��ϱ⿡ �ν��Ͻ����� �������� ������ �ϴ� ��
	//InstanceOnly�� �ν��Ͻ��� ��ü���� �ٸ��� ���� �� �ִ� ���� �ش�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText UseActionText;//�������� �������� ��� �ױ�Ʈ�� FTextŬ���� ���
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		class UStaticMesh* PickupMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		class UTexture2D* Thumbnail;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText ItemDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
		FText ItemDescriptioni;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta =(ClampMin=  0.0))
		float Weight;//ClampMin�� �� ���� ����

	UPROPERTY()
		class UInventoryComponent* OwningInventory;

	virtual void Use(class ADFGhostKnight* Character) PURE_VIRTUAL(UItem, );
	////�������Ʈ ��ü ���� �����ϴ� �����Լ����� ��
	//UFUNCTION(BlueprintImplementableEvent)
		//void OnUse(class ADFGhostKnight* Character);
	
};
