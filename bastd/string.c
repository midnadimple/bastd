#ifndef BASTD_STRING_C
#define BASTD_STRING_C

typedef struct S8 S8;
struct S8 {
	ISize len;
	U8 *raw;
};

#define sl_TYPE S8
#include "slice.c"

#define S8(s) (S8){.len = LENGTH_OF(s), .raw = (U8 *)s}

FUNCTION S8
s8Alloc(ISize len, m_Allocator *perm)
{
	S8 res = {0};
	res.len = len;
	res.raw = m_MAKE(U8, len + 1, perm);
	res.raw[len] = 0;
	return res;
}

FUNCTION S8
s8Concat(S8 s1, S8 s2, m_Allocator *perm)
{
	ISize len = s1.len + s2.len;
	S8 s = s8Alloc(len, perm);
	m_memoryCopy(s.raw, s1.raw, s1.len);
	m_memoryCopy(&s.raw[s1.len], s2.raw, s2.len);
	return s;
}

FUNCTION S8
s8Sub(S8 s, ISize start, ISize end, m_Allocator *perm)
{
	S8 res = {0};
	if (end <= s.len && start < end) {
		res = s8Alloc(end - start, perm);
		m_memoryCopy(res.raw, &s.raw[start], res.len);
	}
	return res;
}

FUNCTION B8
s8Contains(S8 haystack, S8 needle)
{
	B8 found = FALSE;
	for (ISize i = 0, j = 0; i < haystack.len && !found; i++) {
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

FUNCTION ISize
s8IndexOf(S8 haystack, S8 needle)
{
	for (ISize i = 0; i < haystack.len; i += 1) {
		ISize j = 0;
		ISize start = i;
		while (haystack.raw[i] == needle.raw[j]) {
			j += 1;
			i += 1;
			if (j == needle.len) {
				return start;
			}
		}
	}
	return (ISize)-1;
}

FUNCTION S8
s8SubView(S8 haystack, S8 needle)
{
	S8 r = {0};
	ISize start_index = str_index_of(haystack, needle);
	if (start_index < haystack.len) {
		r.raw = &haystack.raw[start_index];
		r.len = needle.len;
	}
	return r;
}

FUNCTION B8
s8Equal(S8 a, S8 b)
{
	if (a.len != b.len) {
		return FALSE;
	}
	return m_memoryDifference(a.raw, b.raw, a.len) == 0;
}

FUNCTION S8
s8View(S8 s, ISize start, ISize end)
{
	if (end < start || end - start > s.len) {
		return (S8){0};
	}
	return (S8){end - start, s.raw + start};
}

FUNCTION S8
s8Clone(S8 s, m_Allocator *perm)
{
	S8 r = {0};
	if (s.len) {
		r.raw = perm->alloc(perm->ctx, NIL, 0, s.len);
		r.len = s.len;
		m_memoryCopy(r.raw, s.raw, s.len);
	}
	return r;
}

FUNCTION sl_S8
s8Split(S8 s, S8 delimiter, m_Allocator *perm)
{
	sl_S8 arr = sl_S8_create(NIL, 0);
	ISize start = 0;
	for (ISize i = 0; i < s.len; i += 1) {
		if (s.raw[i] != delimiter.raw[0]) {
			continue;
		}

		if (m_memoryDifference(&s.raw[i], delimiter.raw, delimiter.len) == 0) {
			// Clone the substring before the delimiter.
			ISize end = i;
			S8 cloned = s8Sub(s, start, end, perm);
			*sl_S8_push(&arr, perm) = cloned;
			start = end + delimiter.len;
		}
	}
	// Get the last segment.
	if (start + delimiter.len < s.len) {
		S8 cloned = s8Sub(s, start, s.len, perm);
		*sl_S8_push(&arr, perm) = cloned;
	}
	return arr;
}

FUNCTION sl_S8
s8SplitView(S8 s, S8 delimiter, m_Allocator *perm) {
	sl_S8 arr = sl_S8_create(NIL, 0);
	ISize start = 0;
	for (ISize i = 0; i < s.len; i += 1) {
		if (s.raw[i] != delimiter.raw[0]) {
			continue;
		}

		if (m_memoryDifference(&s.raw[i], delimiter.raw, delimiter.len) == 0) {
			ISize end = i;
			S8 view = s8View(s, start, end);
			*sl_S8_push(&arr, perm) = view;
			start = end + delimiter.len;
		}
	}
	if (start + delimiter.len < s.len) {
		S8 view = s8View(s, start, s.len);
		*sl_S8_push(&arr, perm) = view;
	}
	return arr;
}

FUNCTION S8
s8Join(sl_S8 s, S8 join, m_Allocator *perm) {
	ISize total_length = s.len * join.len;
	for (ISize i = 0; i < s.len; i += 1) {
		total_length += s.elems[i].len;
	}

	U8 *mem = m_MAKE(U8, total_length + 1, perm);
	ISize offset = 0;
	for (ISize i = 0; i < s.len; i += 1) {
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

#endif//BASTD_STRING_C