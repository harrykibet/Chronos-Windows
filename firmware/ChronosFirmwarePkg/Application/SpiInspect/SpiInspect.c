#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>

#include "../../Library/SpiFlash/SpiFlash.h"

STATIC
VOID
PrintHexId (
  IN CONST CHRONOS_SPI_FLASH *Flash
  )
{
  Print (
    L"SPI NOR ID: %02x %02x %02x\n",
    Flash->ManufacturerId,
    Flash->MemoryType,
    Flash->MemoryCapacity
    );
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  CHRONOS_SPI_FLASH Flash;
  UINT8 Buffer[64];

  (VOID)ImageHandle;
  (VOID)SystemTable;

  Status = ChronosSpiFlashOpen (&Flash);
  if (EFI_ERROR (Status)) {
    Print (
      L"Chronos: EFI_SPI_NOR_FLASH_PROTOCOL unavailable: %r\n",
      Status
      );
    return Status;
  }

  Print (L"Chronos SPI NOR device discovered.\n");
  Print (L"Flash size: %u bytes (0x%x)\n", Flash.FlashSize, Flash.FlashSize);
  Print (L"Erase block: %u bytes\n", Flash.EraseBlockSize);
  PrintHexId (&Flash);

  Status = ChronosSpiFlashRead (&Flash, 0, sizeof (Buffer), Buffer);
  if (EFI_ERROR (Status)) {
    Print (L"Chronos: SPI read failed: %r\n", Status);
    return Status;
  }

  Print (L"First 64 bytes:\n");

  for (UINTN Offset = 0; Offset < sizeof (Buffer); Offset += 16) {
    Print (
      L"%04x: %02x %02x %02x %02x %02x %02x %02x %02x "
      L"%02x %02x %02x %02x %02x %02x %02x %02x\n",
      Offset,
      Buffer[Offset + 0],
      Buffer[Offset + 1],
      Buffer[Offset + 2],
      Buffer[Offset + 3],
      Buffer[Offset + 4],
      Buffer[Offset + 5],
      Buffer[Offset + 6],
      Buffer[Offset + 7],
      Buffer[Offset + 8],
      Buffer[Offset + 9],
      Buffer[Offset + 10],
      Buffer[Offset + 11],
      Buffer[Offset + 12],
      Buffer[Offset + 13],
      Buffer[Offset + 14],
      Buffer[Offset + 15]
      );
  }

  /*
   * Physical writes are intentionally not performed.
   * See ChronosSpiFlashWrite().
   */
  return EFI_SUCCESS;
}
