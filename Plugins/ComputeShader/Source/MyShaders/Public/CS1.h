#pragma once

#include "BaseComputeShader.h"

//#include "CS1.generated.h"

struct MYSHADERS_API FCS1DispatchParams : FBaseComputeShaderDispatchParams
{
	CREATE_DISPATCH_PARAMS_CONSTRUCTOR(FCS1DispatchParams)
};

class MYSHADERS_API FCS1Interface : public FBaseComputeShaderInterface
{

};