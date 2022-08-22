/*#pragma once

#include "BaseComputeShader.h"

//#include "CS1.generated.h"

struct MYSHADERS_API FCS1DispatchParams : FBaseComputeShaderDispatchParams
{
	CREATE_DISPATCH_PARAMS_CONSTRUCTOR(FCS1DispatchParams)
	float FloatInput[2];
	float FloatOutput;
};

class MYSHADERS_API FCS1Interface// : public FBaseComputeShaderInterface
{
public:
	
	// Executes this shader on the render thread
	static void DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FCS1DispatchParams Params, TFunction<void(float OutputVal)> AsyncCallback);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(FCS1DispatchParams Params, TFunction<void(float OutputVal)> AsyncCallback)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(RHICmdList, Params, AsyncCallback);
			});
	}
	
	// Dispatches this shader. Can be called from any thread
	static void Dispatch(FCS1DispatchParams Params, TFunction<void(float OutputVal)> AsyncCallback)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}
		else {
			DispatchGameThread(Params, AsyncCallback);
		}
	}
};
*/