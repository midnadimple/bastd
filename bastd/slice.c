// Shared by all generic types
#ifndef BASTD_SLICE_C
#defien BASTD_SLICE_C

typedef enum slice_CompareResult slice_CompareResult;
enum slice_CompareResult {
	slice_CompareResult_first_elem_less = -1,
	slice_CompareResult_equal = 0,
	slice_CompareResult_first_elem_greater = 1,
};

#endif//BASTD_SLICE_C

// GENERIC SECTION, REDEFINED ON EVERY INCLUDE
#ifndef sl_TYPE
#error "No sl_TYPE given"
#endif

#define sl_NAME CONCAT(sl_, sl_TYPE)

#include "rand.c"

typedef struct sl_NAME sl_NAME;
struct sl_NAME {
	sl_TYPE *elems;
	U64 len;
	U64 cap;
};

FUNCTION sl_NAME
CONCAT(sl_NAME, _create) (sl_TYPE *initial_data, U64 initial_len)
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
		U64 current_size = sizeof(*slice->elems) * slice->cap;
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

FUNCTION sl_TYPE*
CONCAT(sl_NAME, _get) (sl_NAME slice, U64 idx)
{
	if (idx >= slice.len) {
		// TODO not sure what the appropriate error handling is here
		return NIL;
	}
	return slice.elems + idx;
}

typedef slice_CompareResult (* CONCAT(sl_NAME, _CompareProc))(sl_TYPE, sl_TYPE);

FUNCTION void
CONCAT(sl_NAME, _sort) (sl_NAME *slice, CONCAT(sl_NAME, _CompareProc) compare_proc)
{
	// QuickSort
	if (slice->len < 2) {
		return;
	}

	U64 pivot_idx = rand_next() % slice->len;
	sl_NAME 

	for (int i = 0; i < slice->len; i++) {
		sl_TYPE val = slice->elems[i];
		slice_CompareResult cmp = compare_proc(val, slice->elems[pivot_idx]);
		if (cmp == slice_CompareResult_first_elem_less) {
			
		} else {

		}
	}
}

#undef sl_NAME
#undef sl_TYPE