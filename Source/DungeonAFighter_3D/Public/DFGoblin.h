// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DFGoblin.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate)
DECLARE_MULTICAST_DELEGATE(FOnDamagedEndDelegate)
DECLARE_MULTICAST_DELEGATE(FOnDiedDelegate)
UCLASS()
class DUNGEONAFIGHTER_3D_API ADFGoblin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADFGoblin();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** Called for side to side input */

	virtual void PostInitializeComponents() override;

	void MoveRight(float Val);
	void MoveUp(float Value);


	void AttackCheck();

	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	//void OnAttackMontageEnded();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//블루프린트 비주얼 스크립팅 시스템 안에서 보여지며 호출 또는 오버라이드가 가능하다
		//클래스의 디폴트 property 내 델리게이트로 할당할 수 있음
public:
	UFUNCTION(BlueprintCallable)
		void DisableMouseCursor() const;


public:
	void Attack();
	FOnAttackEndDelegate OnAttackEnd;
	FOnDamagedEndDelegate OnDamagedEnd;
	FOnDiedDelegate OnDied;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
		void DrinkHp(float HpAmount);
	UFUNCTION(BlueprintCallable)
		void DrinkMp(float MpAmount);
	UFUNCTION(BlueprintPure)
		float GetHpPercent();
	UFUNCTION(BlueprintPure)
		float GetMpPercent();
	bool CanSetWeapon();
	void SetWeapon(class ADFWeapon* NewWeapon);
	void SetIsAttacked() { IsAttacked = false; }
	//void UseItem(class UItem* Item);
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		class ADFWeapon* CurrentWeapon;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float ChangeDamageColor(float DamageAmount);

private:


	UPROPERTY()
		class UGoblinAnimInstance* AnimInstance;

	UPROPERTY()
		int32 AttackIndex = 0;

	//void OnAssetLoadCompleted();

public:
	UPROPERTY()
		float UpDownValue = 0;
	UPROPERTY()
		float LeftRightValue = 0;
	UPROPERTY()
		float time;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStatComponent* Stat;

	UPROPERTY()
	UMaterial* RedStoredMaterial;
	UPROPERTY()
	UMaterial* StoredMaterial;
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HpBar;


	UPROPERTY()
		float rot = 0;

	bool IsAttacked = false;
	UPROPERTY(VisibleAnywhere, Category = Pawn)
		bool IsAttacking = false;

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;
};
