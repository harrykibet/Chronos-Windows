#include "IntelFlashDescriptor.h"

#include <Library/BaseMemoryLib.h>

STATIC
UINT32
ReadLe32 (
  IN CONST UINT8 *Buffer
  )
{
  return ((UINT32)Buffer[0]) |
         ((UINT32)Buffer[1] << 8) |
         ((UINT32)Buffer[2] << 16) |
         ((UINT32)Buffer[3] << 24);
}

STATIC
BOOLEAN
RangeInsideDescriptor (
  IN UINT32 Offset,
  IN UINT32 Length
  )
{
  if (Offset > CHRONOS_INTEL_FD_SIZE) {
    return FALSE;
  }

  return Length <= (CHRONOS_INTEL_FD_SIZE - Offset);
}

STATIC
BOOLEAN
IsMapOffset (
  IN UINT32 Offset
  )
{
  return Offset != 0 && (Offset % 0x10U) == 0;
}

STATIC
UINT32
MapOffsetFromByte (
  IN UINT32 Value
  )
{
  return (Value & 0xFFU) << 4;
}

STATIC
UINT32
RegionCountFromMap (
  IN UINT32 RegionBase,
  IN UINT32 ComponentBase,
  IN UINT32 MasterBase,
  IN UINT32 StrapBase
  )
{
  UINT32 Candidates[4];
  UINT32 Next;
  UINT32 Index;
  UINT32 I;

  Candidates[0] = ComponentBase;
  Candidates[1] = MasterBase;
  Candidates[2] = StrapBase;
  Candidates[3] = CHRONOS_INTEL_DESCRIPTOR_UPPER_MAP_OFFSET;

  Next = CHRONOS_INTEL_FD_SIZE;

  for (I = 0; I < 4; ++I) {
    if (!IsMapOffset (Candidates[I])) {
      continue;
    }

    if (Candidates[I] > RegionBase && Candidates[I] < Next) {
      Next = Candidates[I];
    }
  }

  if (Next <= RegionBase) {
    return 0;
  }

  Index = (Next - RegionBase) / sizeof (UINT32);

  if (Index > CHRONOS_INTEL_MAX_REGIONS) {
    Index = CHRONOS_INTEL_MAX_REGIONS;
  }

  return Index;
}

STATIC
VOID
DecodeRegion (
  IN UINT32 RawRegister,
  IN UINT32 Index,
  OUT CHRONOS_INTEL_FLASH_REGION *Region
  )
{
  UINT32 BaseUnit;
  UINT32 LimitUnit;

  ZeroMem (Region, sizeof (*Region));

  Region->RawRegister = RawRegister;
  Region->Type = (CHRONOS_INTEL_FLASH_REGION_TYPE)Index;

  /*
   * Intel flash region registers encode base/limit in 4 KiB units.
   * The common descriptor representation reserves bit 15 in each
   * 16-bit half for access/control metadata, leaving 15 address bits.
   */
  BaseUnit = RawRegister & 0x7FFFU;
  LimitUnit = (RawRegister >> 16) & 0x7FFFU;

  Region->Base = BaseUnit << 12;
  Region->Limit = (LimitUnit << 12) | 0xFFFU;

  /*
   * Intel uses a base field of 0x7FFF with a zero limit field as the
   * conventional "unused region" encoding. FLREG0 == 0 is different:
   * it legitimately describes the descriptor at 0x00000000-0x00000FFF.
   */
  if (BaseUnit == 0x7FFFU && LimitUnit == 0) {
    Region->IsUnused = TRUE;
    Region->Base = 0;
    Region->Limit = 0;
    Region->Length = 0;
  } else if (Region->Base > Region->Limit) {
    Region->IsUnused = TRUE;
    Region->Length = 0;
  } else {
    Region->Length = Region->Limit - Region->Base + 1;
  }

  Region->IsReserved =
    (Index == ChronosFlashRegionReserved7) ||
    (Index == ChronosFlashRegionReserved13) ||
    (Index == ChronosFlashRegionReserved14) ||
    (Index == 15 && Region->RawRegister == 0);
}

EFI_STATUS
ChronosIntelFlashDescriptorReadImage (
  IN  CHRONOS_SPI_FLASH *Flash,
  OUT UINT8 *DescriptorImage,
  IN  UINT32 DescriptorImageSize
  )
{
  if (Flash == NULL ||
      DescriptorImage == NULL ||
      DescriptorImageSize < CHRONOS_INTEL_FD_SIZE) {
    return EFI_INVALID_PARAMETER;
  }

  if (Flash->FlashSize < CHRONOS_INTEL_FD_SIZE) {
    return EFI_BAD_BUFFER_SIZE;
  }

  return ChronosSpiFlashRead (
           Flash,
           0,
           CHRONOS_INTEL_FD_SIZE,
           DescriptorImage
           );
}

EFI_STATUS
ChronosIntelFlashDescriptorParse (
  IN  CHRONOS_SPI_FLASH *Flash,
  OUT CHRONOS_INTEL_FLASH_DESCRIPTOR *Descriptor
  )
{
  EFI_STATUS Status;
  UINT8 Image[CHRONOS_INTEL_FD_SIZE];
  UINT32 SignatureOffset;
  UINT32 ComponentBase;
  UINT32 RegionBase;
  UINT32 MasterBase;
  UINT32 StrapBase;
  UINT32 StrapLength;
  UINT32 FlashMap0;
  UINT32 FlashMap1;
  UINT32 FlashMap2;
  UINT32 RegionCount;
  UINT32 Index;

  if (Flash == NULL || Descriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Descriptor, sizeof (*Descriptor));
  ZeroMem (Image, sizeof (Image));

  Status = ChronosIntelFlashDescriptorReadImage (
             Flash,
             Image,
             sizeof (Image)
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SignatureOffset = CHRONOS_INTEL_DESCRIPTOR_SIGNATURE_OFFSET;

  /*
   * Intel documents the descriptor signature at offset 0x10. Chronos reads
   * the first flash block as the descriptor image, so a signature elsewhere
   * is treated as an invalid descriptor rather than guessing a new base.
   */
  if (ReadLe32 (Image + SignatureOffset) != CHRONOS_INTEL_FD_SIGNATURE) {
    return EFI_NOT_FOUND;
  }

  /*
   * FLMAP0/1/2 follow the descriptor signature area. We intentionally decode
   * the little-endian words from the byte image instead of overlaying packed
   * C structures. This avoids alignment/packing assumptions on UEFI builds.
   */
  FlashMap0 = ReadLe32 (Image + 0x14);
  FlashMap1 = ReadLe32 (Image + 0x18);
  FlashMap2 = ReadLe32 (Image + 0x1C);

  ComponentBase = MapOffsetFromByte (FlashMap0);
  RegionBase = MapOffsetFromByte (FlashMap0 >> 16);
  MasterBase = MapOffsetFromByte (FlashMap1);
  StrapBase = MapOffsetFromByte (FlashMap1 >> 16);
  StrapLength = ((FlashMap1 >> 24) & 0xFFU) * sizeof (UINT32);

  if (!IsMapOffset (ComponentBase) ||
      !IsMapOffset (RegionBase) ||
      !IsMapOffset (MasterBase)) {
    return EFI_COMPROMISED_DATA;
  }

  if (StrapBase != 0 && !IsMapOffset (StrapBase)) {
    return EFI_COMPROMISED_DATA;
  }

  if (!RangeInsideDescriptor (ComponentBase, sizeof (UINT32)) ||
      !RangeInsideDescriptor (RegionBase, sizeof (UINT32)) ||
      !RangeInsideDescriptor (MasterBase, sizeof (UINT32))) {
    return EFI_COMPROMISED_DATA;
  }

  if (StrapBase != 0 &&
      !RangeInsideDescriptor (StrapBase, StrapLength)) {
    return EFI_COMPROMISED_DATA;
  }

  RegionCount = RegionCountFromMap (
                  RegionBase,
                  ComponentBase,
                  MasterBase,
                  StrapBase
                  );

  if (RegionCount == 0) {
    return EFI_COMPROMISED_DATA;
  }

  if (!RangeInsideDescriptor (
        RegionBase,
        RegionCount * sizeof (UINT32)
        )) {
    return EFI_COMPROMISED_DATA;
  }

  Descriptor->Signature = CHRONOS_INTEL_FD_SIGNATURE;
  Descriptor->SignatureOffset = SignatureOffset;
  Descriptor->FlashMap0 = FlashMap0;
  Descriptor->FlashMap1 = FlashMap1;
  Descriptor->FlashMap2 = FlashMap2;
  Descriptor->ComponentBase = ComponentBase;
  Descriptor->RegionBase = RegionBase;
  Descriptor->MasterBase = MasterBase;
  Descriptor->StrapBase = StrapBase;
  Descriptor->StrapLength = StrapLength;
  Descriptor->RegionCount = RegionCount;

  for (Index = 0; Index < RegionCount; ++Index) {
    UINT32 RawRegion;

    RawRegion = ReadLe32 (
                  Image + RegionBase + (Index * sizeof (UINT32))
                  );

    DecodeRegion (
      RawRegion,
      Index,
      &Descriptor->Regions[Index]
      );

    if (!Descriptor->Regions[Index].IsUnused) {
      if (Descriptor->Regions[Index].Base >= Flash->FlashSize ||
          Descriptor->Regions[Index].Length >
            (Flash->FlashSize - Descriptor->Regions[Index].Base)) {
        Descriptor->Regions[Index].IsOutOfBounds = TRUE;
      }
    }
  }

  /*
   * Region ranges should not overlap. Overlap does not necessarily mean the
   * descriptor is unreadable, so the parser records the anomaly and leaves
   * the raw map available for forensic analysis.
   */
  for (Index = 0; Index < RegionCount; ++Index) {
    UINT32 Other;

    if (Descriptor->Regions[Index].IsUnused) {
      continue;
    }

    for (Other = 0; Other < Index; ++Other) {
      if (Descriptor->Regions[Other].IsUnused) {
        continue;
      }

      if (Descriptor->Regions[Index].Base <= Descriptor->Regions[Other].Limit &&
          Descriptor->Regions[Other].Base <= Descriptor->Regions[Index].Limit) {
        Descriptor->Regions[Index].IsOverlapping = TRUE;
        Descriptor->Regions[Other].IsOverlapping = TRUE;
      }
    }
  }

  return EFI_SUCCESS;
}

CONST CHAR16 *
ChronosIntelFlashRegionName (
  IN UINT32 RegionIndex
  )
{
  STATIC CONST CHAR16 *Names[CHRONOS_INTEL_MAX_REGIONS] = {
    L"Flash Descriptor",
    L"BIOS",
    L"Intel ME/CSE",
    L"Gigabit Ethernet",
    L"Platform Data",
    L"Device Expansion 1",
    L"Secondary BIOS",
    L"Reserved",
    L"Embedded Controller",
    L"Device Expansion 2",
    L"Intel IE/ESE",
    L"10GbE 0",
    L"10GbE 1",
    L"Reserved",
    L"Reserved",
    L"Platform Trust Technology"
  };

  if (RegionIndex >= CHRONOS_INTEL_MAX_REGIONS) {
    return L"Unknown";
  }

  return Names[RegionIndex];
}
