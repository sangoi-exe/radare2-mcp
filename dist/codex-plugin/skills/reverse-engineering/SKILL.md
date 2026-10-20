---
name: reverse-engineering
description: Analyze binaries with radare2 CLI or native r2mcp tools, including server startup/readiness and NE/OMF relocation provenance. Use for radare2/r2mcp reverse-engineering work, not generic coding tasks.
---

## Choose the available interface

Prefer available structured `r2mcp` tools for the requested target observation; inspect their actual catalog and argument schemas rather than assuming a namespace or tool exists. When MCP is unavailable and direct CLI use is authorized, use the selected radare2 executable. Starting a server subprocess does not register its tools with the agent: an MCP client must activate its configured server and own the transport. Do not install packages or change client configuration merely to follow this skill.

For a bounded Linux CLI metadata read, select matching libraries and keep the filename last:

```sh
LD_LIBRARY_PATH=/matching/host/lib /matching/host/bin/r2 -NN -q -c 'ij' /absolute/task/root/target
```

CLI `-NN` disables scripts/plugins; CLI `-n` skips binary loading and is inappropriate for an ordinary executable. These are not MCP's flag meanings. Use CLI command help for further reads instead of translating tool names into guessed commands.

## Launch a selected native server

The native server defaults to in-process RCore with stdio JSON-RPC. The client starts and retains stdin/stdout; EOF ends the server. Keep diagnostics on stderr, never merged into JSON-RPC stdout. `-H` selects HTTP MCP transport; `-u` selects a remote r2pipe backend, not the same operation.

For an isolated Linux native decompiler session, substitute verified matching paths and give the client this executable, environment and argument list:

```sh
PATH="/matching/host/bin:$PATH" \
LD_LIBRARY_PATH="/matching/host/lib" \
/matching/r2mcp -n -N -C both -s /absolute/task/root \
  -c 'L /matching/libcore_r2ghidra.so' \
  -c 'e r2ghidra.sleighhome=/matching/x86/data/languages' \
  -c 'e r2ghidra.casts=true' \
  -d pd:g
```

Select the actual server executable, linked host libraries, bridge plugin and compiled language directory as one matching set; a workspace path or plugin file alone proves no match. MCP `-n` disables automatic plugins/radare2rc and `-N` disables prompt loading; explicit `-c` commands still run. Omit the native plugin/configuration commands and `-d` when native decompilation is not needed. The bundled launcher may run `r2pm -r r2mcp`; it does not thereby select this workspace build. Raw `run_*` tools require `-r`; ordinary native observation tools do not need it.

## Check readiness before analysis

`-h` is help only. `-t` lists tools and exits before RCore initialization. `-T` runs an initialized local DSL, not MCP transport. None proves a client-connected target or working decompiler.

Through the actual client, complete `initialize`, send `notifications/initialized`, discover `tools/list` (following its returned cursors), then call `open_file` with an absolute `file_path` under the selected `-s` root. Check JSON-RPC `error` and tool `result.isError`, then make bounded `show_info` and `list_sections` calls to verify the intended target and coordinates. `-C both` or `-C structured` exposes supported results in `structuredContent`; text-only results still require reading their content and diagnostics.

A successful `read_instruction_pcode` with explicit `address` and `num_instructions: 1` proves native plugin/language instruction translation at that address, independently of the selected `cmd.pdc`; it does not prove native C decompilation readiness. For native C, select a known native r2ghidra backend with `-d pd:g` as above or `use_decompiler` with `name: "r2ghidra"` and check its returned selection result, then require an actual successful `decompile_function` at an existing or explicitly analyzed function. Inspect the returned C text and diagnostics; successful native C is not an XML-envelope result. Creating new function analysis changes state and needs authorization. Tool discovery, a registered backend or existing language files are not execution proof. `list_functions` can implicitly run `aaa` when no functions exist, despite its read-only catalog mode; do not use it as a harmless readiness check or promise that `-R` makes every listed tool nonmutating.

## Analyze and preserve native evidence

Run only the requested analysis: metadata, sections, bytes and raw instruction p-code need not begin with general `analyze`. When function recovery is needed, choose an explicit `analyze` level and optional `timeout_seconds`; level 0 already runs `aa`, not a no-op. Use explicit addresses for targeted function details and decompilation. Distinguish observed bytes/native output, qualified analysis contracts and unresolved hypotheses.

- For NE/OMF provenance, call `list_relocation_records` with optional `page_size`; pass the returned `pagination.nextCursor` unchanged for the next page. Pages count source records, not application sites. Preserve each record's `native_key`, nested `applications`, OMF `subrecords` and `context`. Absent native scalar fields stay absent; empty structural `frame`, `target` or `reference` objects provide no decoded reference evidence. Missing providers/features are errors, not empty success. Top-level/table status concerns raw enumeration; record `resolution`, `application_status` and reasons are separate evidence. OMF subrecord `status` describes parsing/support, not knowledge of all referenced state; `complete` can coexist with a reference's `undefined_thread` status and absent decoded `method`/`datum`. Consult frame/target reference and application statuses separately. Zero applications alone proves neither resolution nor completeness. OMF targets remain link-unresolved; synthetic sections, NE segment-offset and synthetic-import coordinates are not demonstrated runtime selectors/addresses. Use `get_binary_metadata` only for a known exact native `key`, not as a dump or fallback parser.
- `apply_borland_profiles` explicitly applies/reapplies native qualified contracts independently of general analysis. Preserve refusal diagnostics and conflicting current/manual state; command success does not mean every fact qualified or establish a universal historical ABI.
- `read_instruction_pcode` translates instructions without function analysis. `export_function_pcode` and `get_decompiler_context` require an analyzed function and return native XML inside the JSON2 envelope's `res`. Preserve `res`, `logs`, `error` and `code`, including error content, rather than treating nonempty text/XML as success. Optional `language` is a scoped native Sleigh ID, not a persistent mode switch or proof of selector values. `decompile_function` returns paginated C text with diagnostics; it is not the XML-envelope tools.

For semantic C inspection where signedness or truncation matters, select `r2ghidra.casts=true` in the owned native session as above; the default suppresses needed display casts, so cross-check p-code/type evidence rather than treating omitted printed casts as missing semantics or enabled casts as a correctness guarantee.

Report the selected target/components, requested addresses/domains, actual observations and remaining limits. Do not replace unresolved external/group targets or unknown selector state with guessed runtime addresses.

## Preserve active analysis

Do not close an existing analysis unconditionally at task end. Switching files requires explicit `close_file`, which discards unsaved in-memory analysis; preserve needed work first using a separately selected project operation. With an existing `-D /absolute/artifact/directory`, `save_project` and `restore_project` accept a single filename in `name`. Restore requires the matching original already opened normally and compatible loader/relocation state; it does not open saved paths, switch files or prepare profiles implicitly. Profile-created views may need explicit preparation after reopening, before restore. Check native save/restore status and inspect restored state; neither startup nor file switching provides automatic persistence.
