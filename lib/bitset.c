
void
BitSet_setIdx(BitSet *ba, ISize idx, mem_Arena *a)
{
	ISize extend_size = 0;
	ISize starting_size = 0;
	int i;

	if (ba == NIL) {
		tctx_logAppend(tctx_MsgDebug, S8("Allocating NIL Bitset (%p)"), ba);
	} else {
		starting_size = ba->len;
	}

	if (starting_size < idx + 1) {
		extend_size = (idx + 1 - starting_size) / 8 + 1;

		tctx_logAppend(tctx_MsgDebug, S8("Extending BitSet (pointer %p, length %d) by %d bytes"), idx, ba, ba->len, extend_size);

		if (a == NIL) {
			tctx_logAppend(tctx_MsgDebug, S8("No arena given for extending Bitset (pointer %p, length %d). Doing nothing"), ba, ba->len);
			return;
		}

		for (i = 0; i < extend_size; i++) {
			mem_Slice_push(ba, a);
		} 
	}
	
	#define __BitSet_WORD_BITS (8 * sizeof(unsigned int))
	
	ba->data[idx / __BitSet_WORD_BITS] |= (1 << (idx & (__BitSet_WORD_BITS - 1)));
	
	#undef __BitSet_WORD_BITS
}

