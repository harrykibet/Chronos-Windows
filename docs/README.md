# Chronos Documentation

Chronos documentation is organized around the four execution layers of the research platform.

Each guide intentionally combines offensive understanding with defensive analysis:

~~~text
threat capability
      ↓
how the mechanism works
      ↓
why an advanced threat values it
      ↓
observable behavior
      ↓
detection
      ↓
prevention / mitigation
~~~

## Research Guides

| Document | Domain | Focus |
|---|---|---|
| [Loader Research](loader.md) | Loader / Dropper | Staging, proxy execution, memory-oriented loading, obfuscation and masquerading |
| [User-Mode Research](usermode.md) | User-mode malware | Process injection, reflective loading, token manipulation, LOLBins, persistence and obfuscation |
| [Kernel Research](kernel.md) | Kernel / rootkit | WDM, BYOVD, service-based driver execution, rootkit visibility, kernel integrity |
| [Firmware Research](firmware.md) | UEFI / firmware | UEFI persistence, bootkits, pre-OS evasion, component firmware, ACPI and measured boot |

## Reading Model

For every capability:

1. Capability — what it does.
2. Offensive perspective — why it is useful to an advanced threat.
3. Detection perspective — what defenders can observe.
4. Sources — tradecraft and defensive references.
5. Chronos research question — what the laboratory should measure.

## Source Philosophy

Primary sources should favor:

- MITRE ATT&CK for adversary behavior and technique taxonomy
- Microsoft for Windows security controls, telemetry, Code Integrity, HVCI, Defender and boot integrity
- TianoCore for EDK II architecture and tooling
- UEFI Forum and ACPICA for firmware and ACPI specifications
- high-quality incident-response and threat-intelligence reports for documented real-world cases

Chronos documentation must distinguish:

~~~text
documented behavior
      ≠
research hypothesis
      ≠
detection heuristic
      ≠
prevention guarantee
~~~

## Research Standard

A useful Chronos experiment should produce:

~~~text
mechanism
  →
reproducible observation
  →
telemetry
  →
defensive hypothesis
  →
detector
  →
false-positive analysis
  →
mitigation validation
~~~

The objective is not to produce more malware. The objective is to make sophisticated threat behavior technically understandable enough to build better detection and prevention.
