#include <ntddk.h>
#include "memory/memory.hpp"

#pragma warning(disable : 4100)

const char* get_end_file_path(PANSI_STRING file_path)
{
    SIZE_T string_end = file_path->Length - 1;
    SIZE_T string_trunc_start = 0;

    for (SIZE_T curr = string_end; curr > 0; --curr)
    {
        if (file_path->Buffer[curr] == '\\')
        {
            string_trunc_start = curr + 1;
            break;
        }
    }

    if (string_trunc_start != 0)
    {
        const char* str = reinterpret_cast<const char*>(file_path->Buffer + string_trunc_start);
        return str;
    }

    return nullptr;
}

static HANDLE target_process_id = 0;


// Note: You will BSOD if you run calculator without running the app first lmao.
void image_load_notify_routine(PUNICODE_STRING full_image_name, HANDLE process_id, PIMAGE_INFO image_info)
{
    // Can be NULL sometimes "(The FullImageName parameter can be NULL in cases in which the operating system is unable to obtain the full name of the image at process creation time.)"
    if (!full_image_name)
        return;

    ANSI_STRING process_name;
    RtlUnicodeStringToAnsiString(&process_name, full_image_name, TRUE);

    const char* file_name = get_end_file_path(&process_name);

    if (file_name != nullptr)
    {
        if (!strcmp(file_name, "RobloxPlayerBeta.exe"))
        {
            // Only capture first Roblox
            if (!target_process_id)
            {
                DbgPrintEx(0, 0, "Successfully found RobloxPlayerBeta.exe!\n");
                DbgPrintEx(0, 0, "Process created: %s\n", process_name.Buffer);
                DbgPrintEx(0, 0, "Process ID: %llu\n", reinterpret_cast<DWORD64>(process_id));
                DbgPrintEx(0, 0, "Process base: 0x%p\n", image_info->ImageBase);

                target_process_id = process_id;
            }
        }
        else if (!strcmp(file_name, "Calculator.exe"))
        {
            if (target_process_id)
            {
                DbgPrintEx(0, 0, "Calculator located! Running code.\n");
                
                DWORD_PTR new_fps = 0x3f8111109e88c1b0; // equation: (1 / FPS) use a hex convertor to conv ur double to hex

                memory_interface_t memory_interface{ target_process_id };

                DWORD32 task_scheduler = 0x0;
                memory_interface.read_memory_from_base<DWORD32>(0x39C2D6C, task_scheduler);

                DWORD_PTR fps_ptr = static_cast<DWORD_PTR>(task_scheduler) + 0x118;


                // The memory lib I wrote doesn't work for wtv reason on this part, so I manually hardcode it :skull:
                {
                    SIZE_T return_value = 0;
                    PEPROCESS roblox;
                    PsLookupProcessByProcessId(target_process_id, &roblox);
                    MmCopyVirtualMemory(PsGetCurrentProcess(), &new_fps, roblox, reinterpret_cast<PVOID>(fps_ptr), 8, KernelMode, &return_value);

                    ObDereferenceObject(roblox);
                }

                target_process_id = 0;
            }
        }
    }

    RtlFreeAnsiString(&process_name);
}


EXTERN_C_START

VOID DriverUnload(
    _In_ struct _DRIVER_OBJECT* DriverObject
)
{
    // Won't actually work "If the driver's callback routine is currently running, PsRemoveLoadImageNotifyRoutine waits until the callback routine exits before removing it. Therefore, the callback routine itself must not call PsRemoveLoadImageNotifyRoutine."
    // I don't care that much though, leave it as an exercise for you!
    NTSTATUS status = PsRemoveLoadImageNotifyRoutine(&image_load_notify_routine);

    if (NT_SUCCESS(status))
        DbgPrintEx(0, 0, "Successfully unloaded NoodlesFPS!\n");
    else
        DbgPrintEx(0, 0, "Failed to unload NoodlesFPS!\n");
}

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    DriverObject->DriverUnload = &DriverUnload;

    NTSTATUS status = PsSetLoadImageNotifyRoutine(&image_load_notify_routine);
    if (NT_SUCCESS(status))
    {
        DbgPrintEx(0, 0, "Successfully deployed NoodlesFPS!\n");
        return STATUS_SUCCESS;
    }
    else
    {
        DbgPrintEx(0, 0, "Failed to deploy NoodlesFPS! It WILL not work from this point on.\n");
        return STATUS_UNSUCCESSFUL;
    }
}

EXTERN_C_END