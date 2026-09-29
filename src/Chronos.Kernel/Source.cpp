#include <wdm.h>
#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

VOID ChronosDriverUnload(PDRIVER_OBJECT DriverObject);

NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    /* Set the unload routine */
    DriverObject->DriverUnload = ChronosDriverUnload;

    /* Perform any necessary initialization here */
    return STATUS_SUCCESS;
}

VOID
ChronosDriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    /* Perform any necessary cleanup here */
}

#ifdef __cplusplus
}
#endif
