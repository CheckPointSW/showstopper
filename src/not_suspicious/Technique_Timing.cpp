#include "Technique_Timing.h"

#pragma comment(lib, "winmm.lib")

AntiDebug_Timing::AntiDebug_Timing(const std::string name, const std::shared_ptr<AntiDebug> &parent, DWORD dwDefaultElapsed)
	: Technique(name, parent)
	, m_dwElapsed(dwDefaultElapsed)
{
}

void AntiDebug_Timing::AddParameter(std::string &name, ParamValue &value)
{
	if (name == "elapsed")
		m_dwElapsed = std::get<ParamType::Dword>(value);
}

unsigned __int64 AntiDebug_Timing::FiletimeToUint64(const FILETIME &fileTime) const
{
	ULARGE_INTEGER uiTime;
	uiTime.LowPart  = fileTime.dwLowDateTime;
	uiTime.HighPart = fileTime.dwHighDateTime;
	return uiTime.QuadPart;
}

//
// Techniques' Implementation
//

AntiDebug_RDTSC::AntiDebug_RDTSC(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x500)
{
}
bool AntiDebug_RDTSC::Check() const
{
	__asm
	{
		rdtsc
		xchg   esi, eax
		mov    edi, edx
		rdtsc
		sub    eax, esi
		sbb    edx, edi
		jne    rdtsc_being_debugged
		cmp    eax, m_dwElapsed
		jnbe   rdtsc_being_debugged
	}
	return false;
rdtsc_being_debugged:
	return true;
}


AntiDebug_GetLocalTime::AntiDebug_GetLocalTime(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x1000000)
{
	GetLocalTime(&m_stConstructorCallTime);
}
bool AntiDebug_GetLocalTime::Check() const
{
	SYSTEMTIME stCheckCallTime;
	GetLocalTime(&stCheckCallTime);

	FILETIME ftConstructorCallTime, ftCheckCallTime;
	if (!SystemTimeToFileTime(&m_stConstructorCallTime, &ftConstructorCallTime))
		return false;
	if (!SystemTimeToFileTime(&stCheckCallTime, &ftCheckCallTime))
		return false;

	return (FiletimeToUint64(ftCheckCallTime) - FiletimeToUint64(ftConstructorCallTime)) > m_dwElapsed;
}


AntiDebug_GetSystemTime::AntiDebug_GetSystemTime(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x1000000)
{
	GetSystemTime(&m_stConstructorCallTime);
}
bool AntiDebug_GetSystemTime::Check() const
{
	SYSTEMTIME stCheckCallTime;
	GetSystemTime(&stCheckCallTime);

	FILETIME ftConstructorCallTime, ftCheckCallTime;
	if (!SystemTimeToFileTime(&m_stConstructorCallTime, &ftConstructorCallTime))
		return false;
	if (!SystemTimeToFileTime(&stCheckCallTime, &ftCheckCallTime))
		return false;

	return (FiletimeToUint64(ftCheckCallTime) - FiletimeToUint64(ftConstructorCallTime)) > m_dwElapsed;
}


AntiDebug_GetTickCount::AntiDebug_GetTickCount(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x10)
	, m_dwStart(GetTickCount())
{}
bool AntiDebug_GetTickCount::Check() const
{
	return (GetTickCount() - m_dwStart) > m_dwElapsed;
}


AntiDebug_QueryPerformanceCounter::AntiDebug_QueryPerformanceCounter(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x1000000)
{
	QueryPerformanceCounter(&m_liStart);
}
bool AntiDebug_QueryPerformanceCounter::Check() const
{
	LARGE_INTEGER liEnd;
	QueryPerformanceCounter(&liEnd);
	return (liEnd.QuadPart - m_liStart.QuadPart) > m_dwElapsed;
}


AntiDebug_timeGetTime::AntiDebug_timeGetTime(const std::string name, const std::shared_ptr<AntiDebug> &parent)
	: AntiDebug_Timing(name, parent, 0x1000000)
	, m_dwStart(timeGetTime())
{
}
bool AntiDebug_timeGetTime::Check() const
{
	DWORD dwEnd = timeGetTime();
	return (dwEnd - m_dwStart) > m_dwElapsed;
}
