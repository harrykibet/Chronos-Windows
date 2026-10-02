#ifndef CHRONOS_SPI_FLASH_H
#define CHRONOS_SPI_FLASH_H

#include <Uefi.h>
#include <Protocol/SpiNorFlash.h>

typedef struct {
  EFI_SPI_NOR_FLASH_PROTOCOL *Protocol;
  UINT32 FlashSize;
  UINT32 EraseBlockSize;
  UINT8  ManufacturerId;
  UINT8  MemoryType;
  UINT8  MemoryCapacity;
} CHRONOS_SPI_FLASH;

EFI_STATUS
ChronosSpiFlashOpen(
  OUT CHRONOS_SPI_FLASH *Flash
  );

EFI_STATUS
ChronosSpiFlashRead(
  IN CHRONOS_SPI_FLASH *Flash,
  IN UINT32 Address,
  IN UINT32 Length,
  OUT VOID *Buffer
  );

/*
 * Physical firmware writes are intentionally disabled by default.
 *
 * The UEFI SPI NOR protocol supports writing, but enabling arbitrary
 * physical flash writes before platform-specific protection/region
 * handling has been validated can corrupt firmware and render hardware
 * unbootable.
 *
 * This wrapper therefore returns EFI_WRITE_PROTECTED unless the caller
 * supplies a dedicated lab backend in a future implementation.
 */
EFI_STATUS
ChronosSpiFlashWrite(
  IN CHRONOS_SPI_FLASH *Flash,
  IN UINT32 Address,
  IN UINT32 Length,
  IN CONST VOID *Buffer
  );

EFI_STATUS
ChronosSpiFlashReadImage(
  IN CHRONOS_SPI_FLASH *Flash,
  OUT VOID **Image,
  OUT UINT32 *ImageSize
  );

#endif
