# Chronos-Windows

## A Windows APT Research and Detection Engineering Laboratory

Chronos-Windows is an educational Windows systems-security research project designed to study how sophisticated multi-layer malware architectures can interact with **user mode, kernel mode, and firmware/UEFI execution environments**.

The purpose of Chronos is not to provide a production malware framework or an offensive attack platform.

The purpose is to make the underlying mechanisms understandable.

> **Understand how hostile code works so defensive systems can detect and prevent it based on evidence rather than assumptions.**

The project is intentionally structured around progressively deeper execution layers:

```text
                       CHRONOS
                          │
          ┌───────────────┼────────────────┐
          │               │                │
      User Mode       Kernel Mode       Firmware
          │               │                │
     ┌────┴────┐          │                │
     │         │          │                │
  Loader    Malware    Rootkit          UEFI / ACPI
  Dropper
          │               │                │
          └───────────────┼────────────────┘
                          │
                  Detection Research
                          │
              ┌───────────┴───────────┐
              │                       │
          Telemetry               Prevention
```

Chronos is therefore best understood as a **systems-security laboratory** rather than a conventional application.

---

## 1. What Problem Is Chronos Solving?

Modern security products frequently detect malicious behavior through signatures, heuristics, reputation systems, behavioral telemetry, and integrity measurements.

Those mechanisms are only as good as the defender's understanding of the underlying execution model.

A defender who sees:

```text
suspicious process
        ↓
unexpected driver
        ↓
kernel activity
```

needs to understand what actually happened underneath that observable behavior.

Likewise, a defender investigating firmware compromise needs to understand:

```text
Power-on
   ↓
UEFI firmware
   ↓
boot services
   ↓
DXE execution
   ↓
boot manager
   ↓
Windows
   ↓
user processes
```

Chronos exists to study these layers from the inside.

The educational objective is therefore:

```text
Mechanism
    ↓
Observation
    ↓
Detection hypothesis
    ↓
Controlled experiment
    ↓
Telemetry
    ↓
Detection rule
    ↓
Prevention / mitigation
```

This is fundamentally different from blindly writing signatures for behavior that is not fully understood.

---

# 2. Educational and Safety Scope

Chronos is intended for:

* operating-system research
* malware-analysis education
* Windows internals research
* kernel-development education
* UEFI/firmware research
* defensive security engineering
* detection engineering
* threat-hunting research
* incident-response education
* controlled security laboratories

Chronos is **not intended for deployment against systems that you do not own or explicitly administer**.

The appropriate environment is an isolated research laboratory using disposable virtual machines, snapshots, test operating systems, and controlled firmware environments.

Do not treat the repository as a production security tool.

Do not deploy experimental kernel or firmware components onto machines containing data you cannot afford to lose.

The intended research loop is:

```text
Build
  ↓
Run in isolated laboratory
  ↓
Observe
  ↓
Capture telemetry
  ↓
Analyze
  ↓
Write detection hypothesis
  ↓
Validate detector
  ↓
Destroy / restore laboratory
```

The most important artifact produced by Chronos should ultimately be the **defensive knowledge derived from the experiment**, not the simulated malicious component itself.

---

# 3. Current Repository Status

Chronos-Windows is currently an **early-stage systems-security scaffold**.

The repository already contains the architectural separation required for the larger research program, but several components are intentionally minimal at the current stage.

### Current implementation

| Component          | Current state          | Role                                       |
| ------------------ | ---------------------- | ------------------------------------------ |
| `Chronos.Loader`   | Scaffold               | Future user-mode staging / loader research |
| `Chronos.UserMode` | Scaffold               | Future user-mode malware-behavior research |
| `Chronos.Kernel`   | Minimal WDM driver     | Kernel-mode research foundation            |
| `Chronos.Firmware` | Host-side placeholder  | Future UEFI/firmware research entry point  |
| `Chronos.Common`   | Static library         | Shared low-level utilities                 |
| `docs/`            | Documentation scaffold | Architecture and research documentation    |
| `labs/`            | Experimental area      | Isolated experiments and prototypes        |
| `tests/`           | Test scaffold          | Future verification and analysis tests     |

The current kernel source implements the basic WDM lifecycle:

```text
DriverEntry
    │
    ├── register unload routine
    │
    └── return STATUS_SUCCESS

Unload
    │
    └── cleanup hook
```

It is therefore more accurate to describe the current kernel component as a **WDM driver foundation** than as a completed rootkit.

Likewise, `Chronos.Firmware` is currently a normal Visual Studio application scaffold. It is **not yet a UEFI firmware module**.

That distinction is intentional and important.

---

# 4. Repository Architecture

The current repository is organized approximately as follows:

```text
Chronos-Windows/
│
├── Chronos.slnx
│
├── common/
│   └── Chronos.Common/
│       ├── Chronos.Common.cpp
│       ├── Chronos.Common.vcxproj
│       ├── framework.h
│       ├── pch.cpp
│       └── pch.h
│
├── src/
│   │
│   ├── Chronos.Loader/
│   │   ├── Chronos.Loader.vcxproj
│   │   └── Source.cpp
│   │
│   ├── Chronos.UserMode/
│   │   ├── Chronos.UserMode.vcxproj
│   │   └── Source.cpp
│   │
│   ├── Chronos.Kernel/
│   │   ├── Chronos.Kernel.vcxproj
│   │   ├── Chronos.Kernel.vcxproj.filters
│   │   └── Source.cpp
│   │
│   └── Chronos.Firmware/
│       ├── Chronos.Firmware.vcxproj
│       └── Source.cpp
│
├── docs/
│   └── README.md
│
├── labs/
│   └── README.md
│
├── tests/
│
└── x64/
    └── Release/
```

The `x64/` and project-level `x64/` directories contain generated build artifacts and should not be confused with the logical source architecture.

---

# 5. Why Chronos Uses Four Research Layers

Sophisticated malware is not limited to a single execution context.

Different privileges and execution environments expose different capabilities.

Chronos therefore separates the research into four layers:

```text
Layer 1 — Loader / Dropper
        │
        ▼
Layer 2 — User-Mode Malware
        │
        ▼
Layer 3 — Kernel-Mode Rootkit
        │
        ▼
Layer 4 — Firmware / UEFI
```

This is not meant to imply that every real-world threat uses this exact sequence.

Instead, it provides a useful educational model for studying progressively lower trust boundaries.

Each layer asks a different question:

| Layer     | Research question                                           |
| --------- | ----------------------------------------------------------- |
| Loader    | How does code become present and begin execution?           |
| User mode | What can malicious code do inside normal Windows processes? |
| Kernel    | What changes when code executes with kernel privilege?      |
| Firmware  | What changes when code executes before the OS?              |

---

# 6. Subproject 1 — Chronos.Loader

Path:

```text
src/Chronos.Loader/
```

The Loader represents the **delivery/staging layer** of the laboratory.

In malware terminology, a dropper or loader generally exists to get another component into an execution environment and coordinate the next stage.

Chronos uses the concept for educational research into:

* executable staging
* process creation
* component boundaries
* artifact relationships
* execution ancestry
* staged execution
* trust transitions
* loader telemetry
* defender visibility into process trees

The current implementation is intentionally harmless and only demonstrates that the project can produce and execute a normal Windows application.

The long-term educational role is to answer questions such as:

```text
What artifact started execution?
        ↓
Who created the process?
        ↓
What executable was loaded?
        ↓
What other component appeared afterwards?
        ↓
Which telemetry sources recorded the transition?
```

### Defensive perspective

The Loader layer should eventually be analyzed primarily through:

* process creation telemetry
* parent/child process relationships
* executable image metadata
* file creation events
* module/image loading
* command-line lineage
* code-signing information
* security-product telemetry

The objective is not merely to make a loader work.

The objective is to understand **which observable behaviors a defender can reliably anchor detection to**.

---

# 7. Subproject 2 — Chronos.UserMode

Path:

```text
src/Chronos.UserMode/
```

This project represents the **user-mode malware research layer**.

User-mode execution is where most conventional malware begins because it operates within the normal Windows process model.

This layer is intended to provide controlled experiments around concepts such as:

* process lifecycle
* Windows APIs
* handles and object access
* virtual memory
* threads
* executable and DLL loading
* inter-process communication
* registry interaction
* filesystem interaction
* service interaction
* authentication/session boundaries
* security-token concepts
* behavioral telemetry

The purpose is to understand the difference between:

```text
"this program called API X"
```

and:

```text
"this program created a sequence of behaviors
that collectively indicate malicious intent."
```

That distinction is central to modern detection engineering.

### User-mode research model

```text
Process
  │
  ├── Threads
  │
  ├── Virtual memory
  │
  ├── Handles
  │
  ├── Modules
  │
  ├── Files
  │
  ├── Registry
  │
  ├── IPC
  │
  └── Security token
```

Each of these surfaces can produce defensive telemetry.

The UserMode project should therefore evolve toward **observable experiments**, where every behavior has a corresponding research question:

```text
Behavior
    ↓
Which Windows subsystem implements it?
    ↓
What kernel transition occurs?
    ↓
What telemetry records it?
    ↓
Can a detector distinguish benign from suspicious use?
```

---

# 8. Subproject 3 — Chronos.Kernel

Path:

```text
src/Chronos.Kernel/
```

The Kernel project represents the **kernel-mode research layer**.

The project is configured as a:

```text
ConfigurationType = Driver
DriverType        = WDM
PlatformToolset   = WindowsKernelModeDriver10.0
DebuggerFlavor    = DbgengKernelDebugger
```

The current source is intentionally minimal:

```cpp
NTSTATUS DriverEntry(...);
VOID ChronosDriverUnload(...);
```

This provides the correct foundation for studying Windows kernel execution without pretending that the project is already a fully developed rootkit.

---

## 8.1 WDM vs Visual Studio

This distinction is extremely important.

### WDM

**WDM — Windows Driver Model** — describes the Windows driver architecture and interfaces used by kernel-mode drivers.

It concerns things such as:

* driver entry
* driver objects
* device objects
* IRPs
* I/O dispatch
* synchronization
* kernel memory
* kernel execution context
* driver lifetime
* interaction with the Windows kernel

WDM is therefore a **driver model / operating-system interface concept**.

### Visual Studio

Visual Studio is the **development environment and toolchain**.

It provides:

* compiler
* linker
* debugger integration
* MSBuild
* project configuration
* source navigation
* static analysis
* debugging workflows

### WDK

The **Windows Driver Kit (WDK)** provides the driver-development tooling and headers/libraries necessary for Windows kernel development.

Conceptually:

```text
Visual Studio
     │
     ├── C/C++ compiler
     ├── linker
     ├── debugger
     └── MSBuild
             │
             ▼
            WDK
             │
             ├── kernel headers
             ├── driver libraries
             ├── driver build tools
             ├── signing infrastructure
             └── debugging support
             │
             ▼
          WDM Driver
             │
             ▼
        Windows Kernel
```

Therefore:

> **WDM is not an alternative to Visual Studio.**

They operate at different layers.

A useful mental model is:

```text
WDM
=
"How a Windows kernel driver integrates with Windows"

Visual Studio + WDK
=
"How we build, analyze, debug, and package that driver"
```

---

# 9. Why Kernel Research Requires a Different Mental Model

User mode normally operates under strong OS-managed boundaries.

Kernel mode does not.

A kernel driver can interact directly with privileged operating-system mechanisms and therefore has significantly greater impact when it is incorrect.

This changes the engineering constraints.

A user-mode crash may terminate one process.

A kernel bug can produce:

```text
invalid memory access
        ↓
kernel fault
        ↓
system instability
        ↓
bugcheck / system crash
```

For that reason, Chronos kernel experiments should be performed using:

* isolated virtual machines
* kernel debugging
* snapshots
* test builds
* controlled symbol configurations
* disposable test systems

The kernel project should eventually be developed around a disciplined lifecycle:

```text
DriverEntry
   ↓
Initialization
   ↓
Registration
   ↓
Controlled experiment
   ↓
Telemetry collection
   ↓
Cleanup
   ↓
Unload
```

Every experiment should have an explicit cleanup path.

---

# 10. Rootkit Research

Chronos uses the term **rootkit** to describe the kernel-level research category, not to claim that the current project already implements a mature rootkit.

The educational purpose is to understand why kernel-level malicious behavior can be significantly harder for ordinary user-mode monitoring to observe.

Research topics may include:

* kernel execution context
* driver loading
* callbacks
* I/O paths
* object visibility
* process visibility
* kernel memory
* security boundaries
* integrity mechanisms
* trusted execution paths
* kernel/user transitions

The defensive question is always:

> **What evidence would a security product need to determine that this behavior is occurring?**

The project should therefore document both sides:

```text
Kernel mechanism
       │
       ├── What it changes
       │
       ├── What it can affect
       │
       ├── What telemetry may observe it
       │
       └── What integrity control can prevent it
```

---

# 11. Subproject 4 — Chronos.Firmware

Path:

```text
src/Chronos.Firmware/
```

The firmware layer is fundamentally different from the previous three.

User-mode and kernel-mode components execute under Windows.

UEFI firmware executes **before Windows becomes the operating system environment**.

This creates a different research boundary:

```text
Power On
   │
   ▼
Firmware
   │
   ├── SEC
   ├── PEI
   ├── DXE
   ├── BDS
   └── Runtime services
   │
   ▼
Windows Boot Manager
   │
   ▼
Windows Kernel
   │
   ▼
User Processes
```

The repository currently does **not** contain the final UEFI implementation.

The existing `Chronos.Firmware` project is currently a Visual Studio host-side scaffold.

That is deliberate.

UEFI research should not be implemented by treating firmware modules as ordinary Win32 applications.

The intended firmware architecture is based on:

* **TianoCore EDK II**
* UEFI/PI concepts
* EDK II packages
* `.dsc` platform descriptions
* `.inf` module descriptions
* `.fdf` firmware-volume descriptions
* BaseTools
* ACPI/ASL tooling
* IASL

TianoCore describes EDK II as a modern firmware development environment for UEFI and PI implementations.

---

# 12. Why the Firmware Toolchain Is Different

A normal C++ application looks roughly like:

```text
.cpp
  ↓
MSVC
  ↓
.obj
  ↓
link.exe
  ↓
.exe
```

UEFI firmware development is closer to:

```text
C / C++ / Assembly
        │
        ▼
EDK II build system
        │
        ├── DSC
        ├── INF
        ├── FDF
        ├── BaseTools
        ├── compiler
        ├── linker
        └── firmware utilities
        │
        ▼
PE/COFF firmware modules
        │
        ▼
Firmware Volume
        │
        ▼
UEFI firmware image
```

Visual Studio can still provide the host compiler and development environment on Windows, but the **EDK II build system becomes the firmware project architecture**.

This is why Chronos should eventually keep the firmware implementation conceptually separate from the ordinary MSBuild projects.

---

# 13. TianoCore EDK II

The planned firmware research workspace should look conceptually like:

```text
Chronos-Windows/
│
├── firmware/
│   │
│   ├── edk2/
│   │   ├── BaseTools/
│   │   ├── MdePkg/
│   │   ├── MdeModulePkg/
│   │   ├── OvmfPkg/
│   │   └── ...
│   │
│   ├── ChronosFirmwarePkg/
│   │   ├── ChronosFirmwarePkg.dec
│   │   ├── ChronosFirmware.dsc
│   │   ├── ChronosFirmware.fdf
│   │   └── ...
│   │
│   └── acpica/
│       └── iasl.exe
```

The exact physical layout may evolve, but the conceptual separation should remain:

```text
Chronos research code
        │
        ▼
EDK II platform/module model
        │
        ▼
EDK II BaseTools
        │
        ▼
Compiler / linker
        │
        ▼
Firmware image
```

EDK II's current tooling includes Windows Visual Studio integration, including VS2026 tool definitions, and its build environment can invoke an Intel ACPI ASL compiler through the configured ASL path.

---

# 14. ACPI and the IASL Compiler

ACPI is an important part of firmware research because ACPI exposes hardware and platform description information to the operating system.

The relevant development pipeline is:

```text
ASL source
   │
   │  iasl
   ▼
AML bytecode
   │
   ▼
ACPI table representation
   │
   ▼
Firmware / platform
   │
   ▼
Operating system consumption
```

The **IASL compiler** is the Intel ACPI Source Language compiler provided through ACPICA tooling.

It is not a replacement for the EDK II compiler.

Instead:

```text
EDK II
=
UEFI / firmware build ecosystem

IASL
=
ACPI ASL ↔ AML tooling
```

They therefore solve different parts of the firmware build problem.

TianoCore's platform documentation explicitly identifies ACPICA tools and `iasl` as part of the ACPI build environment.

---

# 15. Why Firmware Research Matters to Detection Engineering

Traditional endpoint security operates mainly after Windows is running.

Firmware research asks a deeper question:

> What happens before the endpoint-security stack even exists?

That leads to a very different defensive model.

Instead of only asking:

```text
Did Windows load a suspicious file?
```

we can also ask:

```text
What is trusted before Windows starts?
What was measured?
What firmware executed?
What boot components were loaded?
Which integrity mechanisms can establish trust?
What survives an operating-system reinstall?
```

This leads naturally into research areas such as:

* Secure Boot
* measured boot
* TPM-backed measurements
* firmware integrity
* boot-chain validation
* UEFI variables
* firmware volumes
* ACPI integrity
* platform configuration
* pre-OS trust boundaries

The objective is not to create a persistent implant for deployment.

The objective is to understand why **pre-OS compromise changes the detection problem**.

---

# 16. The Chronos Execution Model

The long-term Chronos laboratory can be viewed as a sequence of trust boundaries.

```text
                    HARDWARE
                       │
                       ▼
                  UEFI / Firmware
                       │
                       ▼
                  Boot Environment
                       │
                       ▼
                 Windows Kernel
                       │
              ┌────────┴────────┐
              │                 │
              ▼                 ▼
        Kernel Drivers      User Processes
                                  │
                           ┌──────┴──────┐
                           │             │
                           ▼             ▼
                        Loader       Applications
```

Every downward transition introduces a new security boundary.

Chronos studies what happens when code is introduced at each boundary.

---

# 17. Common Library

Path:

```text
common/Chronos.Common/
```

`Chronos.Common` is currently a static library and provides the repository's shared-code foundation.

The solution explicitly makes:

```text
Chronos.Loader
        │
        └── depends on Chronos.Common

Chronos.UserMode
        │
        └── depends on Chronos.Common
```

The kernel and firmware projects currently remain independent.

That separation is useful because kernel-mode and firmware code operate under radically different runtime constraints from ordinary user-mode C++.

The common layer should therefore remain conservative.

It should contain only functionality that is genuinely reusable across appropriate execution contexts.

Avoid turning it into a generic dumping ground for unrelated abstractions.

A healthy future dependency structure is:

```text
Common
   │
   ├── User-mode utilities
   │
   └── Shared data structures

Kernel
   └── Kernel-only implementation

Firmware
   └── Firmware-only implementation
```

rather than:

```text
Everything
   │
   └── Chronos.Common
```

---

# 18. Detection Engineering Is the Actual End Goal

Chronos should ultimately connect every research experiment to a defensive hypothesis.

For example:

| Research layer | Question                            | Defensive evidence                                  |
| -------------- | ----------------------------------- | --------------------------------------------------- |
| Loader         | How did execution start?            | Process ancestry, file events, image metadata       |
| User mode      | What OS facilities were exercised?  | ETW/provider telemetry, process/module behavior     |
| Kernel         | What changed below user mode?       | Driver telemetry, integrity state, kernel events    |
| Firmware       | What executed before Windows?       | Secure Boot state, measurements, firmware inventory |
| Cross-layer    | How does one stage lead to another? | Correlated timeline across telemetry sources        |

The important architectural principle is:

```text
Do not detect the name "Chronos."

Detect the underlying behavior.
```

A useful research result therefore looks like:

```text
Experiment:
    Controlled behavior X

Observation:
    Telemetry Y + Z

Hypothesis:
    Behavior X produces a distinguishable pattern

Detector:
    Rule / model based on Y + Z

Validation:
    Benign workload vs controlled malicious simulation

Result:
    False-positive / false-negative analysis
```

This turns Chronos into a detection-engineering laboratory rather than a malware collection.

---

# 19. Suggested Research Methodology

Every experiment should answer five questions.

### 1. What mechanism is being studied?

Example:

```text
Process creation
Driver initialization
UEFI module dispatch
ACPI table compilation
```

### 2. What execution boundary is involved?

```text
User → Kernel
Kernel → Hardware
Firmware → OS
```

### 3. What observable evidence does it produce?

```text
Process event
Driver event
Integrity measurement
Firmware artifact
```

### 4. Which defensive control should observe it?

```text
EDR
ETW
Windows Code Integrity
Secure Boot
TPM measurement
Firmware inventory
```

### 5. How can the experiment be distinguished from legitimate behavior?

This final question is critical.

A detector that treats every unusual low-level action as malicious is not a useful detector.

Chronos should therefore study:

```text
Malicious simulation
       VS
Legitimate workload
       ↓
Behavioral difference
       ↓
Detection boundary
```

---

# 20. Laboratory Environment

Chronos should be developed primarily inside isolated laboratory environments.

Recommended architecture:

```text
Host
 │
 ├── Development environment
 │      ├── Visual Studio
 │      ├── WDK
 │      ├── EDK II
 │      └── Debugging tools
 │
 └── Research VM
        ├── Disposable Windows installation
        ├── Kernel debugging
        ├── Snapshots
        └── Telemetry collection
```

For firmware experimentation:

```text
Research VM / emulator
        │
        ▼
UEFI-compatible firmware environment
        │
        ▼
EDK II / OVMF-style laboratory
        │
        ▼
Windows research image
```

Do not perform experimental kernel or firmware work on:

* production endpoints
* machines containing irreplaceable data
* organizational infrastructure
* systems you do not control

Snapshots are a core part of the workflow.

---

# 21. Development Environment

## Windows / User-Mode / Kernel

The primary development environment is:

* Windows
* Visual Studio 2026
* Windows SDK
* Windows Driver Kit
* C++20 for ordinary C++ projects
* x64 development
* kernel debugging tools

The repository's ordinary C++ projects are currently configured with the Visual Studio `v145` toolset.

The kernel project additionally uses the Windows kernel-mode driver toolchain:

```text
PlatformToolset = WindowsKernelModeDriver10.0
```

Microsoft currently recommends the latest WDK together with Visual Studio 2026 for driver development.

---

# 22. Building the Current Visual Studio Projects

From a Visual Studio Developer Command Prompt:

```powershell
msbuild Chronos.slnx /m /p:Configuration=Debug /p:Platform=x64
```

Individual projects can also be built independently.

Example:

```powershell
msbuild src\Chronos.UserMode\Chronos.UserMode.vcxproj `
    /p:Configuration=Debug `
    /p:Platform=x64
```

Kernel development should be treated separately from normal application builds because driver signing, deployment, symbols, debugging, and runtime stability introduce additional requirements.

---

# 23. Firmware Development Environment

The intended firmware environment is based on TianoCore EDK II.

A conceptual Windows setup is:

```text
Windows
   │
   ├── Git
   ├── Python
   ├── Visual Studio
   ├── EDK II
   ├── EDK II BaseTools
   ├── NASM
   └── ACPICA / IASL
```

A typical EDK II workflow is conceptually:

```text
Clone EDK II
      ↓
Initialize submodules
      ↓
Build BaseTools
      ↓
Configure build environment
      ↓
Configure IASL / ACPI tooling
      ↓
Build Chronos firmware package
      ↓
Produce firmware artifacts
      ↓
Run inside isolated firmware laboratory
```

The EDK II project documentation recommends initializing the repository's required submodules as part of preparing a complete build environment.

---

# 24. Firmware Build Concepts

Chronos firmware work should introduce EDK II concepts gradually.

### DSC

The `.dsc` file describes the platform build configuration.

Think:

```text
"Which modules and libraries make up this firmware platform?"
```

### INF

The `.inf` file describes a firmware module.

Think:

```text
"What is this module and what does it depend on?"
```

### FDF

The `.fdf` file describes firmware-volume layout.

Think:

```text
"Where do these firmware modules live inside the firmware image?"
```

### DEC

The `.dec` file describes package-level declarations and interfaces.

Think:

```text
"What interfaces does this package expose?"
```

These are fundamentally different from an ordinary:

```text
.vcxproj
```

project.

---

# 25. ACPI Research Workflow

ACPI research should similarly remain explicit.

```text
ASL
 │
 ▼
IASL
 │
 ▼
AML
 │
 ▼
ACPI table / firmware integration
 │
 ▼
OS interpretation
```

This allows Chronos to study the relationship between:

```text
Firmware
   ↓
ACPI
   ↓
Operating System
```

from both the offensive-research and defensive-validation perspectives.

The point is not simply learning how to compile AML.

The point is understanding what information the operating system ultimately trusts about its hardware and firmware environment.

---

# 26. Source Tree Design Principles

Chronos should follow a **layer-oriented systems architecture**, not copy a conventional application architecture such as Android's feature/core pattern literally.

For an application, something like:

```text
core/
feature/
data/
domain/
ui/
```

can be extremely effective.

Firmware and kernel development have different ownership boundaries.

Chronos should therefore prefer:

```text
execution boundary
        ↓
subsystem
        ↓
research component
        ↓
experiment
```

rather than artificially forcing all code into application-style Clean Architecture layers.

The important architectural boundaries are:

```text
User Mode
    X
Kernel Mode
    X
Firmware
```

where `X` represents a strict ownership boundary.

Crossing those boundaries should be explicit.

---

# 27. Recommended Long-Term Repository Structure

As the implementation grows, the target architecture can evolve toward:

```text
Chronos-Windows/
│
├── Chronos.slnx
│
├── common/
│   └── Chronos.Common/
│
├── src/
│   │
│   ├── Chronos.Loader/
│   │   ├── include/
│   │   ├── src/
│   │   ├── tests/
│   │   └── Chronos.Loader.vcxproj
│   │
│   ├── Chronos.UserMode/
│   │   ├── include/
│   │   ├── src/
│   │   ├── tests/
│   │   └── Chronos.UserMode.vcxproj
│   │
│   ├── Chronos.Kernel/
│   │   ├── include/
│   │   ├── src/
│   │   ├── tests/
│   │   ├── driver/
│   │   └── Chronos.Kernel.vcxproj
│   │
│   └── Chronos.Firmware/
│       └── ...
│
├── firmware/
│   ├── edk2/
│   ├── ChronosFirmwarePkg/
│   └── acpica/
│
├── detection/
│   ├── hypotheses/
│   ├── telemetry/
│   ├── rules/
│   ├── analytics/
│   └── reports/
│
├── labs/
│   ├── usermode/
│   ├── kernel/
│   ├── firmware/
│   └── cross-layer/
│
├── tests/
│   ├── unit/
│   ├── integration/
│   ├── detection/
│   └── fixtures/
│
└── docs/
    ├── architecture/
    ├── windows/
    ├── kernel/
    ├── firmware/
    ├── acpi/
    └── detection/
```

This is a target architecture, not a description of everything that already exists today.

---

# 28. Labs Directory

The `labs/` directory exists for experimentation.

Experiments should be isolated from production-quality subsystem implementations.

For example:

```text
labs/
├── usermode/
│   ├── process-observation/
│   └── memory-observation/
│
├── kernel/
│   ├── driver-lifecycle/
│   └── callback-research/
│
├── firmware/
│   ├── edk2/
│   └── acpi/
│
└── cross-layer/
    └── telemetry-correlation/
```

A lab experiment should answer one focused research question.

Avoid allowing experimental code to silently become production infrastructure.

---

# 29. Documentation Philosophy

The `docs/` directory should eventually explain the system from several perspectives.

Recommended documentation categories:

```text
docs/
├── architecture/
│   ├── system-model.md
│   ├── execution-boundaries.md
│   └── dependency-model.md
│
├── windows/
│   ├── process-model.md
│   ├── memory-model.md
│   └── security-boundaries.md
│
├── kernel/
│   ├── wdm.md
│   ├── driver-lifecycle.md
│   └── debugging.md
│
├── firmware/
│   ├── uefi.md
│   ├── edk2.md
│   └── boot-chain.md
│
├── acpi/
│   ├── asl.md
│   ├── aml.md
│   └── iasl.md
│
└── detection/
    ├── telemetry-model.md
    ├── detection-hypotheses.md
    └── validation.md
```

Each technical document should explain:

```text
What it is
Why it exists
How it works
What assumptions it makes
What telemetry it creates
What defenders can observe
What security boundaries apply
```

---

# 30. Testing Strategy

Chronos should eventually have several categories of tests.

## Unit tests

Test deterministic logic without touching sensitive system facilities.

```text
input
 ↓
logic
 ↓
expected result
```

## Integration tests

Validate interactions between components.

```text
Loader
   ↓
UserMode
   ↓
Expected observable state
```

## Detection tests

These are particularly important.

```text
Simulation
    ↓
Telemetry
    ↓
Detection rule
    ↓
Expected detection
```

## Regression tests

Every newly understood behavior should become a repeatable experiment where possible.

The goal is to prevent the research repository from becoming a collection of demonstrations that cannot be reproduced.

---

# 31. What Makes an Experiment Valuable?

A useful Chronos experiment should produce at least one of the following:

* a new understanding of Windows internals
* a new understanding of kernel execution
* a new understanding of UEFI/firmware behavior
* a new defensive telemetry source
* a new detection hypothesis
* a new prevention mechanism
* a measurable difference between benign and suspicious behavior
* a reproducible security finding
* a documented failure mode

A demonstration that merely says:

```text
"It works."
```

is not enough.

A better experiment says:

```text
Mechanism X produces observable behavior Y
under condition Z.

Telemetry source A records Y.

Control B prevents Z.

Therefore detector C can use A to identify the behavior.
```

That is the standard Chronos should work toward.

---

# 32. Cross-Layer Research

The most interesting future work will happen at the boundaries between the four layers.

For example:

```text
Loader
  │
  ▼
User Mode
  │
  ▼
Kernel
  │
  ▼
Firmware / Boot
```

The research question is not merely:

> "Can each component exist?"

It is:

> "How does evidence flow across execution boundaries?"

A defender may see completely different telemetry depending on which layer created the behavior.

This makes cross-layer correlation a major future goal.

---

# 33. Detection Timeline Model

Chronos experiments should preferably generate a timeline such as:

```text
T0  ── system boot
T1  ── firmware initialization
T2  ── boot manager
T3  ── Windows kernel initialization
T4  ── driver initialization
T5  ── process creation
T6  ── user-mode execution
T7  ── observable security event
```

The research task then becomes:

```text
Which event caused the next event?
```

That produces a causal model instead of a collection of isolated alerts.

---

# 34. Threat-Model Vocabulary

Chronos uses several terms that should not be conflated.

### Loader

A component responsible for staging or initiating another component.

### Dropper

A form of loader whose purpose commonly includes placing or unpacking another component.

### Malware

Software whose behavior is intentionally harmful, unauthorized, or deceptive.

In Chronos, these behaviors are studied in controlled simulations.

### Driver

A kernel-mode component integrated with the Windows driver model.

### Rootkit

A class of software designed to maintain privileged access and/or manipulate visibility or trust mechanisms.

Chronos studies the underlying kernel mechanisms associated with this category.

### UEFI malware

Code or modifications that execute within the firmware/boot environment.

### Bootkit

Malicious modification of the boot chain occurring before the operating system is fully established.

### APT

Advanced Persistent Threat is a threat category describing sophisticated, targeted, persistent adversaries and campaigns.

Chronos studies **APT-relevant technical mechanisms**, not any particular real-world threat actor.

---

# 35. Defensive Mindset

Every component in this repository should eventually have two documents:

```text
Implementation
      +
Detection Analysis
```

For example:

```text
Chronos.Kernel
      │
      ├── How the mechanism works
      │
      └── How defenders can detect the mechanism
```

Likewise:

```text
Chronos.Firmware
      │
      ├── How the firmware mechanism works
      │
      └── How platform integrity can detect/prevent it
```

This prevents the project from drifting into implementation for implementation's sake.

---

# 36. Security Boundaries to Study

Chronos should repeatedly return to these trust boundaries:

```text
User
 ↓
Kernel
 ↓
Hardware
```

and:

```text
Firmware
 ↓
Bootloader
 ↓
Kernel
 ↓
User Mode
```

and:

```text
Developer
 ↓
Build system
 ↓
Binary
 ↓
Loader
 ↓
Execution
```

Each boundary creates assumptions.

Security engineering is largely the discipline of identifying which assumptions can fail.

---

# 37. Design Principles

Chronos follows several architectural principles.

### 1. Understand mechanisms, not just indicators

Do not begin with:

```text
"What signature can detect this?"
```

Begin with:

```text
"What actually happens?"
```

### 2. Separate execution domains

User-mode, kernel-mode, and firmware code should not be casually coupled.

### 3. Prefer reproducible experiments

Every important observation should be reproducible.

### 4. Record defensive telemetry

An experiment without telemetry is incomplete from a detection-engineering perspective.

### 5. Keep dangerous behavior isolated

Experiments belong in disposable laboratories.

### 6. Distinguish implementation from research

Prototype code belongs in `labs/`.

Stable subsystem architecture belongs under `src/`.

### 7. Treat firmware as a separate engineering ecosystem

UEFI/EDK II should not be forced into the same build architecture as ordinary Windows executables.

---

# 38. Roadmap

A logical development progression is:

```text
Phase 1
│
├── Repository architecture
├── Build reproducibility
└── Documentation foundation
│
▼
Phase 2
│
├── User-mode Windows internals experiments
├── Loader research
└── Telemetry capture
│
▼
Phase 3
│
├── WDM driver research
├── Kernel debugging
└── Kernel telemetry analysis
│
▼
Phase 4
│
├── EDK II integration
├── UEFI module research
├── ACPI / IASL experiments
└── Firmware laboratory
│
▼
Phase 5
│
├── Cross-layer experiments
├── Detection correlation
├── Regression tests
└── Defensive validation
│
▼
Phase 6
│
└── Chronos detection research platform
```

The final milestone is not "more malware."

The final milestone is a system where researchers can ask:

```text
"What would a sophisticated attack look like
at every execution layer,
and what evidence proves that it happened?"
```

---

# 39. Repository Rules for Contributors

When adding code:

1. Keep execution boundaries explicit.
2. Keep user-mode code out of kernel-specific abstractions.
3. Keep firmware code independent of ordinary Win32 assumptions.
4. Put experiments under `labs/`.
5. Add documentation for non-obvious Windows internals.
6. Add tests wherever behavior can be reproduced deterministically.
7. Document the defensive telemetry produced by significant experiments.
8. Never assume that an observed artifact is malicious without analyzing its legitimate equivalents.
9. Prefer isolated virtualized research environments.
10. Do not introduce real-world targeting, credential theft, destructive payloads, or unauthorized persistence into the laboratory.

---

# 40. What Chronos Is Ultimately Teaching

Chronos is intentionally broader than "how malware works."

It is teaching the relationship between:

```text
Operating systems
       +
Hardware
       +
Firmware
       +
Compiler / toolchain
       +
Execution model
       +
Security boundaries
       +
Telemetry
       +
Detection
```

A complete systems-security understanding requires all of them.

For example:

```text
A process
    ↓
calls a Windows API
    ↓
which transitions into the kernel
    ↓
which interacts with a kernel subsystem
    ↓
which may touch hardware
    ↓
whose configuration originates from firmware
```

Once the entire chain becomes understandable, defensive engineering becomes much more rigorous.

Instead of guessing:

```text
"This looks suspicious."
```

the researcher can reason:

```text
"Here is the execution path.
Here is the trust boundary crossed.
Here is the observable evidence.
Here is the integrity control.
Here is the detection opportunity."
```

That is the core educational purpose of Chronos.

---

# 41. Final Principle

> **Chronos is a laboratory for understanding hostile execution across the Windows stack so that defensive systems can be designed from first principles.**

The four research domains are:

```text
┌──────────────────────────────────────────────┐
│                 CHRONOS                     │
├──────────────────────────────────────────────┤
│  1. Loader / Dropper                        │
│     Study delivery and execution staging    │
│                                              │
│  2. User-Mode Malware                       │
│     Study Windows process-level behavior    │
│                                              │
│  3. Kernel / Rootkit                        │
│     Study privileged execution and WDM      │
│                                              │
│  4. Firmware / UEFI                         │
│     Study pre-OS trust and boot integrity   │
└──────────────────────────────────────────────┘
                      │
                      ▼
             Defensive Understanding
                      │
                      ▼
              Detection + Prevention
```

The project succeeds when researchers can move in both directions:

```text
Attack mechanism
      ↓
Operating-system / firmware behavior
      ↓
Observable evidence
      ↓
Detection
      ↓
Prevention
```

and:

```text
Security control
      ↓
What assumption does it make?
      ↓
Which layer does it protect?
      ↓
What happens outside that layer?
```

That second path is just as important as the first.

Chronos exists to make both paths understandable.
