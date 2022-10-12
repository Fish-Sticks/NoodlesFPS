#pragma once
#include "../undocumented.hpp"

class memory_interface_t
{
private:
    PEPROCESS current_process = nullptr;
    PEPROCESS target_process = nullptr;
    HANDLE target_pid = 0;
    PVOID target_base = 0;
    SIZE_T return_size = 0;
public:
    memory_interface_t(HANDLE process_id) : target_pid{ process_id }
    {
        NTSTATUS result;

        this->current_process = PsGetCurrentProcess();
        result = PsLookupProcessByProcessId(target_pid, &this->target_process);

        if (!NT_SUCCESS(result))
        {
            DbgPrintEx(0, 0, "Failed to find EPROCESS struct for pid %llu\n", reinterpret_cast<DWORD_PTR>(process_id));
            return;
        }

        this->target_base = PsGetProcessSectionBaseAddress(this->target_process);

        DbgPrintEx(0, 0, "Successfully initialized memory interface!\n");
    }

    memory_interface_t(const memory_interface_t&) = delete;

    ~memory_interface_t()
    {
        ObDereferenceObject(this->target_process);
        
        DbgPrintEx(0, 0, "Memory interface safely deconstructed!\n");
    }

    template <typename T>
    NTSTATUS read_memory(DWORD_PTR address, T& output)
    {
        return MmCopyVirtualMemory(this->target_process, reinterpret_cast<PVOID>(address), PsGetCurrentProcess(), &output, sizeof(T), KernelMode, &this->return_size);
    }

    template <typename T>
    NTSTATUS write_memory(DWORD_PTR address, T& output)
    {
        return MmCopyVirtualMemory(this->target_process, reinterpret_cast<PVOID>(address), PsGetCurrentProcess(), &output, sizeof(T), KernelMode, &this->return_size);
    }

    template <typename T>
    NTSTATUS read_memory_from_base(DWORD_PTR offset, T& output)
    {
        return MmCopyVirtualMemory(this->target_process, reinterpret_cast<PVOID>(reinterpret_cast<DWORD_PTR>(this->target_base) + offset), PsGetCurrentProcess(), &output, sizeof(T), KernelMode, &this->return_size);
    }

    template <typename T>
    NTSTATUS write_memory_from_base(DWORD_PTR offset, T& input)
    {
        return MmCopyVirtualMemory(PsGetCurrentProcess(), &input, this->target_process, reinterpret_cast<PVOID>(reinterpret_cast<DWORD_PTR>(this->target_base) + offset), sizeof(T), KernelMode, &this->return_size);
    }

    SIZE_T get_last_return_size()
    {
        return this->return_size;
    }
};