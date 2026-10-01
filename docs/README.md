# Chronos Documentation

Chronos documentation is the technical research layer behind the project root README.

The four guides are structured as research specifications rather than simple technique summaries.

~~~text
capability
   ↓
prerequisites
   ↓
mechanism
   ↓
APT relevance
   ↓
observable state change
   ↓
telemetry
   ↓
detection hypothesis
   ↓
prevention / mitigation
   ↓
lab validation
   ↓
false positives + limitations
~~~

## Guides

| Guide | Domain | Research focus |
|---|---|---|
| [Loader Research](loader.md) | Loader / dropper | Staging, trusted proxy execution, memory-oriented loading, obfuscation, masquerading, execution gating and provenance |
| [User-Mode Research](usermode.md) | User-mode malware | Process injection, process hollowing, reflective loading, token manipulation, LOLBins, command interpreters, persistence, obfuscation and process integrity |
| [Kernel Research](kernel.md) | Kernel / rootkit | WDM, privilege escalation, BYOVD, service-loaded drivers, rootkits, callback paths, object/process visibility, kernel memory and integrity |
| [Firmware Research](firmware.md) | UEFI / firmware | System/component firmware, bootkits, EFI/ESP, UEFI variables, ACPI, EDK II, IASL, Secure Boot and Measured Boot |

## Two-Sided Research Model

Every capability is studied from two directions.

### Offensive understanding

What security property does the capability give to a sophisticated threat actor?

This includes:

- required trust or privilege;
- underlying Windows/UEFI primitive;
- why the capability is useful;
- what limitations it has;
- how it interacts with adjacent stages.

### Defensive understanding

What evidence remains visible, and which control can detect or constrain it?

This includes:

- telemetry sources;
- behavioral correlation;
- legitimate equivalents;
- false-positive pressure;
- prevention controls;
- evidence blind spots;
- validation methodology.

The offensive material is intentionally mechanism-oriented rather than an operational playbook for deploying attacks against real systems.

## APT Relevance

The capability documents prioritize behaviors that map to documented ATT&CK techniques and real-world threat reporting.

Examples include:

- staged tool transfer;
- trusted-binary proxy execution;
- process injection;
- token manipulation;
- vulnerable-driver / BYOVD abuse;
- service-based driver execution;
- rootkit behavior;
- system/component firmware persistence;
- bootkits;
- pre-OS defense evasion.

MITRE ATT&CK currently documents real-world procedure examples for these techniques, including APT and ransomware-group activity.

## Research Quality Standard

A capability is not considered sufficiently documented until Chronos can explain:

1. mechanism;
2. required privilege/trust;
3. APT relevance;
4. legitimate equivalents;
5. observable telemetry;
6. detection hypothesis;
7. false-positive sources;
8. prevention/mitigation;
9. validation experiment;
10. residual uncertainty.

## Source Hierarchy

Prefer:

- MITRE ATT&CK for adversary behavior and technique taxonomy;
- Microsoft for Windows telemetry, Code Integrity, HVCI, Defender and boot security;
- TianoCore for EDK II architecture and build tooling;
- UEFI Forum and ACPICA for firmware/ACPI specifications;
- high-quality incident-response and threat-intelligence reports for documented incidents.

The documentation distinguishes:

~~~text
documented adversary behavior
      ≠
laboratory hypothesis
      ≠
detection heuristic
      ≠
prevention guarantee
~~~

## Research Method

Every major Chronos capability should eventually produce this evidence chain:

~~~text
controlled setup
   ↓
known-good baseline
   ↓
controlled behavior
   ↓
telemetry capture
   ↓
timeline / graph
   ↓
defensive hypothesis
   ↓
detector
   ↓
benign control
   ↓
false-positive analysis
   ↓
mitigation validation
~~~

## What "APT-Level" Means in Chronos

APT-level does not mean "maximum complexity."

It means the capability is evaluated against the properties that make sophisticated campaigns difficult to defend:

- multi-stage execution;
- privilege transitions;
- trusted-component abuse;
- persistence across trust boundaries;
- defense evasion;
- stealth and provenance manipulation;
- pre-OS execution;
- low-level integrity attacks;
- deliberate separation of capability from observable artifacts.

The goal is to understand those properties well enough to build stronger detection and prevention mechanisms.

## Laboratory Boundary

Chronos experiments belong in controlled environments:

- disposable virtual machines;
- kernel-debuggable research images;
- firmware emulators/VMs;
- synthetic data;
- reversible configuration;
- isolated networks.

The repository should remain a research platform for understanding mechanisms and validating defenses, not a generalized offensive deployment framework.
