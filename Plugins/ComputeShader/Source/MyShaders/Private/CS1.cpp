/*
#include "MyShaders/Public/CS1.h"
#include "BaseComputeShader.h"

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

DECLARE_STATS_GROUP(TEXT("CS1"), STATGROUP_CS1, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("CS1 Execute"), STAT_CS1_Execute, STATGROUP_CS1);

/*This class carries our parameter declarationsand acts as the bridge between cpp and HLSL.
class MYSHADERS_API FCS1 : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FCS1);
	SHADER_USE_PARAMETER_STRUCT(FCS1, FGlobalShader);

	class FCS1_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);
	using FPermutationDomain = TShaderPermutationDomain<FCS1_Perm_TEST>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )

		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float>, FloatInput)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float>, FloatOutput)

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
														// change names someday
		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_MySimpleComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_MySimpleComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_MySimpleComputeShader_Z);

	}
}; // class FCS1

// This will tell the engine to create the shader and where the shader entry point is.
//              ShaderType                ShaderPath              Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FCS1, "/MyShaders/CS1.usf", "CS1", SF_Compute);

void FCS1Interface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FCS1DispatchParams Params, TFunction<void(float OutputVal)> AsyncCallback)
{

	FRDGBuilder GraphBuilder(RHICmdList);
	{
		SCOPE_CYCLE_COUNTER(STAT_CS1_Execute);
		DECLARE_GPU_STAT(CS1)
		RDG_EVENT_SCOPE(GraphBuilder, "CS1");
		RDG_GPU_STAT_SCOPE(GraphBuilder, CS1);

		typename FCS1::FPermutationDomain PermutationVector;

		// Add any static permutation options here
		// PermutationVector.Set<FMySimpleComputeShader::FMyPermutationName>(12345);

		TShaderMapRef<FCS1> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);

		if (ComputeShader.IsValid())
		{
			FRDGBuffer* OutputBuffer;
			FRDGBuffer* InputBuffer;

			FCS1::FParameters* PassParameters = GraphBuilder.AllocParameters<FCS1::FParameters>();

			const void* RawData = (void*)Params.FloatInput;
			int NumInputs = 2;
			int InputSize = sizeof(int);
			InputBuffer = CreateUploadBuffer(GraphBuilder, TEXT("InputBuffer"), InputSize, NumInputs, RawData, InputSize * NumInputs);

			PassParameters->FloatInput = GraphBuilder.CreateSRV(FRDGBufferSRVDesc(InputBuffer, EPixelFormat::PF_R32_FLOAT/*PF_R32_SINT));

			OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(int32), 1), TEXT("OutputBuffer"));

			PassParameters->FloatOutput = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, EPixelFormat::PF_R32_FLOAT/*PF_R32_SINT));

			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(Params.X, Params.Y, Params.Z), FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("ExecuteCS1"),
				PassParameters,
				ERDGPassFlags::AsyncCompute,
				[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList)
				{
					FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
				});

			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteCS1Output"));
			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);

			auto RunnerFunc = [GPUBufferReadback, AsyncCallback](auto&& RunnerFunc) -> void {
				if (GPUBufferReadback->IsReady()) {

					int32* Buffer = (int32*)GPUBufferReadback->Lock(1);
					int OutVal = Buffer[0];

					GPUBufferReadback->Unlock();

					AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutVal]() {
						AsyncCallback(OutVal);
						});

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
	}
	GraphBuilder.Execute();
}

*/