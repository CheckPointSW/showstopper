#include "Technique_DirectDbgInteraction.h"
#include <VersionHelpers.h>
#include <tlhelp32.h>
#include "ProcessHelper.h"
#include "StringHelper.h"
#include <iostream>


AntiDebug_SelfDebugging::AntiDebug_SelfDebugging(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
	, m_hDbgEvent(CreateEventW(NULL, FALSE, FALSE, EVENT_SELFDBG_EVENT_NAME))
{
}
AntiDebug_SelfDebugging::~AntiDebug_SelfDebugging()
{
	if (m_hDbgEvent)
		CloseHandle(m_hDbgEvent);
}
bool AntiDebug_SelfDebugging::Check() const
{
	WCHAR wszFilePath[MAX_PATH], wszCmdLine[MAX_PATH];
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	if (!m_hDbgEvent)
		return false;

	if (!GetModuleFileNameW(NULL, wszFilePath, _countof(wszFilePath)))
		return false;

	swprintf_s(wszCmdLine, L"%s -a SelfDebugging,%d", wszFilePath, GetCurrentProcessId());
	if (CreateProcessW(NULL, wszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return WAIT_OBJECT_0 == WaitForSingleObject(m_hDbgEvent, 0);
	}

	return false;
}


bool AntiDebug_GenerateConsoleCtrlEvent::m_bDebugged{ false };
std::atomic<bool> AntiDebug_GenerateConsoleCtrlEvent::m_bCtlCCatched{ false };
static LONG WINAPI CtrlEventExeptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == DBG_CONTROL_C)
	{
		AntiDebug_GenerateConsoleCtrlEvent::m_bDebugged = true;
		AntiDebug_GenerateConsoleCtrlEvent::m_bCtlCCatched.store(true);
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}
static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		AntiDebug_GenerateConsoleCtrlEvent::m_bCtlCCatched.store(true);
		return TRUE;
	default:
		return FALSE;
	}
}
AntiDebug_GenerateConsoleCtrlEvent::AntiDebug_GenerateConsoleCtrlEvent(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_GenerateConsoleCtrlEvent::Check() const
{
	PVOID hVeh = nullptr;
	BOOL bCtrlHadnlerSet = FALSE;

	__try
	{
		hVeh = AddVectoredExceptionHandler(TRUE, CtrlEventExeptionHandler);
		if (!hVeh)
			__leave;

		bCtrlHadnlerSet = SetConsoleCtrlHandler(CtrlHandler, 1);
		if (!bCtrlHadnlerSet)
			__leave;

		GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
		while (!AntiDebug_GenerateConsoleCtrlEvent::m_bCtlCCatched.load())
			;
	}
	__finally
	{
		if (bCtrlHadnlerSet)
			SetConsoleCtrlHandler(CtrlHandler, 0);

		if (hVeh)
			RemoveVectoredExceptionHandler(hVeh);
	}

	return AntiDebug_GenerateConsoleCtrlEvent::m_bDebugged;
}


AntiDebug_BlockInput::AntiDebug_BlockInput(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_BlockInput::Check() const
{
	BOOL bFirstResult = FALSE, bSecondResult = FALSE;
	__try
	{
		bFirstResult = BlockInput(TRUE);
		bSecondResult = BlockInput(TRUE);
	}
	__finally
	{
		BlockInput(FALSE);
	}
	return bFirstResult && bSecondResult;
}


AntiDebug_NtSetInformationThread::AntiDebug_NtSetInformationThread(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_NtSetInformationThread::Check() const
{
	NTSTATUS status = ntdll::NtSetInformationThread(
		NtCurrentThread, 
		ntdll::THREAD_INFORMATION_CLASS::ThreadHideFromDebugger, 
		0, 
		0);
	return status >= 0;
}


DWORD AntiDebug_SuspendThread::m_dwDebuggerProcessId = -1;
AntiDebug_SuspendThread::AntiDebug_SuspendThread(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_SuspendThread::Check() const
{
	THREADENTRY32 ThreadEntry = { 0 };
	ThreadEntry.dwSize = sizeof(THREADENTRY32);

	DWORD dwParentProcessId = process_helper::GetParentProcessId(GetCurrentProcessId());
	if (0 == dwParentProcessId)
		return false;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwParentProcessId);
	if(Thread32First(hSnapshot, &ThreadEntry))
	{
		do
		{
			if ((ThreadEntry.th32OwnerProcessID == dwParentProcessId) && IsDebuggerProcess(dwParentProcessId))
			{
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, ThreadEntry.th32ThreadID);
				if (hThread)
					SuspendThread(hThread);
				break;
			}
		} while(Thread32Next(hSnapshot, &ThreadEntry));
	}

	if (hSnapshot)
		CloseHandle(hSnapshot);

	return false;
}
bool AntiDebug_SuspendThread::IsDebuggerProcess(DWORD dwProcessId) const
{
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&dwProcessId));
	return AntiDebug_SuspendThread::m_dwDebuggerProcessId == dwProcessId;
}
BOOL CALLBACK AntiDebug_SuspendThread::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwProcessId = *(PDWORD)lParam;

	DWORD dwWindowProcessId;
	GetWindowThreadProcessId(hwnd, &dwWindowProcessId);

	if (dwProcessId == dwWindowProcessId)
	{
		std::wstring wsWindowTitle{ string_heper::ToLower(std::wstring(GetWindowTextLengthW(hwnd) + 1, L'\0')) };
		GetWindowTextW(hwnd, &wsWindowTitle[0], wsWindowTitle.size());

		if (string_heper::FindSubstringW(wsWindowTitle, L"dbg") || string_heper::FindSubstringW(wsWindowTitle, L"debugger"))
		{
			AntiDebug_SuspendThread::m_dwDebuggerProcessId = dwProcessId;
			return FALSE;
		}
		return FALSE;
	}

	return TRUE;
}
