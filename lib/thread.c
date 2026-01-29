THREAD_LOCAL ThreadContext tctx;

mem_ArenaTemp
tctx_getScratchArena(mem_Arena **conflicts, ISize num_conflicts)
{
	mem_ArenaTemp res = {0};
	mem_Arena *scratch_slot;
	mem_Arena *conflict_ptr;
	ISize i, j;
	B32 is_conflict;

	for (i = 0; i < tctx_SCRATCH_ARENA_COUNT; i++)  {
		scratch_slot = tctx.scratch_arenas[i];
		is_conflict = FALSE;
		
		for (j = 0; j < num_conflicts; j++) {
			conflict_ptr = conflicts[j];
			if (scratch_slot == conflict_ptr) {
				is_conflict = TRUE;
				break;
			}
		}

		if (is_conflict) {
			return res;
		}
	}

	if (scratch_slot == NIL) {
		scratch_slot = mem_Arena_create(MiB(64), MiB(1));
	}

	res = mem_ArenaTemp_begin(scratch_slot);
	return res;
}

void
tctx_logAppendFileLine(ISize line, S8 filename, tctx_MsgKind kind, S8 fmt, ...)
{
	Buffer buf = Buffer(mem_make(tctx.log_arena, U8, 2048), 2048);
	tctx_Msg *msg = mem_Slice_push(&tctx.log_msg_list, tctx.log_arena);
	va_list args;
	va_start(args, fmt);
	__printVaList(&buf, fmt, args);
	
	msg->kind = kind;
	msg->msg = Buffer_buildS8(&buf, tctx.log_arena);
	msg->line = line;
	msg->filename = filename;

	va_end(args);
}

void
tctx_logOutputToConsole(void)
{
	int i;
	tctx_Msg msg;

	for (i = 0; i < tctx.log_msg_list.len; i++) {
		msg = tctx.log_msg_list.data[i];

		if (msg.kind == tctx_MsgDebug && !DEBUG_BUILD) {
			continue;
		}

		switch (msg.kind) {
		case tctx_MsgInfo:
			printError(S8("INFO / "));
			break;
		case tctx_MsgWarn:
			printError(S8("WARN / "));
			break;
		case tctx_MsgError:
			printError(S8("ERROR / "));
			break;
		default: break;
		}

		#if DEBUG_BUILD
		printError(S8("%s / Line %d: %s\n"), msg.filename, (I64)msg.line, msg.msg);
		#else
		printError(S8("%s\n"), msg.msg);
		#endif

	}

	mem_Arena_clear(tctx.log_arena);
	tctx.log_msg_list.len = 0;
}
