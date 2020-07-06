#ifndef _TECHNIQUE_TIMIG_H_
#define _TECHNIQUE_TIMIG_H_

#include "Techniques_Def.h"

class AntiDebug_Timing : public Technique
{
public:
	AntiDebug_Timing(const std::string name, const std::shared_ptr<AntiDebug> &parent, DWORD dwDefaultElapsed);
	virtual void AddParameter(std::string &name, ParamValue &value);

protected:
	unsigned __int64 FiletimeToUint64(const FILETIME &fileTime) const;

	DWORD m_dwElapsed;
};

//
// Techniques' Implementation
//

class AntiDebug_RDTSC : public AntiDebug_Timing
{
public:
	AntiDebug_RDTSC(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;
};

class AntiDebug_GetLocalTime : public AntiDebug_Timing
{
public:
	AntiDebug_GetLocalTime(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	SYSTEMTIME m_stConstructorCallTime;
};

class AntiDebug_GetSystemTime : public AntiDebug_Timing
{
public:
	AntiDebug_GetSystemTime(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	SYSTEMTIME m_stConstructorCallTime;
};

class AntiDebug_GetTickCount : public AntiDebug_Timing
{
public:
	AntiDebug_GetTickCount(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	DWORD m_dwStart;
};

class AntiDebug_QueryPerformanceCounter : public AntiDebug_Timing
{
public:
	AntiDebug_QueryPerformanceCounter(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	LARGE_INTEGER m_liStart;
};

class AntiDebug_timeGetTime : public AntiDebug_Timing
{
public:
	AntiDebug_timeGetTime(const std::string name, const std::shared_ptr<AntiDebug> &parent);
	virtual bool Check() const;

private:
	DWORD m_dwStart;
};

#endif // _TECHNIQUE_TIMIG_H_
