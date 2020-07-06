#ifndef _PROCESS_HELPER_H_
#define _PROCESS_HELPER_H_

namespace process_helper
{

bool EnableDebugPrivilege(bool Enable);
bool Is64BitWindows();
bool IsSuspiciousProcessName(PCSTR pszProcessName);
DWORD GetParentProcessId(DWORD dwCurrentProcessId);

}

#endif // _PROCESS_HELPER_H_
