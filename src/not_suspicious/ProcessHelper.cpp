#include <Windows.h>
#include <tlhelp32.h>
#include <string>

#include "ProcessHelper.h"
#include "StringHelper.h"

bool process_helper::EnableDebugPrivilege(bool Enable = true) 
{
	bool Success = false;
	HANDLE hToken = NULL;

	do
	{
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
			break;

		TOKEN_PRIVILEGES tp; 
		tp.PrivilegeCount = 1;
		if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
			break;

		tp.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
		if(!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
			break;

		Success = true;
	}
	while( 0 );

	if (hToken != NULL)
		CloseHandle(hToken);
	return Success;
}

bool process_helper::Is64BitWindows()
{
#if defined(_WIN64)
	return true;
#elif defined(_WIN32)
	BOOL bIsWow64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &bIsWow64) && bIsWow64;
#else
	return false;
#endif
}

bool process_helper::IsSuspiciousProcessName(PCSTR pszProcessName)
{
	if (!pszProcessName)
		return true;

	std::string sProcessName = string_heper::ToLower(std::string{ pszProcessName });
	if (string_heper::FindSubstringA(sProcessName, "dbg.exe") ||
		string_heper::FindSubstringA(sProcessName, "debugger") ||
		string_heper::StartsWith(sProcessName, "ida") ||
		(sProcessName == "devenv.exe"))
		return true;

	return false;
}

DWORD process_helper::GetParentProcessId(DWORD dwCurrentProcessId)
{
	DWORD dwParentProcessId = 0;
	PROCESSENTRY32W ProcessEntry = { 0 };
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(Process32FirstW(hSnapshot, &ProcessEntry))
	{
		do
		{
			if (ProcessEntry.th32ProcessID == dwCurrentProcessId)
			{
				dwParentProcessId = ProcessEntry.th32ParentProcessID;
				break;
			}
		} while(Process32NextW(hSnapshot, &ProcessEntry));
	}

	CloseHandle(hSnapshot);
	return dwParentProcessId;
}
