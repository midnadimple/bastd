// GENERIC FILE, REDEFINED ON EVERY INCLUDE
#ifndef sl_TYPE
#error "No sl_TYPE given"
#endif

#define sl_NAME CONCAT(sl_, sl_TYPE)

typedef struct sl_NAME sl_NAME;
struct sl_NAME {
	sl_TYPE *elems;
	ISize len;
	ISize cap;
};

FUNCTION sl_NAME
CONCAT(sl_NAME, _create) (sl_TYPE *initial_data, ISize initial_len)
{
	sl_NAME res = {0};
	res.elems = initial_data;

	res.len = initial_len;
	res.cap = initial_len;
	
	return res;
}

FUNCTION sl_TYPE*
CONCAT(sl_NAME, _push) (sl_NAME *slice, m_Allocator *perm)
{
	if (slice->len >= slice->cap) {
		// Slice is full, Grow slice to double its current size
		slice->cap = slice->cap ? slice->cap : 1;
		ISize current_size = sizeof(*slice->elems) * slice->cap;
		slice->elems = perm->alloc(perm->ctx, slice->elems, current_size, current_size*2);
		slice->cap *= 2;
	}

	// Return the slot the end of the slice and increase the length
	return slice->elems + slice->len++;
}

FUNCTION sl_TYPE
CONCAT(sl_NAME, _pop) (sl_NAME *slice)
{
	return *(slice->elems + --slice->len);
}

#undef sl_NAME
#undef sl_TYPE