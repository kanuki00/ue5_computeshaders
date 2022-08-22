#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"

//#include "BaseComputeShader.generated.h"

#define CREATE_DISPATCH_PARAMS_CONSTRUCTOR(StructName) \
	StructName(int x, int y, int z) \
	{\
		X = x;\
		Y = y;\
		Z = z;\
	}

class FBaseComputeShader;

struct MYSHADERS_API FBaseComputeShaderDispatchParams
{
	FBaseComputeShaderDispatchParams() {}

	FBaseComputeShaderDispatchParams(int x, int y, int z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	int X, Y, Z;

	int Input[2];
	
	//float Input1;
	//float Input2;
	int Output;
};

typedef TFunction<void(int OutputVal)> CALL;

// This is a public interface that we define so outside code can invoke our compute shader.
class MYSHADERS_API FBaseComputeShaderInterface {
public:
	// Executes this shader on the render thread
	static void DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FBaseComputeShaderDispatchParams Params, CALL AsyncCallback);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(FBaseComputeShaderDispatchParams Params, CALL AsyncCallback)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)([Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(RHICmdList, Params, AsyncCallback);
			}
		);
	}

	// Dispatches this shader. Can be called from any thread
	static void Dispatch(FBaseComputeShaderDispatchParams Params, CALL AsyncCallback)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}
		else {
			DispatchGameThread(Params, AsyncCallback);
		}
	}
};

// This class is only needed for blueprint implementation. 

/*
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBaseComputeShaderLibrary_AsyncExecutionCompleted, const int, Value);

UCLASS() // Change the _API to match your project
class MYSHADERS_API UBaseComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	int Arg1;
	int Arg2;

	// Execute the actual load
	virtual void Activate() override {
		// Create a dispatch parameters struct and fill it the input array with our args
		FBaseComputeShaderDispatchParams Params(1, 1, 1);
		Params.Input[0] = Arg1;
		Params.Input[1] = Arg2;

		// Dispatch the compute shader and wait until it completes
		FBaseComputeShaderInterface::Dispatch(Params, [this](int OutputVal) {
			this->Completed.Broadcast(OutputVal);
			}
		);
	}

	UPROPERTY(BlueprintAssignable)
		FOnBaseComputeShaderLibrary_AsyncExecutionCompleted Completed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UMySimpleComputeShaderLibrary_AsyncExecution* ExecuteBaseComputeShader(UObject* WorldContextObject, int Arg1, int Arg2) 
	{
		UMySimpleComputeShaderLibrary_AsyncExecution* Action = NewObject<UMySimpleComputeShaderLibrary_AsyncExecution>();
		Action->Arg1 = Arg1;
		Action->Arg2 = Arg2;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}
};
*/