#include <Windows.h>
#include <winternl.h>
#include "NtDll.h"

namespace ntdll
{

#define FUNCNAME (__FUNCTION__ + (sizeof("ntdll::") - 1))

HMODULE hNtdll = NULL;

FARPROC GetProcAddress(PCSTR pszFuncName)
{
	if (NULL == hNtdll)
		hNtdll = LoadLibraryA("ntdll.dll");
	if (hNtdll)
		return ::GetProcAddress(hNtdll, pszFuncName);
	return NULL;
}

//
// Functions Pointers Declaration
//

typedef NTSTATUS (WINAPI *TNtQueryInformationProcess)(
	IN HANDLE                    ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	OUT PVOID                    ProcessInformation,
	IN ULONG                     ProcessInformationLength,
	OUT PULONG                   ReturnLength
	);

typedef NTSTATUS (NTAPI *TNtSetInformationProcess)(
	IN HANDLE                    ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	IN PVOID                     ProcessInformation,
	IN ULONG                     ProcessInformationLength
	);

typedef PDEBUG_BUFFER (WINAPI *TRtlCreateQueryDebugBuffer)(
	ULONG    Size,
	BOOLEAN  EventPair
	);

typedef NTSTATUS (WINAPI *TRtlQueryProcessHeapInformation)( 
	PRTL_DEBUG_INFORMATION Buffer 
	);

typedef NTSTATUS (WINAPI *TRtlQueryProcessDebugInformation)(
	ULONG  ProcessId,
	ULONG  DebugInfoClassMask,
	PDEBUG_BUFFER  DebugBuffer
	);

typedef NTSTATUS (WINAPI *TNtQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength
	);

typedef NTSTATUS (WINAPI *TNtSetInformationThread)(
	HANDLE                   ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID                    ThreadInformation,
	ULONG                    ThreadInformationLength
	);

typedef NTSTATUS (WINAPI *TNtQueryInformationThread)(
	HANDLE                   ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID                    ThreadInformation,
	ULONG                    ThreadInformationLength,
	PULONG                   ReturnLength
	);

typedef DWORD (WINAPI *TCsrGetProcessId)(VOID);


typedef NTSTATUS (WINAPI *TNtQueryObject)(
	HANDLE                   Handle,
	OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID                    ObjectInformation,
	ULONG                    ObjectInformationLength,
	PULONG                   ReturnLength
	);	

typedef NTSTATUS (WINAPI *TNtQueryVirtualMemory)(
	HANDLE                   ProcessHandle,
	PVOID                    BaseAddress,
	MEMORY_INFORMATION_CLASS MemoryInformationClass,
	PVOID                    MemoryInformation,
	SIZE_T                   MemoryInformationLength,
	PSIZE_T                  ReturnLength
	);

typedef NTSTATUS (WINAPI *TRtlProcessFlsData)(
	PRTL_UNKNOWN_FLS_DATA Buffer
	);

typedef NTSTATUS (WINAPI *TNtSetDebugFilterState)(
	ULONG ComponentId, 
	ULONG Level, 
	BOOLEAN State
	);

typedef ULONG (WINAPI *TDbgPrint)(
	PCSTR Format,
	...   
	);

//
// Functions Pointers 
//

TNtQueryInformationProcess       pfnNtQueryInformationProcess = nullptr;
TNtSetInformationProcess         pfnNtSetInformationProcess = nullptr;
TNtQuerySystemInformation        pfnNtQuerySystemInformation = nullptr;
TNtQueryVirtualMemory            pfnNtQueryVirtualMemory = nullptr;
TNtQueryObject                   pfnNtQueryObject = nullptr;

TRtlCreateQueryDebugBuffer       pfnRtlCreateQueryDebugBuffer = nullptr;
TRtlQueryProcessHeapInformation  pfnRtlQueryProcessHeapInformation = nullptr;
TRtlQueryProcessDebugInformation pfnRtlQueryProcessDebugInformation = nullptr;

TNtSetInformationThread          pfnNtSetInformationThread = nullptr;
TNtQueryInformationThread        pfnNtQueryInformationThread = nullptr;

TCsrGetProcessId                 pfnCsrGetProcessId = nullptr;

TRtlProcessFlsData               pfnRtlProcessFlsData = nullptr;

TNtSetDebugFilterState           pfnNtSetDebugFilterState = nullptr;

TDbgPrint                        pfnDbgPrint = nullptr;

//
// Functions Implementation
//

NTSTATUS WINAPI NtQueryInformationProcess(
	IN HANDLE                    ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	OUT PVOID                    ProcessInformation,
	IN ULONG                     ProcessInformationLength,
	OUT PULONG                   ReturnLength
	)
{
	if (!pfnNtQueryInformationProcess)
		pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(FUNCNAME);
	if (pfnNtQueryInformationProcess)
		return pfnNtQueryInformationProcess(
			ProcessHandle, 
			ProcessInformationClass, 
			ProcessInformation, 
			ProcessInformationLength, 
			ReturnLength);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtSetInformationProcess(
	HANDLE                    ProcessHandle,
	PROCESS_INFORMATION_CLASS ProcessInformationClass,
	PVOID                     ProcessInformation,
	ULONG                     ProcessInformationLength
	)
{
	if (!pfnNtSetInformationProcess)
		pfnNtSetInformationProcess = (TNtSetInformationProcess)GetProcAddress(FUNCNAME);
	if (pfnNtSetInformationProcess)
		return pfnNtSetInformationProcess(
			ProcessHandle,
			ProcessInformationClass,
			ProcessInformation,
			ProcessInformationLength
		);
	return STATUS_ORDINAL_NOT_FOUND;
}

PDEBUG_BUFFER WINAPI RtlCreateQueryDebugBuffer(
	ULONG  Size,
	BOOLEAN  EventPair
	)
{
	if (!pfnRtlCreateQueryDebugBuffer)
		pfnRtlCreateQueryDebugBuffer = (TRtlCreateQueryDebugBuffer)GetProcAddress(FUNCNAME);
	if (pfnRtlCreateQueryDebugBuffer)
		return pfnRtlCreateQueryDebugBuffer(
			Size, 
			EventPair);
	return NULL;
}

NTSTATUS WINAPI RtlQueryProcessHeapInformation(
	PRTL_DEBUG_INFORMATION Buffer
	)
{
	if (!pfnRtlQueryProcessHeapInformation)
		pfnRtlQueryProcessHeapInformation = (TRtlQueryProcessHeapInformation)GetProcAddress(FUNCNAME);
	if (pfnRtlQueryProcessHeapInformation)
		return pfnRtlQueryProcessHeapInformation(Buffer);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI RtlQueryProcessDebugInformation(
	ULONG  ProcessId,
	ULONG  DebugInfoClassMask,
	PDEBUG_BUFFER  DebugBuffer
	)
{
	if (!pfnRtlQueryProcessDebugInformation)
		pfnRtlQueryProcessDebugInformation = (TRtlQueryProcessDebugInformation)GetProcAddress(FUNCNAME);
	if (pfnRtlQueryProcessDebugInformation)
		return pfnRtlQueryProcessDebugInformation(
			ProcessId,
			DebugInfoClassMask,
			DebugBuffer);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength
	)
{
	if (!pfnNtQuerySystemInformation)
		pfnNtQuerySystemInformation = (TNtQuerySystemInformation)GetProcAddress(FUNCNAME);
	if (pfnNtQuerySystemInformation)
		return pfnNtQuerySystemInformation(
			SystemInformationClass, 
			SystemInformation, 
			SystemInformationLength, 
			ReturnLength);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtSetInformationThread(
	HANDLE          ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID           ThreadInformation,
	ULONG           ThreadInformationLength
	)
{
	if (!pfnNtSetInformationThread)
		pfnNtSetInformationThread = (TNtSetInformationThread)GetProcAddress(FUNCNAME);
	if (pfnNtSetInformationThread)
		return pfnNtSetInformationThread(
			ThreadHandle,
			ThreadInformationClass,
			ThreadInformation,
			ThreadInformationLength
		);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtQueryInformationThread(
	HANDLE                   ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID                    ThreadInformation,
	ULONG                    ThreadInformationLength,
	PULONG                   ReturnLength
	)
{
	if (!pfnNtQueryInformationThread)
		pfnNtQueryInformationThread = (TNtQueryInformationThread)GetProcAddress(FUNCNAME);
	if (pfnNtQueryInformationThread)
		return pfnNtQueryInformationThread(
			ThreadHandle,
			ThreadInformationClass,
			ThreadInformation,
			ThreadInformationLength,
			ReturnLength
		);
	return STATUS_ORDINAL_NOT_FOUND;
}

DWORD WINAPI CsrGetProcessId(VOID)
{
	if (!pfnCsrGetProcessId)
		pfnCsrGetProcessId = (TCsrGetProcessId)GetProcAddress(FUNCNAME);
	if (pfnCsrGetProcessId)
		pfnCsrGetProcessId();
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtQueryObject(
	HANDLE                   Handle,
	OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID                    ObjectInformation,
	ULONG                    ObjectInformationLength,
	PULONG                   ReturnLength
	)
{
	if (!pfnNtQueryObject)
		pfnNtQueryObject = (TNtQueryObject)GetProcAddress(FUNCNAME);
	if (pfnNtQueryObject)
		return pfnNtQueryObject(
			Handle,
			ObjectInformationClass, 
			ObjectInformation, 
			ObjectInformationLength, 
			ReturnLength);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtQueryVirtualMemory(
	HANDLE                   ProcessHandle,
	PVOID                    BaseAddress,
	MEMORY_INFORMATION_CLASS MemoryInformationClass,
	PVOID                    MemoryInformation,
	SIZE_T                   MemoryInformationLength,
	PSIZE_T                  ReturnLength
	)
{
	if (!pfnNtQueryVirtualMemory)
		pfnNtQueryVirtualMemory = (TNtQueryVirtualMemory)GetProcAddress(FUNCNAME);
	if (pfnNtQueryVirtualMemory)
		return pfnNtQueryVirtualMemory(
			ProcessHandle,
			BaseAddress,
			MemoryInformationClass, 
			MemoryInformation, 
			MemoryInformationLength, 
			ReturnLength);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI RtlProcessFlsData(PRTL_UNKNOWN_FLS_DATA Buffer)
{
	if (!pfnRtlProcessFlsData)
		pfnRtlProcessFlsData = (TRtlProcessFlsData)GetProcAddress(FUNCNAME);
	if (pfnRtlProcessFlsData)
		return pfnRtlProcessFlsData(Buffer);
	return STATUS_ORDINAL_NOT_FOUND;
}

NTSTATUS WINAPI NtSetDebugFilterState(ULONG ComponentId, ULONG Level, BOOLEAN State)
{
	if (!pfnNtSetDebugFilterState)
		pfnNtSetDebugFilterState = (TNtSetDebugFilterState)GetProcAddress(FUNCNAME);
	if (pfnNtSetDebugFilterState)
		return pfnNtSetDebugFilterState(ComponentId, Level, State);
	return STATUS_ORDINAL_NOT_FOUND;
}

ULONG WINAPI DbgPrint(PCSTR Format, ...)
{
	ULONG ulResult = 0;
	if (!pfnDbgPrint)
		pfnDbgPrint = (TDbgPrint)GetProcAddress(FUNCNAME);
	if (pfnDbgPrint)
	{
		va_list args;
		va_start(args, Format);
		ulResult = pfnDbgPrint(Format, args);
		va_end(args);
	}
	return ulResult;
}

}
