# Chronos User-Mode Research

## Purpose

Chronos.UserMode studies advanced behavior inside ordinary Windows processes. The goal is to understand techniques that sophisticated malware may use for execution, stealth, security-context manipulation, persistence and communications, then determine which telemetry can distinguish those behaviors from legitimate software.

The core model is:

~~~text
Windows primitive
      ↓
malware capability
      ↓
observable behavior
      ↓
telemetry
      ↓
detection / prevention
~~~

## 1. Process Injection

### Capability

ATT&CK T1055 covers code execution inside another live process. Variants differ in mechanism, but the security property is consistent: execution is decoupled from the original process identity.

### Offensive perspective

Injection can let malicious logic execute in the context of a legitimate process and can complicate process-based detection.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1055/

### Defensive perspective

Correlate cross-process access, memory modification, suspicious remote thread creation, unusual DLL loads and process context. A single API call is usually weak evidence.

**Detection source:**  
MITRE T1055 detection strategy: https://attack.mitre.org/techniques/T1055/  
Microsoft Sysmon: https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-configuration-files

Important Sysmon events include CreateRemoteThread (8), ProcessAccess (10), ImageLoad (7) and ProcessTampering (25).

---

## 2. Reflective Code Loading

### Capability

T1620 describes code executing from process memory without the normal disk-backed execution path.

### Offensive perspective

Memory-oriented execution can reduce ordinary file and module provenance.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1620/

### Defensive perspective

Look for discrepancies between process identity and memory execution provenance, including executable private memory, unusual thread starts, suspicious process-access patterns and process tampering.

**Detection source:**  
https://attack.mitre.org/techniques/T1620/  
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-configuration-files

---

## 3. Access Token Manipulation

### Capability

Windows access tokens represent a security identity and associated privileges. T1134 covers abuse of token semantics, including token impersonation and theft.

### Offensive perspective

An attacker may attempt to separate the process that obtained access from the security identity under which later operations execute.

The security question becomes:

~~~text
Which token actually authorized this operation?
~~~

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1134/

### Defensive perspective

Correlate token duplication/impersonation behavior with the resulting security context and process creation. MITRE's current Windows detection strategy specifically models this behavior chain.

**Detection source:**  
https://attack.mitre.org/techniques/T1134/001/

---

## 4. Living-off-the-Land and Trusted Execution

### Capability

APT operators may prefer native interpreters, administrative components and signed binaries over deploying new tools when that reduces operational friction.

### Offensive perspective

The capability exploits the difference between:

~~~text
"this binary is normally trusted"
~~~

and:

~~~text
"this execution context is trustworthy"
~~~

Native functionality can therefore become an execution or staging mechanism.

**Offensive/tradecraft sources:**  
System Binary Proxy Execution: https://attack.mitre.org/techniques/T1218/  
Command and Scripting Interpreter: https://attack.mitre.org/techniques/T1059/

### Defensive perspective

Detection should be contextual. Correlate user, parent process, command line, child process, destination, signer and execution location.

Microsoft ASR explicitly targets behaviors such as script-based downloads, obfuscated scripts and code injection.

**Detection source:**  
https://learn.microsoft.com/en-us/defender-endpoint/attack-surface-reduction-rules-reference

---

## 5. User-Mode Persistence

### Capability

T1547.001 documents Registry Run Keys and Startup Folder persistence.

The architectural pattern is:

~~~text
configuration change
      ↓
normal Windows startup behavior
      ↓
unexpected executable
~~~

### Offensive perspective

Persistence converts a transient foothold into repeatable execution.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1547/001/

### Defensive perspective

Correlate registry changes, startup configuration, file provenance, signature, first execution, user identity and parent process.

**Detection source:**  
https://attack.mitre.org/techniques/T1547/001/  
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-configuration-files

---

## 6. Obfuscation

### Capability

T1027 covers transformation of commands and payloads to make static analysis harder.

### Offensive perspective

Obfuscation changes representation rather than semantics.

~~~text
representation A
      ↓
transformation
      ↓
representation B
      ↓
same behavior
~~~

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1027/

### Defensive perspective

Prefer behavioral detection over static signatures where possible. Useful evidence includes high-entropy or encoded content, decoding before execution, abnormal command syntax and suspicious script ancestry.

**Detection source:**  
https://attack.mitre.org/detectionstrategies/DET0378/  
https://learn.microsoft.com/en-us/defender-endpoint/attack-surface-reduction-rules-reference

---

## 7. User-Mode APT Research Model

A mature user-mode threat should be represented as a behavior graph:

~~~text
initial process
   ├── obtains data
   ├── accesses other processes
   ├── changes security context
   ├── manipulates memory
   ├── loads modules
   ├── communicates externally
   ├── establishes persistence
   └── transfers execution
~~~

The detector should reason over relationships rather than isolated events.

## 8. Research Questions

- Which signals are strong enough to identify process injection without hard-coded malware indicators?
- How much visibility disappears when execution moves to memory?
- How can legitimate administrative tooling be separated from malicious LOLBin use?
- Which token transitions are rare enough to be useful?
- Which persistence changes are normal in enterprise environments?
- Which telemetry survives attempts to hide the original executable?

## 9. Safety Boundary

Experiments should use isolated VMs, synthetic data, local test processes and reversible configuration changes. The user-mode project should not become a general post-exploitation or credential-harvesting framework.
