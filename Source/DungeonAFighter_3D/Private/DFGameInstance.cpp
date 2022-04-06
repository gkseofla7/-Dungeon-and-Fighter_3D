// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGameInstance.h"

UDFGameInstance::UDFGameInstance()
{
	/*ConstructorHelper�� ��� UObject�� �����ڿ����� ��� ������
	* static Ű���尡 �־��� ������ ���� �𸮾� ������Ʈ�� �ν��Ͻ����� �����ؼ� ����ϴ� �ڿ��̹Ƿ�
	* �ν��Ͻ����� ���ʿ��ϰ� �ּ��� �ε��� �ʿ䰡 ���� ����
	* �ϳ��� �𸮾� ������Ʈ�� UClass�� CDO�� ���������µ� FObjectFinder�� CDO��ü�� ã��
	*/
	static ConstructorHelpers::FObjectFinder<UDataTable> DATA(TEXT("DataTable'/Game/Data/StatTable.StatTable'"));

	MyStats = DATA.Object;
}

void UDFGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("MyGameInstance %d"), GetStatData(1)->Attack);
}

FMyCharacterData* UDFGameInstance::GetStatData(int32 Level)
{
	return MyStats->FindRow<FMyCharacterData>(*FString::FromInt(Level),TEXT(""));
}
