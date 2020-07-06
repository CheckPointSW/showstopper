#ifndef _TECHNIQUE_DIRECT_DBG_INTERACTION_H_
#define _TECHNIQUE_DIRECT_DBG_INTERACTION_H_

#include "Techniques_Def.h"
#include <atomic>

class AntiDebug_SelfDebugging : public Technique
{
public:
	AntiDebug_SelfDebugging(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	~AntiDebug_SelfDebugging();
	virtual bool Check() const;

private:
	mutable HANDLE m_hDbgEvent;
};

class AntiDebug_GenerateConsoleCtrlEvent : public Technique
{
public:
	AntiDebug_GenerateConsoleCtrlEvent(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

	static bool m_bDebugged;
	static std::atomic<bool> m_bCtlCCatched;
};

class AntiDebug_BlockInput : public Technique
{
public:
	AntiDebug_BlockInput(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_NtSetInformationThread : public Technique
{
public:
	AntiDebug_NtSetInformationThread(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_SuspendThread : public Technique
{
public:
	AntiDebug_SuspendThread(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

	bool IsDebuggerProcess(DWORD dwProcessId) const;
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	static DWORD m_dwDebuggerProcessId;
};

#endif // _TECHNIQUE_DIRECT_DBG_INTERACTION_H_
