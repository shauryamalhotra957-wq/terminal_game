# Security and maintenance policy

Terminal Game is a local Windows console program.

- Do not commit downloaded binaries, build output, personal screenshots, or unrelated executable files.
- Review changes to file parsing, map loading, and console input for crashes or unintended process execution.
- Keep the game self-contained; do not add network access or shell commands without an explicit design review.
- Treat map files and future save data as untrusted input and validate their dimensions before use.

Report reproducible crashes, unsafe command execution, or malicious repository changes privately to the repository owner.
