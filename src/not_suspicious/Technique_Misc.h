#ifndef _TECHNIQUE_MISC_H_
#define _TECHNIQUE_MISC_H_

#include "Techniques_Def.h"
#include <vector>

class AntiDebug_FindWindow : public Technique
{
public:
	AntiDebug_FindWindow(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	const std::vector<std::string> m_vWindowClasses = {
		"OLLYDBG",
		"WinDbgFrameClass", // WinDbg
		"ID",               // Immunity Debugger
		"Zeta Debugger",
		"Rock Debugger",
		"ObsidianGUI",
	};
};

class AntiDebug_ParentProcessCheck_NtQueryInformationProcess : public Technique
{
public:
	AntiDebug_ParentProcessCheck_NtQueryInformationProcess(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot : public Technique
{
public:
	AntiDebug_ParentProcessCheck_CreateToolhelp32Snapshot(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_Selectors : public Technique
{
public:
	AntiDebug_Selectors(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_DbgPrint: public Technique
{
public:
	AntiDebug_DbgPrint(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_DbgSetDebugFilterState : public Technique
{
public:
	AntiDebug_DbgSetDebugFilterState(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

#endif // _TECHNIQUE_MISC_H_
