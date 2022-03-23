// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "DFGameInstance.generated.h"

USTRUCT()//에디터에서 사용 가능한 구조체를 만들고자 할때
struct FMyCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMp;
};
UCLASS()
class DUNGEONAFIGHTER_3D_API UDFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDFGameInstance();

	virtual void Init() override;

	FMyCharacterData* GetStatData(int32 Level);
private:
	UPROPERTY()
	class UDataTable* MyStats;
	
};
