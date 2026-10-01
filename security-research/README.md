# Chronos Security Research

This directory is reserved for defensive vulnerability-research artifacts.

Chronos intentionally uses `security-research/` rather than an `exploits/` directory. The distinction is architectural: the project studies vulnerability discovery, proof, remediation, regression and coordinated disclosure rather than maintaining a weaponized exploit collection.

## Recommended structure

~~~text
security-research/
└── vulnerabilities/
    └── CHR-YYYY-NNN/
        ├── README.md
        ├── affected-components.md
        ├── reproduction.md
        ├── root-cause.md
        ├── impact.md
        ├── mitigation.md
        ├── regression/
        ├── minimized-inputs/
        ├── crash-artifacts/
        └── disclosure/
            ├── vendor-report.md
            ├── timeline.md
            └── communications.md
~~~

Prefer minimized crash inputs, sanitized logs, debugger observations, root-cause analyses, patches, regression tests, fuzzing corpora and disclosure records.

Do not store weaponized exploit chains, credential-theft tooling, persistence payloads, destructive payloads or real-target attack infrastructure.

See docs/vulnerability-research.md for the methodology.