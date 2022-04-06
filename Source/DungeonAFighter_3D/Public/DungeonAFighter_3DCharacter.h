// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DungeonAFighter_3DCharacter.generated.h"//반드시 마지막에 선언해줘야됨

UCLASS(config=Game)//해당클래스가 언리얼 오브젝트임을 선언하기 위함
class ADungeonAFighter_3DCharacter : public ACharacter
{
	GENERATED_BODY()//이 매크로를 사용시에 생성자를 정의하지 않고도 빌드를 할 수 있음
		//해당 클래스가 언리얼 오브젝트임을 선언하기 위함
		//생성자가 필요한경우 GENERATED_UCLASS_BODY로 변경

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;
	//UPROPERTY의 경우! 언리얼 엔진에서는 런타임 동안 객체가 더 이상 사용되지 않으면 할당된 메모리를
	//자동으로 소멸시키는 기능을 제공, 선언한 객체를 자동으로 관리하게 만들려면 코드에서 UPROPERTY 매크로를 사용
	//보여주기만 할경우 Visible, Edit: 편집이 가능하게 할경우
	//meta = (AllowProtectedAccess = true)는 변수를 private하면서 private변수를 블루 프린트에
	///노출할 수 있으며 에디터에서 편집이 가능하게 설정할 수 있음, 즉c++에서는 외부 조작을 허용하고 싶지
	//않지만 블루 프린트에서는 필요할 경우 추가함
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

	// APawn interface 여기서는 폰에 바인딩, 보통 컨트롤러에서 처리하는게 좋다고 함
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	ADungeonAFighter_3DCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
