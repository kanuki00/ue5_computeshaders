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

	float Input[2];
};

typedef TFunction<void(float OutputVal)> CALL;

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