#define __BitSet_WORD_BITS (8 * sizeof(unsigned int))

BitSet
BitSet_alloc(mem_Arena *a, ISize len)
{
	BitSet res = {0};
	ISize alloc_size = (len + __BitSet_WORD_BITS - 1) / __BitSet_WORD_BITS;
	res.ba = mem_make(a, unsigned int, alloc_size);
	return res;
}

void
BitSet_setIdx(BitSet *ba, ISize idx)
{
	res->ba[idx / __BitSet_WORD_BITS] |= (1 << (idx & (__BitSet_WORD_BITS - 1)));
}

#undef __BitSet_WORD_BITS