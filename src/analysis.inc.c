/* r2mcp - MIT - Copyright 2026 - pancake */

static char *tool_apply_borland_profiles(ServerState *ss, RJson *tool_args) {
	if (ss->http_mode || ss->rstate->frida_mode || !ss->rstate->core) {
		return jsonrpc_tooltext_error_response ("Qualified Borland profiles require a local native analysis core.");
	}
	char *res = r2mcp_cmd (ss, "{\"cmd\":\"a:borland\",\"trim\":false}");
	return native_command_result (ss, tool_args, res, true);
}

static char *tool_show_function_details(ServerState *ss, RJson *tool_args) {
	ut64 address;
	if (!validate_address_param (ss, tool_args, "address", &address)) {
		return jsonrpc_error_numeric_param ("address");
	}
	char *res = r2mcp_cmdf (ss, "'@0x%"PFMT64x"'afij", address);
	r_str_trim (res);
	RJson *functions = r_json_parsedup (res);
	if (!functions || functions->type != R_JSON_ARRAY) {
		char *response = jsonrpc_tooltext_error_response (res);
		r_json_free (functions);
		free (res);
		return response;
	}
	if (!functions->children.first) {
		r_json_free (functions);
		free (res);
		return jsonrpc_tooltext_error_response ("No analyzed function at the requested address.");
	}
	PJ *pj = pj_new ();
	pj_o (pj);
	pj_kn (pj, "address", address);
	pj_k (pj, "functions");
	pj_raw (pj, res);
	pj_end (pj);
	r_json_free (functions);
	free (res);
	char *json = pj_drain (pj);
	char *response = jsonrpc_tool_response (json, json, ss->content_mode);
	free (json);
	return response;
}

static char *tool_get_binary_metadata(ServerState *ss, RJson *tool_args) {
	const char *key;
	if (!validate_required_string_param (tool_args, "key", &key) || R_STR_ISEMPTY (key)) {
		return jsonrpc_error_missing_param ("key");
	}
	RCore *core = ss->rstate->core;
	if (ss->http_mode || !core) {
		return jsonrpc_tooltext_error_response ("Binary metadata requires a local native binary provider.");
	}
	PJ *pj = NULL;
	R_CRITICAL_ENTER (core);
	RBinFile *file = core->bin->cur;
	RBinPlugin *plugin = file? r_bin_file_cur_plugin (file): NULL;
	Sdb *db = plugin && plugin->get_sdb? plugin->get_sdb (file): NULL;
	const char *value = db? sdb_const_get (db, key, NULL): NULL;
	if (value) {
		pj = pj_new ();
		pj_o (pj);
		pj_ks (pj, "key", key);
		pj_ks (pj, "value", value);
		pj_end (pj);
	}
	R_CRITICAL_LEAVE (core);
	if (!pj) {
		return jsonrpc_tooltext_error_response (db? "The exact metadata key is absent from the current binary provider.": "The current binary plugin has no native metadata provider.");
	}
	char *json = pj_drain (pj);
	char *response = jsonrpc_tool_response (json, json, ss->content_mode);
	free (json);
	return response;
}

static char *tool_get_comment(ServerState *ss, RJson *tool_args) {
	ut64 address;
	if (!validate_address_param (ss, tool_args, "address", &address)) {
		return jsonrpc_error_numeric_param ("address");
	}
	RCore *core = ss->rstate->core;
	if (ss->http_mode || !core) {
		return jsonrpc_tooltext_error_response ("Comments require a local native analysis core.");
	}
	PJ *pj = pj_new ();
	R_CRITICAL_ENTER (core);
	const char *comment = r_meta_get_string (core->anal, R_META_TYPE_COMMENT, address);
	pj_o (pj);
	pj_kn (pj, "address", address);
	if (comment) {
		pj_ks (pj, "comment", comment);
	} else {
		pj_knull (pj, "comment");
	}
	pj_end (pj);
	R_CRITICAL_LEAVE (core);
	char *json = pj_drain (pj);
	char *response = jsonrpc_tool_response (json, json, ss->content_mode);
	free (json);
	return response;
}

static char *tool_get_data_type(ServerState *ss, RJson *tool_args) {
	const char *name;
	if (!validate_required_string_param (tool_args, "name", &name) || R_STR_ISEMPTY (name)) {
		return jsonrpc_error_missing_param ("name");
	}
	RCore *core = ss->rstate->core;
	if (ss->http_mode || !core) {
		return jsonrpc_tooltext_error_response ("Named types require a local native analysis core.");
	}
	R_CRITICAL_ENTER (core);
	RAnalBaseType *type = r_anal_get_base_type (core->anal, name);
	R_CRITICAL_LEAVE (core);
	if (!type) {
		return jsonrpc_tooltext_error_response ("Native named base type is absent or unreadable.");
	}
	const char *kind;
	switch (type->kind) {
	case R_ANAL_BASE_TYPE_KIND_STRUCT: kind = "struct"; break;
	case R_ANAL_BASE_TYPE_KIND_UNION: kind = "union"; break;
	case R_ANAL_BASE_TYPE_KIND_ENUM: kind = "enum"; break;
	case R_ANAL_BASE_TYPE_KIND_TYPEDEF: kind = "typedef"; break;
	case R_ANAL_BASE_TYPE_KIND_ATOMIC: kind = "atomic"; break;
	default:
		r_anal_base_type_free (type);
		return jsonrpc_tooltext_error_response ("Unsupported native base-type kind.");
	}
	PJ *pj = pj_new ();
	pj_o (pj);
	pj_ks (pj, "name", type->name);
	pj_ks (pj, "kind", kind);
	pj_kn (pj, "size_bits", type->size);
	if (type->type) {
		pj_ks (pj, "type", type->type);
	}
	size_t i;
	if (type->kind == R_ANAL_BASE_TYPE_KIND_STRUCT || type->kind == R_ANAL_BASE_TYPE_KIND_UNION) {
		RVecAnalTypeMember *members = r_anal_base_type_members (type);
		pj_ka (pj, "members");
		for (i = 0; i < RVecAnalTypeMember_length (members); i++) {
			RAnalTypeMember *member = RVecAnalTypeMember_at (members, i);
			pj_o (pj);
			pj_ks (pj, "name", member->name);
			pj_ks (pj, "type", member->type);
			pj_kn (pj, "offset_bytes", member->offset);
			pj_kn (pj, "bitsize", member->bitsize);
			pj_kn (pj, "count", member->count);
			pj_end (pj);
		}
		pj_end (pj);
	} else if (type->kind == R_ANAL_BASE_TYPE_KIND_ENUM) {
		pj_ka (pj, "cases");
		for (i = 0; i < RVecAnalEnumCase_length (&type->enum_data.cases); i++) {
			RAnalEnumCase *entry = RVecAnalEnumCase_at (&type->enum_data.cases, i);
			pj_o (pj);
			pj_ks (pj, "name", entry->name);
			pj_ki (pj, "value", entry->val);
			pj_end (pj);
		}
		pj_end (pj);
	}
	pj_end (pj);
	r_anal_base_type_free (type);
	char *json = pj_drain (pj);
	char *response = jsonrpc_tool_response (json, json, ss->content_mode);
	free (json);
	return response;
}
