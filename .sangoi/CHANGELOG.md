# MCP workspace changelog

## 2026-10-18

- Removed the unused hosted CI workflow and its direct README references in `1a27b89`, as requested. The same file's tag-triggered packaging/releases are also removed; local build, test and packaging commands and the existing Git history are unchanged. Path, reference, ancestry and whitespace checks pass. The complete paired local Code Gate returned `PROCEED_WITH_NITS`, with no required source correction; its progress-wording nit is resolved. Candidate `67f78e1` was published to `origin/master` by normal fast-forward and its exact remote SHA was verified. No credential expansion, force push or history rewrite occurred; no runtime tests were rerun for this automation/documentation-only change.
