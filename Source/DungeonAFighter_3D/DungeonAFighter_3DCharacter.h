// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DungeonAFighter_3DCharacter.generated.h"//�ݵ�� �������� ��������ߵ�

UCLASS(config=Game)//�ش�Ŭ������ �𸮾� ������Ʈ���� �����ϱ� ����
class ADungeonAFighter_3DCharacter : public ACharacter
{
	GENERATED_BODY()//�� ��ũ�θ� ���ÿ� �����ڸ� �������� �ʰ� ���带 �� �� ����
		//�ش� Ŭ������ �𸮾� ������Ʈ���� �����ϱ� ����
		//�����ڰ� �ʿ��Ѱ�� GENERATED_UCLASS_BODY�� ����

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;
	//UPROPERTY�� ���! �𸮾� ���������� ��Ÿ�� ���� ��ü�� �� �̻� ������ ������ �Ҵ�� �޸𸮸�
	//�ڵ����� �Ҹ��Ű�� ����� ����, ������ ��ü�� �ڵ����� �����ϰ� ������� �ڵ忡�� UPROPERTY ��ũ�θ� ���
	//�����ֱ⸸ �Ұ�� Visible, Edit: ������ �����ϰ� �Ұ��
	//meta = (AllowProtectedAccess = true)�� ������ private�ϸ鼭 private������ ��� ����Ʈ��
	///������ �� ������ �����Ϳ��� ������ �����ϰ� ������ �� ����, ��c++������ �ܺ� ������ ����ϰ� ����
	//������ ��� ����Ʈ������ �ʿ��� ��� �߰���
	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

protected:

	/** Called for side to side input */
	void MoveRight(float Val);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface ���⼭�� ���� ���ε�, ���� ��Ʈ�ѷ����� ó���ϴ°� ���ٰ� ��
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	ADungeonAFighter_3DCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
