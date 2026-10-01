# Chronos User-Mode Research

## Mission

Chronos.UserMode studies advanced Windows user-mode capabilities that appear in sophisticated intrusion chains.

The project is organized around mechanisms rather than malware families:

~~~text
Windows primitive
      ↓
capability
      ↓
execution effect
      ↓
telemetry
      ↓
detection
      ↓
prevention
~~~

The objective is to understand what an attacker gains, what Windows state changes, what evidence survives, and which controls can prevent or constrain the behavior.

---

# 1. Capability Map

| Capability | ATT&CK | APT value | Defensive anchor |
|---|---|---|---|
| Process injection | T1055 | Hide execution inside another process | Cross-process memory/thread events |
| Process hollowing | T1055.012 | Separate process identity from effective code | Process/memory consistency |
| Reflective loading | T1620 | Reduce disk-backed provenance | Memory/thread/module evidence |
| Token manipulation | T1134 | Alter security context | Token operations + resulting identity |
| LOLBin/proxy execution | T1218 | Reuse trusted software | Contextual process analytics |
| Command/scripting abuse | T1059 | Use native execution environments | Script/process lineage |
| User-mode persistence | T1547.001 | Survive sessions/reboots | Configuration change + execution |
| Obfuscation | T1027 | Defeat static signatures | Reconstruction behavior |
| Process tampering | T1564-related/defense evasion | Change expected process state | Process integrity telemetry |

---

# 2. Process Injection

## Capability

T1055 covers execution of code in the address space of another live process. Current ATT&CK lists multiple sub-techniques including DLL injection, PE injection, thread execution hijacking, APC, process hollowing and process doppelgänging.

Source:
https://attack.mitre.org/techniques/T1055/

### Offensive perspective

The architectural advantage is execution-context substitution:

~~~text
malicious logic
      ↓
victim process context
      ↓
execution
~~~

ATT&CK explicitly describes process injection as a way to evade process-based defenses and potentially access another process's resources or privileges.

### Defensive perspective

MITRE's current behavioral detection correlates memory manipulation, suspicious thread creation and unusual module loading.

Source:
https://attack.mitre.org/techniques/T1055/

Microsoft Sysmon can contribute:

- Event 1 Process Create;
- Event 7 Image Load;
- Event 8 CreateRemoteThread;
- Event 10 ProcessAccess.

Source:
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-events

### Detection hypothesis

~~~text
process A accesses process B
        +
memory-related activity
        +
thread/module change
        +
unexpected ancestry
        =
higher-confidence injection hypothesis
~~~

### Benign equivalents

Legitimate software may perform similar actions:

- debuggers;
- accessibility software;
- security products;
- profilers;
- application compatibility tooling.

This is why one event should rarely become the detector.

### Prevention

- endpoint behavioral prevention;
- process protection for high-value applications;
- ASR where applicable;
- least privilege;
- application control.

---

# 3. Process Hollowing

## Capability

T1055.012 describes process hollowing.

The broad concept is a legitimate process identity combined with altered effective execution state.

### Offensive perspective

The attacker attempts to separate:

~~~text
initial executable identity
          ≠
effective code identity
~~~

This is useful when analysts trust only the process image that originally created the process.

Source:
https://attack.mitre.org/techniques/T1055/012/

### Defensive perspective

Look for inconsistencies among:

- original image;
- mapped image state;
- memory regions;
- thread start;
- module inventory;
- signer;
- parent process.

The goal is a consistency check rather than a single "hollow process" signature.

### Research question

Which cross-view inconsistencies remain measurable after the process is running normally?

---

# 4. Reflective Code Loading

## Capability

T1620 covers execution of code from process memory without relying on a conventional disk-backed module transition.

~~~text
disk-backed
artifact → image → execution

memory-oriented
data → memory → execution
~~~

### Offensive perspective

This reduces ordinary file provenance.

Source:
https://attack.mitre.org/techniques/T1620/

### Defensive perspective

Investigate discrepancies among:

- executable/module inventory;
- executable private memory;
- thread start locations;
- cross-process access;
- process ancestry.

Use memory evidence together with process telemetry rather than treating memory alone as malicious.

---

# 5. Access Token Manipulation

## Capability

T1134 covers abuse of Windows access-token semantics, including impersonation/theft.

### Offensive perspective

Tokens define effective security identity and privileges.

The important transition is:

~~~text
process
 ↓
token selection
 ↓
effective security context
 ↓
resource access
~~~

Source:
https://attack.mitre.org/techniques/T1134/

### Defensive perspective

Correlate:

- token duplication/impersonation;
- resulting SID/user;
- integrity level;
- process creation;
- privileged resource access.

A high-value feature is a mismatch between the expected role of a process and the security context under which it performs sensitive work.

Detection source:
https://attack.mitre.org/techniques/T1134/001/

---

# 6. Living-off-the-Land and System Binary Proxy Execution

## Capability

T1218 covers abuse of trusted operating-system binaries.

### Offensive perspective

Native utilities already exist, have legitimate uses and may carry trusted signatures.

The core distinction is:

~~~text
trusted binary
≠
trusted invocation
~~~

Source:
https://attack.mitre.org/techniques/T1218/

### Defensive perspective

MITRE's current proxy-execution detection correlates trusted Microsoft-signed binaries with process creation, unusual parentage, module loads and network connections.

Source:
https://attack.mitre.org/detectionstrategies/DET0081/

For Regsvr32, MITRE recommends examining unusual DLL/scriptlet paths, network activity, signer state and expected parents.

Source:
https://attack.mitre.org/detectionstrategies/DET0282/

### Chronos research question

Can role-aware profiling distinguish legitimate administrative use from malicious proxy execution better than a static blocklist?

---

# 7. Command and Scripting Interpreter Abuse

## Capability

T1059 covers command and scripting interpreters.

### Offensive perspective

The value is availability: interpreters are already present, administrator workflows use them, and they can execute dynamically generated logic.

Source:
https://attack.mitre.org/techniques/T1059/

### Defensive perspective

Profile:

- parent;
- child;
- command-line structure;
- script host;
- encoding/obfuscation;
- user;
- destination;
- resulting files.

A scripting engine becomes much more suspicious when its surrounding execution graph is unusual.

### Research question

Which features distinguish administrative automation from adversary command execution without banning the interpreter itself?

---

# 8. User-Mode Persistence

## Capability

T1547.001 documents Registry Run Keys and Startup Folder persistence.

### Offensive perspective

The objective is to convert transient execution into recurring execution during normal user startup.

Source:
https://attack.mitre.org/techniques/T1547/001/

### Defensive perspective

Correlate:

~~~text
configuration change
      +
artifact provenance
      +
startup trigger
      +
subsequent process creation
~~~

Sysmon Registry Events 12–14 plus Event 1 Process Create can provide useful evidence.

Source:
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-events

### False positives

Normal installers and enterprise software often modify startup configuration.

Therefore detection should use:

- signer;
- install source;
- user;
- path;
- first-seen time;
- change initiator;
- subsequent behavior.

---

# 9. Obfuscation

## Capability

T1027 changes representation of commands, scripts or files to reduce static detectability.

MITRE documents APT19, APT32 and Aquatic Panda among actors using command or script obfuscation.

Sources:
https://attack.mitre.org/techniques/T1027/
https://attack.mitre.org/techniques/T1027/010/

### Defensive perspective

MITRE's current behavioral detection correlates suspicious processes with creation or modification of encoded/compressed/encrypted content and abnormal command syntax.

Source:
https://attack.mitre.org/detectionstrategies/DET0378/

Chronos should measure whether behavioral features remain stable when representation changes.

---

# 10. Process Tampering

Process tampering is best treated as an integrity problem:

~~~text
expected process state
        ↓
unexpected modification
        ↓
different execution behavior
~~~

### Defensive perspective

Investigate changes together with:

- process identity;
- memory state;
- thread state;
- module inventory;
- parent/child lineage.

Sysmon Event 25 provides Process Tampering telemetry.

Source:
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-events

---

# 11. User-Mode APT Behavior Graph

A useful abstraction is:

~~~text
initial execution
      ↓
discovery/configuration
      ↓
security-context manipulation
      ↓
process/memory manipulation
      ↓
trusted execution
      ↓
persistence
      ↓
network communication
      ↓
handoff to another privilege boundary
~~~

Each arrow is a possible detection opportunity.

The final detector should be based on relationships, not merely the presence of individual capabilities.

---

# 12. Detection Research Matrix

Every capability should record:

| Field | Required analysis |
|---|---|
| Primitive | Which Windows facility is being abused? |
| Preconditions | What access/privilege is required? |
| State change | What object or process changes? |
| APT value | Stealth, privilege, persistence, execution |
| Telemetry | Which events observe it? |
| Blind spots | What remains invisible? |
| Benign analogues | Who legitimately does the same thing? |
| Detector | What behavioral correlation is proposed? |
| Prevention | Which control constrains it? |
| Validation | How is precision measured? |

---

# 13. Validation Method

Use at least three workloads:

~~~text
A. normal application
B. legitimate security/admin tool
C. synthetic suspicious analogue
~~~

Capture telemetry, align event timestamps and compare process graphs.

Measure:

- event coverage;
- precision;
- false positives;
- detection latency;
- contextual features;
- prevention effectiveness.

Do not validate only by asking whether a single alert fired.

---

# 14. Research Questions

- Which injection signals are stable across different implementations?
- What evidence remains when execution moves to memory?
- How can token manipulation be separated from legitimate impersonation?
- Which trusted binaries create the most detection ambiguity?
- Which startup mechanisms are common enough to create false positives?
- Which behavioral features survive command obfuscation?
- Which user-mode signals remain available after an attacker changes process state?

---

# 15. Safety Boundary

Chronos.UserMode should remain a controlled behavioral research environment.

Do not evolve it into:

- a credential-harvesting system;
- a general post-exploitation framework;
- a remote-access toolkit;
- autonomous lateral-movement tooling;
- destructive payload infrastructure.

The research target is Windows mechanism + telemetry + detection + prevention.
