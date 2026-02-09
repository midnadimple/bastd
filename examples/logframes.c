#include "../lib.h"
#include "../lib.c"

int
entry(S8Slice args)
{
	tctx_logFrameBegin(S8("Test #1"), FALSE);
	tctx_logAppend(tctx_MsgInfo, S8("This is a test. Should always output"));

	tctx_logFrameBegin(S8("Test #2"), TRUE);
	tctx_logAppend(tctx_MsgError, S8("This is another test. Should only output in debug mode"));

	tctx_logFrameEnd(0, NIL);
	tctx_logFrameEnd(0, NIL);
	return 0;
}