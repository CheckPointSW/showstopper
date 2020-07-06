#include "Technique_DebugFlags.h"
#include <VersionHelpers.h>


CHECK_TECHNIQUE(IsDebuggerPresent)
{
	return TRUE == IsDebuggerPresent();
}


CHECK_TECHNIQUE(CheckRemoteDebuggerPresent)
{
	BOOL bDebuggerPresent = FALSE;
	if (TRUE == CheckRemoteDebuggerPresent(GetCurrentProcess(), &bDebuggerPresent))
		return TRUE == bDebuggerPresent;
	return false;
}


CHECK_TECHNIQUE(ProcessDebugPort)
{
#ifndef _WIN64
	DWORD ProcessInformation;
#else
	DWORD64 ProcessInformation;
#endif // _WIN64

	NTSTATUS status = ntdll::NtQueryInformationProcess(
		GetCurrentProcess(),
		ntdll::PROCESS_INFORMATION_CLASS::ProcessDebugPort,
		&ProcessInformation,
		sizeof(ProcessInformation),
		NULL);
	if (!SUCCEEDED(status))
		return false;
	return -1 == ProcessInformation;
}


CHECK_TECHNIQUE(ProcessDebugFlags)
{
#ifndef _WIN64
	DWORD ProcessInformation;
#else
	DWORD64 ProcessInformation;
#endif // _WIN64

	NTSTATUS status = ntdll::NtQueryInformationProcess(
		GetCurrentProcess(),
		ntdll::PROCESS_INFORMATION_CLASS::ProcessDebugFlags,
		&ProcessInformation,
		sizeof(ProcessInformation),
		NULL);
	if (!SUCCEEDED(status))
		return false;
	return 0 == ProcessInformation;
}


CHECK_TECHNIQUE(ProcessDebugObjectHandle)
{
#ifndef _WIN64
	DWORD ProcessInformation;
#else
	DWORD64 ProcessInformation;
#endif // _WIN64

	NTSTATUS status = ntdll::NtQueryInformationProcess(
		GetCurrentProcess(),
		ntdll::PROCESS_INFORMATION_CLASS::ProcessDebugObjectHandle,
		&ProcessInformation,
		sizeof(ProcessInformation),
		NULL);
	if (!SUCCEEDED(status))
		return false;
	return 0 != ProcessInformation;
}


CHECK_TECHNIQUE(RtlQueryProcessHeapInformation)
{
	ntdll::PDEBUG_BUFFER pDebugBuffer = ntdll::RtlCreateQueryDebugBuffer(0, FALSE);
	if (!SUCCEEDED(ntdll::RtlQueryProcessHeapInformation((ntdll::PRTL_DEBUG_INFORMATION)pDebugBuffer)))
		return false;

	ULONG dwFlags = ((ntdll::PRTL_PROCESS_HEAPS)pDebugBuffer->HeapInformation)->Heaps[0].Flags;
	return dwFlags & ~HEAP_GROWABLE;
}


CHECK_TECHNIQUE(RtlQueryProcessDebugInformation)
{
	ntdll::PDEBUG_BUFFER pDebugBuffer = ntdll::RtlCreateQueryDebugBuffer(0, FALSE);
	if (!SUCCEEDED(ntdll::RtlQueryProcessDebugInformation(GetCurrentProcessId(), PDI_HEAPS | PDI_HEAP_BLOCKS, pDebugBuffer)))
		return false;

	ULONG dwFlags = ((ntdll::PRTL_PROCESS_HEAPS)pDebugBuffer->HeapInformation)->Heaps[0].Flags;
	return dwFlags & ~HEAP_GROWABLE;
}


CHECK_TECHNIQUE(SystemKernelDebuggerInformation)
{
	NTSTATUS status;
	ntdll::SYSTEM_KERNEL_DEBUGGER_INFORMATION SystemInfo;

	status = ntdll::NtQuerySystemInformation(
		(ntdll::SYSTEM_INFORMATION_CLASS)ntdll::SystemKernelDebuggerInformation,
		&SystemInfo,
		sizeof(SystemInfo),
		NULL);

	return SUCCEEDED(status)
		? (SystemInfo.DebuggerEnabled && !SystemInfo.DebuggerNotPresent)
		: false;
}


CHECK_TECHNIQUE(BeingDebugged)
{
#ifndef _WIN64
	PPEB pPeb = (PPEB)__readfsdword(0x30);
#else
	PPEB pPeb = (PPEB)__readgsqword(0x60);
#endif // _WIN64
	return 0 != pPeb->BeingDebugged;
}


CHECK_TECHNIQUE(NtGlobalFlag)
{
#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

#ifndef _WIN64
	PPEB pPeb = (PPEB)__readfsdword(0x30);
	DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0x68);
#else
	PPEB pPeb = (PPEB)__readgsqword(0x60);
	DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0xBC);
#endif // _WIN64

	return NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
}


CHECK_TECHNIQUE(HeapFlags)
{
	BOOL bIsWow64;
	if (ERROR_SUCCESS != IsWow64Process(GetCurrentProcess(), &bIsWow64))
		bIsWow64 = FALSE;

#ifndef _WIN64
	PPEB pPeb = (PPEB)__readfsdword(0x30);
	PVOID pHeapBase = !bIsWow64
		? (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x18))
		: (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x1030));
	DWORD dwHeapFlagsOffset = IsWindowsVistaOrGreater()
		? 0x40
		: 0x0C;
	DWORD dwHeapForceFlagsOffset = IsWindowsVistaOrGreater()
		? 0x44 
		: 0x10;
#else
	PPEB pPeb = (PPEB)__readgsqword(0x60);
	PVOID pHeapBase = (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x30));
	DWORD dwHeapFlagsOffset = IsWindowsVistaOrGreater()
		? 0x70 
		: 0x14;
	DWORD dwHeapForceFlagsOffset = IsWindowsVistaOrGreater()
		? 0x74 
		: 0x18;
#endif // _WIN64

	PDWORD pdwHeapFlags = (PDWORD)((PBYTE)pHeapBase + dwHeapFlagsOffset);
	PDWORD pdwHeapForceFlags = (PDWORD)((PBYTE)pHeapBase + dwHeapForceFlagsOffset);
	return (*pdwHeapFlags & ~HEAP_GROWABLE || *pdwHeapForceFlags != 0);
}


CHECK_TECHNIQUE(HeapProtection)
{
	PROCESS_HEAP_ENTRY HeapEntry = { 0 };
	do
	{
		if (!HeapWalk(GetProcessHeap(), &HeapEntry))
			return false;
	} while (HeapEntry.wFlags != PROCESS_HEAP_ENTRY_BUSY);

	PVOID pOverlapped = (PBYTE)HeapEntry.lpData + HeapEntry.cbData;
	return (DWORD)(*(PDWORD)pOverlapped) == 0xABABABAB;
}
