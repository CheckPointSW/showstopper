#ifndef _TECHNIQUE_DEBUG_FLAGS_H_
#define _TECHNIQUE_DEBUG_FLAGS_H_

#include "Techniques_Def.h"

//
// Using Win32 API
//

DECLARE_TECHNIQUE(IsDebuggerPresent);
DECLARE_TECHNIQUE(CheckRemoteDebuggerPresent);
DECLARE_TECHNIQUE(ProcessDebugPort);
DECLARE_TECHNIQUE(ProcessDebugFlags);
DECLARE_TECHNIQUE(ProcessDebugObjectHandle);
DECLARE_TECHNIQUE(RtlQueryProcessHeapInformation);
DECLARE_TECHNIQUE(RtlQueryProcessDebugInformation);
DECLARE_TECHNIQUE(SystemKernelDebuggerInformation);

//
// Manual checks
//

DECLARE_TECHNIQUE(BeingDebugged);
DECLARE_TECHNIQUE(NtGlobalFlag);
DECLARE_TECHNIQUE(HeapFlags);
DECLARE_TECHNIQUE(HeapProtection);

#endif // _TECHNIQUE_DEBUG_FLAGS_H_

