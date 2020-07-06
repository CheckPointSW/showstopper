#include "Technique_MemoryChecks.h"
#include <TlHelp32.h>
#include <intrin.h>

#pragma intrinsic(_ReturnAddress)


//
// Process memory checks : Breakpoints
//

AntiDebug_INT3Scan::AntiDebug_INT3Scan(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_INT3Scan::Check() const
{
	return CheckForSpecificByte(0xCC, _ReturnAddress(), 1);
}
bool AntiDebug_INT3Scan::CheckForSpecificByte(BYTE cByte, PVOID pMemory, SIZE_T nMemorySize) const
{
	PBYTE pBytes = (PBYTE)pMemory; 
	for (SIZE_T i = 0; ; i++)
	{
		if (((nMemorySize > 0) && (i >= nMemorySize)) ||
			((nMemorySize == 0) && (pBytes[i] == 0xC3)))
			break;

		if (pBytes[i] == cByte)
		{
			BYTE prev = pBytes[i - 1];
			BYTE next = pBytes[i + 1];
			if ((pBytes[i-1] != cByte) && (pBytes[i+1] != cByte))
				return true;
		}
	}
	return false;
}


AntiDebug_AntiStepOver::AntiDebug_AntiStepOver(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_AntiStepOver::Check() const
{
	PVOID pRetAddress = _ReturnAddress();
	bool bBpFound = *(PBYTE)pRetAddress == 0xCC;
	if (bBpFound)
	{
		DWORD dwOldProtect;
		if (VirtualProtect(pRetAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		{
			*(PBYTE)pRetAddress = 0x90;
			VirtualProtect(pRetAddress, 1, dwOldProtect, &dwOldProtect);
		}
	}
	return bBpFound;
}


AntiDebug_AntiStepOverReadFile::AntiDebug_AntiStepOverReadFile(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_AntiStepOverReadFile::Check() const
{
	PVOID pRetAddress = _ReturnAddress();
	bool bBpFound = *(PBYTE)pRetAddress == 0xCC;
	if (bBpFound)
	{
		DWORD dwOldProtect, dwRead;
		CHAR szFilePath[MAX_PATH];
		HANDLE hFile;

		if (VirtualProtect(pRetAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		{
			if (GetModuleFileNameA(NULL, szFilePath, MAX_PATH))
			{
				hFile = CreateFileA(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (INVALID_HANDLE_VALUE != hFile)
					ReadFile(hFile, pRetAddress, 1, &dwRead, NULL);
			}
			VirtualProtect(pRetAddress, 1, dwOldProtect, &dwOldProtect);
		}
	}
	return bBpFound;
}


AntiDebug_AntiStepOverWriteProcessMemory::AntiDebug_AntiStepOverWriteProcessMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
	, m_pbPatch(nullptr)
{
	m_pbPatch = (PBYTE)calloc(1, sizeof(BYTE));
	if (m_pbPatch)
		*m_pbPatch = 0x90; // nop
}
AntiDebug_AntiStepOverWriteProcessMemory::~AntiDebug_AntiStepOverWriteProcessMemory()
{
	if (m_pbPatch)
		free(m_pbPatch);
}
bool AntiDebug_AntiStepOverWriteProcessMemory::Check() const
{
	PVOID pRetAddress = _ReturnAddress();
	bool bBpFound = *(PBYTE)pRetAddress == 0xCC;
	if (bBpFound)
	{
		DWORD dwOldProtect;
		if (VirtualProtect(pRetAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		{
			WriteProcessMemory(GetCurrentProcess(), pRetAddress, m_pbPatch, 1, NULL);
			VirtualProtect(pRetAddress, 1, dwOldProtect, &dwOldProtect);
		}
	}
	return bBpFound;
}


AntiDebug_MemoryBreakpoints::AntiDebug_MemoryBreakpoints(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_MemoryBreakpoints::Check() const
{
	DWORD dwOldProtect = 0;
	SYSTEM_INFO SysInfo = { 0 };

	GetSystemInfo(&SysInfo);
	PVOID pPage = VirtualAlloc(NULL, SysInfo.dwPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if (NULL == pPage)
		return false; 

	PBYTE pMem = (PBYTE)pPage;
	*pMem = 0xC3; 

	if (!VirtualProtect(pPage, SysInfo.dwPageSize, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOldProtect))
		return false;

	__try
	{
		__asm
		{
			mov eax, pPage
			push mem_bp_being_debugged
			jmp eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		VirtualFree(pPage, NULL, MEM_RELEASE);
		return false;
	}

mem_bp_being_debugged:
	VirtualFree(pPage, NULL, MEM_RELEASE);
	return true;
}


AntiDebug_HardwareBreakpoints::AntiDebug_HardwareBreakpoints(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{}
bool AntiDebug_HardwareBreakpoints::Check() const
{
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT)); 
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS; 

	if(!GetThreadContext(GetCurrentThread(), &ctx))
		return false;

	return ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3;
}


AntiDebug_Toolhelp32ReadProcessMemory::AntiDebug_Toolhelp32ReadProcessMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{}
bool AntiDebug_Toolhelp32ReadProcessMemory::Check() const
{
	PVOID pRetAddress = _ReturnAddress();
	BYTE uByte;
	if (FALSE == Toolhelp32ReadProcessMemory(GetCurrentProcessId(), _ReturnAddress(), &uByte, sizeof(BYTE), NULL))
		return false;
	return (uByte == 0xCC);
}


//
// Process memory checks : Other
//

// https://www.virusbulletin.com/virusbulletin/2012/12/journey-sirefef-packer-research-case-study
// http://waliedassar1.rssing.com/chan-33272685/all_p2.html
// https://www.dropbox.com/s/7a4231rzn57zkh2/xAntix.cpp
AntiDebug_NtQueryVirtualMemory::AntiDebug_NtQueryVirtualMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_NtQueryVirtualMemory::Check() const
{
#ifndef _WIN64
	NTSTATUS status;
	PBYTE pMem = nullptr;
	DWORD dwMemSize = 0;

	do
	{
		dwMemSize += 0x1000;
		pMem = (PBYTE)_malloca(dwMemSize);
		if (!pMem)
			return false;

		memset(pMem, 0, dwMemSize);
		status = ntdll::NtQueryVirtualMemory(
			GetCurrentProcess(), 
			NULL, 
			ntdll::MemoryWorkingSetList, 
			pMem, 
			dwMemSize, 
			NULL);
	} while (status == STATUS_INFO_LENGTH_MISMATCH);

	ntdll::PMEMORY_WORKING_SET_LIST pWorkingSet = (ntdll::PMEMORY_WORKING_SET_LIST)pMem;
	for (ULONG i = 0; i < pWorkingSet->NumberOfPages; i++)
	{
		DWORD dwAddr = pWorkingSet->WorkingSetList[i].VirtualPage << 0x0C;
		DWORD dwEIP = 0;
		__asm
		{
			push eax
			call $+5
			pop eax
			mov dwEIP, eax
			pop eax
		}

		if (dwAddr == (dwEIP & 0xFFFFF000))
			return (pWorkingSet->WorkingSetList[i].Shared == 0) || (pWorkingSet->WorkingSetList[i].ShareCount == 0);
	}
#endif // _WIN64
	return false;
}


// https://habr.com/en/post/178183/
AntiDebug_FunctionPatch::AntiDebug_FunctionPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
}
bool AntiDebug_FunctionPatch::Check() const
{
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	if (!hKernel32)
		return false;

	FARPROC pIsDebuggerPresent = GetProcAddress(hKernel32, "IsDebuggerPresent");
	if (!pIsDebuggerPresent)
		return false;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return false;

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hSnapshot, &ProcessEntry))
		return false;

	bool bDebuggerPresent = false;
	HANDLE hProcess = NULL;
	DWORD dwFuncBytes = 0;
	const DWORD dwCurrentPID = GetCurrentProcessId();
	do
	{
		__try
		{
			if (dwCurrentPID == ProcessEntry.th32ProcessID)
				continue;

			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);
			if (NULL == hProcess)
				continue;

			if (!ReadProcessMemory(hProcess, pIsDebuggerPresent, &dwFuncBytes, sizeof(DWORD), NULL))
				continue;

			if (dwFuncBytes != *(PDWORD)pIsDebuggerPresent)
			{
				bDebuggerPresent = true;
				break;
			}
		}
		__finally
		{
			if (hProcess)
				CloseHandle(hProcess);
		}
	} while (Process32NextW(hSnapshot, &ProcessEntry));

	if (hSnapshot)
		CloseHandle(hSnapshot);
	return bDebuggerPresent;
}


AntiDebug_DbgBreakPointPatch::AntiDebug_DbgBreakPointPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
	, m_dwAttachTimeout(0)
{
}
void AntiDebug_DbgBreakPointPatch::AddParameter(std::string &name, ParamValue &value)
{
	if (name == "attach_tmeout")
		m_dwAttachTimeout = std::get<ParamType::Dword>(value);
}
bool AntiDebug_DbgBreakPointPatch::Check() const
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	if (!hNtdll)
		return false;

	FARPROC pDbgBreakPoint = GetProcAddress(hNtdll, "DbgBreakPoint");
	if (!pDbgBreakPoint)
		return false;

	DWORD dwOldProtect;
	if (!VirtualProtect(pDbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		return false;

	*(PBYTE)pDbgBreakPoint = (BYTE)0xC3; // ret
	Sleep(m_dwAttachTimeout);
	return false;
}


// https://habr.com/en/post/178183/
AntiDebug_DbgUiRemoteBreakinPatch::AntiDebug_DbgUiRemoteBreakinPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
	, m_dwAttachTimeout(0)
{
}
void AntiDebug_DbgUiRemoteBreakinPatch::AddParameter(std::string &name, ParamValue &value)
{
	if (name == "attach_tmeout")
		m_dwAttachTimeout = std::get<ParamType::Dword>(value);
}
bool AntiDebug_DbgUiRemoteBreakinPatch::Check() const
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	if (!hNtdll)
		return false;

	FARPROC pDbgUiRemoteBreakin = GetProcAddress(hNtdll, "DbgUiRemoteBreakin");
	if (!pDbgUiRemoteBreakin)
		return false;

	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	if (!hKernel32)
		return false;

	FARPROC pTerminateProcess = GetProcAddress(hKernel32, "TerminateProcess");
	if (!pTerminateProcess)
		return false;

	DbgUiRemoteBreakinPatch patch = { 0 };
	patch.push_0 = '\x6A\x00';
	patch.push = '\x68';
	patch.CurrentPorcessHandle = 0xFFFFFFFF;
	patch.mov_eax = '\xB8';
	patch.TerminateProcess = (DWORD)pTerminateProcess;
	patch.call_eax = '\xFF\xD0';

	DWORD dwOldProtect;
	if (!VirtualProtect(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch), PAGE_READWRITE, &dwOldProtect))
		return false;

	::memcpy_s(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch),
		&patch, sizeof(DbgUiRemoteBreakinPatch));
	VirtualProtect(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch), dwOldProtect, &dwOldProtect);
	Sleep(m_dwAttachTimeout);
	return false;
}


static __declspec(naked) int CodeChecksumTest()
{
	__asm
	{
		push edx
		mov edx, 0 
		mov eax, 10
		mov ecx, 2
		div ecx
		pop edx
		ret
	}
}
AntiDebug_CodeChecksum::AntiDebug_CodeChecksum(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: Technique(name, parent)
{
	CodeChecksumTest();
	m_pFuncAddr = &CodeChecksumTest;
	m_nFuncSize = DetectFunctionSize(m_pFuncAddr);
	m_dwChecksum = CRC32((PBYTE)m_pFuncAddr, m_nFuncSize);
	DWORD Checksum = CRC32((PBYTE)m_pFuncAddr, m_nFuncSize);
}
bool AntiDebug_CodeChecksum::Check() const
{
	return CRC32((PBYTE)m_pFuncAddr, m_nFuncSize) != m_dwChecksum;
}
size_t AntiDebug_CodeChecksum::DetectFunctionSize(PVOID pFunc)
{
	PBYTE pMem = (PBYTE)pFunc;
	size_t nFuncSize = 0;
	do
	{
		++nFuncSize;
	} while (*(pMem++) != 0xC3);
	return nFuncSize;
}
unsigned AntiDebug_CodeChecksum::Reverse(unsigned x) const
{
	x = ((x & 0x55555555) <<  1) | ((x >>  1) & 0x55555555);
	x = ((x & 0x33333333) <<  2) | ((x >>  2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) <<  4) | ((x >>  4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) |
		((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}
unsigned int AntiDebug_CodeChecksum::CRC32(unsigned char *message, int size) const
{
	unsigned int byte, crc = 0xFFFFFFFF;
	for (int i = 0; i < size; i++)
	{
		byte = message[i];
		byte = Reverse(byte);
		for (int j = 0; j <= 7; j++)
		{
			if ((int)(crc ^ byte) < 0)
				crc = (crc << 1) ^ 0x04C11DB7;
			else
				crc = crc << 1;
			byte = byte << 1;
		}
	}
	return Reverse(~crc);
}
