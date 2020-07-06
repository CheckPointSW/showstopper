#include "Technique_HandlesValidation.h"


CHECK_TECHNIQUE(OpenProcess)
{
	HANDLE hCsr = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ntdll::CsrGetProcessId());
	if (hCsr != NULL)
	{
		CloseHandle(hCsr);
		return true;
	}        
	else
		return false;
}


CHECK_TECHNIQUE(CreateFile)
{
	CHAR szFileName[MAX_PATH];
	if (0 == GetModuleFileNameA(NULL, szFileName, sizeof(szFileName)))
		return false;
	return INVALID_HANDLE_VALUE == CreateFileA(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
}


CHECK_TECHNIQUE(CloseHandle)
{
	__try
	{
		CloseHandle((HANDLE)0xDEADBEEF);
		return false;
	}
	__except (EXCEPTION_INVALID_HANDLE == GetExceptionCode()
		? EXCEPTION_EXECUTE_HANDLER 
		: EXCEPTION_CONTINUE_SEARCH)
	{
		return true;
	}
}


CHECK_TECHNIQUE(LoadLibrary)
{
	CHAR szBuffer[] = { "C:\\Windows\\System32\\calc.exe" };
	LoadLibraryA(szBuffer);
	return INVALID_HANDLE_VALUE == CreateFileA(szBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
}


// Source: https://www.codeproject.com/Articles/30815/An-Anti-Reverse-Engineering-Guide#NtQueryObject
CHECK_TECHNIQUE(NtQueryObject)
{
	bool bDebugged = false;
	NTSTATUS status;
	LPVOID pMem = nullptr;
	ULONG dwMemSize;
	ntdll::POBJECT_ALL_INFORMATION pObjectAllInfo;
	PBYTE pObjInfoLocation;

	status = ntdll::NtQueryObject(
		NULL,
		ntdll::ObjectAllTypesInformation,
		&dwMemSize, sizeof(dwMemSize), &dwMemSize);
	if (STATUS_INFO_LENGTH_MISMATCH != status)
		goto NtQueryObject_Cleanup;

	pMem = VirtualAlloc(NULL, dwMemSize, MEM_COMMIT, PAGE_READWRITE);
	if (!pMem)
		goto NtQueryObject_Cleanup;

	status = ntdll::NtQueryObject(
		(HANDLE)-1,
		ntdll::ObjectAllTypesInformation,
		pMem, dwMemSize, &dwMemSize);
	if (!SUCCEEDED(status))
		goto NtQueryObject_Cleanup;

	pObjectAllInfo = (ntdll::POBJECT_ALL_INFORMATION)pMem;
	pObjInfoLocation = (PBYTE)pObjectAllInfo->ObjectTypeInformation;
	for(UINT i = 0; i < pObjectAllInfo->NumberOfObjects; i++)
	{

		ntdll::POBJECT_TYPE_INFORMATION pObjectTypeInfo = (ntdll::POBJECT_TYPE_INFORMATION)pObjInfoLocation;

		if (wcscmp(L"DebugObject", pObjectTypeInfo->TypeName.Buffer) == 0)
		{
			if (pObjectTypeInfo->TotalNumberOfObjects > 0)
				bDebugged = true;
			break;
		}

		// Get the address of the current entries
		// string so we can find the end
		pObjInfoLocation = (PBYTE)pObjectTypeInfo->TypeName.Buffer;

		// Add the size
		pObjInfoLocation += pObjectTypeInfo->TypeName.Length;

		// Skip the trailing null and alignment bytes
		ULONG tmp = ((ULONG)pObjInfoLocation) & -4;

		// Not pretty but it works
		pObjInfoLocation = ((PBYTE)tmp) + sizeof(DWORD);
	}

NtQueryObject_Cleanup:
	if (pMem)
		VirtualFree(pMem, 0, MEM_RELEASE);

	return bDebugged;
}
