// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DFGhostKnight.generated.h"

UCLASS()
class DUNGEONAFIGHTER_3D_API ADFGhostKnight : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADFGhostKnight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** Called for side to side input */

	virtual void PostInitializeComponents() override;

	void MoveRight(float Val);
	void MoveUp(float Value);

	void Attack();
	void AttackCheck();
	
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//블루프린트 비주얼 스크립팅 시스템 안에서 보여지며 호출 또는 오버라이드가 가능하다
		//클래스의 디폴트 property 내 델리게이트로 할당할 수 있음
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void DrinkHp(float HpAmount);
	bool CanSetWeapon();
	void SetWeapon(class ADFWeapon* NewWeapon);
	void UseItem(class UItem* Item);
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		class ADFWeapon* CurrentWeapon;

private:
	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* Inventory;
	UPROPERTY(VisibleAnywhere, Category = Pawn)
	bool IsAttacking = false;

	UPROPERTY()
	class UDFGKAnimInstance* AnimInstance;

	UPROPERTY()
		int32 AttackIndex = 0;

public:
	UPROPERTY()
		float UpDownValue = 0;
	UPROPERTY()
		float LeftRightValue = 0;
	UPROPERTY()
	float time;

	UPROPERTY(VisibleAnywhere)
		class UStatComponent* Stat;


	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HpBar;
	UPROPERTY()
		float rot = 0;

};
