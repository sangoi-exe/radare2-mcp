# Remove unused MCP CI

## Authority and baseline

The user requested removal of the unused workflow after the MCP push was rejected for missing OAuth `workflow` scope. The confirmed paraphrase includes deleting the workflow and its badge, preserving code/history, accepting loss of that workflow's tag packaging/releases, and completing the pending push. The exact confirmation is current-session rollout line 3749, `sim`, turn `01a14d6b-21cf-7a42-b116-98a2428bd8a1`, `2026-10-18T02:10:29.359-03:00` (America/Sao_Paulo). The canonical contract is `.sangoi/planning/2026-10-18-remove-unused-ci.md`.

Clean local baseline: `333e07cf36304c8e9c8f0424b9479e9f7c0624cf`. The MCP remote remained `e8fb3ac34e98d633661127d9b13e1b0174b8f2d8` after its rejected push. Host, bridge and engine publication was already confirmed in the preceding operation. This task does not change those repositories or repeat the prior functional review.

## Implementation and bounded proof

The Initial Intent Contract audit on `/root/mcp_remove_ci_plan` returned `PROCEED`, with complete coverage and no material finding. Its nonblocking follower note identified the README Windows build sentence's comparison with CI. Root verified the sentence and clarified the same-file footprint without another user checkpoint; the Meson/Ninja commands remain unchanged.

Worker C01 on `/root/mcp_remove_ci_worker` deleted the 240-line `.github/workflows/ci.yml`, removed the root README CI badge, and removed only the Windows sentence's CI comparison. Root inspected the complete two-file delta and staged patch, then committed `1a27b8961cc29dc1204b382d71a855d464a7a64e` with the required Codex trailer. The README edits and workflow deletion are one inseparable capability-removal change; factual task documents use a separate commit.

Worker and root checks establish actual workflow path absence, passing `git diff --check`, the exact two-file implementation diff, and preserved baseline ancestry. The candidate tree has no tracked `.github` files. A tracked live-root text CI/path scan retains only `test-http.py:10`, which states that its test can run in Linux CI and Windows development environments; it does not advertise this repository's removed automation. Binary assets are excluded from text-reference evidence and remain unchanged. README saved-analysis workflow terminology is unrelated and remains. Task evidence is an explicit historical/disposition root, not an obsolete runtime owner. There are no generated-owner or negative-test roots in this removal frontier.

Runtime, test, local Make/Meson and packaging implementations are unchanged from the baseline. No rebuild, runtime test, replacement workflow, validator or temporary artifact was created. Existing functional commits and their prior proof remain intact. These local checks do not prove future GitHub acceptance.

## Code Gate

The first native Claude invocation failed with `EAI_AGAIN` and zero semantic coverage. Its recovery request was denied before process launch because explicit disclosure authority was missing. Root stopped, asked the user, and received `sim` at `2026-10-18T02:30:15.523-03:00`, turn `01a14d7d-3b49-7932-b951-48a61cca9f8a`, current-session rollout line 4017. The grant covers only this delta and direct-reference evidence, without credentials. Fresh initial and reciprocal calls then succeeded with exit 0, observed `claude-opus-5-5` identity and no denials. The earlier failures remain failures; no waived or fabricated review is recorded.

The actual independent initials are root rollout lines 3956 and 4053; reciprocal replies are 4089 and 4108. Both reviewers account for the entire five-file delta and relevant direct followers. Root inspected the actual reports and resolved the prospective progress clause as bookkeeping, while retaining future GitHub rejection as an unverified inference rather than an external-contract fact. Native Git/path/reference checks establish local deletion and preservation, not future publication acceptance.

The single formal `Senior Code Reviewer / Task change-set review` on `/root/mcp_remove_ci_senior` returned `PROCEED_WITH_NITS` at `2026-10-18T02:41:30.841-03:00` (America/Sao_Paulo), rollout line 4145. Semantic coverage is complete; no material finding or required source correction remains. The only nit is the plan's already-fulfilled evidence-commit next action, superseded during factual closeout. The matching transition is `record-nits-enter-closeout`, with no rerun. No Claude-failure exception or review override was applied.

## Publication and closeout

Read-before-write reconfirmed origin `sangoi-exe/radare2-mcp`, authenticated identity `sangoi-exe`, candidate `67f78e1fe6a31d69130fe9e208058848f69b9265` and remote `master` at `e8fb3ac34e98d633661127d9b13e1b0174b8f2d8`. The only local changes were root-owned factual closeout documents, excluded from that exact-candidate push.

The ordinary push of the reviewed candidate succeeded with exit 0, advancing remote `master` from `e8fb3ac` to `67f78e1`. A subsequent `git ls-remote` returned the complete candidate SHA. This actual observation refutes the predicted rejection for this candidate; it does not establish a universal rule about GitHub's workflow permission checks. No credential scope expansion, alternate transport, force push, tag change or history rewrite occurred. The existing `gh` credential helper was selected per Git process without changing stored Git configuration.

The requested deletion and pending MCP publication are complete. The progress-wording nit is resolved in the canonical plan; the actual verdict remains `PROCEED_WITH_NITS`, not relabeled as a different outcome. This final documentary checkpoint is `bookkeeping memorialization` of the established result. It changes only the three task documents, not the reviewed implementation or proof, and requires no additional semantic Gate. No runtime test or rebuild is claimed. The deleted workflow remains recoverable from Git history.
