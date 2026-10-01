# Chronos Firmware / UEFI Research

## Purpose

Chronos.Firmware studies pre-OS execution, UEFI trust, bootkits, firmware persistence, ACPI and hardware-backed integrity.

The current Chronos.Firmware Visual Studio project is only a host-side scaffold. The intended firmware implementation belongs in a TianoCore EDK II workspace with its own package and build model.

## 1. Why Firmware Is a Different Security Boundary

A simplified startup sequence is:

~~~text
power-on
   ↓
platform firmware
   ↓
UEFI phases
   ↓
boot manager
   ↓
Windows boot components
   ↓
Windows kernel
   ↓
user mode
~~~

An attacker operating below Windows can influence the environment in which OS defenses begin.

MITRE ATT&CK models this as T1542 Pre-OS Boot.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1542/

## 2. UEFI System-Firmware Persistence

### Capability

T1542.001 covers modification of system firmware to persist below the operating system.

### Offensive perspective

Firmware persistence can survive ordinary OS reinstallation and can execute before normal host monitoring.

ATT&CK documents LoJax and Hacking Team's UEFI rootkit as real-world examples.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1542/001/

### Defensive perspective

Detection and prevention rely on a trust anchor below the OS:

- Secure Boot
- firmware integrity validation
- TPM measurements
- hardware-rooted trust
- controlled vendor updates
- remote attestation

MITRE explicitly recommends boot integrity controls for system-firmware threats.

**Detection / prevention source:**  
https://attack.mitre.org/techniques/T1542/001/  
https://learn.microsoft.com/windows/security/threat-protection/windows-defender-system-guard/how-hardware-based-root-of-trust-helps-protect-windows

## 3. UEFI Bootkits

### Capability

T1542.003 covers bootkits that modify the boot path so malicious code can execute before Windows is fully established.

### Offensive perspective

The security advantage is temporal:

~~~text
attacker starts
      ↓
before
      ↓
OS security stack fully starts
~~~

BlackLotus is a documented modern example of this class.

**Offensive/tradecraft sources:**  
https://attack.mitre.org/techniques/T1542/003/  
https://www.microsoft.com/en-us/security/blog/2023/04/11/guidance-for-investigating-attacks-using-cve-2022-21894-the-blacklotus-campaign/

### Defensive perspective

Microsoft's BlackLotus investigation guidance demonstrates a multi-artifact approach using EFI files, boot configuration, registry and event-log evidence plus network behavior.

Individual artifacts can have low fidelity; the correlation is more meaningful.

**Detection source:**  
https://www.microsoft.com/en-us/security/blog/2023/04/11/guidance-for-investigating-attacks-using-cve-2022-21894-the-blacklotus-campaign/

## 4. Pre-OS Defense Evasion

### Capability

Pre-OS code can influence the environment in which Windows security mechanisms are initialized.

### Offensive perspective

The attacker attempts to gain a temporal position below the ordinary defender trust boundary.

Microsoft's BlackLotus analysis describes how a UEFI bootkit could interfere with OS security mechanisms after establishing this position.

**Offensive source:**  
https://www.microsoft.com/en-us/security/blog/2023/04/11/guidance-for-investigating-attacks-using-cve-2022-21894-the-blacklotus-campaign/

### Defensive perspective

Defenders move trust below Windows using Secure Boot, Measured Boot, TPM and remote attestation.

**Detection / prevention source:**  
https://learn.microsoft.com/windows/security/threat-protection/windows-defender-system-guard/how-hardware-based-root-of-trust-helps-protect-windows  
https://learn.microsoft.com/en-us/windows/security/hardware-security/tpm/how-windows-uses-the-tpm

## 5. Component-Firmware Persistence

### Capability

T1542.002 covers malicious modification of firmware belonging to system components rather than only the primary platform firmware.

### Offensive perspective

The threat persists below the host OS and may have weaker integrity controls than the main platform firmware.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1542/002/

### Defensive perspective

Research should baseline firmware inventory, version, vendor provenance, cryptographic verification and hardware-backed measurements.

**Detection / prevention source:**  
https://attack.mitre.org/techniques/T1542/  
https://learn.microsoft.com/en-us/windows/security/hardware-security/tpm/trusted-platform-module-overview

## 6. ACPI as a Firmware-to-OS Trust Boundary

ACPI provides standardized firmware-described platform information to the operating system.

Chronos should treat ACPI as a trust-boundary research area:

~~~text
ASL
 ↓
IASL
 ↓
AML
 ↓
ACPI tables
 ↓
firmware
 ↓
Windows interpretation
~~~

The goal is to understand what firmware-originated information Windows consumes and how that state can be baselined.

### Offensive perspective

The important security property is that firmware-originated state exists below ordinary application trust.

**Offensive/research source:**  
TianoCore EDK II toolchain: https://github.com/tianocore/edk2/blob/master/BaseTools/Conf/tools_def.template  
UEFI specifications: https://uefi.org/specifications

### Defensive perspective

Use known-good table baselines, firmware inventories, vendor update records, measured boot and attestation where available.

**Detection / prevention source:**  
https://learn.microsoft.com/en-us/windows/threat-protection/secure-the-windows-10-boot-process  
https://learn.microsoft.com/en-us/windows/security/hardware-security/tpm/how-windows-uses-the-tpm

## 7. EDK II Architecture

Chronos firmware code should eventually be organized around a dedicated EDK II package.

~~~text
ChronosFirmwarePkg
      │
      ├── DEC
      ├── DSC
      ├── INF
      ├── FDF
      └── source modules
              │
              ▼
        EDK II BaseTools
              │
              ├── compiler
              ├── linker
              ├── GenFw
              ├── firmware-volume tools
              └── IASL
              │
              ▼
        firmware artifacts
~~~

EDK II currently contains VS2026 toolchain definitions and ASL/IASL integration.

**Toolchain source:**  
https://github.com/tianocore/edk2/blob/master/BaseTools/Conf/tools_def.template

## 8. DSC / INF / FDF / DEC

- DSC: platform composition and build configuration
- INF: firmware-module description and dependencies
- FDF: firmware-volume/image layout
- DEC: package declarations and interfaces

These are not simply replacements for a Visual Studio project file; they represent the architecture of the firmware build graph.

## 9. IASL and ACPI Compilation

IASL is the Intel ACPI Source Language compiler used to transform ASL into AML and integrate ACPI tables into firmware builds.

TianoCore's current tool definitions explicitly configure IASL for VS2026 platform and ACPI builds.

**Source:**  
https://github.com/tianocore/edk2/blob/master/BaseTools/Conf/tools_def.template

## 10. Secure Boot vs Measured Boot

The distinction is essential.

~~~text
Secure Boot
=
permit only trusted signed boot components

Measured Boot
=
record what was actually loaded
~~~

Measured Boot uses TPM-backed measurements of firmware and Windows startup components and can support remote attestation.

**Detection sources:**  
https://learn.microsoft.com/en-us/windows/threat-protection/secure-the-windows-10-boot-process  
https://learn.microsoft.com/en-us/windows/security/hardware-security/tpm/how-windows-uses-the-tpm

## 11. Firmware Detection Timeline

A mature firmware investigation should correlate:

~~~text
firmware version
      ↓
Secure Boot state
      ↓
EFI / boot artifacts
      ↓
boot configuration
      ↓
TPM measurements
      ↓
Windows startup evidence
      ↓
post-boot behavior
~~~

The goal is to reconstruct the complete trust chain instead of relying on one file or log.

## 12. Research Questions

- Which boot components are visible to Windows and which require lower-level evidence?
- What survives an OS reinstall?
- How does Secure Boot alter the threat model?
- How does Measured Boot provide evidence that differs from execution authorization?
- Which firmware changes should be treated as maintenance rather than attack?
- How can ACPI tables be baselined?
- Which measurements can support remote attestation?
- What is the correct incident-response path for suspected pre-OS compromise?

## 13. Safety Boundary

Firmware experiments should use virtualized or emulated UEFI environments, disposable images, snapshots and reversible packages. Do not test experimental firmware on production hardware or systems where a firmware failure would be unacceptable.
