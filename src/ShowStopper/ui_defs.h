#ifndef _UI_DEFS_H_
#define _UI_DEFS_H_

#include <vector>
#include <string>
#include <windef.h>

#define WM_UPDATE_ADDRESS (WM_APP + 1)

enum class Group
{
	DebugFlags_WinApi,
	DebugFlags_Manual,
	Handles,
	Exceptions,
	Timing,
	ProcessMemory_Breakpoints,
	ProcessMemory_Other,
	CpuInstructions,
	Intercative,
	Misc,
};

enum ConfigMode
{
	Basic,

	AllInteractive,
	AllAntiTrace,
	AllAntiBreakpoint,
	AllAntiAttach,
	AllCrashCausing,
	AllAdminPrriv,

	Group_Flags_WinApi,
	Group_Flags_Manual,
	Group_Handles,
	Group_Exceptions,
	Group_Timing,
	Group_ProcMem_Bp,
	Group_ProcMem_Other,
	Group_Asm,
	Group_DbgInteraction,
	Group_Misc,
};

enum Columns
{
	CheckBox,
	TechniqueName,
	TechniqueAddress,
	TechniqueTags,
};

const wchar_t g_aColumnNames[4][MAX_PATH] = {
	L"",
	L"Technique Name",
	L"Check Virtual Address",
	L"Tags",
};

const std::vector<std::wstring> g_vGroups = {
	L"[Debug flags][Using Win32 API]",
	L"[Debug flags][Manual checks]",
	L"[Handles Validation]",
	L"[Exceptions]",
	L"[Timing]",
	L"[Process Memory Checks][Breakpoints]",
	L"[Process Memory Checks][Other]",
	L"[Checks with assembly instructions]",
	L"[Direct debugger interaction]",
	L"[Misc]",
};

const std::vector<std::wstring> g_vParamTypes = 
{
	L"dword", 
	L"qword", 
	L"real", 
	L"string",
};

struct Parameter
{
	std::wstring Name;
	std::wstring Type;
	std::wstring Value;	
};

#endif // _UI_DEFS_H_
