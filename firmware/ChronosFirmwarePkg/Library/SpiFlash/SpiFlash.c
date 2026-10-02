#include "SpiFlash.h"

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define CHRONOS_SPI_READ_CHUNK 0x1000U

STATIC
BOOLEAN
ChronosRangeIsValid (
  IN CONST CHRONOS_SPI_FLASH *Flash,
  IN UINT32 Address,
  IN UINT32 Length
  )
{
  if (Flash == NULL) {
    return FALSE;
  }

  if (Length == 0) {
    return TRUE;
  }

  if (Address >= Flash->FlashSize) {
    return FALSE;
  }

  return Length <= (Flash->FlashSize - Address);
}

EFI_STATUS
ChronosSpiFlashOpen (
  OUT CHRONOS_SPI_FLASH *Flash
  )
{
  EFI_STATUS Status;
  EFI_SPI_NOR_FLASH_PROTOCOL *Protocol;
  EFI_HANDLE *Handles;
  UINTN HandleCount;
  UINTN Index;
  UINT8 Id[3];

  if (Flash == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Flash, sizeof (*Flash));

  Handles = NULL;
  HandleCount = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSpiNorFlashProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  /*
   * A platform may expose multiple SPI NOR devices. Until Chronos has a
   * board-specific selector, use the first protocol instance and report
   * its identity. Future code should select by device identity/region.
   */
  Status = EFI_NOT_FOUND;

  for (Index = 0; Index < HandleCount; ++Index) {
    Protocol = NULL;

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiSpiNorFlashProtocolGuid,
                    (VOID **)&Protocol
                    );
    if (EFI_ERROR (Status) || Protocol == NULL) {
      continue;
    }

    if (Protocol->GetFlashid == NULL ||
        Protocol->ReadData == NULL) {
      continue;
    }

    Status = Protocol->GetFlashid (Protocol, Id);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Flash->Protocol = Protocol;
    Flash->FlashSize = Protocol->FlashSize;
    Flash->EraseBlockSize = Protocol->EraseBlockBytes;
    Flash->ManufacturerId = Id[0];
    Flash->MemoryType = Id[1];
    Flash->MemoryCapacity = Id[2];

    Status = EFI_SUCCESS;
    break;
  }

  FreePool (Handles);
  return Status;
}

EFI_STATUS
ChronosSpiFlashRead (
  IN CHRONOS_SPI_FLASH *Flash,
  IN UINT32 Address,
  IN UINT32 Length,
  OUT VOID *Buffer
  )
{
  EFI_STATUS Status;
  UINT8 *Destination;
  UINT32 Offset;
  UINT32 Remaining;
  UINT32 Chunk;

  if (Flash == NULL || Flash->Protocol == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!ChronosRangeIsValid (Flash, Address, Length)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Length == 0) {
    return EFI_SUCCESS;
  }

  Destination = (UINT8 *)Buffer;
  Offset = 0;
  Remaining = Length;

  while (Remaining > 0) {
    Chunk = (Remaining > CHRONOS_SPI_READ_CHUNK)
              ? CHRONOS_SPI_READ_CHUNK
              : Remaining;

    Status = Flash->Protocol->ReadData (
                               Flash->Protocol,
                               Address + Offset,
                               Chunk,
                               Destination + Offset
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Offset += Chunk;
    Remaining -= Chunk;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ChronosSpiFlashWrite (
  IN CHRONOS_SPI_FLASH *Flash,
  IN UINT32 Address,
  IN UINT32 Length,
  IN CONST VOID *Buffer
  )
{
  /*
   * Deliberately disabled for the physical platform backend.
   *
   * Before physical writes are enabled Chronos must implement:
   *   - platform/chipset identification;
   *   - flash descriptor parsing;
   *   - BIOS-region boundary validation;
   *   - read/write permission validation;
   *   - protected-region refusal;
   *   - immutable golden-image comparison;
   *   - write/erase verification;
   *   - recovery strategy.
   *
   * The read path is therefore the first hardware primitive.
   */
  (VOID)Flash;
  (VOID)Address;
  (VOID)Length;
  (VOID)Buffer;

  return EFI_WRITE_PROTECTED;
}

EFI_STATUS
ChronosSpiFlashReadImage (
  IN CHRONOS_SPI_FLASH *Flash,
  OUT VOID **Image,
  OUT UINT32 *ImageSize
  )
{
  EFI_STATUS Status;
  VOID *Buffer;

  if (Flash == NULL || Image == NULL || ImageSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Image = NULL;
  *ImageSize = 0;

  if (Flash->FlashSize == 0) {
    return EFI_NOT_FOUND;
  }

  Buffer = AllocateZeroPool (Flash->FlashSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ChronosSpiFlashRead (
             Flash,
             0,
             Flash->FlashSize,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return Status;
  }

  *Image = Buffer;
  *ImageSize = Flash->FlashSize;
  return EFI_SUCCESS;
}
