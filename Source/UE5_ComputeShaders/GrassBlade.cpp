#include "GrassBlade.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "MyShaders/Public/BaseComputeShader.h"

AGrassBlade::AGrassBlade()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh>GrassMeshAsset(TEXT("StaticMesh'/Game/Assets/StaticMesh/SM_GrassBlade.SM_GrassBlade'"));
	UStaticMesh* Asset = GrassMeshAsset.Object;
	Mesh->SetStaticMesh(Asset);

	Mesh->SetCollisionProfileName("NoCollision");
}

void AGrassBlade::BeginPlay()
{
	Super::BeginPlay();

	Params = FBaseComputeShaderDispatchParams(1, 1, 1);

	Params.Input[0] = 10000.0f;
	Params.Input[1] = OurInVal;

	int r[1];
	for (auto i : r)
	{
		FBaseComputeShaderInterface::Dispatch(Params, [this](float OutVal) {
			this->OurOutVal = OutVal;
			});
	}
	
}

void AGrassBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OurInVal += DeltaTime;

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime / 2, FColor::Red, FString::SanitizeFloat(OurOutVal));
}

