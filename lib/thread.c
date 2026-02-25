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
		scratch_slot = mem_Arena_create(-1, -1);
	}

	res = mem_ArenaTemp_begin(scratch_slot);
	return res;
}

void
tctx_logFrameBegin(S8 name, B32 debug_only)
{
	tctx_MsgFrame *frame = mem_Slice_push(&tctx.log_frames, tctx.log_arena);
	frame->name = name;
	frame->debug_only = debug_only;
}

void
tctx_logFrameEnd(U32 kind_mask, Buffer *buf)
{
	int i;
	tctx_Msg msg;
	tctx_MsgFrame frame;

	if (buf == NIL) {
		buf = &tctx.console_error;
	}

	frame = mem_Slice_pop(&tctx.log_frames);

	if (frame.debug_only && RELEASE_BUILD) {
		return;
	}

	printBuffer(buf, S8("||| %s |||\n"), frame.name);

	for (i = 0; i < frame.msg_list.len; i++) {
		msg = frame.msg_list.data[i];

		if (kind_mask != 0 && (msg.kind & kind_mask) == 0) {
			continue;
		}

		if (frame.debug_only) {
			printBuffer(buf, S8("DEBUG "));
		}

		switch (msg.kind) {
		case tctx_MsgInfo:
			printBuffer(buf, S8("INFO / "));
			break;
		case tctx_MsgWarn:
			printBuffer(buf, S8("WARN / "));
			break;
		case tctx_MsgError:
			printBuffer(buf, S8("ERROR / "));
			break;
		default: break;
		}

		if (RELEASE_BUILD) {
			printBuffer(buf, S8("%s\n"), msg.msg);
		} else {
			printBuffer(buf, S8("%s / Line %d: %s\n"), msg.filename, (int)msg.line, msg.msg);
		}
	}
}

void
tctx_logAppendFileLine(ISize line, S8 filename, tctx_MsgKind kind, S8 fmt, ...)
{
	Buffer buf;
	tctx_Msg *msg;
	tctx_MsgFrame *frame;
	va_list args;

	assert(tctx.log_frames.len > 0 || tctx.log_frames.data != NIL);
	frame = tctx.log_frames.data + tctx.log_frames.len - 1;

	buf = Buffer(mem_make(tctx.log_arena, U8, 2048), 2048);
	msg = mem_Slice_push(&frame->msg_list, tctx.log_arena);
	
	va_start(args, fmt);
	__printVaList(&buf, fmt, args);
	va_end(args);
	
	msg->kind = kind;
	msg->msg = Buffer_buildS8(&buf, tctx.log_arena);
	msg->line = line;
	msg->filename = filename;
}