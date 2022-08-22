// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

// Shader includes
//#include "MyShaders/Public/MySimpleComputeShader.h"
#include "MyShaders/Public/BaseComputeShader.h"
#include "MyShaders/Public/CS1.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"), false);
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere()));

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FBaseComputeShaderDispatchParams Params(1, 1, 1);

	Params.Input[0] = 56;
	Params.Input[1] = 2;

	FBaseComputeShaderInterface::Dispatch(Params, [&](int OutputVal) {
		Result = OutputVal;
		});
	
	if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.02f, FColor::Orange, FString::FromInt(Result));
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForwardBind);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRightBind);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpBind);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRightBind);
}

void APlayerCharacter::MoveForwardBind(float Axis)
{
	AddMovementInput(GetActorForwardVector(), Axis);
}

void APlayerCharacter::MoveRightBind(float Axis)
{
	AddMovementInput(GetActorRightVector(), Axis);
}

void APlayerCharacter::LookUpBind(float Axis)
{
	AddControllerPitchInput(-Axis);
}

void APlayerCharacter::LookRightBind(float Axis)
{
	AddControllerYawInput(Axis);
}

