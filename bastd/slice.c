// Shared by all generic types
#ifndef BASTD_SLICE_C
#define BASTD_SLICE_C

typedef enum slice_CompareResult slice_CompareResult;
enum slice_CompareResult {
	slice_CompareResult_first_elem_less = -1,
	slice_CompareResult_equal = 0,
	slice_CompareResult_first_elem_greater = 1,
};

// I wish C had generics

FUNCTION slice_CompareResult
sl_I64_compare(I64 a, I64 b)
{
	if (a < b) {
		return slice_CompareResult_first_elem_less;
	} else if (a > b) {
		return slice_CompareResult_first_elem_greater;
	} else {
		return slice_CompareResult_equal;
	}
}

FUNCTION slice_CompareResult
sl_U64_compare(U64 a, U64 b)
{
	if (a < b) {
		return slice_CompareResult_first_elem_less;
	} else if (a > b) {
		return slice_CompareResult_first_elem_greater;
	} else {
		return slice_CompareResult_equal;
	}
}

FUNCTION slice_CompareResult
sl_F64_compare(F64 a, F64 b)
{
	if (a < b) {
		return slice_CompareResult_first_elem_less;
	} else if (a > b) {
		return slice_CompareResult_first_elem_greater;
	} else {
		return slice_CompareResult_equal;
	}
}

FUNCTION slice_CompareResult
sl_S8_compare(S8 a, S8 b)
{
	if (a.len < b.len) {
		return slice_CompareResult_first_elem_less;
	} else if (a.len > b.len) {
		return slice_CompareResult_first_elem_greater;
	}

	ISize diff = S8_difference(a, b);
	if (diff > 0) {
		return slice_CompareResult_first_elem_less;
	} else {
		return slice_CompareResult_first_elem_greater;
	}
	
	return slice_CompareResult_equal;
}

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

FUNCTION I64
CONCAT(sl_NAME, _partition) (sl_NAME *slice, I64 start, I64 end, CONCAT(sl_NAME, _CompareProc) compare_proc)
{
	sl_TYPE pivot = slice->elems[end];
	I64 i = start - 1;

	for (I64 j = start; j < end; j++) {
		slice_CompareResult res = compare_proc(slice->elems[j], pivot);
		if (res == slice_CompareResult_first_elem_less) {
			i++;
			sl_TYPE tmp = slice->elems[i];
			slice->elems[i] = slice->elems[j];
			slice->elems[j] = tmp;
		}
	}

	sl_TYPE tmp = slice->elems[i + 1];
	slice->elems[i + 1] = slice->elems[end];
	slice->elems[end] = tmp;

	return i + 1;
}

FUNCTION void
CONCAT(sl_NAME, _sort) (sl_NAME *slice, I64 start, I64 end, CONCAT(sl_NAME, _CompareProc) compare_proc)
{
	// iterative QuickSort
	if (end < 0) {
		end = slice->len - 1;
	}

	if (start < end) {
		I64 pivot_idx = CONCAT(sl_NAME, _partition)(slice, start, end, compare_proc);
		CONCAT(sl_NAME, _sort)(slice, start, pivot_idx - 1, compare_proc);
		CONCAT(sl_NAME, _sort)(slice, pivot_idx + 1, end, compare_proc);
	}
}

#ifndef sl_UNPRINTABLE
FUNCTION void
CONCAT(Buffer_append_, sl_NAME) (Buffer *output, sl_NAME slice)
{
	Buffer_append_U8(output, '{');
	for (int i = 0; i < slice.len - 1; i++) {
		CONCAT(Buffer_append_, sl_TYPE)(output, slice.elems[i]);
		Buffer_append_S8(output, S8(", "));
	}
	CONCAT(Buffer_append_, sl_TYPE)(output, slice.elems[slice.len - 1]); // nice printing
	Buffer_append_U8(output, '}');
}
#endif

#undef sl_NAME
#undef sl_TYPE