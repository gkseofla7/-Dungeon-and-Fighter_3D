// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonAFighter_3DCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADungeonAFighter_3DCharacter::ADungeonAFighter_3DCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//RootComponent란? Actor의 맴버 변수, 언리얼에서는 Actor는 여러 기능의 조합으로 성격과 역할을 규정
	//그 기능들을 Component라는 개념으로 모듈화 시킴
	//Transform 정보는 RootComponent에 들어있음, 즉 RootComponent설정이 필요
	//따라서 RootComponent로 지정된 Component의 정보에 움직임과 충돌 처리가 결정됨
	//USceneComponent(Root)는 USceneComponent간 Hiearchical attachment를 지원, rendering과 collision 기능은 가지지 않음
	// USceneComponent의 자식 클래스인 UPrimitiveComponent는 collision 데이터로 사용되거나 rendering geometry 유형을 생성하거나 담는 component
	//UCapsuleComponent : 주로 Pawn의 RootComponent로 사용되며 충돌 감지에는 사용되나 rendering정보는 가지지 않음
	//UStaticMeshComponent/USkeletalMeshComponent는 충돌감지 + rendering geometry
	/*하 여기서 Class Default Object란?
	하나의 모듈에서 다른 모듈을 참조할때 언리얼 오브젝트의 초기화 과정에 대한 이해가 필요함
	언리얼 오브젝트는 언리얼 엔진의 관리를 받는 특수한 객체
	언리얼 오브젝트는 언리얼 엔진 내에서 객체를 쉽고, 효율적으로 관리하기 위해 고안됨
	CDO : 객체의 초기 값을 자체적으로 관리함
	Reflection : 객체 정보를 런타임에서 실시간 조회가 가능함
	GC : 참조되지 않는 객체를 메모리에서 자동 해제할 수 있음
	Serialization : 객체와 속성 정보를 통으로 안전하게 보관하고 로딩함
	Delegate : 함수를 묶어서 효과적으로 관리하고 호출할 수 있음
	Replication : 네트워크 상에서 객체간에 동기화를 시킬 수 있음
	Editor Integration : 언리얼 에디터 인터페이스를 통해 값을 편집할 수 있음
	class 선언 앞에 모듈이름_API 매크로는 상황에 따라 부가 설정을 해주는 매크로
	*/
	/* 컴파일 단계에서 언리얼 오브잭트마다 UCLASS가 생성된다면(여기서 UCLASS는! 메타 정보를 보관하는데
	* 메타 정보는 언리얼 오브젝트의 정보를 담음) 실행 초기의 런타임 과정에서는 언리얼 오브젝트마다 클래스 정보와 함께
	* 언리얼 오브젝트의 인스턴스가 생성됨, 이 인스턴스는 언리얼 오브젝트의 기본 세팅을 지정하는데 사용, 이를 클래스 기본 객체(Class Default Object)라 부름
	* CDO를 만드는 이유는 언리얼 오브젝트를 생성할 때마다 매번 초기화 시키지 않고 기본 인스턴스를 미리 만들어 놓고
	* 복제하는 방식으로 메커니즘이 구성됨
	*/
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADungeonAFighter_3DCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADungeonAFighter_3DCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADungeonAFighter_3DCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADungeonAFighter_3DCharacter::TouchStopped);
}

void ADungeonAFighter_3DCharacter::MoveRight(float Value)
{
	// add movement in that direction

	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void ADungeonAFighter_3DCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void ADungeonAFighter_3DCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}

