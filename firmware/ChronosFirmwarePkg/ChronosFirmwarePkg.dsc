[Defines]
  PLATFORM_NAME                  = ChronosFirmware
  PLATFORM_GUID                  = 6B5EAFB3-4E5F-49B4-A9A2-6E2D1E6F2D11
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001C
  OUTPUT_DIRECTORY               = Build/ChronosFirmware
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[Packages]
  MdePkg/MdePkg.dec
  MdeModulePkg/MdeModulePkg.dec
  ChronosFirmwarePkg/ChronosFirmwarePkg.dec

[LibraryClasses]
  UefiApplicationEntryPoint
  UefiLib
  UefiBootServicesTableLib
  BaseLib
  MemoryAllocationLib
  BaseMemoryLib
  PrintLib
  DebugLib

[Components]
  ChronosFirmwarePkg/Application/SpiInspect/SpiInspect.inf
