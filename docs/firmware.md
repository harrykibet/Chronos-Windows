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

# Deep Capability Appendix

This appendix turns the firmware guide from a technique overview into a research specification.

## A. System-Firmware Persistence: Capability Decomposition

Separate the capability into five questions:

1. Acquisition — how did a process obtain authority to update firmware?
2. Authorization — which signer or platform policy accepted the update?
3. Write path — which firmware update mechanism performed the write?
4. Resulting state — what firmware version/image now exists?
5. Boot evidence — what changed in the subsequent measured/verified boot?

Offensive research should study the dependency chain, not a vendor-specific flashing procedure.

Defensive hypothesis:

~~~text
unexpected privileged updater
      +
unexpected firmware version transition
      +
missing maintenance/change record
      +
boot-measurement change
      =
high-value firmware investigation
~~~

The strongest signal is often the discrepancy between the claimed maintenance event and the platform state after reboot.

## B. Component Firmware: Trust Boundary Analysis

Component firmware should be modeled independently from host software.

For every component, record:

| Attribute | Research value |
|---|---|
| Hardware identity | Which physical component is involved |
| Vendor | Expected firmware authority |
| Version | Current known-good state |
| Update channel | How legitimate updates occur |
| Signature model | What authorizes firmware |
| Measurement | Whether integrity can be attested |
| Host interface | How Windows interacts with it |
| Persistence | Whether OS reinstall affects it |

Offensive value comes from persistence outside the normal host filesystem.

Defensive value comes from maintaining a hardware-aware inventory rather than assuming that the disk is the only persistence surface.

MITRE's current component-firmware strategy specifically discusses anomalous firmware interactions, unexpected updates and privileged firmware access.

Source:
https://attack.mitre.org/techniques/T1542/002/

## C. Bootkit: Boot-Chain State Machine

Research the boot path as a state machine:

~~~text
Firmware
  ↓
Boot manager
  ↓
EFI executable
  ↓
OS loader
  ↓
Kernel
  ↓
Drivers
~~~

For each transition record:

- artifact identity;
- signature state;
- measurement state;
- parent/child relationship;
- expected location;
- expected signer;
- timestamp;
- configuration source.

MITRE's current T1542.003 strategy specifically calls out EFI System Partition changes, boot-record modification and privileged low-level disk access.

Source:
https://attack.mitre.org/techniques/T1542/003/

The defensive experiment should ask:

> Which modifications are visible before Windows starts, which become visible only after startup, and which are represented in TPM measurements?

## D. EFI System Partition Forensics

Chronos should build an ESP baseline containing:

- approved directory layout;
- approved boot executables;
- hashes where stable;
- signatures;
- timestamps;
- boot configuration references;
- expected update mechanisms.

Do not make a static hash list the only defense. Enterprise and OEM maintenance changes legitimately modify boot artifacts.

A better model is:

~~~text
ESP artifact
+
who changed it
+
how it was changed
+
why it was changed
+
what boot state followed
~~~

## E. UEFI Variable / NVRAM Research

Treat UEFI variables as stateful configuration rather than generic storage.

Document:

- variable identity;
- namespace;
- attributes;
- expected writers;
- lifecycle;
- whether the value influences boot;
- how the value is observable from Windows;
- whether the value is measured or attested.

Detection should focus on changes that are both unexpected and security-relevant.

## F. Secure Boot Research

Chronos should explicitly test three states:

~~~text
1. Secure Boot enabled
2. Secure Boot disabled
3. Secure Boot policy changed between boots
~~~

The experiment should compare:

- permitted boot components;
- measured state;
- Windows-visible state;
- event evidence;
- recovery behavior.

Secure Boot is an authorization control. It should not be described as a complete malware-detection mechanism.

Source:
https://learn.microsoft.com/en-us/windows/threat-protection/secure-the-windows-10-boot-process

## G. Measured Boot Research

Chronos should model Measured Boot as evidence collection.

~~~text
component
   ↓
measurement
   ↓
TPM PCR state
   ↓
attestation / comparison
~~~

Research questions:

- Which startup components affect measurements?
- How does a configuration change alter the measurements?
- Which measurements are available to a remote verifier?
- What does a matching measurement establish?
- What does it not establish?

Source:
https://learn.microsoft.com/en-us/windows/security/hardware-security/tpm/how-windows-uses-the-tpm

The crucial distinction is:

~~~text
Secure Boot = authorization
Measured Boot = evidence
~~~

## H. ACPI Research

ACPI experiments should maintain a reproducible artifact chain:

~~~text
ASL source
 ↓
IASL compiler version
 ↓
AML
 ↓
table identity
 ↓
firmware image
 ↓
OS-visible ACPI state
~~~

Record the exact source and generated artifacts so a defender can compare:

~~~text
expected table
vs.
observed table
~~~

This transforms ACPI from a vague firmware topic into a measurable integrity problem.

TianoCore's current toolchain explicitly integrates Intel ASL/IASL and includes VS2026 definitions.

Source:
https://github.com/tianocore/edk2/blob/master/BaseTools/Conf/tools_def.template

## I. EDK II Research Boundaries

Firmware code should have explicit ownership boundaries:

~~~text
Platform description
       ↓
Package
       ↓
Module
       ↓
Firmware volume
       ↓
Boot environment
~~~

Chronos should not copy application architecture into firmware merely for familiarity.

The important boundaries are build-time composition, firmware-volume placement, execution phase and firmware-to-OS interfaces.

## J. Detection Capability Matrix

For each firmware capability record:

| Layer | Evidence | Detector | Limitation |
|---|---|---|---|
| Firmware | Version/inventory | Baseline comparison | OEM variation |
| ESP | File and metadata | Boot artifact analytics | Legitimate updates |
| UEFI variables | Variable changes | Policy/baseline | Platform-specific behavior |
| Secure Boot | Policy state | Configuration monitoring | Does not prove runtime cleanliness |
| Measured Boot | TPM measurements | Attestation | Measurement interpretation |
| ACPI | Table content | Known-good comparison | Hardware revisions |
| OS startup | Boot events | Correlation | OS visibility may be incomplete |

## K. Cross-Layer Firmware Investigation

A strong investigation should correlate:

~~~text
firmware state
   ↓
boot state
   ↓
kernel state
   ↓
driver state
   ↓
user-mode state
~~~

The lower the suspicious artifact appears in that chain, the more important independent evidence becomes.

## L. Firmware Failure Modes

False positives and uncertainty are especially important here:

- OEM firmware changes;
- BIOS/UEFI updates;
- recovery environments;
- hardware replacement;
- virtualization differences;
- secure-boot policy transitions;
- expected ACPI variation.

A detector that treats all firmware variation as malicious will be operationally unusable.

## M. Firmware Lab Validation

The preferred experiment sequence is:

~~~text
known-good firmware
      ↓
capture inventory
      ↓
capture boot measurements
      ↓
make one controlled change
      ↓
reboot
      ↓
capture again
      ↓
compare
      ↓
revert snapshot
~~~

The research objective is to discover what evidence a defender can reliably obtain when the change occurs below the operating system.

## N. Safety Boundary

All firmware experiments should use emulation, virtualization or disposable laboratory hardware specifically intended for research.

Do not use production firmware, unknown flashing utilities or irreversible modification paths as part of normal Chronos experiments.

# Hardware Surface Expansion

SPI NOR flash is Chronos's first firmware storage target, but it is not the complete platform trust surface.

See [Firmware Hardware Surfaces](firmware-hardware-surfaces.md) for the broader model covering:

- SPI flash regions;
- UEFI NVRAM/firmware variables;
- TPM;
- Embedded Controller;
- PCI Option ROMs;
- NVMe/storage firmware;
- NIC/GPU/device firmware;
- BMC/management-controller firmware;
- UEFI UpdateCapsule and ESRT;
- Secure/Measured Boot relationships.

The first implementation deliberately uses the standard UEFI Platform Initialization SPI NOR protocol instead of immediately programming chipset-specific controller registers. The UEFI PI specification defines the SPI NOR protocol with flash-ID, read, write and erase operations.

For physical hardware, Chronos currently enables read-only SPI acquisition. Physical writes are intentionally blocked until flash-region permissions, descriptor parsing, protected regions, backup/verification, recovery and platform-specific safeguards are implemented.