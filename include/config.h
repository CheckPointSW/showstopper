#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <map>
#include <string>

/*
 * JSON has the following format:
 *
 * {
 *     "name": "<GroupName>",
 *     "subgroups": [
 *         {
 *             "name": "<TechniqueName>",
 *             "tags": [
 *                 "admin",                // Need admin privileges
 *                 "crash",                // If the program crashes on a hit
 *                 "trace",                // Need to trace the program to hit
 *                 "break",                // Need to set a breakpoint to hit
 *                 "attach"                // Need to attach to the process to hit
 *             ],
 *             "parameters": [
 *                 {
 *                     "name": "<ParamName>",
 *                     "type": "<Dword/Qword/Real/String>",
 *                     "value": <ParamValue>
 *                 },
 *                 ...
 *             ]
 *         },
 *         ...
 *     ]
 * }
 *
 */

namespace Config
{
	enum class ETechnique
	{
	// Debug flags : Using Win32 API
		IsDebuggerPresent,
		CheckRemoteDebuggerPresent,
		ProcessDebugPort,
		ProcessDebugFlags,
		ProcessDebugObjectHandle,
		RtlQueryProcessHeapInformation,
		RtlQueryProcessDebugInformation,
		SystemKernelDebuggerInformation,

	// Debug flags : Manual checks
		BeingDebuggedFlag,
		NtGlobalFlag,
		HeapFlags,
		HeapProtection,

	// Handles validation
		OpenProcess,
		CreateFileA,
		CloseHandle,
		LoadLibraryA,
		NtQueryObject,

	// Exceptions
		UnhandledExceptionFilter,
		RaiseException,
		VectoredExceptionHandler,

	// Timing
		RDTSC,
		GetLocalTime,
		GetSystemTime,
		GetTickCount,
		QueryPerformanceCounter,
		timeGetTime,

	// Process memory checks : Breakpoints
		ScanINT3,
		AntiStepOver,
		AntiStepOverReadFile,
		AntiStepOverWriteProcessMemory,
		Toolhelp32ReadProcessMemory,
		MemoryBreakpoints,
		HardwareBreakpoints,

	// Process memory checks : Other
		NtQueryVirtualMemory,
		FunctionPatch,
		DbgBreakPointPatch,
		DbgUiRemoteBreakinPatch,
		CodeChecksum,

	// Checks with assembly instructions
		INT3,
		INT3Long,
		INT2D,
		ICE,
		StackSegmentRegister,
		InstructionCounting,
		PopfAndTrapFlag,
		InstructionPrefixes,
		DebugRegistersModification,

	// Direct debugger interaction
		SelfDebugging,
		GenerateConsoleCtrlEvent,
		BlockInput,
		NtSetInformationThread,
		SuspendThread,

	// Misc
		FindWindowA,
		ParentProcessCheck_NtQueryInformationProcess,
		ParentProcessCheck_CreateToolhelp32Snapshot,
		Selectors,
		DbgPrint,
		DbgSetDebugFilterState,

	};

	const std::map<ETechnique, std::string> TechniqueToName = {
		// Debug flags : Using Win32 API
		{ ETechnique::IsDebuggerPresent, "IsDebuggerPresent()" },
		{ ETechnique::CheckRemoteDebuggerPresent, "CheckRemoteDebuggerPresent()" },
		{ ETechnique::ProcessDebugPort, "ProcessDebugPort" },
		{ ETechnique::ProcessDebugFlags, "ProcessDebugFlags" },
		{ ETechnique::ProcessDebugObjectHandle, "ProcessDebugObjectHandle" },
		{ ETechnique::RtlQueryProcessHeapInformation, "RtlQueryProcessHeapInformation" },
		{ ETechnique::RtlQueryProcessDebugInformation, "RtlQueryProcessDebugInformation" },
		{ ETechnique::SystemKernelDebuggerInformation, "SystemKernelDebuggerInformation" },

		// Debug flags : Manual checks
		{ ETechnique::BeingDebuggedFlag, "PEB!BeingDebugged" },
		{ ETechnique::NtGlobalFlag, "NtGlobalFlag" },
		{ ETechnique::HeapFlags, "HeapFlags" },
		{ ETechnique::HeapProtection, "HeapProtection" },

		// Handles validation
		{ ETechnique::OpenProcess, "OpenProcess" },
		{ ETechnique::CreateFileA, "CreateFile" },
		{ ETechnique::CloseHandle, "CloseHandle" },
		{ ETechnique::LoadLibraryA, "LoadLibrary" },
		{ ETechnique::NtQueryObject, "NtQueryObject" },

		// Exceptions
		{ ETechnique::UnhandledExceptionFilter, "UnhandledExceptionFilter" },
		{ ETechnique::RaiseException, "RaiseException" },
		{ ETechnique::VectoredExceptionHandler, "VectoredExceptionHandler" },

		// Timing
		{ ETechnique::RDTSC, "RDTSC" },
		{ ETechnique::GetLocalTime, "GetLocalTime" },
		{ ETechnique::GetSystemTime, "GetSystemTime" },
		{ ETechnique::GetTickCount, "GetTickCount" },
		{ ETechnique::QueryPerformanceCounter, "QueryPerformanceCounter" },
		{ ETechnique::timeGetTime, "timeGetTime" },

		// Process memory checks : Breakpoints
		{ ETechnique::ScanINT3, "ScanINT3" },
		{ ETechnique::AntiStepOver, "AntiStepOver" },
		{ ETechnique::AntiStepOverReadFile, "AntiStepOverReadFile" },
		{ ETechnique::AntiStepOverWriteProcessMemory, "AntiStepOverWriteProcessMemory" },
		{ ETechnique::Toolhelp32ReadProcessMemory, "Toolhelp32ReadProcessMemory" },
		{ ETechnique::MemoryBreakpoints, "MemoryBreakpoints" },
		{ ETechnique::HardwareBreakpoints, "HardwareBreakpoints" },

		// Process memory checks : Other
		{ ETechnique::NtQueryVirtualMemory, "NtQueryVirtualMemory" },
		{ ETechnique::FunctionPatch, "FunctionPatch" },
		{ ETechnique::DbgBreakPointPatch, "DbgBreakPointPatch" },
		{ ETechnique::DbgUiRemoteBreakinPatch, "DbgUiRemoteBreakinPatch" },
		{ ETechnique::CodeChecksum, "CodeChecksum" },

		// Checks with assembly instructions
		{ ETechnique::INT3, "INT3" },
		{ ETechnique::INT3Long, "INT3Long" },
		{ ETechnique::INT2D, "INT2D" },
		{ ETechnique::ICE, "ICE" },
		{ ETechnique::StackSegmentRegister, "StackSegmentRegister" },
		{ ETechnique::InstructionCounting, "InstructionCounting" },
		{ ETechnique::PopfAndTrapFlag, "PopfAndTrapFlag" },
		{ ETechnique::InstructionPrefixes, "InstructionPrefixes" },
		{ ETechnique::DebugRegistersModification, "DebugRegistersModification" },

		// Direct debugger interaction
		{ ETechnique::SelfDebugging, "SelfDebugging" },
		{ ETechnique::GenerateConsoleCtrlEvent, "GenerateConsoleCtrlEvent" },
		{ ETechnique::BlockInput, "BlockInput" },
		{ ETechnique::NtSetInformationThread, "NtSetInformationThread" },
		{ ETechnique::SuspendThread, "SuspendThread" },

		// Misc
		{ ETechnique::FindWindowA, "FindWindow" },
		{ ETechnique::ParentProcessCheck_NtQueryInformationProcess, "ParentProcessCheck_NtQueryInformationProcess" },
		{ ETechnique::ParentProcessCheck_CreateToolhelp32Snapshot, "ParentProcessCheck_CreateToolhelp32Snapshot" },
		{ ETechnique::Selectors, "Selectors" },
		{ ETechnique::DbgPrint, "DbgPrint" },
		{ ETechnique::DbgSetDebugFilterState, "DbgSetDebugFilterState" },

	};
}

#endif // _CONFIG_H_
