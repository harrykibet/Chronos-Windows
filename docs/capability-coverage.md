# Chronos Capability Coverage Matrix

## Purpose

This document is the coverage contract for Chronos.

No defensive research platform can honestly claim to enumerate every offensive technique ever created. Tradecraft changes continuously, private tooling is undocumented, and new ATT&CK techniques and sub-techniques are added over time.

Chronos therefore defines "comprehensive coverage" as:

1. current MITRE ATT&CK Enterprise techniques and sub-techniques relevant to Windows host execution, persistence, defense evasion, privilege escalation, credential access, discovery, collection, command-and-control, impact and pre-OS execution;
2. Windows-specific low-level mechanisms that are important to detection but do not map one-to-one to an ATT&CK technique;
3. UEFI/firmware mechanisms relevant to Windows boot trust;
4. a detection/control mapping for every covered mechanism;
5. a review process for newly published techniques.

MITRE's current Enterprise technique catalog is the baseline, and current technique pages are being updated through 2026. For example, Process Injection currently has 12 sub-techniques, while Pre-OS Boot currently has five. ([MITRE Process Injection](https://attack.mitre.org/techniques/T1055/), [MITRE Pre-OS Boot](https://attack.mitre.org/techniques/T1542/))

---

# 1. Coverage Status

| Domain | Coverage baseline |
|---|---|
| Loader | Execution, staging, proxy execution, obfuscation, evasion, provenance |
| User mode | Execution, persistence, privilege, credential access, discovery, collection, C2, defense evasion |
| Kernel | Driver loading, privilege escalation, BYOVD, rootkits, kernel interception, memory/integrity |
| Firmware | UEFI, bootkits, firmware persistence, ACPI, NVRAM, Secure/Measured Boot |
| Cross-layer | Trust transitions, telemetry correlation, remediation |

Each entry should eventually have:

~~~text
OFFENSIVE MECHANISM
      ↓
WINDOWS / FIRMWARE PRIMITIVE
      ↓
OBSERVABLE ARTIFACT
      ↓
TELEMETRY SOURCE
      ↓
DETECTION ANALYTIC
      ↓
MITIGATION
      ↓
LAB VALIDATION
~~~

---

# 2. Loader Coverage

| Technique / mechanism | ATT&CK | Defensive evidence |
|---|---|---|
| Ingress Tool Transfer | T1105 | Network + file + process correlation |
| User Execution | T1204 | User-to-process lineage |
| Obfuscated Files | T1027 | Reconstruction + entropy + content behavior |
| Deobfuscate/Decode Files | T1140 | Decode event followed by execution |
| Command Obfuscation | T1027.010 | Interpreter + encoded argument behavior |
| System Binary Proxy Execution | T1218 | Trusted image + abnormal invocation |
| Trusted Developer Utilities Proxy Execution | T1127 | Build/developer utility use outside role |
| Indirect Command Execution | T1202 | Parent/child indirection |
| Native API | T1106 | Low-level API sequence and call context |
| Shared Modules | T1129 | Unexpected module loading |
| Reflective Code Loading | T1620 | Memory/module/thread inconsistency |
| Process Injection | T1055 | Process access + memory + thread |
| Hijack Execution Flow | T1574 | DLL/path/service manipulation |
| Signed Binary Proxy Execution | T1218 family | Signer + context |
| Masquerading | T1036 | Identity/provenance mismatch |
| Hide Artifacts | T1564 family | Hidden files/directories/metadata anomalies |
| File and Directory Discovery | T1083 | Unusual discovery burst |
| Virtualization/Sandbox Evasion | T1497 | Environment checks + conditional behavior |
| Execution Guardrails | T1480 | Environment-dependent execution |
| Time Based Evasion | T1497.003 | Delayed activation patterns |
| Indicator Removal | T1070 family | Create/delete/clear sequence |
| File Deletion | T1070.004 | Short-lived artifact lifecycle |
| Timestomp | T1070.006 | Timestamp inconsistencies |
| Process Termination | T1489 / defense-evasion context | Security process termination correlation |
| Service Execution | T1569.002 | Service/process correlation |
| Command and Scripting Interpreter | T1059 family | Interpreter lineage |
| WMI | T1047 | WMI process/consumer activity |
| Scheduled Task | T1053.005 | Task creation + execution |
| Event Triggered Execution | T1546 family | Persistence trigger changes |

---

# 3. User-Mode Coverage

## Execution

- T1059 Command and Scripting Interpreter and Windows-relevant sub-techniques.
- T1106 Native API.
- T1129 Shared Modules.
- T1202 Indirect Command Execution.
- T1218 System Binary Proxy Execution.
- T1127 Trusted Developer Utilities Proxy Execution.
- T1047 Windows Management Instrumentation.
- T1569.002 Service Execution.
- T1053.005 Scheduled Task.
- T1105 Ingress Tool Transfer.
- T1204 User Execution.
- T1055 Process Injection and current Windows sub-techniques.

ATT&CK currently lists 13 Command and Scripting Interpreter sub-techniques, including PowerShell, Windows Command Shell, Visual Basic, Python, JavaScript, AutoHotKey/AutoIT and newer platform-specific interpreters. ([MITRE T1059](https://attack.mitre.org/techniques/T1059/))

## Persistence

- T1547.001 Registry Run Keys / Startup Folder.
- T1547.002 Authentication Package.
- T1547.003 Time Providers.
- T1547.004 Winlogon Helper DLL.
- T1547.005 Security Support Provider.
- T1547.007 Re-opened Applications.
- T1547.009 Shortcut Modification.
- T1547.010 Port Monitors.
- T1547.012 Print Processors.
- T1547.014 Active Setup.
- T1053.005 Scheduled Task.
- T1543.003 Windows Service.
- T1574 Hijack Execution Flow.
- T1136 account creation/manipulation where relevant.
- T1098 Account Manipulation where applicable.

The current ATT&CK T1547 catalog contains multiple Windows autostart mechanisms beyond Run Keys, including Authentication Packages, Winlogon Helper DLL, Security Support Providers, Port Monitors and Active Setup. ([MITRE T1547](https://attack.mitre.org/techniques/T1547/))

## Privilege Escalation

- T1548.002 Bypass User Account Control.
- T1068 Exploitation for Privilege Escalation.
- T1134 Access Token Manipulation.
- T1574 Hijack Execution Flow.
- T1547 privileged autostarts.
- T1053 privileged scheduled execution.
- T1543 privileged service execution.

ATT&CK currently models six Abuse Elevation Control Mechanism sub-techniques, including Windows UAC bypass. ([MITRE T1548](https://attack.mitre.org/techniques/T1548/))

## Defense Evasion

- T1027 Obfuscated Files or Information.
- T1027.010 Command Obfuscation.
- T1140 Deobfuscate/Decode.
- T1036 Masquerading.
- T1564 Hide Artifacts.
- T1070 Indicator Removal.
- T1112 Modify Registry.
- T1218 System Binary Proxy Execution.
- T1127 Trusted Developer Utilities.
- T1202 Indirect Execution.
- T1574 Hijack Execution Flow.
- T1553 Subvert Trust Controls.
- T1562.001 Impair Defenses.
- T1480 Execution Guardrails.
- T1497 Virtualization/Sandbox Evasion.
- T1106 Native API.
- API unhooking / alternate syscall paths as a research category.
- Dynamic API resolution / API hashing as an implementation-evasion category.
- Fileless or memory-resident execution as a cross-technique behavior.
- Process tampering.

## Credential Access

Chronos should cover, at a defensive-analysis level:

- T1003 OS Credential Dumping;
- T1003.001 LSASS Memory;
- T1003.002 Security Account Manager;
- T1003.004 LSA Secrets;
- T1003.005 Cached Domain Credentials;
- T1555 Credentials from Password Stores;
- T1555.003 Credentials from Web Browsers;
- T1555.004 Windows Credential Manager;
- T1555.005 Password Managers;
- T1056 Input Capture;
- T1056.001 Keylogging;
- T1056.002 GUI Input Capture;
- T1056.003 Web Portal Capture;
- T1187 Forced Authentication;
- T1212 Exploitation for Credential Access;
- T1552 Unsecured Credentials.

MITRE's current T1003 catalog contains eight sub-techniques and documents APT28, APT32, APT39 and other real-world credential-dumping activity. ([MITRE T1003](https://attack.mitre.org/techniques/T1003/))

MITRE's current T1555 catalog includes browser credentials, Windows Credential Manager and password managers; its detection strategy correlates suspicious process access to sensitive stores with file/process activity. ([MITRE T1555](https://attack.mitre.org/techniques/T1555/), [MITRE DET0430](https://attack.mitre.org/detectionstrategies/DET0430/))

## Discovery

Chronos should cover:

- T1057 Process Discovery;
- T1082 System Information Discovery;
- T1083 File and Directory Discovery;
- T1012 Query Registry;
- T1007 System Service Discovery;
- T1016 System Network Configuration Discovery;
- T1049 System Network Connections Discovery;
- T1069 Permission Groups Discovery;
- T1087 Account Discovery;
- T1124 System Time Discovery;
- T1518 Software Discovery;
- T1518.001 Security Software Discovery;
- T1497 Virtualization/Sandbox Evasion;
- T1614 System Location Discovery;
- T1018 Remote System Discovery.

## Collection

- T1113 Screen Capture;
- T1115 Clipboard Data;
- T1123 Audio Capture;
- T1056 Input Capture;
- T1005 Data from Local System;
- T1114 Email Collection;
- T1530 Data from Cloud Storage where applicable;
- T1560 Archive Collected Data;
- T1074 Data Staged;
- T1213 Data from Information Repositories;
- T1185 Browser Session Cookie;
- T1539 Steal Web Session Cookie;
- application-specific credential/session collection.

## Command and Control

User-mode research should cover:

- T1071.001 Web Protocols;
- T1071.002 File Transfer Protocol;
- T1071.003 Mail Protocols;
- T1071.004 DNS;
- T1071.005 Publish/Subscribe Protocols;
- T1573 Encrypted Channel;
- T1573.001 Symmetric Cryptography;
- T1573.002 Asymmetric Cryptography;
- T1090 Proxy;
- T1090.001 Internal Proxy;
- T1090.002 External Proxy;
- T1090.003 Multi-hop Proxy;
- T1090.004 Domain Fronting;
- T1102 Web Service;
- T1102.001 Dead Drop Resolver;
- T1568 Dynamic Resolution;
- T1568.001 Fast Flux DNS;
- T1568.002 Dynamic DNS;
- T1008 Fallback Channels;
- T1104 Multi-Stage Channels;
- T1055/IPC-based covert channels where relevant;
- named pipes and other IPC as supporting evidence.

The goal is to detect **communication behavior and infrastructure relationships**, not to reproduce operational C2 infrastructure.

## Impact / Destructive Behavior

The malware simulator should also model defensively:

- T1485 Data Destruction;
- T1486 Data Encrypted for Impact;
- T1490 Inhibit System Recovery;
- T1489 Service Stop;
- T1491 Defacement;
- T1561 Disk Wipe;
- T1531 Account Access Removal.

These should be implemented only as inert or reversible simulations in Chronos.

---

# 4. Kernel Coverage

## ATT&CK-aligned

- T1068 Exploitation for Privilege Escalation.
- T1543.003 Windows Service.
- T1547.006 Kernel Modules and Extensions.
- T1014 Rootkit.
- T1562.001 Impair Defenses.
- T1574 execution-flow hijacking where kernel-relevant.
- T1542 cross-layer boot manipulation.
- T1210 exploitation of remote services where kernel impact is involved.

## Windows kernel mechanisms outside one-to-one ATT&CK mapping

Chronos should explicitly study these mechanisms:

### Driver loading

- ordinary signed driver loading;
- vulnerable signed driver abuse;
- test-signed development drivers;
- legacy/blocked drivers;
- filter drivers;
- minifilter drivers;
- boot-start drivers.

### Kernel interception

- process-create callbacks;
- thread-create callbacks;
- image-load callbacks;
- registry callbacks;
- object callbacks;
- filesystem filter paths;
- network filtering paths;
- WFP callouts;
- NDIS filtering.

### Control-flow manipulation

- kernel inline hooks;
- dispatch-table/IRP manipulation;
- system-service interception concepts;
- callback redirection;
- import/export and function-pointer manipulation.

Chronos should document these as **mechanism classes**, not provide bypass recipes.

### Kernel-state concealment

- DKOM-style object manipulation;
- process visibility inconsistency;
- driver visibility inconsistency;
- service visibility inconsistency;
- network visibility inconsistency;
- filesystem filter concealment.

### Memory abuse

- arbitrary kernel read/write as a vulnerability primitive;
- use-after-free;
- type confusion;
- race-condition exploitation;
- pool corruption;
- out-of-bounds access;
- function-pointer corruption;
- code/data integrity violation.

The research purpose is to understand the resulting security boundary failure, not provide exploit construction instructions.

### Defense impairment

- security-driver interference;
- security-process interference via privileged interfaces;
- logging/telemetry interference;
- integrity-policy tampering;
- vulnerable-driver installation.

### Defensive coverage

Kernel detectors should consider:

- Sysmon Driver Load Event 6;
- Code Integrity logs;
- service creation;
- registry modifications;
- PnP/driver installation events;
- driver signer/hash/version;
- vulnerable-driver inventory;
- HVCI/Memory Integrity state;
- App Control policy;
- Tamper Protection state;
- kernel memory acquisition;
- crash/dump evidence;
- hypervisor/VBS state.

Microsoft documents Sysmon Driver Load, Process Access, Image Load, Registry, WMI, RawAccessRead and ProcessTampering events as available telemetry surfaces. ([Microsoft Sysmon events](https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-events))

---

# 5. Firmware Coverage

## ATT&CK-aligned

Current Pre-OS Boot contains:

- T1542.001 System Firmware;
- T1542.002 Component Firmware;
- T1542.003 Bootkit;
- T1542.004 ROMMONkit;
- T1542.005 TFTP Boot.

Only the first three are directly relevant to the Windows/UEFI Chronos target; the final two belong primarily to network-device research.

MITRE's current T1542 catalog has five sub-techniques. ([MITRE T1542](https://attack.mitre.org/techniques/T1542/))

Chronos should additionally cover:

- T1195.003 Compromise Hardware Supply Chain;
- T1195.002 Compromise Software Supply Chain;
- T1553 Subvert Trust Controls;
- T1553.006 Code Signing Policy Modification;
- T1601 Modify System Image where network-device variants become relevant;
- cross-layer Secure Boot/bootloader trust attacks.

## Firmware mechanism coverage

### Platform firmware

- SPI flash integrity;
- firmware-volume modification;
- DXE driver insertion;
- PEI/DXE phase abuse;
- firmware variable/NVRAM manipulation;
- firmware update/capsule abuse;
- recovery-image abuse;
- vendor updater trust-chain abuse.

### Boot chain

- EFI System Partition modification;
- bootloader replacement;
- boot configuration manipulation;
- boot-manager trust abuse;
- boot-chain rollback conditions;
- bootkit state persistence;
- recovery-environment abuse.

### Hardware/option firmware

- PCIe Option ROMs;
- NIC firmware;
- storage-controller firmware;
- SSD/HDD firmware;
- GPU/adapter firmware;
- Thunderbolt/other peripheral firmware where platform support exists.

### SMM / privileged firmware

- System Management Mode trust boundary;
- SMRAM isolation;
- SMI/SMM handler integrity;
- firmware-level control outside the Windows kernel.

Chronos should treat SMM as an architectural research topic and focus on trust/measurement/detection rather than providing implant construction.

### ACPI

- DSDT/SSDT integrity;
- AML compilation;
- table replacement;
- table injection;
- namespace changes;
- OS-visible firmware state;
- baseline comparison.

### Defensive firmware controls

- Secure Boot;
- Trusted Boot;
- Early Launch Anti-Malware;
- Measured Boot;
- TPM PCR measurements;
- remote attestation;
- System Guard / DRTM where supported;
- firmware signature validation;
- hardware root of trust;
- SPI write protections;
- signed capsule updates;
- rollback protection;
- vendor recovery/reflash;
- component-firmware inventory;
- platform health attestation.

Microsoft documents Secure Boot, Trusted Boot, ELAM and Measured Boot as complementary startup protections. ([Microsoft secure boot process](https://learn.microsoft.com/en-us/windows/security/operating-system-security/system-security/secure-the-windows-10-boot-process))

---

# 6. Detection Engine Requirements

The future Chronos detection engine should not be a signature database.

It should operate across five evidence dimensions:

## 6.1 Identity

- hash;
- signer;
- certificate chain;
- publisher;
- version;
- path;
- firmware identity.

## 6.2 Lineage

- parent/child process;
- creator;
- service initiator;
- driver installer;
- boot component relationship.

## 6.3 State transition

- memory changed;
- registry changed;
- file changed;
- driver loaded;
- firmware changed;
- boot state changed;
- security context changed.

## 6.4 Temporal correlation

~~~text
A
 →
B
 →
C
 →
D
~~~

should be treated differently from isolated A, B, C or D events.

## 6.5 Cross-view consistency

Compare:

~~~text
process view
+
filesystem view
+
memory view
+
driver view
+
boot/firmware view
+
network view
~~~

A discrepancy between views can be more valuable than an individual indicator.

---

# 7. Coverage Maintenance

The catalog must be treated as a living security specification.

When ATT&CK adds or changes:

- technique;
- sub-technique;
- detection strategy;
- mitigation;
- data component;

Chronos should update the relevant domain document and coverage matrix.

Every capability should have a status:

~~~text
DISCOVERED
  ↓
DOCUMENTED
  ↓
SIMULATED
  ↓
TELEMETRY CAPTURED
  ↓
DETECTOR IMPLEMENTED
  ↓
VALIDATED
  ↓
REGRESSION COVERED
~~~

This prevents documentation from claiming coverage that the codebase has not actually implemented.

---

# 8. Completeness Principle

"All known methods" should be interpreted operationally as:

> every relevant technique known to the selected threat-model corpus, plus platform mechanisms not represented one-to-one in that corpus, with explicit tracking of what has and has not been implemented.

This is stronger than claiming the impossible standard of knowing every private, undisclosed or future technique.

The goal of Chronos is therefore not merely a large malware codebase.

It is a **living adversary-behavior coverage system** whose detector can be benchmarked against a documented capability matrix.
