#ifndef CHRONOS_INTEL_FLASH_DESCRIPTOR_H
#define CHRONOS_INTEL_FLASH_DESCRIPTOR_H

#include <Uefi.h>

#include "../../Library/SpiFlash/SpiFlash.h"

#define CHRONOS_INTEL_FD_SIZE                 0x1000U
#define CHRONOS_INTEL_FD_SIGNATURE            0x0FF0A55AU
#define CHRONOS_INTEL_MAX_REGIONS             16U
#define CHRONOS_INTEL_REGION_GRANULARITY      0x1000U
#define CHRONOS_INTEL_DESCRIPTOR_SIGNATURE_OFFSET 0x10U
#define CHRONOS_INTEL_DESCRIPTOR_OEM_SIZE     0x100U
#define CHRONOS_INTEL_DESCRIPTOR_UPPER_MAP_OFFSET \
  (CHRONOS_INTEL_FD_SIZE - CHRONOS_INTEL_DESCRIPTOR_OEM_SIZE - sizeof (UINT32))

typedef enum {
  ChronosFlashRegionDescriptor = 0,
  ChronosFlashRegionBios        = 1,
  ChronosFlashRegionMe          = 2,
  ChronosFlashRegionGbe         = 3,
  ChronosFlashRegionPdr         = 4,
  ChronosFlashRegionDeviceExp1  = 5,
  ChronosFlashRegionBios2       = 6,
  ChronosFlashRegionReserved7   = 7,
  ChronosFlashRegionEc          = 8,
  ChronosFlashRegionDeviceExp2  = 9,
  ChronosFlashRegionIe          = 10,
  ChronosFlashRegion10Gbe0      = 11,
  ChronosFlashRegion10Gbe1      = 12,
  ChronosFlashRegionReserved13  = 13,
  ChronosFlashRegionReserved14  = 14,
  ChronosFlashRegionPtt         = 15
} CHRONOS_INTEL_FLASH_REGION_TYPE;

typedef struct {
  UINT32 RawRegister;
  UINT32 Base;
  UINT32 Limit;
  UINT32 Length;
  BOOLEAN IsUnused;
  BOOLEAN IsReserved;
  BOOLEAN IsOutOfBounds;
  BOOLEAN IsOverlapping;
  CHRONOS_INTEL_FLASH_REGION_TYPE Type;
} CHRONOS_INTEL_FLASH_REGION;

typedef struct {
  UINT32 Signature;
  UINT32 SignatureOffset;

  UINT32 FlashMap0;
  UINT32 FlashMap1;
  UINT32 FlashMap2;

  UINT32 ComponentBase;
  UINT32 RegionBase;
  UINT32 MasterBase;
  UINT32 StrapBase;
  UINT32 StrapLength;

  UINT32 RegionCount;

  CHRONOS_INTEL_FLASH_REGION Regions[CHRONOS_INTEL_MAX_REGIONS];
} CHRONOS_INTEL_FLASH_DESCRIPTOR;

/**
  Read and parse the Intel Flash Descriptor from SPI NOR flash.

  This function only reads the descriptor and derives metadata. It does not
  modify flash contents or controller configuration.
**/
EFI_STATUS
ChronosIntelFlashDescriptorParse (
  IN  CHRONOS_SPI_FLASH *Flash,
  OUT CHRONOS_INTEL_FLASH_DESCRIPTOR *Descriptor
  );

/**
  Read a complete descriptor image into a caller-owned buffer.
**/
EFI_STATUS
ChronosIntelFlashDescriptorReadImage (
  IN  CHRONOS_SPI_FLASH *Flash,
  OUT UINT8 *DescriptorImage,
  IN  UINT32 DescriptorImageSize
  );

/**
  Return a stable display name for a region index.
**/
CONST CHAR16 *
ChronosIntelFlashRegionName (
  IN UINT32 RegionIndex
  );

#endif
