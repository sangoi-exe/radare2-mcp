/* r2mcp - MIT - Copyright 2026 */

static void relocation_fields(PJ *pj, Sdb *db, const char *key, const char *const *numbers, const char *const *strings) {
	r_strf_buffer (256);
	const char *const *field;
	for (field = numbers; field && *field; field++) {
		const char *native_key = r_strf ("%s.%s", key, *field);
		if (sdb_const_get (db, native_key, NULL)) {
			pj_kn (pj, *field, sdb_num_get (db, native_key, NULL));
		}
	}
	for (field = strings; field && *field; field++) {
		const char *value = sdb_const_get (db, r_strf ("%s.%s", key, *field), NULL);
		if (value) {
			pj_ks (pj, *field, value);
		}
	}
}

static ut64 relocation_count(Sdb *db, const char *key, const char *field) {
	r_strf_buffer (256);
	return sdb_num_get (db, r_strf ("%s.%s", key, field), NULL);
}

static void relocation_applications(PJ *pj, Sdb *db, const char *key) {
	static const char *const numbers[] = { "paddr", "vaddr", "target_vaddr", "subrecord_index", "source_segment", "source_offset", "width", NULL };
	static const char *const strings[] = { "original", "source_address_domain", NULL };
	ut64 count = relocation_count (db, key, "application_count");
	ut64 i;
	pj_ka (pj, "applications");
	for (i = 0; i < count; i++) {
		r_strf_var (application, 128, "%s.application.%"PFMT64u, key, i);
		pj_o (pj);
		relocation_fields (pj, db, application, numbers, strings);
		pj_end (pj);
	}
	pj_end (pj);
}

static void relocation_omf_reference(PJ *pj, Sdb *db, const char *key, const char *which) {
	static const char *const numbers[] = { "thread", "thread_index", "thread_paddr", "method", "datum", NULL };
	static const char *const strings[] = { "status", "identity_status", "kind", "name", NULL };
	r_strf_var (reference, 160, "%s.%s", key, which);
	pj_ko (pj, which);
	relocation_fields (pj, db, reference, numbers, strings);
	pj_end (pj);
}

static void relocation_omf_subrecords(PJ *pj, Sdb *db, const char *key) {
	static const char *const numbers[] = { "paddr", "size", "raw", "thread_index", "locat", "fixdat", "location", "width", "segment_relative", "data_record_offset", "displacement_present", "displacement_width", "displacement", NULL };
	static const char *const strings[] = { "kind", "status", "reason", "location_name", "resolution", "application_status", "target_address_domain", NULL };
	static const char *const thread_numbers[] = { "frame", NULL };
	ut64 count = relocation_count (db, key, "subrecord_count");
	ut64 i;
	pj_ka (pj, "subrecords");
	for (i = 0; i < count; i++) {
		r_strf_var (subrecord, 128, "%s.subrecord.%"PFMT64u, key, i);
		r_strf_var (kind_key, 160, "%s.kind", subrecord);
		const char *kind = sdb_const_get (db, kind_key, NULL);
		pj_o (pj);
		pj_kn (pj, "subrecord_index", i);
		relocation_fields (pj, db, subrecord, numbers, strings);
		if (kind && !strcmp (kind, "thread")) {
			relocation_fields (pj, db, subrecord, thread_numbers, NULL);
			relocation_omf_reference (pj, db, subrecord, "reference");
		} else if (kind && !strcmp (kind, "fixup")) {
			relocation_omf_reference (pj, db, subrecord, "frame");
			relocation_omf_reference (pj, db, subrecord, "target");
		}
		pj_end (pj);
	}
	pj_end (pj);
}

static void relocation_omf_context(PJ *pj, Sdb *db) {
	static const char *const counts[] = { "count", NULL };
	static const char *const status[] = { "status", NULL };
	static const char *const external_numbers[] = { "index", "type_index", "record_paddr", "paddr", "local", NULL };
	static const char *const external_strings[] = { "name", "index_status", NULL };
	static const char *const group_numbers[] = { "index", "paddr", "name_index", "member_count", NULL };
	static const char *const group_strings[] = { "name", "status", NULL };
	static const char *const dialect[] = { "dialect", NULL };
	ut64 count = relocation_count (db, "omf.externals", "count");
	ut64 i;
	pj_ko (pj, "context");
	relocation_fields (pj, db, "omf", NULL, dialect);
	pj_ko (pj, "externals");
	relocation_fields (pj, db, "omf.externals", counts, status);
	pj_ka (pj, "entries");
	for (i = 0; i < count; i++) {
		r_strf_var (key, 64, "omf.external.%"PFMT64u, i + 1);
		pj_o (pj);
		pj_ks (pj, "native_key", key);
		relocation_fields (pj, db, key, external_numbers, external_strings);
		pj_end (pj);
	}
	pj_end (pj);
	pj_end (pj);
	count = relocation_count (db, "omf.groups", "count");
	pj_ko (pj, "groups");
	relocation_fields (pj, db, "omf.groups", counts, status);
	pj_ka (pj, "entries");
	for (i = 0; i < count; i++) {
		r_strf_var (key, 64, "omf.group.%"PFMT64u, i + 1);
		ut64 members = relocation_count (db, key, "member_count");
		ut64 j;
		pj_o (pj);
		pj_ks (pj, "native_key", key);
		relocation_fields (pj, db, key, group_numbers, group_strings);
		pj_ka (pj, "members");
		for (j = 0; j < members; j++) {
			r_strf_var (member, 128, "%s.member.%"PFMT64u, key, j);
			static const char *const segment[] = { "segment", NULL };
			pj_o (pj);
			relocation_fields (pj, db, member, segment, NULL);
			pj_end (pj);
		}
		pj_end (pj);
		pj_end (pj);
	}
	pj_end (pj);
	pj_end (pj);
	pj_end (pj);
}

static void relocation_omf_records(PJ *pj, Sdb *db) {
	static const char *const numbers[] = { "paddr", "size", "type", "subrecord_count", "fixup_count", "application_count", "data_record_paddr", "data_record_type", "data_paddr", "source_segment", "data_offset", NULL };
	static const char *const strings[] = { "status", "resolution", "application_status", "reason", "raw", "record_address_domain", "source_address_domain", "target_address_domain", NULL };
	ut64 count = relocation_count (db, "omf.relocs", "count");
	ut64 i;
	relocation_omf_context (pj, db);
	pj_ka (pj, "records");
	for (i = 0; i < count; i++) {
		r_strf_var (key, 64, "omf.reloc.%"PFMT64u, i);
		pj_o (pj);
		pj_ks (pj, "native_key", key);
		relocation_fields (pj, db, key, numbers, strings);
		relocation_omf_subrecords (pj, db, key);
		relocation_applications (pj, db, key);
		pj_end (pj);
	}
	pj_end (pj);
}

static void relocation_ne_records(PJ *pj, Sdb *db) {
	static const char *const table_numbers[] = { "observed_count", "count", "declared_count", NULL };
	static const char *const table_strings[] = { "status", "reason", NULL };
	static const char *const numbers[] = { "paddr", "source_segment", "record_index", "type", "flags", "source_offset", "target_word0", "target_word1", "additive", "application_count", "target_vaddr", "import_ordinal", NULL };
	static const char *const strings[] = { "resolution", "application_status", "reason", "record_address_domain", "source_address_domain", "target_address_domain", "import_name", NULL };
	ut64 segments = relocation_count (db, "ne.relocs", "segment_count");
	ut64 segment;
	pj_ko (pj, "context");
	pj_ka (pj, "segments");
	for (segment = 0; segment < segments; segment++) {
		r_strf_var (key, 64, "ne.reloc.%"PFMT64u, segment + 1);
		pj_o (pj);
		pj_ks (pj, "native_key", key);
		pj_kn (pj, "source_segment", segment + 1);
		relocation_fields (pj, db, key, table_numbers, table_strings);
		pj_end (pj);
	}
	pj_end (pj);
	pj_end (pj);
	pj_ka (pj, "records");
	for (segment = 0; segment < segments; segment++) {
		r_strf_var (table, 64, "ne.reloc.%"PFMT64u, segment + 1);
		ut64 count = relocation_count (db, table, "observed_count");
		ut64 i;
		for (i = 0; i < count; i++) {
			r_strf_var (key, 96, "%s.%"PFMT64u, table, i);
			pj_o (pj);
			pj_ks (pj, "native_key", key);
			relocation_fields (pj, db, key, numbers, strings);
			relocation_applications (pj, db, key);
			pj_end (pj);
		}
	}
	pj_end (pj);
}

static char *tool_list_relocation_records(ServerState *ss, RJson *tool_args) {
	RCore *core = ss->rstate->core;
	if (ss->rstate->http_mode || ss->rstate->frida_mode || !core) {
		return jsonrpc_tooltext_error_response ("Relocation records require a local native NE or OMF binary provider.");
	}
	static const char *const numbers[] = { "count", "segment_count", "declared_segment_count", NULL };
	static const char *const strings[] = { "status", "reason", NULL };
	const char *error = NULL;
	PJ *pj = NULL;
	R_CRITICAL_ENTER (core);
	RBinFile *file = core->bin->cur;
	RBinPlugin *plugin = file? r_bin_file_cur_plugin (file): NULL;
	const char *format = plugin? plugin->meta.name: NULL;
	bool ne = format && !strcmp (format, "ne");
	bool omf = format && !strcmp (format, "omf");
	Sdb *db = plugin && plugin->get_sdb? plugin->get_sdb (file): NULL;
	if (!db || (!ne && !omf)) {
		error = "The current binary provider does not support native NE or OMF relocation records.";
	} else if (!sdb_const_get (db, ne? "ne.relocs.status": "omf.relocs.status", NULL)) {
		error = "Native relocation-record provenance is unavailable; a matching host provider is required.";
	} else {
		pj = pj_new ();
		pj_o (pj);
		pj_ks (pj, "format", format);
		relocation_fields (pj, db, ne? "ne.relocs": "omf.relocs", numbers, strings);
		if (ne) {
			relocation_ne_records (pj, db);
		} else {
			relocation_omf_records (pj, db);
		}
		pj_end (pj);
	}
	R_CRITICAL_LEAVE (core);
	if (error) {
		return jsonrpc_tooltext_error_response (error);
	}
	return tool_cmd_response_paginated (ss, pj_drain (pj), tool_args);
}
