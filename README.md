# Radare2 MCP Server

[![radare2](https://img.shields.io/badge/radare2-6.2.4-green)](https://github.com/radareorg/radare2)

<img width="400" alt="r2mcp logo" src="./r2mcp.png" />

An MCP server to use **radare2** with AI agents such as OpenCode, Mai, VSCode, Claude, CLION, ...

## Features

This implementation provides:

- 💻 Fully written in C using the native r2 APIs
- 🧩 Works from the CLI, as an r2 plugin and as an MCP server
- 🔍 Seamless binary analysis with radare2
- 🔗 Connect to any local or remote r2/iaito session via r2pipe
- 🔒 Supports readonly mode, sandbox lock and restrict tools
- 🔩 Fine grained tools configuration
- 🔁 Direct stdin/stdout communication model
- 🌐 HTTP MCP server mode with optional `X-Session-ID` multiplexing
- 🛠️ Optional raw access to run r2 commands or r2js scripts

## Installation

<img width="400" alt="Screenshot_2025-03-22_at_5 34 47_PM" src="https://github.com/user-attachments/assets/5322c3fc-fc07-4770-96a3-5a6d82d439c2" />
<img width="400" alt="Screenshot_2025-03-22_at_5 36 17_PM" src="https://github.com/user-attachments/assets/132a1de0-6978-4202-8dce-aa3d60551b9a" />

### Using r2pm

The simplest way to install the package is by using `r2pm`:

```bash
$ r2pm -Uci r2mcp
```

The `r2mcp` executable will be copied into r2pm's bindir in your home directory. However, this binary is not supposed to be executed directly from the shell; it will only work when launched by the MCP service handler of your language model of choice.

```bash
$ r2pm -r r2mcp
```

That's the common mcpServer JSON configuration file:

```json
{
  "mcpServers": {
    "radare2": {
      "command": "r2pm",
      "args": ["-r", "r2mcp"]
    }
  }
}
```

### Codex

To install the codex plugin just run the following command:

```bash
make codex-plugin-install
```

This will create a personal marketplace in your home and copy the files from dist/codex-plugin inside.

### Using Docker

Alternatively, you can build the Docker image:

```bash
docker build -t r2mcp .
```

Update your MCP client configuration file (see below) to use the Docker image to use:

- `"command": "docker"`
- `"args": ["run", "--rm", "-i", "-v", "/tmp/data:/data", "r2mcp"]`.

## Configuration

### HTTP Server Mode

Use `-H [address:]port` to run r2mcp as an HTTP MCP server instead of using
stdin/stdout. A bare port binds only to `127.0.0.1`; use `0.0.0.0:port`
explicitly to make the server reachable from the network:

```bash
r2mcp -H 8765
```

```bash
# Public binding; protect this with -a <token> or -A.
r2mcp -H 0.0.0.0:8765 -A
```

Per-session HTTP state is enabled with `-X max[:idle_seconds]` and routed by
the `X-Session-ID` request header:

```bash
r2mcp -H 8765 -X 8:600
```

Each selected session owns its native core or remote endpoint and target mode. With `-L`, `open_session` and `close_session` affect only that `X-Session-ID`; a failed connection leaves its previous target unchanged. Requests without the header use the default target, including a proxy configured by `-u` or `r2mcp.baseurl`. New header-selected sessions start with their own native core and do not inherit that default proxy. Closing or evicting one session does not disconnect another.

HTTP bearer authentication is enabled with `-a <token>`. Clients must send
`Authorization: Bearer <token>` on every `GET` and `POST` request:

```bash
r2mcp -H 8765 -a my-secret-token
```

Use `-A` or `-a random` to generate a random token at startup. The generated
token is printed once on stderr:

```bash
r2mcp -H 8765 -A
```

The same token setting can be supplied with `R2MCP_AUTH_TOKEN`; set it to
`random` to generate one.

`-X` requires radare2 ABI 91 or newer because older radare2 headers do not
expose the HTTP request header API. With older ABIs, `-H` still works as a
single shared HTTP server, but `-X` is ignored. Bearer authentication also
requires this header API.

### r2 Core Plugin Mode

The r2 core plugin exposes the `r2mcp` command inside an existing radare2
session. Starting HTTP mode from r2 reuses the current `RCore`, so MCP tools
operate on the file and analysis state already loaded in that session:

```bash
r2 -e r2mcp.port=8765 -c 'r2mcp start' /bin/ls
```

Useful subcommands:

- `r2mcp start` or `r2mcp http` starts the HTTP MCP server in the background.
- `r2mcp stop`, `r2mcp restart`, and `r2mcp status` manage the background server.
- `r2mcp logs on|off` and `r2mcp logs file <path>` control plugin debug logs.
- `r2mcp approve on|off` and `r2mcp approve url <url>` control supervisor approvals.
- `r2mcp yolo on|off` disables approvals and exposes dangerous run tools.
- `r2mcp config` lists the `r2mcp.*` eval keys.

The plugin registers eval keys for the same server options exposed by the CLI:
`r2mcp.port`, `r2mcp.log`, `r2mcp.logfile`, `r2mcp.auth`, `r2mcp.approve`, `r2mcp.svc`,
`r2mcp.yolo`, `r2mcp.mini`, `r2mcp.permissive`, `r2mcp.run`,
`r2mcp.readonly`, `r2mcp.ignore_analysis`, `r2mcp.prompts`,
`r2mcp.prompts.dir`, `r2mcp.sandbox`, `r2mcp.sandbox.grain`,
`r2mcp.content`, `r2mcp.enabled`, `r2mcp.disabled`,
`r2mcp.session_tools`, `r2mcp.sessions`, `r2mcp.sessions.max`,
`r2mcp.sessions.timeout`, `r2mcp.decompiler`, and `r2mcp.baseurl`.

Without an explicit sandbox root, local file paths containing hidden components are refused. Set `-s /absolute/task/root` (or `r2mcp.sandbox` in plugin mode) to allow files within that root's canonical boundary, including hidden components. Choose a controlled task directory, not a home-wide exemption. Paths must remain absolute; parent-segment traversal, outside-root paths and symlink escapes are refused. Failed or refused `open_file` calls return a tool result with `isError: true`.

With an explicit root, `open_file` accepts only absolute local paths or `file://` paths qualified by that boundary. Other resource URIs, including memory, remote and archive plugin URIs, are unsupported because their resources cannot be qualified by this local-path boundary; they fail before native open, even when a plugin could otherwise handle them. Without an explicit root, native URI admission remains available subject to the native backend and sandbox settings.

When loading the plugin manually with `-i`, set these keys after the plugin is
loaded, for example with `-c 'e r2mcp.port=8765'`, because radare2 processes
early `-e` options before ad hoc plugin initialization.

### Native projects and explicit file lifecycle

Use `-D /absolute/project-artifacts` to select an existing project artifact directory. It is canonicalized at startup and is independent of the binary-input `-s` boundary; it does not admit those artifacts as binary inputs or grant access outside `-s`. Project operations fail visibly when `-D` is unset. Existing `-p` (permissive tools) and `-P` (prompt directories) retain their meanings.

```bash
r2mcp -n -N -s /absolute/task/inputs -D /absolute/task/projects
```

The ordinary `save_project` and `restore_project` tools accept a required `name`, a single literal filename under `-D`. `save_project` invokes only the host's strict `prj snapshot` action: it requires a restorable read-only original and refuses non-restorable sources, including in-memory sessions, before publishing an artifact. Generic `prj save` remains a separate native host action for merge-capable projects, not an MCP fallback or alias. `restore_project` invokes native `prj restore`. Both tools use the existing native project format and command status, including successful operations with no output; project/cache bytes remain opaque to MCP. Restore requires the matching original to be explicitly opened normally first, with matching loader/base/layout and relocation settings. It restores analysis and effective cache edits onto that read-only original without opening saved paths, replaying commands/configuration or switching files. Older project artifacts without the required native identity/cache state are not migrated or loaded through a fallback.

For a raw/headerless file, `open_file` applies an explicit `baddr` through the normal loader's initial map address, before load completion. Reopen the same original with the same `arch`, `bits`, `cpu` and `baddr` before restoring its snapshot; no post-load remapping or baseline recertification is needed.

Opening a different file or changing the current base is refused while a file remains open; the current target and analysis are retained. Opening the same file at the same base remains a no-op. Save any state you need, then call `close_file` explicitly before another `open_file`. `close_file` deliberately discards unsaved in-memory analysis. No autosave occurs on close or process exit. A save → explicit close → normal open of the same original → restore workflow uses the host persistence owner, not a separate MCP analysis store.

Projects that require profile-created original-backed views need an explicit preparation step after every normal open. For BCC20, use `open_file` → `apply_borland_profiles` → `restore_project`, including after a save/close/reopen cycle. Normal loading alone does not create its 15 static body views. Restore does not apply profiles implicitly or change loader/relocation settings to accept an incompatible project. Inspect restored state before reapplying profiles: a saved current-byte or manual-analysis edit may legitimately cause qualification refusal without being lost.

### Native decompiler observations

`apply_borland_profiles` is an explicit ordinary native operation with no arguments. It runs the host's existing `a:borland` qualified executable profiles and helper contracts without running general `analyze`, preparing analysis implicitly or changing its cached level. Every call reapplies qualification, including after restore or an earlier general analysis. The full native JSON2 envelope preserves qualification counts, refusal output and diagnostics; successful command execution does not mean every fact qualified. Conflicting manual/current state is preserved, not erased to force a count. General `analyze` remains separate: its entry/function/variable recovery can create state that a later profile application legitimately refuses. Missing native operations and failed/empty responses are tool errors.

Native analysis reads are explicit and target-qualified. `show_function_details` requires `address` and returns `{address, functions}`, with `functions` taken from canonical targeted `afij`; it no longer uses the current seek implicitly. `get_binary_metadata` requires an exact `key` and returns its native `{key, value}` strings from the current binary plugin's Sdb. There is no database dump, integer guessing or fallback database. Missing keys/providers are errors. `get_comment` requires `address` and returns `{address, comment}`, where `null` means no native comment. `get_data_type` requires a native base-type `name` and reports its actual kind, API-provided `size_bits`, definition string when present, members with native offsets/bit sizes/array counts, or enum names/values. A zero native size/count remains zero; these reads do not parse C, infer layouts or create missing definitions.

`list_relocation_records` is a read-only local native NE/OMF query. It reads only the current format provider's cached Sdb provenance, without `irj`, relocation application, binary reparsing or implicit analysis. Unsupported providers and hosts without this provenance return tool errors. Optional `page_size` and `cursor` paginate the sole top-level `records` array by source record, not application site; use `pagination.nextCursor` from the previous response. With `-C both` or `-C structured`, the same object is available in `structuredContent`. Each record retains its `native_key`, native fields and nested `applications`, including zero-application records. Numeric native fields remain numbers, textual fields remain strings, and absent native scalar fields stay absent rather than becoming zero or resolved targets. Structural `frame`, `target` or `reference` objects can be empty for incomplete subrecords; an empty object provides no decoded reference evidence. `get_binary_metadata` remains a separate exact-key query.

Top-level native `status`, optional `count` and NE segment counts describe raw record enumeration, not whether every target resolved or every application was enumerated. NE table status/count/reason evidence is under `context.segments`; record `resolution`, `application_status` and optional `reason` describe separate outcomes. OMF records preserve raw FIXUPP/FIXUPP32 payloads, data-record coordinates, ordered `subrecords` (including THREAD definitions, frame/target references and displacement evidence) and application `subrecord_index` associations. OMF subrecord `status` describes parsing/support outcome, not whether all referenced state is known: `complete` can coexist with `frame.status` or `target.status` of `undefined_thread`, which withholds that reference's decoded `method` and `datum`. Application status remains separate. OMF external/group tables and their own statuses are nested under `context`, so they cannot replace records as the pagination unit. OMF targets remain link-unresolved; its synthetic section coordinates and NE segment-offset/synthetic-import coordinates are analysis domains, not proven Windows runtime selectors or addresses. An empty applications array alone establishes neither successful resolution nor complete enumeration; consult the native statuses and reasons.

With a matching native r2ghidra plugin and language directory loaded, ordinary tools provide three distinct observations without enabling `-r`:

- `read_instruction_pcode`: required `address`, optional `num_instructions` (default 1, range 1–256). Calls raw Sleigh translation without creating or analyzing a function.
- `export_function_pcode`: required analyzed-function `address`. Exports complete native function XML, including varnodes, ordered p-code operands and retained operation state, rather than an opcode-only summary.
- `get_decompiler_context`: required analyzed-function `address`. Exports architecture debug XML containing imported types/prototypes and the actual supplied context/tracked values. It does not infer unknown DS/SS from register zeros or substitute automatic language matching for an explicit override.

`decompile_function` and all three native observation tools accept optional `language`, a non-empty native Sleigh language ID. The previous `r2ghidra.lang` value is owned and restored after success or error, with set → command → restore inside one current-core critical section. Omission retains the prior setting/native automatic selection. Unknown IDs fail through the native decompiler rather than falling back. Generic decompilation still executes `pdc` and returns paginated C text; an explicit language is accepted only when the selected backend is native r2ghidra. A per-call language does not create static views, change persistent mode state or prepare analysis implicitly.

Keep the linked RCore libraries, r2ghidra plugin, pinned native engine and compiled language files matched; a plugin path alone does not select the host libraries. BCC20 resident code uses automatic Real mode, while its prepared static body views can be observed with per-call `language: "x86:LE:16:Protected Mode"`. Those views are analysis coordinates, not runtime overlay/selector bindings; an original FAR `03e0:002f` is not thereby resolved to a static-view callee. Use targeted function details and `list_sections` for complete coordinates and section geometry/permissions. The 16-bit hexdump display can truncate a high view coordinate (for example, a request at `0x40010000` displays `1000:0000`); bind its returned bytes to the explicit request and structural coordinates, not a guessed runtime selector. `list_memory_maps` reports debugger maps, not the analysis I/O-map set. Native `omj` after explicit preparation and restoration of a saved artifact is a complementary reconstructed-map observation, not a direct read of all internal server maps.

Each result carries the existing host JSON2 envelope: `res` is the complete, untrimmed native text/XML, while `logs`, `error` and `code` retain native diagnostics/status. XML is returned in full, without pagination or XML conversion; parse the `res` string, not the outer envelope. Structured/both content modes also expose that same envelope as `structuredContent`. Missing plugins, failed native operations and empty native output return `isError: true`. Function/context exports require existing host analysis; the observation tools do not manufacture functions, prototypes or selector values.

Use the default in-process `r2ghidra.timeout=0` for captured observations. The bridge's forked timeout path does not return child output to the parent command capture; an empty captured result is reported as a tool error rather than silently selecting another execution path.

For an isolated stdio launch, load the plugin explicitly after disabling user plugins/rc and select its matching language directory, for example:

```bash
r2mcp -n -N -s /absolute/task/root -c 'L /matching/libcore_r2ghidra.so' -c 'e r2ghidra.sleighhome=/matching/x86/data/languages' -d pd:g
```

### SQL through r2xsql

Install the [r2xsql](https://github.com/radareorg/r2xsql) core plugin for the
same radare2 build that r2mcp uses. The `sql` tool is available without `-r`
in local mode. Call `open_file`, `analyze`, and `sql` with arguments such as:

```json
{"query":"SELECT name, size FROM funcs WHERE size > 100 ORDER BY size DESC LIMIT 10"}
```

The `sql` tool passes one statement directly to the loaded SQL core plugin,
preserving SQL operators and quoted text. Results are the plugin's text output.
It supports SQL writes, so it is excluded from read-only mode (`-R`).
Use it with a local r2mcp server or r2mcp's core plugin; remote r2 HTTP client
mode (`-u`) does not expose this tool.

An installed plugin loads automatically. To load a build explicitly, add
`-c 'L /absolute/path/core_r2xsql.so'` to the r2mcp command. Startup commands
run in order before MCP requests or `-T` tests, and their output is discarded
to keep stdout valid for MCP. On macOS/Windows use the actual plugin filename.

### Claude Desktop Integration

In the Claude Desktop app, press `CMD + ,` to open the Developer settings. Edit the configuration file and restart the client after editing the JSON file as explained below:

1. Locate your Claude Desktop configuration file:

   - macOS: `~/Library/Application Support/Claude/claude_desktop_config.json`
   - Windows: `%APPDATA%\Claude\claude_desktop_config.json`

2. Add the following to your configuration file:

```json
{
  "mcpServers": {
    "radare2": {
      "command": "r2pm",
      "args": ["-r", "r2mcp"]
    }
  }
}
```

## VS Code Integration

To use r2mcp with GitHub Copilot Chat in Visual Studio Code by [adding it to your user configuration](https://code.visualstudio.com/docs/copilot/chat/mcp-servers#_add-an-mcp-server-to-your-user-configuration) (see other options [here](https://code.visualstudio.com/docs/copilot/chat/mcp-servers#_add-an-mcp-server)):

1. Open the Command Palette with `CMD + Shift + P` (macOS) or `Ctrl + Shift + P` (Windows/Linux).
2. Search for and select `Copilot: Open User Configuration` (typically found in `~/Library/Application Support/Code/User/mcp.json` in macOS).
3. Add the following to your configuration file:

```json
{
  "servers": {
    "radare2": {
      "type": "stdio",
      "command": "r2pm",
      "args": ["-r", "r2mcp"]
    }
  },
  "inputs": []
}
```

## Zed Integration

You can use r2mcp with Zed as well by [adding it to your configuration](https://zed.dev/docs/ai/mcp):

1. Open the command palette: `CMD + Shift + P` (macOS) or `Ctrl + Shift + P` (Windows/Linux). 
2. Search of `agent: open configuration` or search of `settings`.
3. Add your server as such:

```json
  "context_servers": {
    "r2-mcp-server": {
      "source": "custom",
      "command": "r2pm",
      "args": ["-r", "r2mcp"],
      "env": {}
    }
  }
```
Note: you will need another LLM agent, such as Claude, Gemini or else to be able to use it.

## For Developers

### Build from Source

#### Linux/macOS

To test the server locally, you can build and install it with make:

```bash
make install
```

This will compile the server and place the `r2mcp` binary in `/usr/local/bin` on macOS.

#### Windows

For Windows, just use meson and ninja:

```cmd
meson b
ninja -C b
```
