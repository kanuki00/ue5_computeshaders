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

// Profiling Macros PM1
/*
DECLARE_STATS_GROUP(TEXT("BaseComputeShader"), STATGROUP_BaseComputeShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("BaseComputeShader Execute"), STAT_BaseComputeShader_Execute, STATGROUP_BaseComputeShader);
*/

/*This class carries our parameter declarationsand acts as the bridge between cppand HLSL.*/
class MYSHADERS_API FBaseComputeShader : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FBaseComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FBaseComputeShader, FGlobalShader);

	class FBaseComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);
	using FPermutationDomain = TShaderPermutationDomain<FBaseComputeShader_Perm_TEST>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )

		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<int>, Input)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<int>, Output)

	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_MySimpleComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_MySimpleComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_MySimpleComputeShader_Z);

	}
private:
}; // class FBaseComputeShader

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                ShaderPath              Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FBaseComputeShader, "/MyShaders/BaseComputeShader.usf", "BaseComputeShader", SF_Compute);

void FBaseComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FBaseComputeShaderDispatchParams Params, TFunction<void(int OutputVal)> AsyncCallback) 
{ 
	FRDGBuilder GraphBuilder(RHICmdList);
	// new scope
	{
		// PM1
		/*
		SCOPE_CYCLE_COUNTER(STAT_BaseComputeShader_Execute);
		DECLARE_GPU_STAT(BaseComputeShader)
		RDG_EVENT_SCOPE(GraphBuilder, "BaseComputeShader");
		RDG_GPU_STAT_SCOPE(GraphBuilder, BaseComputeShader);
		*/

		typename FBaseComputeShader::FPermutationDomain PermutationVector;

		// Add any static permutation options here
		// PermutationVector.Set<FMySimpleComputeShader::FMyPermutationName>(12345);

		TShaderMapRef<FBaseComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);

		if (ComputeShader.IsValid())
		{
			FRDGBuffer* OutputBuffer;
			FRDGBuffer* InputBuffer;

			FBaseComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FBaseComputeShader::FParameters>();

			const void* RawData = (void*)Params.Input;
			int NumInputs = 2;
			int InputSize = sizeof(int);
			InputBuffer = CreateUploadBuffer(GraphBuilder, TEXT("InputBuffer"), InputSize, NumInputs, RawData, InputSize * NumInputs);

			PassParameters->Input = GraphBuilder.CreateSRV(FRDGBufferSRVDesc(InputBuffer, PF_R32_SINT));

			OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(int32), 1), TEXT("OutputBuffer"));

			PassParameters->Output = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_SINT));

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
					AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
						RunnerFunc(RunnerFunc);
						});
				}
			};

			AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
				RunnerFunc(RunnerFunc);
				});
		}
		else {
			// We silently exit here as we don't want to crash the game if the shader is not found or has an error.
		} 
	} // end new scope

	GraphBuilder.Execute();
}