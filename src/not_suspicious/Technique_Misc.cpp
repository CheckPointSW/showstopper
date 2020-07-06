#include "Technique_Misc.h"
#include "ProcessHelper.h"
#undef UNICODE
#include <tlhelp32.h>


AntiDebug_FindWindow::AntiDebug_FindWindow(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_FindWindow::Check() const
{
	for (auto &sWndClass : m_vWindowClasses)
	{
		if (NULL != FindWindowA(sWndClass.c_str(), NULL))
			return true;
	}
	return false;
}


AntiDebug_ParentProcessCheck_NtQueryInformationProcess::AntiDebug_ParentProcessCheck_NtQueryInformationProcess(
	const std::string name, 
	const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_ParentProcessCheck_NtQueryInformationProcess::Check() const
{
	HWND hExplorerWnd = GetShellWindow();
	if (!hExplorerWnd)
		return false;

	DWORD dwExplorerProcessId;
	GetWindowThreadProcessId(hExplorerWnd, &dwExplorerProcessId);

	ntdll::PROCESS_BASIC_INFORMATION ProcessInfo;
	NTSTATUS status = ntdll::NtQueryInformationProcess(
		GetCurrentProcess(),
		ntdll::PROCESS_INFORMATION_CLASS::ProcessBasicInformation,
		&ProcessInfo,
		sizeof(ProcessInfo),
		NULL);
	if (!NT_SUCCESS(status))
		return false;

	return (DWORD)ProcessInfo.InheritedFromUniqueProcessId != dwExplorerProcessId;
}


AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot::AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot(
	const std::string name, 
	const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot::Check() const
{
	bool bDebugged = false;
	DWORD dwParentProcessId = process_helper::GetParentProcessId(GetCurrentProcessId());

	PROCESSENTRY32 ProcessEntry = { 0 };
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(Process32First(hSnapshot, &ProcessEntry))
	{
		do
		{
			if ((ProcessEntry.th32ProcessID == dwParentProcessId) &&
				process_helper::IsSuspiciousProcessName(ProcessEntry.szExeFile))
			{
				bDebugged = true;
				break;
			}
		} while(Process32Next(hSnapshot, &ProcessEntry));
	}

	CloseHandle(hSnapshot);
	return bDebugged;
}


AntiDebug_Selectors::AntiDebug_Selectors(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_Selectors::Check() const
{
	__asm
	{
		push 3
		pop  gs

	__asm SeclectorsLbl:
		mov  ax, gs
		cmp  al, 3
		je   SeclectorsLbl
		
		push 3
		pop  gs
		mov  ax, gs
		cmp  al, 3
		jne  Selectors_Debugged
	}

	return false;

Selectors_Debugged:
	return true;
}


AntiDebug_DbgPrint::AntiDebug_DbgPrint(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_DbgPrint::Check() const
{
	__try
	{
		RaiseException(DBG_PRINTEXCEPTION_C, 0, 0, NULL);
		return true;
	}
	__except(GetExceptionCode() == DBG_PRINTEXCEPTION_C)
	{
		return false;
	}
}


AntiDebug_DbgSetDebugFilterState::AntiDebug_DbgSetDebugFilterState(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_DbgSetDebugFilterState::Check() const
{
	return NT_SUCCESS(ntdll::NtSetDebugFilterState(0, 0, TRUE));
}
