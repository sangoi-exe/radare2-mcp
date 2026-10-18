/* r2mcp - MIT - Copyright 2026 - pancake */

char *r2mcp_project_directory(const char *directory) {
	char *canonical = r2mcp_path_realpath (directory);
	if (canonical && !r_file_is_directory (canonical)) {
		R_FREE (canonical);
	}
	return canonical;
}

typedef struct {
	ServerState *ss;
	const char *command;
	int status;
	char *output;
} ProjectCommand;

static void *run_project_command(void *user) {
	ProjectCommand *operation = user;
	RCore *core = operation->ss->rstate->core;
	r_cons_push (core->cons);
	operation->status = r_core_cmd0 (core, operation->command);
	operation->output = strdup (r_str_get (r_cons_get_buffer (core->cons, NULL)));
	r_cons_pop (core->cons);
	return operation;
}

char *r2mcp_project_command(ServerState *ss, const char *name, bool restore) {
	if (R_STR_ISEMPTY (ss->project_dir)) {
		return jsonrpc_tooltext_error_response ("Native project operations require an explicit artifact directory: start r2mcp with -D <directory>.");
	}
	if (ss->rstate->http_mode || ss->rstate->frida_mode || !ss->rstate->core || !ss->rstate->file_opened) {
		return jsonrpc_tooltext_error_response ("Open the matching original normally in the local native core before saving or restoring a project.");
	}
	if (R_STR_ISEMPTY (name) || !strcmp (name, ".") || !strcmp (name, "..") || strpbrk (name, "/\\")) {
		return jsonrpc_tooltext_error_response ("Project name must be a single filename under the configured -D directory.");
	}
	char *path = r_file_new (ss->project_dir, name, NULL);
	if (r_file_exists (path) && !r2mcp_path_is_within_sandbox (path, ss->project_dir)) {
		free (path);
		return jsonrpc_tooltext_error_response ("Project artifact resolves outside the configured -D directory.");
	}
	char *original = r2mcp_path_realpath (ss->rstate->current_file);
	if (!restore && original && !strcmp (path, original)) {
		free (original);
		free (path);
		return jsonrpc_tooltext_error_response ("A project artifact must not replace the opened original.");
	}
	free (original);
	char *argument = r_str_arg_escape (path);
	char *command = r_str_newf ("'prj %s %s", restore? "restore": "snapshot", argument);
	free (argument);
	ProjectCommand operation = { .ss = ss, .command = command, .status = -1 };
	R_CRITICAL_ENTER (ss->rstate->core);
	bool was_sandboxed = r_sandbox_enable (false);
	if (was_sandboxed && r_sandbox_disable (true)) {
		R_CRITICAL_LEAVE (ss->rstate->core);
		free (command);
		free (path);
		return jsonrpc_tooltext_error_response ("Cannot enter the admitted native project artifact I/O scope.");
	}
	r2mcp_log_reset (ss);
	r_sandbox_run_path (R_SANDBOX_GRAIN_FILES | R_SANDBOX_GRAIN_DISK | R_SANDBOX_GRAIN_HIDDEN, ss->project_dir, run_project_command, &operation);
	char *diagnostics = r2mcp_log_drain (ss);
	if (was_sandboxed) {
		r_sandbox_disable (false);
	}
	R_CRITICAL_LEAVE (ss->rstate->core);
	char *message = r_str_newf ("Native project %s %s (status %d): %s\n%s%s", restore? "restore": "save", operation.status? "failed": "succeeded", operation.status, path, r_str_get (operation.output), r_str_get (diagnostics));
	char *response = operation.status? jsonrpc_tooltext_error_response (message): jsonrpc_tooltext_response (message);
	free (message);
	free (diagnostics);
	free (operation.output);
	free (command);
	free (path);
	return response;
}
