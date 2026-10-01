# Chronos Kernel Research

## Mission

Chronos.Kernel studies the boundary between Windows user mode and kernel mode and the advanced capabilities that become possible when a threat obtains privileged execution.

The repository currently contains a minimal WDM driver foundation. It should be treated as a research starting point, not as a completed rootkit.

The research model is:

~~~text
entry path
   ↓
driver execution
   ↓
kernel primitive
   ↓
security-relevant effect
   ↓
telemetry
   ↓
integrity control
~~~

---

# 1. Capability Map

| Capability | ATT&CK / concept | APT value | Defensive anchor |
|---|---|---|---|
| Privilege escalation | T1068 | Cross user→kernel boundary | Exploit mitigation + driver controls |
| BYOVD | T1068 | Abuse trusted vulnerable driver | Driver provenance + blocklist |
| Service-based driver loading | T1543.003 | Persistence / SYSTEM execution | Service + registry + driver telemetry |
| Kernel rootkit | T1014 | Modify visibility | Independent telemetry/integrity |
| Kernel defense impairment | T1562.001 | Weaken enforcement | HVCI + Tamper Protection + App Control |
| Kernel memory abuse | T1068 / integrity | Modify privileged state | HVCI / VBS / Code Integrity |
| Callback/event interception | Kernel architecture | Observe/alter control flow | Driver provenance + integrity |
| Process/object visibility manipulation | T1014 | Hide execution | Cross-view consistency |
| Vulnerable interface abuse | T1068 | Privileged operations | Vulnerability inventory |
| Early-boot manipulation | T1542 / T1014 | Lower-layer control | Secure/Measured Boot |

---

# 2. WDM, Visual Studio and WDK

Chronos.Kernel is configured as:

~~~text
ConfigurationType = Driver
DriverType = WDM
PlatformToolset = WindowsKernelModeDriver10.0
DebuggerFlavor = DbgengKernelDebugger
~~~

WDM is the Windows driver architecture. Visual Studio is the development environment. The WDK provides driver-specific headers, libraries, tools and build/debug integration.

Microsoft's driver security guidance treats kernel code as high-impact because an unsafe driver can affect the entire operating system.

Source:
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

---

# 3. Privilege Escalation and BYOVD

## Capability

T1068 covers exploitation for privilege escalation. MITRE specifically describes Bring Your Own Vulnerable Driver as an adversary bringing a signed vulnerable driver to a compromised machine and using its vulnerability to obtain kernel-level capability.

MITRE's current page documents real-world examples, including Embargo using a vulnerable driver.

Source:
https://attack.mitre.org/techniques/T1068/

### Offensive perspective

The security transition is:

~~~text
user-level foothold
      ↓
vulnerable privileged component
      ↓
kernel execution capability
~~~

The driver itself can be legitimate. The malicious part is the abuse of its trusted and vulnerable interface.

### Defensive perspective

Build a driver trust record from:

- driver name;
- signer;
- certificate state;
- version;
- hash;
- vulnerability status;
- origin;
- installing process;
- installation time;
- Code Integrity decision;
- subsequent behavior.

Microsoft provides the vulnerable-driver blocklist and an ASR rule that blocks abuse of exploited vulnerable signed drivers.

Sources:
https://learn.microsoft.com/en-us/defender-endpoint/attack-surface-reduction-rules-reference
https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/design/microsoft-recommended-driver-block-rules

### Detection hypothesis

~~~text
unexpected driver provenance
      +
known vulnerability
      +
unusual installation initiator
      +
driver load
      +
privileged post-load behavior
~~~

is more informative than "unsigned driver" alone.

### Chronos research question

Can a detector distinguish:

~~~text
expected signed driver
vs.
signed but vulnerable driver
vs.
unexpected malicious driver
~~~

without depending entirely on file hashes?

---

# 4. Service-Based Driver Loading

## Capability

T1543.003 covers creation or modification of Windows services.

### Offensive perspective

The Service Control Manager is an ordinary Windows mechanism that can support persistent background execution and can participate in driver loading.

MITRE's current technique page documents driver installation and CreateService-related activity and its detection strategy correlates service creation, registry changes, startup behavior and anomalous paths.

Sources:
https://attack.mitre.org/techniques/T1543/003/
https://attack.mitre.org/detectionstrategies/DET0552/

### Defensive perspective

A useful chain is:

~~~text
service creation/modification
      ↓
registry change
      ↓
new executable/.sys artifact
      ↓
driver load
      ↓
SYSTEM execution
~~~

Microsoft Sysmon Event 6 records driver loads with signature/hash context, while Registry Events 12–14 and Process Create Event 1 support surrounding analysis.

Source:
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-events

### False-positive sources

- legitimate driver installers;
- hardware vendors;
- security products;
- virtualization software;
- enterprise deployment systems.

### Research question

Which service/driver attributes produce the strongest separation between normal software installation and intrusion-driven driver persistence?

---

# 5. Kernel Rootkits

## Capability

T1014 covers rootkits that hide programs, files, network connections, services, drivers or other system components.

MITRE explicitly describes rootkits as potentially operating at user, kernel, hypervisor or firmware levels and records APT28's LoJax as a real-world example.

Source:
https://attack.mitre.org/techniques/T1014/

### Offensive perspective

The central capability is observer manipulation.

~~~text
security tool
      ↓
kernel query
      ↓
compromised kernel
      ↓
modified answer
~~~

This attacks the assumption that the operating system's own view is authoritative.

### Defensive perspective

Use multiple trust domains:

- driver inventory;
- Code Integrity;
- EDR telemetry;
- kernel debugging;
- memory acquisition;
- boot measurements;
- offline analysis.

MITRE's current rootkit detection strategy looks for anomalous kernel drivers, concealed services and abnormal boot-component modifications.

Source:
https://attack.mitre.org/techniques/T1014/

### Research question

Which system views can be compared to identify inconsistent state without assuming any single view is authoritative?

---

# 6. Callback and Event-Path Interception

Windows provides legitimate kernel callback mechanisms for observing operating-system events.

These are important to study because a malicious privileged component may try to observe or influence activity at a low level.

### Offensive perspective

The security advantage is proximity to the event source:

~~~text
high-level observer
       ↓
kernel event path
       ↓
security-relevant state
~~~

The Chronos objective is to understand privileged interception as a security primitive, not to provide a ready-made interception framework.

### Defensive perspective

Research should establish:

- expected callback registrations;
- owning driver;
- registration lifetime;
- signer/provenance;
- driver unload behavior;
- integrity of the surrounding subsystem.

### Research question

Can an unexpected privileged observer be detected from registration/provenance evidence even when it produces little user-mode activity?

---

# 7. Object and Process Visibility Manipulation

Rootkit-style behavior can attempt to create:

~~~text
actual state
      ≠
reported state
~~~

### Offensive perspective

The capability is valuable when a defender relies on normal process/service/file enumeration.

### Defensive perspective

Build cross-view tests:

~~~text
OS enumeration
      +
driver/Code Integrity telemetry
      +
memory image
      +
event timeline
      =
consistency analysis
~~~

A discrepancy is an investigation trigger, not automatic proof of malicious activity.

---

# 8. Kernel Defense Impairment

## Capability

T1562.001 covers disabling or modifying security tools and controls.

### Offensive perspective

Kernel privilege moves an attacker closer to the mechanisms that enforce user-mode security.

~~~text
security control
      ↓
enforcement boundary
      ↓
privileged modification attempt
~~~

Source:
https://attack.mitre.org/techniques/T1562/001/

### Defensive perspective

Microsoft's layered protections include:

- Tamper Protection;
- Memory Integrity/HVCI;
- vulnerable-driver blocklist;
- App Control for Business;
- ASR.

Sources:
https://learn.microsoft.com/en-us/defender-endpoint/tamper-resiliency
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

### Research question

Which controls still provide evidence when a user-mode security process is no longer trustworthy?

---

# 9. Kernel Memory and Code Integrity

## Capability

Kernel memory is security-critical because it holds executable code and operating-system state.

### Offensive perspective

The relevant security conversion is:

~~~text
memory-safety / trust failure
          ↓
privileged write capability
          ↓
security-sensitive state
~~~

Chronos should study this as a privilege/integrity problem, not as an exploit-development cookbook.

Source:
https://attack.mitre.org/techniques/T1068/

### Defensive perspective

HVCI uses virtualization-based security to isolate Code Integrity decision-making. Microsoft requires HVCI-compatible drivers to use NX memory, avoid writable+executable sections, avoid direct modification of executable system memory and avoid dynamic kernel code.

Sources:
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/implement-hvci-compatible-code

### Research question

Which kernel behaviors become impossible or measurably harder under HVCI?

---

# 10. Driver Trust Is Multidimensional

Chronos should model trust as:

~~~text
signature
  ↓
signer
  ↓
version
  ↓
vulnerability status
  ↓
origin
  ↓
installation event
  ↓
load event
  ↓
post-load behavior
~~~

A valid signature proves something about provenance. It does not prove benign intent or absence of exploitable vulnerabilities.

---

# 11. Kernel Detection Timeline

A mature analytic should correlate:

~~~text
user-mode precursor
      ↓
artifact creation
      ↓
service / driver registration
      ↓
Code Integrity decision
      ↓
driver load
      ↓
kernel behavior
      ↓
security-impacting state change
~~~

This is stronger than treating a driver file, signature or service in isolation.

---

# 12. Debugging and Failure Analysis

Kernel experiments have a larger blast radius than user-mode experiments.

Potential failures include:

- bugchecks;
- deadlocks;
- corrupted kernel state;
- persistent test configuration;
- debugger instability;
- unrecoverable VM state.

Research workflow:

~~~text
snapshot
  ↓
attach kernel debugger
  ↓
run one experiment
  ↓
capture trace
  ↓
analyze state/crash
  ↓
revert snapshot
~~~

Microsoft's driver security guidance recommends security review, Driver Verifier, CodeQL and HVCI-compatible driver design.

Source:
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

---

# 13. Capability Research Matrix

| Field | Required analysis |
|---|---|
| Entry path | How privileged execution is obtained |
| Primitive | Driver/service/callback/memory |
| Privilege | Kernel/SYSTEM implications |
| Target | Process, memory, object, security state |
| APT value | Persistence, stealth, privilege, defense evasion |
| Telemetry | Driver/service/Code Integrity/etc. |
| Detection | Correlated behavioral hypothesis |
| Prevention | HVCI, App Control, blocklist, policy |
| Failure mode | Bugcheck, compatibility, false positives |
| Validation | Benign driver vs synthetic suspicious analogue |

---

# 14. Research Questions

- Which driver metadata best predicts malicious use?
- How effective is the vulnerable-driver blocklist against current BYOVD patterns?
- Which driver-loading signals survive user-mode tampering?
- How can kernel visibility manipulation be identified through independent views?
- Which behaviors change when HVCI is enabled?
- Which legitimate drivers produce the most false-positive pressure?
- Can service, registry, driver-load and post-load events be correlated into a high-confidence analytic?

---

# 15. Safety Boundary

Chronos.Kernel should remain a defensive laboratory.

Do not evolve it into:

- a general rootkit framework;
- an EDR bypass;
- a kernel persistence kit;
- destructive kernel logic;
- an unauthorized privilege-escalation toolkit.

The research target is kernel mechanism + telemetry + detection + prevention.
