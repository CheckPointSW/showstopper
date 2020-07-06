#include "Technique_Assembler.h"
#include "hwbrk.h"


CHECK_TECHNIQUE(INT3)
{
	__try
	{
		__asm int 3;
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}


bool AntiDebug_INT3Long::m_bDebugged = false;
AntiDebug_INT3Long::AntiDebug_INT3Long(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_INT3Long::Check() const
{
	__try
	{
		__asm __emit(0xCD);
		__asm __emit(0x03);
	}
	__except (AntiDebug_INT3Long::INT3LongExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		return AntiDebug_INT3Long::m_bDebugged;
	}
}
int AntiDebug_INT3Long::INT3LongExceptionFilter(UINT code, PEXCEPTION_POINTERS ep)
{
	AntiDebug_INT3Long::m_bDebugged = code != EXCEPTION_BREAKPOINT;
	return EXCEPTION_EXECUTE_HANDLER;
}


CHECK_TECHNIQUE(INT2D)
{
	__try
	{
		__asm xor eax, eax;
		__asm int 0x2d;
		__asm nop;
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}


CHECK_TECHNIQUE(ICE)
{
	__try
	{
		__asm __emit 0xF1;
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}


CHECK_TECHNIQUE(StackSegmentRegister)
{
	bool bTraced = false;

	__asm
	{
		push ss
		pop ss
		pushf
		test byte ptr [esp+1], 1
		jz movss_not_being_debugged
	}

	bTraced = true;

movss_not_being_debugged:
	// restore stack
	__asm popf;

	return bTraced;
}


__declspec(naked) DWORD WINAPI InstructionCountingFunc(LPVOID lpThreadParameter)
{
	__asm
	{
		xor eax, eax
		nop
		nop
		nop
		nop
		cmp al, 4
		jne being_debugged
	}

	ExitThread(FALSE);

being_debugged:
	ExitThread(TRUE);
}
static LONG WINAPI InstructionCountingExeptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		pExceptionInfo->ContextRecord->Eax += 1;
		pExceptionInfo->ContextRecord->Eip += 1;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
AntiDebug_InstructionCounting::AntiDebug_InstructionCounting(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
	, m_pThreadAddr(nullptr)
	, m_hHwBps{0, 0, 0, 0}
{
}
bool AntiDebug_InstructionCounting::Check() const
{
	PVOID hVeh = nullptr;
	HANDLE hThread = nullptr;
	bool bDebugged = false;

	__try
	{
		hVeh = AddVectoredExceptionHandler(TRUE, InstructionCountingExeptionHandler);
		if (!hVeh)
			__leave;

		hThread = CreateThread(0, 0, InstructionCountingFunc, NULL, CREATE_SUSPENDED, 0);
		if (!hThread)
			__leave;

		m_pThreadAddr = &InstructionCountingFunc;
		if (*(PBYTE)m_pThreadAddr == 0xE9)
			m_pThreadAddr = (PVOID)((DWORD)m_pThreadAddr + 5 + *(PDWORD)((PBYTE)m_pThreadAddr + 1));

		for (size_t i = 0; i < m_nInstructionCount; i++)
			m_hHwBps[i] = SetHardwareBreakpoint(hThread, HWBRK_TYPE_CODE, HWBRK_SIZE_1, (PVOID)((DWORD)m_pThreadAddr + 2 + i));

		ResumeThread(hThread);
		WaitForSingleObject(hThread, INFINITE);

		DWORD dwThreadExitCode;
		if (TRUE == GetExitCodeThread(hThread, &dwThreadExitCode))
			bDebugged = (TRUE == dwThreadExitCode);
	}
	__finally
	{
		if (hThread)
			CloseHandle(hThread);

		for (int i = 0; i < 4; i++)
		{
			if (m_hHwBps[i])
				RemoveHardwareBreakpoint(m_hHwBps[i]);
		}

		if (hVeh)
			RemoveVectoredExceptionHandler(hVeh);
	}

	return bDebugged;
}


CHECK_TECHNIQUE(PopfAndTrapFlag)
{
	__try
	{
		__asm
		{
			pushfd
			mov dword ptr [esp], 0x100
			popfd
			nop
		}
		return true;
	}
	__except(GetExceptionCode() == EXCEPTION_SINGLE_STEP
		? EXCEPTION_EXECUTE_HANDLER
		: EXCEPTION_CONTINUE_EXECUTION)
	{
		return false;
	}
}


CHECK_TECHNIQUE(InstructionPrefixes)
{
	__try
	{
		// 0xF3 0x64 disassembles as PREFIX REP:
		__asm __emit 0xF3
		__asm __emit 0x64
		// One byte INT 1
		__asm __emit 0xF1
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}


static LONG DebugRegistersManipulationExeptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	bool bDebugged = false;
	if (pExceptionInfo->ContextRecord->Dr0 != 0 || pExceptionInfo->ContextRecord->Dr1 != 0 || 
		pExceptionInfo->ContextRecord->Dr2 != 0 || pExceptionInfo->ContextRecord->Dr3 != 0)
		bDebugged = true;

	pExceptionInfo->ContextRecord->Eip += 2;
	pExceptionInfo->ContextRecord->Dr0 = 0;
	pExceptionInfo->ContextRecord->Dr1 = 0;
	pExceptionInfo->ContextRecord->Dr2 = 0;
	pExceptionInfo->ContextRecord->Dr3 = 0;

	return bDebugged
		? EXCEPTION_CONTINUE_EXECUTION
		: EXCEPTION_EXECUTE_HANDLER;
}
CHECK_TECHNIQUE(DebugRegistersModification)
{
	__try
	{
		THROW_DIVIDE_BY_ZERO_EXCEPTION;
		return true;
	}
	__except(DebugRegistersManipulationExeptionHandler(GetExceptionInformation()))
	{
		return false;
	}
}
