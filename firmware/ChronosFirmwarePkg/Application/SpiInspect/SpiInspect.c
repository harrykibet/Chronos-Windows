#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>

#include "../../Library/SpiFlash/SpiFlash.h"
#include "../../Library/FlashDescriptor/IntelFlashDescriptor.h"

STATIC
VOID
PrintDescriptor (
  IN CONST CHRONOS_INTEL_FLASH_DESCRIPTOR *Descriptor
  )
{
  UINT32 Index;

  Print (L"\nIntel Flash Descriptor\n");
  Print (L"  Signature offset : 0x%04x\n", Descriptor->SignatureOffset);
  Print (L"  Signature        : 0x%08x\n", Descriptor->Signature);
  Print (L"  FLMAP0           : 0x%08x\n", Descriptor->FlashMap0);
  Print (L"  FLMAP1           : 0x%08x\n", Descriptor->FlashMap1);
  Print (L"  FLMAP2           : 0x%08x\n", Descriptor->FlashMap2);
  Print (L"  FCBA             : 0x%04x\n", Descriptor->ComponentBase);
  Print (L"  FRBA             : 0x%04x\n", Descriptor->RegionBase);
  Print (L"  FMBA             : 0x%04x\n", Descriptor->MasterBase);
  Print (L"  FPSBA            : 0x%04x\n", Descriptor->StrapBase);
  Print (L"  Strap length     : 0x%04x\n", Descriptor->StrapLength);
  Print (L"  Region count     : %u\n\n", Descriptor->RegionCount);

  Print (
    L"  %-2s %-22s %-12s %-12s %-12s %s %s %s\n",
    L"#",
    L"Name",
    L"Base",
    L"Limit",
    L"Length",
    L"State",
    L"Bounds",
    L"Overlap"
    );

  for (Index = 0; Index < Descriptor->RegionCount; ++Index) {
    CONST CHRONOS_INTEL_FLASH_REGION *Region;

    Region = &Descriptor->Regions[Index];

    Print (
      L"  %-2u %-22s %08x     %08x     %08x     %s%s%s%s\n",
      Index,
      ChronosIntelFlashRegionName (Index),
      Region->Base,
      Region->Limit,
      Region->Length,
      Region->IsUnused ? L"unused" : L"active",
      Region->IsReserved ? L" reserved" : L"",
      Region->IsOutOfBounds ? L" out-of-bounds" : L"",
      Region->IsOverlapping ? L" overlapping" : L""
      );
  }

  Print (L"\n");

  if (Descriptor->RegionCount > 0) {
    CONST CHRONOS_INTEL_FLASH_REGION *Bios;

    Bios = &Descriptor->Regions[ChronosFlashRegionBios];

    if (!Bios->IsUnused) {
      Print (
        L"  BIOS region: 0x%08x - 0x%08x (%u bytes)\n",
        Bios->Base,
        Bios->Limit,
        Bios->Length
        );
    }
  }
}

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
  CHRONOS_INTEL_FLASH_DESCRIPTOR Descriptor;
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

  Status = ChronosIntelFlashDescriptorParse (&Flash, &Descriptor);
  if (EFI_ERROR (Status)) {
    Print (
      L"Chronos: Intel Flash Descriptor not available/valid: %r\n",
      Status
      );
  } else {
    PrintDescriptor (&Descriptor);
  }

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
