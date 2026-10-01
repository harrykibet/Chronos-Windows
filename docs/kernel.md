# Chronos Kernel Research

## Purpose

Chronos.Kernel studies advanced Windows kernel capabilities relevant to sophisticated threats. The current repository implementation is a minimal WDM driver foundation, not a completed rootkit.

The kernel boundary matters because kernel code executes inside the operating system's privileged environment. A compromise can influence memory, device access, driver state, process visibility and security enforcement.

## 1. WDM, Visual Studio and WDK

Chronos.Kernel is configured with:

~~~text
ConfigurationType = Driver
DriverType        = WDM
PlatformToolset   = WindowsKernelModeDriver10.0
DebuggerFlavor    = DbgengKernelDebugger
~~~

WDM is the driver model. Visual Studio is the development environment. The WDK supplies driver-specific headers, libraries, tools and integration.

Microsoft's security checklist treats kernel drivers as high-impact components because a kernel error affects the entire operating system.

**Foundation source:**  
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

## 2. Driver-Based Kernel Execution / BYOVD

### Capability

T1068 covers exploitation for privilege escalation, including Bring Your Own Vulnerable Driver patterns where an attacker introduces or abuses a signed vulnerable driver to obtain kernel-level capability.

### Offensive perspective

The key advantage is crossing:

~~~text
user mode
   ↓
kernel mode
   ↓
security-sensitive state
~~~

The vulnerable driver may be legitimate software; the malicious behavior comes from exploiting its trust and vulnerable interface.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1068/

### Defensive perspective

Detection and prevention should consider driver installation, signature state, signer, version, path, hash, vulnerability status and subsequent privileged behavior.

Microsoft provides layered protections including the vulnerable-driver blocklist, ASR protection against exploited vulnerable signed drivers and App Control for Business.

**Detection / prevention source:**  
https://learn.microsoft.com/en-us/defender-endpoint/attack-surface-reduction-rules-reference  
https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/design/microsoft-recommended-driver-block-rules

## 3. Service-Based Driver Persistence

### Capability

Windows services can be used to execute drivers during system startup. ATT&CK documents this under T1543.003.

### Offensive perspective

The service-control subsystem provides an ordinary Windows mechanism for SYSTEM-level background execution.

ATT&CK documents real-world adversary use, including APT activity, where services were used to load malicious components.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1543/003/

### Defensive perspective

Correlate:

~~~text
service creation/modification
      +
registry modification
      +
new executable/.sys artifact
      +
driver load
      +
SYSTEM execution
~~~

MITRE's current analytic identifies Security Event 4697, Sysmon Process Create, Registry events and Driver Load as useful data sources.

**Detection source:**  
https://attack.mitre.org/detectionstrategies/DET0552/  
https://learn.microsoft.com/en-us/windows/security/operating-system-security/sysmon/sysmon-configuration-files

## 4. Rootkit / Visibility Manipulation

### Capability

T1014 describes rootkits that hide programs, files, network connections, services, drivers or other system components.

The fundamental problem is:

~~~text
security tool asks the OS for truth
             ↓
compromised kernel alters the answer
~~~

### Offensive perspective

The capability is valuable because it attacks the observer's trust relationship with the operating system itself.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1014/

### Defensive perspective

Do not rely on one visibility source. Correlate driver-load evidence, Code Integrity records, independent telemetry, memory forensics and boot-integrity state where appropriate.

**Detection source:**  
https://attack.mitre.org/datacomponents/DC0079/  
https://learn.microsoft.com/en-us/windows-hardware/drivers/install/code-integrity-diagnostic-system-log-events

## 5. Kernel-Based Defense Impairment

### Capability

A privileged component may attempt to interfere with security controls or their data paths.

The key security property is:

~~~text
defensive control
      ↓
enforcement boundary
      ↓
privileged code
      ↓
attempted impairment
~~~

### Offensive perspective

APT-level kernel abuse matters because the attacker is closer to the mechanisms that enforce user-mode security.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1562/001/

### Defensive perspective

Microsoft recommends layered driver defenses, including Tamper Protection, the vulnerable-driver blocklist, ASR and App Control policies.

**Detection / prevention source:**  
https://learn.microsoft.com/en-us/defender-endpoint/tamper-resiliency  
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

## 6. Kernel Memory and Integrity Abuse

### Capability

Kernel-level code can interact with memory and operating-system state that ordinary applications cannot access.

The APT-level research question is how a vulnerability or privileged driver can turn a memory-safety or trust failure into control over security-sensitive state.

### Offensive perspective

The useful abstraction is privilege transition through a vulnerable kernel component, not a particular exploit implementation.

**Offensive/tradecraft source:**  
https://attack.mitre.org/techniques/T1068/

### Defensive perspective

HVCI / Memory Integrity attempts to isolate code-integrity decisions and constrains executable kernel memory behavior.

**Detection / prevention source:**  
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/implement-hvci-compatible-code  
https://learn.microsoft.com/en-us/windows-hardware/test/hlk/testref/driver-compatibility-with-device-guard

## 7. Driver Trust Model

Chronos should treat driver trust as multidimensional:

~~~text
signed?
signer expected?
version expected?
known vulnerable?
where did it originate?
why was it loaded?
what happened after load?
~~~

A valid signature is evidence of provenance, not proof of benign intent.

## 8. Kernel Detection Timeline

A mature analytic should correlate:

~~~text
user-mode precursor
      ↓
driver artifact creation
      ↓
service / registration
      ↓
Code Integrity decision
      ↓
driver load
      ↓
subsequent privileged behavior
~~~

This is stronger than treating a driver file, signature or service in isolation.

## 9. Safety and Debugging

Use disposable VMs, kernel debugging, snapshots, symbols and carefully controlled driver signing for research.

Microsoft explicitly advises that development/test drivers must not be production-signed and recommends security review, Driver Verifier, CodeQL and HVCI-compatible design practices.

**Source:**  
https://learn.microsoft.com/en-us/windows-hardware/drivers/driversecurity/driver-security-checklist

## 10. Research Questions

- Which driver artifacts remain visible even after user-mode tampering?
- What signals separate normal driver updates from suspicious driver loading?
- How does HVCI change the viable kernel attack surface?
- How should BYOVD activity be correlated with its user-mode precursor?
- How can defenders validate kernel state using evidence not controlled by the compromised kernel?
