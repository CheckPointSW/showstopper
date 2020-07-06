#ifndef _NTDLL_H_
#define _NTDLL_H_

namespace ntdll
{

//
// Macro
//

#define NtCurrentThread ((HANDLE)-2)

#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

#define PDI_MODULES                       0x01
#define PDI_BACKTRACE                     0x02
#define PDI_HEAPS                         0x04
#define PDI_HEAP_TAGS                     0x08
#define PDI_HEAP_BLOCKS                   0x10
#define PDI_LOCKS                         0x20

//
// Types
//

typedef enum _PROCESS_INFORMATION_CLASS {
	ProcessBasicInformation = 0,
	ProcessDebugPort = 7,
	ProcessLdtInformation = 10,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27,
	ProcessBreakOnTermination = 29,
	ProcessDebugObjectHandle = 30,
	ProcessDebugFlags = 31,
} PROCESS_INFORMATION_CLASS;

typedef enum _THREAD_INFORMATION_CLASS {
	ThreadMemoryPriority = 0,
	ThreadAbsoluteCpuPriority = 1,
	ThreadDynamicCodePolicy = 2,
	ThreadPowerThrottling = 3,
	ThreadInformationClassMax = 4,
	ThreadHideFromDebugger = 17,
} THREAD_INFORMATION_CLASS;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemKernelDebuggerInformation = 35,
} SYSTEM_INFORMATION_CLASS;

typedef enum _OBJECT_INFORMATION_CLASS
{
	ObjectAllTypesInformation = 3,
} OBJECT_INFORMATION_CLASS;

typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation,
	MemoryWorkingSetList,
} MEMORY_INFORMATION_CLASS;

typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct _RTL_HEAP_TAG 
{ 
	ULONG NumberOfAllocations; 
	ULONG NumberOfFrees; 
	ULONG BytesAllocated; 
	USHORT TagIndex; 
	USHORT CreatorBackTraceIndex; 
	WCHAR TagName[24]; 
} RTL_HEAP_TAG, *PRTL_HEAP_TAG; 

typedef struct _RTL_HEAP_ENTRY 
{ 
	ULONG Size; 
	USHORT Flags; 
	USHORT AllocatorBackTraceIndex; 
	union 
	{ 
		struct 
		{ 
			ULONG Settable; 
			ULONG Tag; 
		} s1; 
		struct 
		{ 
			ULONG CommittedSize; 
			PVOID FirstBlock; 
		} s2; 
	} u; 
} RTL_HEAP_ENTRY, *PRTL_HEAP_ENTRY;

typedef struct _RTL_HEAP_INFORMATION 
{ 
	PVOID BaseAddress; 
	ULONG Flags; 
	USHORT EntryOverhead; 
	USHORT CreatorBackTraceIndex; 
	ULONG BytesAllocated; 
	ULONG BytesCommitted; 
	ULONG NumberOfTags; 
	ULONG NumberOfEntries; 
	ULONG NumberOfPseudoTags; 
	ULONG PseudoTagGranularity; 
	ULONG Reserved[5]; 
	PRTL_HEAP_TAG Tags; 
	PRTL_HEAP_ENTRY Entries; 
} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

typedef struct _RTL_PROCESS_HEAPS 
{ 
	ULONG NumberOfHeaps; 
	RTL_HEAP_INFORMATION Heaps[1]; 
} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS; 

typedef struct _DEBUG_BUFFER {
	HANDLE  SectionHandle;
	PVOID  SectionBase;
	PVOID  RemoteSectionBase;
	ULONG  SectionBaseDelta;
	HANDLE  EventPairHandle;
	ULONG  Unknown[2];
	HANDLE  RemoteThreadHandle;
	ULONG  InfoClassMask;
	ULONG  SizeOfInfo;
	ULONG  AllocatedSize;
	ULONG  SectionSize;
	PVOID  ModuleInformation;
	PVOID  BackTraceInformation;
	PVOID  HeapInformation;
	PVOID  LockInformation;
	PVOID  Reserved[8];
} DEBUG_BUFFER, *PDEBUG_BUFFER;

typedef struct _RTL_DEBUG_INFORMATION 
{ 
	HANDLE SectionHandleClient; 
	PVOID ViewBaseClient; 
	PVOID ViewBaseTarget; 
	ULONG ViewBaseDelta; 
	HANDLE EventPairClient; 
	HANDLE EventPairTarget; 
	HANDLE TargetProcessId; 
	HANDLE TargetThreadHandle; 
	ULONG Flags; 
	ULONG OffsetFree; 
	ULONG CommitSize; 
	ULONG ViewSize; 
	struct _RTL_PROCESS_MODULES *Modules; 
	struct _RTL_PROCESS_BACKTRACES *BackTraces; 
	struct _RTL_PROCESS_HEAPS *Heaps; 
	struct _RTL_PROCESS_LOCKS *Locks; 
	PVOID SpecificHeap; 
	HANDLE TargetProcessHandle; 
	PVOID Reserved[6]; 
} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION { 
	BOOLEAN DebuggerEnabled; 
	BOOLEAN DebuggerNotPresent; 
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION
{
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfHandles;
	ULONG TotalNumberOfObjects;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_INFORMATION
{
	ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
} OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;

typedef union _PSAPI_WORKING_SET_BLOCK {
	ULONG Flags;
	struct {
		ULONG Protection :5;
		ULONG ShareCount :3;
		ULONG Shared     :1;
		ULONG Reserved   :3;
		ULONG VirtualPage:20;
	};
} PSAPI_WORKING_SET_BLOCK, *PPSAPI_WORKING_SET_BLOCK;

typedef struct _MEMORY_WORKING_SET_LIST
{
	ULONG NumberOfPages;
	PSAPI_WORKING_SET_BLOCK WorkingSetList[1];
} MEMORY_WORKING_SET_LIST, *PMEMORY_WORKING_SET_LIST;

typedef struct _PROCESS_LDT_INFORMATION
{
	ULONG Start;
	ULONG Length;
	LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

typedef struct _RTL_UNKNOWN_FLS_DATA {
	PVOID unk1;
	PVOID unk2;
	PVOID unk3;
	PVOID Argument;
} RTL_UNKNOWN_FLS_DATA, *PRTL_UNKNOWN_FLS_DATA;

//
// Functions
//

NTSTATUS WINAPI NtQueryInformationProcess(
	IN HANDLE                    ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	OUT PVOID                    ProcessInformation,
	IN ULONG                     ProcessInformationLength,
	OUT PULONG                   ReturnLength
	);

NTSTATUS WINAPI NtSetInformationProcess(
	IN HANDLE                    ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	IN PVOID                     ProcessInformation,
	IN ULONG                     ProcessInformationLength
);

PDEBUG_BUFFER WINAPI RtlCreateQueryDebugBuffer(
	ULONG  Size,
	BOOLEAN  EventPair
	);

NTSTATUS WINAPI RtlQueryProcessHeapInformation( 
	PRTL_DEBUG_INFORMATION Buffer 
	);

NTSTATUS WINAPI RtlQueryProcessDebugInformation(
	ULONG  ProcessId,
	ULONG  DebugInfoClassMask,
	PDEBUG_BUFFER  DebugBuffer
	);

NTSTATUS WINAPI NtQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength
	);

NTSTATUS WINAPI NtSetInformationThread(
	HANDLE          ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID           ThreadInformation,
	ULONG           ThreadInformationLength
	);

NTSTATUS WINAPI NtQueryInformationThread(
	HANDLE                   ThreadHandle,
	THREAD_INFORMATION_CLASS ThreadInformationClass,
	PVOID                    ThreadInformation,
	ULONG                    ThreadInformationLength,
	PULONG                   ReturnLength
	);

DWORD WINAPI CsrGetProcessId(VOID);

NTSTATUS WINAPI NtQueryObject(
	HANDLE                   Handle,
	OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID                    ObjectInformation,
	ULONG                    ObjectInformationLength,
	PULONG                   ReturnLength
	);	

NTSTATUS WINAPI NtQueryVirtualMemory(
	HANDLE                   ProcessHandle,
	PVOID                    BaseAddress,
	MEMORY_INFORMATION_CLASS MemoryInformationClass,
	PVOID                    MemoryInformation,
	SIZE_T                   MemoryInformationLength,
	PSIZE_T                  ReturnLength
	);

NTSTATUS WINAPI RtlProcessFlsData(
	PRTL_UNKNOWN_FLS_DATA Buffer
	);

NTSTATUS WINAPI NtSetDebugFilterState(
	ULONG ComponentId, 
	ULONG Level, 
	BOOLEAN State
	);

ULONG WINAPI DbgPrint(
	PCSTR Format,
	...   
	);

}

#endif // _NTDLL_H_
