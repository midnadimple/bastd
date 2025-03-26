#ifndef BASTD_STRING_C
#define BASTD_STRING_C

FUNCTION S8
S8_alloc(U64 len, m_Allocator *perm)
{
	S8 res = {0};
	res.len = len;
	res.raw = m_MAKE(U8, len + 1, perm);
	res.raw[len] = 0;
	return res;
}

FUNCTION I64
S8_difference(S8 s1, S8 s2)
{
	return m_memoryDifference(s1.raw, s2.raw);
}

FUNCTION S8
S8_concat(S8 s1, S8 s2, m_Allocator *perm)
{
	U64 len = s1.len + s2.len;
	S8 s = S8_alloc(len, perm);
	m_memoryCopy(s.raw, s1.raw, s1.len);
	m_memoryCopy(&s.raw[s1.len], s2.raw, s2.len);
	return s;
}

FUNCTION S8
S8_sub(S8 s, U64 start, U64 end, m_Allocator *perm)
{
	S8 res = {0};
	if (end <= s.len && start < end) {
		res = S8_alloc(end - start, perm);
		m_memoryCopy(res.raw, &s.raw[start], res.len);
	}
	return res;
}

FUNCTION B32
S8_contains(S8 haystack, S8 needle)
{
	B32 found = FALSE;
	for (U64 i = 0, j = 0; i < haystack.len && !found; i++) {
		while (haystack.raw[i] == needle.raw[j]) {
			j += 1;
			i += 1;
			if (j == needle.len) {
				found = TRUE;
				break;
			}
		}
	}
	return found;
}

FUNCTION U64
S8_indexOf(S8 haystack, S8 needle)
{
	for (U64 i = 0; i < haystack.len; i += 1) {
		U64 j = 0;
		U64 start = i;
		while (haystack.raw[i] == needle.raw[j]) {
			j += 1;
			i += 1;
			if (j == needle.len) {
				return start;
			}
		}
	}
	return (U64)-1;
}

FUNCTION S8
S8_subView(S8 haystack, S8 needle)
{
	S8 r = {0};
	U64 start_index = S8_indexOf(haystack, needle);
	if (start_index < haystack.len) {
		r.raw = &haystack.raw[start_index];
		r.len = needle.len;
	}
	return r;
}

FUNCTION B32
S8_equal(S8 a, S8 b)
{
	if (a.len != b.len) {
		return FALSE;
	}
	return m_memoryDifference(a.raw, b.raw) == 0;
}

FUNCTION B32
S8_empty(S8 s)
{
	return S8_equal(s, (S8){.len = 0, .raw = NIL});
}

FUNCTION S8
S8_view(S8 s, U64 start, U64 end)
{
	if (end < start || end - start > s.len) {
		return (S8){0};
	}
	return (S8){end - start, s.raw + start};
}

FUNCTION S8
S8_clone(S8 s, m_Allocator *perm)
{
	S8 r = {0};
	if (s.len) {
		r.raw = perm->alloc(perm->ctx, NIL, 0, s.len);
		r.len = s.len;
		m_memoryCopy(r.raw, s.raw, s.len);
	}
	return r;
}

#define sl_TYPE S8
#include "slice.c"

FUNCTION sl_S8
S8_split(S8 s, S8 delimiter, m_Allocator *perm)
{
	sl_S8 arr = sl_S8_create(NIL, 0);
	U64 start = 0;
	for (U64 i = 0; i < s.len; i += 1) {
		if (s.raw[i] != delimiter.raw[0]) {
			continue;
		}

		if (m_memoryDifference(&s.raw[i], delimiter.raw) == 0) {
			// Clone the substring before the delimiter.
			U64 end = i;
			S8 cloned = S8_sub(s, start, end, perm);
			*sl_S8_push(&arr, perm) = cloned;
			start = end + delimiter.len;
		}
	}
	// Get the last segment.
	if (start + delimiter.len < s.len) {
		S8 cloned = S8_sub(s, start, s.len, perm);
		*sl_S8_push(&arr, perm) = cloned;
	}
	return arr;
}

FUNCTION sl_S8
S8_splitView(S8 s, S8 delimiter, m_Allocator *perm) {
	sl_S8 arr = sl_S8_create(NIL, 0);
	U64 start = 0;
	for (U64 i = 0; i < s.len; i += 1) {
		if (s.raw[i] != delimiter.raw[0]) {
			continue;
		}

		if (m_memoryDifference(&s.raw[i], delimiter.raw) == 0) {
			U64 end = i;
			S8 view = S8_view(s, start, end);
			*sl_S8_push(&arr, perm) = view;
			start = end + delimiter.len;
		}
	}
	if (start + delimiter.len < s.len) {
		S8 view = S8_view(s, start, s.len);
		*sl_S8_push(&arr, perm) = view;
	}
	return arr;
}

FUNCTION S8
S8_join(sl_S8 s, S8 join, m_Allocator *perm) {
	U64 total_length = s.len * join.len;
	for (U64 i = 0; i < s.len; i += 1) {
		total_length += s.elems[i].len;
	}

	U8 *mem = m_MAKE(U8, total_length + 1, perm);
	U64 offset = 0;
	for (U64 i = 0; i < s.len; i += 1) {
		m_memoryCopy(&mem[offset], s.elems[i].raw, s.elems[i].len);
		offset += s.elems[i].len;

		if (i == s.len - 1) {
			break;
		}

		m_memoryCopy(&mem[offset], join.raw, join.len);
		offset += join.len;
	}

	mem[total_length] = 0;

	return (S8){total_length, mem};
}

// Should probably get a better hashing func, but oh well
FUNCTION U64
S8_hash_prime19(S8 s)
{
	U64 h = 0x100;
	for (U64 i = 0; i < s.len; i++) {
		h ^= s.raw[i];
		h *= 1111111111111111111u;
	}
	return h;
}

#endif//BASTD_STRING_C