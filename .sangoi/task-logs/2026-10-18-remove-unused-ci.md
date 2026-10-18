# Remove unused MCP CI

## Authority and baseline

The user requested removal of the unused workflow after the MCP push was rejected for missing OAuth `workflow` scope. The confirmed paraphrase includes deleting the workflow and its badge, preserving code/history, accepting loss of that workflow's tag packaging/releases, and completing the pending push. The exact confirmation is current-session rollout line 3749, `sim`, turn `01a14d6b-21cf-7a42-b116-98a2428bd8a1`, `2026-10-18T02:10:29.359-03:00` (America/Sao_Paulo). The canonical contract is `.sangoi/planning/2026-10-18-remove-unused-ci.md`.

Clean local baseline: `333e07cf36304c8e9c8f0424b9479e9f7c0624cf`. The MCP remote remained `e8fb3ac34e98d633661127d9b13e1b0174b8f2d8` after its rejected push. Host, bridge and engine publication was already confirmed in the preceding operation. This task does not change those repositories or repeat the prior functional review.

## Implementation and bounded proof

The Initial Intent Contract audit on `/root/mcp_remove_ci_plan` returned `PROCEED`, with complete coverage and no material finding. Its nonblocking follower note identified the README Windows build sentence's comparison with CI. Root verified the sentence and clarified the same-file footprint without another user checkpoint; the Meson/Ninja commands remain unchanged.

Worker C01 on `/root/mcp_remove_ci_worker` deleted the 240-line `.github/workflows/ci.yml`, removed the root README CI badge, and removed only the Windows sentence's CI comparison. Root inspected the complete two-file delta and staged patch, then committed `1a27b8961cc29dc1204b382d71a855d464a7a64e` with the required Codex trailer. The README edits and workflow deletion are one inseparable capability-removal change; factual task documents use a separate commit.

Worker and root checks establish actual workflow path absence, passing `git diff --check`, the exact two-file implementation diff, and preserved baseline ancestry. The candidate tree has no tracked `.github` files. A tracked live-root text CI/path scan retains only `test-http.py:10`, which states that its test can run in Linux CI and Windows development environments; it does not advertise this repository's removed automation. Binary assets are excluded from text-reference evidence and remain unchanged. README saved-analysis workflow terminology is unrelated and remains. Task evidence is an explicit historical/disposition root, not an obsolete runtime owner. There are no generated-owner or negative-test roots in this removal frontier.

Runtime, test, local Make/Meson and packaging implementations are unchanged from the baseline. No rebuild, runtime test, replacement workflow, validator or temporary artifact was created. Existing functional commits and their prior proof remain intact. These local checks do not prove future GitHub acceptance.

## Current state

Implementation is committed. The local Code Gate and subsequent exact-candidate publication remain pending. Root will publish only by normal fast-forward, with read-before-write and remote-SHA verification. A rejection or ambiguous result does not authorize another transport, OAuth expansion or history rewriting.
