// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
//#include "../../../Unreal Projects/UE5_ComputeShaders/Plugins/ComputeShader/Source/MyShaders/Public/MyShaders.h"
#include "MyShaders/Public/MySimpleComputeShader.h"

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

void APlayerCharacter::Set(int Out)
{
	if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.02f, FColor::Orange, FString::FromInt(Out));
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

	FMySimpleComputeShaderDispatchParams Params(1, 1, 1);

	Params.Input[0] = 6;
	Params.Input[1] = 5;
	
	FMySimpleComputeShaderInterface::Dispatch(Params, [this](int OutputVal) {
		this->Set(OutputVal);
	});
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, & APlayerCharacter::MoveForwardBind);
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

