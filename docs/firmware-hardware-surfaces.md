# Firmware Hardware Surfaces

SPI NOR flash is the correct first storage surface for Chronos, but it should not be treated as the only firmware surface.

On modern Intel platforms, the SPI flash can contain multiple logical regions rather than only a single BIOS blob. Intel documents platform-dependent regions including a flash descriptor, BIOS, CSME/ME, GbE, Platform Data, Embedded Controller and silicon-security-engine regions. Access is controlled by multiple masters and region permissions. citeturn954817search0turn954817search38

## Priority order

### 1. SPI NOR flash

Primary Chronos target.

Study:

- descriptor;
- BIOS/UEFI region;
- CSME/ME region;
- GbE/NIC region;
- Platform Data Region;
- EC-related region where present;
- firmware-volume structure;
- protection/access permissions.

The UEFI PI specification provides an `EFI_SPI_NOR_FLASH_PROTOCOL` with read, write, erase, ID and status operations. citeturn651627search0turn857482search2

Chronos should use that protocol when the platform exposes it rather than immediately programming chipset-specific SPI registers.

### 2. UEFI NVRAM / firmware variables

This is a logical firmware-state surface, not necessarily a separate chip.

Windows exposes UEFI firmware variables through `GetFirmwareEnvironmentVariableEx`, while kernel mode has `ExGetFirmwareEnvironmentVariable`. Variable attributes include non-volatile, boot-service, runtime and authenticated-write attributes. citeturn462919search4turn462919search7

Study:

- BootOrder;
- Boot#### entries;
- Secure Boot variables;
- vendor variables;
- authenticated variables;
- variable attributes;
- unexpected persistent changes.

### 3. TPM

A TPM is not the place to read the BIOS image. It is a trust/measurement device.

Chronos should study:

- TPM 2.0;
- PCR measurements;
- Secure/Measured Boot;
- attestation;
- relationship between measured firmware state and the actual SPI image.

### 4. Embedded Controller

The EC can have its own firmware or a platform-defined region in the SPI architecture. Intel documents EC as a possible SPI region/master on several platforms. citeturn954817search0turn954817search38

Chronos should inventory it but keep EC programming separate from the generic SPI NOR layer.

### 5. PCI Option ROMs

PCI devices can carry Option ROMs containing legacy or UEFI driver images. UEFI specifies PCI Option ROM handling and PE/COFF-based UEFI images. citeturn403613search0turn403613search24

Relevant devices include:

- NICs;
- storage controllers;
- GPUs;
- HBAs;
- other PCIe adapters.

Chronos should read and validate Option ROMs before considering any write/update mechanism.

### 6. Device firmware

Windows distinguishes system firmware from device firmware and supports firmware update flows for eligible devices. citeturn954817search3

Important future surfaces:

- NVMe SSD firmware;
- storage-controller firmware;
- NIC firmware;
- GPU firmware;
- management-controller/BMC firmware on servers;
- Thunderbolt/peripheral firmware where applicable.

Microsoft documents dedicated NVMe storage firmware update mechanisms and separate device-firmware update paths. citeturn462919search2turn462919search8

### 7. UEFI UpdateCapsule / ESRT

This is an update mechanism, not a storage device.

Windows hands system/device firmware update payloads to platform firmware through UEFI `UpdateCapsule`. The ESRT identifies firmware resources and tracks current version/update status. citeturn462919search0turn462919search5

Chronos should observe this path as a **firmware provenance and update-integrity surface**.

## Recommended Chronos architecture

~~~text
                 FirmwareResearch
                       │
          ┌────────────┼────────────┐
          │            │            │
       Storage       State        Devices
          │            │            │
     SPI NOR       UEFI vars    PCI Option ROM
       flash          TPM       NIC / NVMe / GPU
          │          Secure       EC / BMC
          │        Measured
          │           Boot
          └────────────┬────────────┘
                       │
                 Integrity Engine
~~~

The first implementation should therefore be:

1. SPI NOR discovery;
2. manufacturer/device ID;
3. flash-size discovery;
4. read-only image acquisition;
5. flash-descriptor/region parsing;
6. UEFI NVRAM inventory;
7. TPM/boot-measurement inventory;
8. device-firmware inventory;
9. Option ROM inventory.

Physical firmware writes come later, and only through a dedicated laboratory backend after read/backup/region-validation/recovery workflows are proven.

## Why this matters for Chronos

A firmware threat model that monitors only BIOS SPI data can miss the broader persistence surface.

For example:

~~~text
SPI firmware
      +
UEFI variables
      +
TPM measurements
      +
PCI Option ROM
      +
device firmware
      +
boot artifacts
~~~

provides a much stronger model of platform integrity.
