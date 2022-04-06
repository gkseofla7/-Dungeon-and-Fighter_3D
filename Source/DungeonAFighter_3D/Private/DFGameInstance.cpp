// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGameInstance.h"

UDFGameInstance::UDFGameInstance()
{
	/*ConstructorHelper의 경우 UObject의 생성자에서만 사용 가능함
	* static 키워드가 주어진 이유는 여러 언리얼 오브젝트의 인스턴스들이 공유해서 사용하는 자원이므로
	* 인스턴스마다 불필요하게 애셋을 로드할 필요가 없기 때문
	* 하나의 언리얼 오브젝트는 UClass와 CDO로 나뉘어지는데 FObjectFinder는 CDO객체를 찾음
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
