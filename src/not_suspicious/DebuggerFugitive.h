#ifndef _DEBUGGER_FUGITIVE_H_
#define _DEBUGGER_FUGITIVE_H_

#include "Utils.h"
#include "config.h"
#include "UiProxy.h"
#include "Technique_DebugFlags.h"
#include "Technique_HandlesValidation.h"
#include "Technique_Exceptions.h"
#include "Technique_Timing.h"
#include "Technique_MemoryChecks.h"
#include "Technique_Assembler.h"
#include "Technique_DirectDbgInteraction.h"
#include "Technique_Misc.h"

using anti_debug_ptr = std::shared_ptr<AntiDebug>;

class DebuggerFugitive
{
public:
	DebuggerFugitive() {}
	DebuggerFugitive(std::map<std::string, bool> &cnf, bool showInfo)
		: m_mExecutionOptions(cnf)
		, m_bShowInfo(showInfo)
	{}
	~DebuggerFugitive() {}

	BEGIN_TECHNIQUE_MAP()
	// Debug flags : Using Win32 API
		ON_TECHNIQUE_ID(Config::ETechnique::IsDebuggerPresent, AntiDebug_IsDebuggerPresent)
		ON_TECHNIQUE_ID(Config::ETechnique::CheckRemoteDebuggerPresent, AntiDebug_CheckRemoteDebuggerPresent)
		ON_TECHNIQUE_ID(Config::ETechnique::ProcessDebugPort, AntiDebug_ProcessDebugPort)
		ON_TECHNIQUE_ID(Config::ETechnique::ProcessDebugFlags, AntiDebug_ProcessDebugFlags)
		ON_TECHNIQUE_ID(Config::ETechnique::ProcessDebugObjectHandle, AntiDebug_ProcessDebugObjectHandle)
		ON_TECHNIQUE_ID(Config::ETechnique::RtlQueryProcessHeapInformation, AntiDebug_RtlQueryProcessHeapInformation)
		ON_TECHNIQUE_ID(Config::ETechnique::RtlQueryProcessDebugInformation, AntiDebug_RtlQueryProcessDebugInformation)
		ON_TECHNIQUE_ID(Config::ETechnique::SystemKernelDebuggerInformation, AntiDebug_SystemKernelDebuggerInformation)
		
	// Debug flags : Manual checks
		ON_TECHNIQUE_ID(Config::ETechnique::BeingDebuggedFlag, AntiDebug_BeingDebugged)
		ON_TECHNIQUE_ID(Config::ETechnique::NtGlobalFlag, AntiDebug_NtGlobalFlag)
		ON_TECHNIQUE_ID(Config::ETechnique::HeapFlags, AntiDebug_HeapFlags)
		ON_TECHNIQUE_ID(Config::ETechnique::HeapProtection, AntiDebug_HeapProtection)
		
	// Handles validation
		ON_TECHNIQUE_ID(Config::ETechnique::OpenProcess, AntiDebug_OpenProcess)
		ON_TECHNIQUE_ID(Config::ETechnique::CreateFileA, AntiDebug_CreateFile)
		ON_TECHNIQUE_ID(Config::ETechnique::CloseHandle, AntiDebug_CloseHandle)
		ON_TECHNIQUE_ID(Config::ETechnique::LoadLibraryA, AntiDebug_LoadLibrary)
		ON_TECHNIQUE_ID(Config::ETechnique::NtQueryObject, AntiDebug_NtQueryObject)

	// Exceptions
		ON_TECHNIQUE_ID(Config::ETechnique::UnhandledExceptionFilter, AntiDebug_UnhandledExceptionFilter)
		ON_TECHNIQUE_ID(Config::ETechnique::RaiseException, AntiDebug_RaiseException)
		ON_TECHNIQUE_ID(Config::ETechnique::VectoredExceptionHandler, AntiDebug_VectoredExceptionHandler)

	// Timing
		ON_TECHNIQUE_ID(Config::ETechnique::RDTSC, AntiDebug_RDTSC)
		ON_TECHNIQUE_ID(Config::ETechnique::GetLocalTime, AntiDebug_GetLocalTime)
		ON_TECHNIQUE_ID(Config::ETechnique::GetSystemTime, AntiDebug_GetSystemTime)
		ON_TECHNIQUE_ID(Config::ETechnique::GetTickCount, AntiDebug_GetTickCount)
		ON_TECHNIQUE_ID(Config::ETechnique::QueryPerformanceCounter, AntiDebug_QueryPerformanceCounter)
		ON_TECHNIQUE_ID(Config::ETechnique::timeGetTime, AntiDebug_timeGetTime)

	// Process memory checks : Breakpoints
		ON_TECHNIQUE_ID(Config::ETechnique::ScanINT3, AntiDebug_INT3Scan)
		ON_TECHNIQUE_ID(Config::ETechnique::AntiStepOver, AntiDebug_AntiStepOver)
		ON_TECHNIQUE_ID(Config::ETechnique::AntiStepOverReadFile, AntiDebug_AntiStepOverReadFile)
		ON_TECHNIQUE_ID(Config::ETechnique::AntiStepOverWriteProcessMemory, AntiDebug_AntiStepOverWriteProcessMemory)
		ON_TECHNIQUE_ID(Config::ETechnique::Toolhelp32ReadProcessMemory, AntiDebug_Toolhelp32ReadProcessMemory)
		ON_TECHNIQUE_ID(Config::ETechnique::MemoryBreakpoints, AntiDebug_MemoryBreakpoints)
		ON_TECHNIQUE_ID(Config::ETechnique::HardwareBreakpoints, AntiDebug_HardwareBreakpoints)

	// Process memory checks : Other
		ON_TECHNIQUE_ID(Config::ETechnique::NtQueryVirtualMemory, AntiDebug_NtQueryVirtualMemory)
		ON_TECHNIQUE_ID(Config::ETechnique::FunctionPatch, AntiDebug_FunctionPatch)
		ON_TECHNIQUE_ID(Config::ETechnique::DbgBreakPointPatch, AntiDebug_DbgBreakPointPatch)
		ON_TECHNIQUE_ID(Config::ETechnique::DbgUiRemoteBreakinPatch, AntiDebug_DbgUiRemoteBreakinPatch)
		ON_TECHNIQUE_ID(Config::ETechnique::CodeChecksum, AntiDebug_CodeChecksum)

	// Checks with assembly instructions
		ON_TECHNIQUE_ID(Config::ETechnique::INT3, AntiDebug_INT3)
		ON_TECHNIQUE_ID(Config::ETechnique::INT3Long, AntiDebug_INT3Long)
		ON_TECHNIQUE_ID(Config::ETechnique::INT2D, AntiDebug_INT2D)
		ON_TECHNIQUE_ID(Config::ETechnique::ICE, AntiDebug_ICE)
		ON_TECHNIQUE_ID(Config::ETechnique::StackSegmentRegister, AntiDebug_StackSegmentRegister)
		ON_TECHNIQUE_ID(Config::ETechnique::InstructionCounting, AntiDebug_InstructionCounting)
		ON_TECHNIQUE_ID(Config::ETechnique::PopfAndTrapFlag, AntiDebug_PopfAndTrapFlag)
		ON_TECHNIQUE_ID(Config::ETechnique::InstructionPrefixes, AntiDebug_InstructionPrefixes)
		ON_TECHNIQUE_ID(Config::ETechnique::DebugRegistersModification, AntiDebug_DebugRegistersModification)

	// Direct debugger interaction
		ON_TECHNIQUE_ID(Config::ETechnique::SelfDebugging, AntiDebug_SelfDebugging)
		ON_TECHNIQUE_ID(Config::ETechnique::GenerateConsoleCtrlEvent, AntiDebug_GenerateConsoleCtrlEvent)
		ON_TECHNIQUE_ID(Config::ETechnique::BlockInput, AntiDebug_BlockInput)
		ON_TECHNIQUE_ID(Config::ETechnique::NtSetInformationThread, AntiDebug_NtSetInformationThread)
		ON_TECHNIQUE_ID(Config::ETechnique::SuspendThread, AntiDebug_SuspendThread)

	// Misc
		ON_TECHNIQUE_ID(Config::ETechnique::FindWindowA, AntiDebug_FindWindow)
		ON_TECHNIQUE_ID(Config::ETechnique::ParentProcessCheck_NtQueryInformationProcess, AntiDebug_ParentProcessCheck_NtQueryInformationProcess)
		ON_TECHNIQUE_ID(Config::ETechnique::ParentProcessCheck_CreateToolhelp32Snapshot, AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot)
		ON_TECHNIQUE_ID(Config::ETechnique::Selectors, AntiDebug_Selectors)
		ON_TECHNIQUE_ID(Config::ETechnique::DbgPrint, AntiDebug_DbgPrint)
		ON_TECHNIQUE_ID(Config::ETechnique::DbgSetDebugFilterState, AntiDebug_DbgSetDebugFilterState)
	END_TECHNIQUE_MAP()

	bool ParseConfig(const char *szConfig);
	void Execute();

private:
	void ParseNode(boost::property_tree::ptree &root, anti_debug_ptr &parent);
	anti_debug_ptr GetTechniqueByName(const std::string szName, const anti_debug_ptr &pParent = nullptr);
	
	ParamType ParseParamType(std::string &type);
	ParamValue ParseParamValue(std::string &value, ParamType type);
	
	std::list<std::string> GetNodeTags(std::pair<const std::string, boost::property_tree::ptree> &node);
	bool CheckTags(std::list<std::string> &nodeTags);
	
	void *GetCheckAddress(void *pInstance);
	
	void HandleException(std::exception_ptr pException);

	void ReadConfigFile(const char *szFilePath, boost::property_tree::ptree &root);

private:
	std::list<anti_debug_ptr> m_lstAntiDebugGroups;
	std::map<std::string, bool> m_mExecutionOptions;
	bool m_bShowInfo;
};

#endif // _DEBUGGER_FUGITIVE_H_
