#ifndef BASTD_ARGS_C
#define BASTD_ARGS_C

FUNCTION sl_S8
getArgsSlice(m_Allocator *perm)
{
	int argc;
	char **argv = os_getArgcAndArgv(&argc);
	sl_S8 res = sl_S8_create(NIL, 0);

	for (int i = 0; i < argc; i++) {
		S8 *val = sl_S8_push(&res, perm);
		val->raw = argv[i];
		
		// strlen, bcs the length is broken here for some reason
		char *p = val->raw;
		while (*p != NIL) p++;
		val->len = p - val->raw;
	}

	return res;
}

#endif//BASTD_ARGS_C