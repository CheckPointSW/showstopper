#ifndef _TECHNIQUE_ASSEMBLER_H_
#define _TECHNIQUE_ASSEMBLER_H_

#include "Techniques_Def.h"

DECLARE_TECHNIQUE(INT3);

class AntiDebug_INT3Long : public Technique
{
public:
	AntiDebug_INT3Long(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

	static int INT3LongExceptionFilter(UINT code, PEXCEPTION_POINTERS ep);
	static bool m_bDebugged;
};


DECLARE_TECHNIQUE(INT2D);
DECLARE_TECHNIQUE(ICE);
DECLARE_TECHNIQUE(StackSegmentRegister);


class AntiDebug_InstructionCounting : public Technique
{
public:
	AntiDebug_InstructionCounting(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	const size_t m_nInstructionCount = 4;
	mutable PVOID m_pThreadAddr;
	mutable HANDLE m_hHwBps[4];
};


DECLARE_TECHNIQUE(PopfAndTrapFlag);
DECLARE_TECHNIQUE(InstructionPrefixes);
DECLARE_TECHNIQUE(DebugRegistersModification);

#endif // _TECHNIQUE_ASSEMBLER_H_
