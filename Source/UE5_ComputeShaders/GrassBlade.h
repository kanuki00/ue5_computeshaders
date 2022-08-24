#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MyShaders/Public/BaseComputeShader.h"

#include "GrassBlade.generated.h"




UCLASS()
class UE5_COMPUTESHADERS_API AGrassBlade : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrassBlade();

	UStaticMeshComponent* Mesh;

	FBaseComputeShaderDispatchParams Params;

	float OurInVal;
	float OurOutVal;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
