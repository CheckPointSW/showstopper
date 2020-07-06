#include "Technique_Exceptions.h"


CHECK_TECHNIQUE(UnhandledExceptionFilter)
{
	__try
	{
		THROW_DIVIDE_BY_ZERO_EXCEPTION;
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}


CHECK_TECHNIQUE(RaiseException)
{
	__try
	{
		RaiseException(DBG_CONTROL_C, 0, 0, NULL);
		return true;
	}
	__except(DBG_CONTROL_C == GetExceptionCode()
		? EXCEPTION_EXECUTE_HANDLER 
		: EXCEPTION_CONTINUE_SEARCH)
	{
		return false;
	}
}


static LONG CALLBACK VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	PCONTEXT ctx = pExceptionInfo->ContextRecord;
	if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
	{
		ctx->Eip += 2;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

CHECK_TECHNIQUE(VectoredExceptionHandler)
{
	HANDLE hExeptionHandler = NULL;
	bool bDebugged = false;
	__try
	{
		hExeptionHandler = AddVectoredExceptionHandler(1, VectoredExceptionHandler);

		__try
		{
			__asm int 3;
			bDebugged = true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	__finally
	{
		if (hExeptionHandler)
			RemoveVectoredExceptionHandler(hExeptionHandler);
	}

	return bDebugged;
}
