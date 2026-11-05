# YAML command admission

The first image-host-pinned source2b4424b2 created push run42038483415 named .github/workflows/android.yml rather than executing the workflow: a plain YAML command contained the colon-space sequence in pip --only-binary=:all:. The exact same single pip command now uses a YAML block scalar. The invalid source remains in immutable Git history; no failed workflow-admission result is called build or Android execution. Runtime/dependency pins and all state/pixel/gesture predicates remain unchanged.
