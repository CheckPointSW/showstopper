#ifndef _TECHNIQUES_DEF_H_
#define _TECHNIQUES_DEF_H_

#include "Utils.h"
#include <Windows.h>
#include <winternl.h>
#include "Technique.h"
#include "NtDll.h"
#include "globals.h"

#define THROW_DIVIDE_BY_ZERO_EXCEPTION __asm \
{                       \
   __asm xor eax, eax   \
   __asm div eax        \
}

#endif // _TECHNIQUES_DEF_H_
