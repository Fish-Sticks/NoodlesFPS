#pragma once
#include <ntddk.h>

EXTERN_C_START

NTSTATUS NTAPI MmCopyVirtualMemory
(
    PEPROCESS SourceProcess,
    PVOID SourceAddress,
    PEPROCESS TargetProcess,
    PVOID TargetAddress,
    SIZE_T BufferSize,
    KPROCESSOR_MODE PreviousMode,
    PSIZE_T ReturnSize
);

PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);
NTSTATUS NTAPI PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS* process);

EXTERN_C_END