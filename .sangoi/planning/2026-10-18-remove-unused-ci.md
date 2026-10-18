# Intent Contract: remove unused MCP CI

Status: active
Task mode: Doctrine
Lifecycle phase: Implementation
Strategy epoch: 1
Plan kind: Targeted Adjustment Plan
Plan kind rationale: Remove one unused automation surface and its direct advertisement, without changing the MCP product or build/test commands.

## Confirmed intent and decisions

Remove `.github/workflows/ci.yml` and its CI badge in the root `README.md`, preserve the existing code and Git history, and complete the pending MCP push. The removal also eliminates the tag-triggered packaging and release automation defined in that file. Local builds and tests remain available.

The user requested the removal with `remove esse workflow, não usamos ci` at `2026-10-18T02:06:00.790-03:00` (America/Sao_Paulo), turn `01a14d67-08a9-72d3-a838-4f500e9882dd`. The user confirmed the complete paraphrase above with `sim` at `2026-10-18T02:10:29.359-03:00`, turn `01a14d6b-21cf-7a42-b116-98a2428bd8a1`, session `01a14c49-9c6e-7df2-a1eb-2c29aaec3480`, rollout line 3749. The earlier `pode dar push` at rollout line 3589 supplies publication authority; the latest confirmation retains that requested result.

## Current owner and evidence

The active workspace is the independent repository `/home/lucas/work/radare2/r2mcp`. At intake, its clean local `master` was `333e07cf36304c8e9c8f0424b9479e9f7c0624cf` and `origin/master` was `e8fb3ac34e98d633661127d9b13e1b0174b8f2d8`, with four unpublished commits. The confirmed GitHub identity is `sangoi-exe` and origin is its `radare2-mcp` fork. The other three component pushes are already complete and are not edit targets.

At intake, the only tracked workflow was `.github/workflows/ci.yml`. It owned push/PR CI plus tag-triggered packaging and releases; the root `README.md:3` advertised it. The existing native Make/Meson/test owners do not depend on the workflow file. GitHub rejected the prior MCP push because the OAuth credential lacks the `workflow` scope; the remote branch remained unchanged at that readback.

## Selected route and Scope

Delete the workflow and its direct README badge in a new ordinary commit. Preserve the four existing commits and all MCP runtime, test, build and packaging implementations. Do not replace the workflow, disable it through a parallel mechanism, increase credentials, rewrite history, force-push, delete tags, or edit the other repositories. No compatibility surface or migration is requested.

The removed workflow and badge have disposition `delete/update now`, authorized by the current user. Their replacement is `deleted with no replacement`. The consequence is no hosted CI or automatic tag packaging/release from this workflow; local commands remain unchanged. Earlier commits and the completed native-baseline plan are historical evidence, not live CI guidance.

Implementation source footprint: `.github/workflows/ci.yml` and its direct CI references in `README.md` only. Besides the badge, the Windows build paragraph compares its unchanged Meson/Ninja commands with CI; remove that comparison without changing the commands. The Plan audit identified this direct follower as a nonblocking documentary correction inside the admitted two-file surface, not a new material decision. Root-owned evidence stays in this plan, `.sangoi/task-logs/2026-10-18-remove-unused-ci.md` and `.sangoi/CHANGELOG.md`. No new scripts, validators, instructions or generated artifacts are needed.

## Acceptance and first directly verifiable slice

- The candidate contains no workflow file and no stale live CI badge/reference in the admitted repository surface.
- The complete candidate diff contains only the selected deletion, direct README cleanup and factual task evidence. Runtime, test and local build/packaging files remain unchanged from `333e07cf`.
- The candidate passes whitespace/reference checks and the applicable local Code Gate. No runtime rebuild is required for deletion of unused automation and its badge.
- Publish only the final fixed candidate to `origin/master` by normal fast-forward and verify the remote SHA. A rejection or ambiguous result stops publication for an exact user decision; it does not authorize history rewriting, credential expansion or another transport.

The first slice is removal of the workflow and badge with native Git/path/reference inspection. The existing four functional commits retain their prior review and proof; this task does not repeat or widen that review.

## Execution and continuity

Audit the confirmed Intent Contract, dispatch one bounded non-web Worker, integrate and commit the selected delta, complete the narrow local Senior/Claude Code Gate, then publish and verify the exact candidate. Root owns Git, publication and factual bookkeeping. The Worker owns only the two declared implementation paths.

The registered Initial Intent Contract audit on `/root/mcp_remove_ci_plan` returned `PROCEED`, with complete field coverage and no material finding. Root applied the matching `publish-plan-locator-and-enter-implementation` transition and clarified the direct-reference footprint before Worker dispatch. No Gate override or extra user checkpoint is involved.

Worker C01 on `/root/mcp_remove_ci_worker` completed the two-file deletion/reference cleanup. Root inspected the full diff and staged patch, verified absence and reference classification, and committed it as `1a27b8961cc29dc1204b382d71a855d464a7a64e`. The original base remains an ancestor. Native Git shows no runtime, build, test or packaging implementation change. No builds or tests were rerun for this automation/documentation-only delta. Code review and publication remain pending.

Material decisions still open: none. Residual-risk acceptances: none. Blocker: none. Next action: commit the factual task evidence and obtain the narrow local `Task change-set review` before publication.
