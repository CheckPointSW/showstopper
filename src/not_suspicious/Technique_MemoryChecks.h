#ifndef _TECHNIQUE_MEMORY_CHECKS_H_
#define _TECHNIQUE_MEMORY_CHECKS_H_

#include "Techniques_Def.h"

//
// Process memory checks : Breakpoints
//

class AntiDebug_INT3Scan : public Technique
{
public:
	AntiDebug_INT3Scan(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	bool CheckForSpecificByte(BYTE cByte, PVOID pMemory, SIZE_T nMemorySize = 0) const;
};

class AntiDebug_AntiStepOver : public Technique
{
public:
	AntiDebug_AntiStepOver(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_AntiStepOverReadFile : public Technique
{
public:
	AntiDebug_AntiStepOverReadFile(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_AntiStepOverWriteProcessMemory : public Technique
{
public:
	AntiDebug_AntiStepOverWriteProcessMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	~AntiDebug_AntiStepOverWriteProcessMemory();
	virtual bool Check() const;

private:
	PBYTE m_pbPatch;
};

class AntiDebug_MemoryBreakpoints : public Technique
{
public:
	AntiDebug_MemoryBreakpoints(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_HardwareBreakpoints : public Technique
{
public:
	AntiDebug_HardwareBreakpoints(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_Toolhelp32ReadProcessMemory : public Technique
{
public:
	AntiDebug_Toolhelp32ReadProcessMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

//
// Process memory checks : Other
//

class AntiDebug_NtQueryVirtualMemory : public Technique
{
public:
	AntiDebug_NtQueryVirtualMemory(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_FunctionPatch : public Technique
{
public:
	AntiDebug_FunctionPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_DbgBreakPointPatch : public Technique
{
public:
	AntiDebug_DbgBreakPointPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual void AddParameter(std::string &name, ParamValue &value);
	virtual bool Check() const;

private:
	DWORD m_dwAttachTimeout;
};

class AntiDebug_DbgUiRemoteBreakinPatch : public Technique
{
#pragma pack(push, 1)
	struct DbgUiRemoteBreakinPatch
	{
		WORD  push_0;
		BYTE  push;
		DWORD CurrentPorcessHandle;
		BYTE  mov_eax;
		DWORD TerminateProcess;
		WORD  call_eax;
	};
#pragma pack(pop)

public:
	AntiDebug_DbgUiRemoteBreakinPatch(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual void AddParameter(std::string &name, ParamValue &value);
	virtual bool Check() const;

private:
	DWORD m_dwAttachTimeout;
};

class AntiDebug_CodeChecksum : public Technique
{
public:
	AntiDebug_CodeChecksum(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	size_t DetectFunctionSize(PVOID pFunc);
	unsigned Reverse(unsigned x) const;
	unsigned int CRC32(unsigned char *message, int size) const;

	DWORD m_dwChecksum;
	PVOID m_pFuncAddr;
	size_t m_nFuncSize;
};

#endif // _TECHNIQUE_MEMORY_CHECKS_H_
