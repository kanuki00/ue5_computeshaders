#include "BaseComputeShader.h"
#include "MyShaders/Public/BaseComputeShader.h"
#include "PixelShaderUtils.h"
#include "RenderCore/Public/RenderGraphUtils.h"
#include "MeshPassProcessor.inl"
#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "UnifiedBuffer.h"
#include "CanvasTypes.h"
#include "MaterialShader.h"

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class MYSHADERS_API FBaseComputeShader : public FGlobalShader
{
public:

	// Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(FBaseComputeShader);

	// Tells the engine that this shader uses a structure for its parameters
	SHADER_USE_PARAMETER_STRUCT(FBaseComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )

		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<int>, IntInput)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<int>, IntOutput)

	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		//const FPermutationDomain PermutationVector(Parameters.PermutationId);

		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		//const FPermutationDomain PermutationVector(Parameters.PermutationId);

		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_MySimpleComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_MySimpleComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_MySimpleComputeShader_Z);

	}
private:
}; // class FBaseComputeShader

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                ShaderPath              Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FBaseComputeShader, "/MyShaders/BaseComputeShader.usf", "BaseComputeShader", SF_Compute);

void FBaseComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FBaseComputeShaderDispatchParams Params, CALL AsyncCallback) 
{ 
	FRDGBuilder GraphBuilder(RHICmdList);
	// new scope
	{
		FShader::FPermutationDomain PermutationVector;

		TShaderMapRef<FBaseComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);

		if (ComputeShader.IsValid())
		{
			FBaseComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FBaseComputeShader::FParameters>();

			FRDGBuffer* InputBuffer = CreateUploadBuffer(GraphBuilder, TEXT("InputBuffer"), sizeof(int), 2, (void*)Params.Input, sizeof(int) * 2);
			FRDGBuffer* OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(int), 1), TEXT("OutputBuffer"));

			PassParameters->IntInput = GraphBuilder.CreateSRV(FRDGBufferSRVDesc(InputBuffer, PF_R32_SINT));
			PassParameters->IntOutput = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_SINT)); //PF_R32_FLOAT
			
			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(Params.X, Params.Y, Params.Z), FComputeShaderUtils::kGolden2DGroupSize);

			GraphBuilder.AddPass(RDG_EVENT_NAME("ExecuteBaseComputeShader"), PassParameters, ERDGPassFlags::AsyncCompute, [&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList)
				{
					FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
				}
			);

			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteBaseComputeShaderOutput"));
			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);

			auto RunnerFunc = [GPUBufferReadback, AsyncCallback](auto&& RunnerFunc) -> void {
				if (GPUBufferReadback->IsReady()) {

					int32* Buffer = (int32*)GPUBufferReadback->Lock(1);
					int OutVal = Buffer[0];

					GPUBufferReadback->Unlock();

					AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutVal]() 
						{
						AsyncCallback(OutVal);
						}
					);

					delete GPUBufferReadback;
				}
				else {
					AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() 
						{
						RunnerFunc(RunnerFunc);
						}
					);
				}
			};

			AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() 
				{
				RunnerFunc(RunnerFunc);
				}
			);
		}
		else {
			// We silently exit here as we don't want to crash the game if the shader is not found or has an error.
		} 
	} // end new scope

	GraphBuilder.Execute();
}