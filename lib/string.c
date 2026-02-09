S8
S8_alloc(ISize len, mem_Arena *a)
{
	S8 s = {0};
	s.len = len;
	s.data = mem_make(a, U8, len + 1);
	return s;
}

S8
S8_clone(S8 src, mem_Arena *a)
{
	S8 dst = {0};
	if (!S8_isNil(src)) {
		dst.len = src.len;
		dst.data = mem_make(a, U8, src.len);
		mem_copy(dst.data, src.data, src.len);
	}
	return dst;
}

S8
S8_sub(S8 s, ISize start, ISize end, mem_Arena *a)
{
	S8 sub = {0};
	if (end <= s.len && start < end) {
		sub = S8_alloc(end - start, a);
		mem_copy(sub.data, &s.data[start], sub.len);
	}
	return sub;
}

S8
S8_concat(S8 s1, S8 s2, mem_Arena *a)
{
	ISize len = s1.len + s2.len;
	S8 s = S8_alloc(len, a);
	mem_copy(s.data, s1.data, s1.len);
	mem_copy(&s.data[s1.len], s2.data, s2.len);
	return s;
}

B32
S8_eq(S8 a, S8 b)
{
	if (a.len != b.len) {
		return FALSE;
	}
	return mem_compare(a.data, b.data, a.len) == 0;
}

B32
S8_isNil(S8 k)
{
	return k.len <= 0 || k.data == NIL;
}

S8
S8_replace(S8 haystack, S8 needle, S8 replacement, mem_Arena *a)
{
	ISize replaceable;
    ISize new_len;
    S8 res;
    B32 replaced;
    ISize o = 0;
    ISize i = 0;
    S8 sub;

    if (S8_isNil(needle)) {
        return haystack;
    }

    replaceable = S8_count(haystack, needle);
    if (replaceable == 0) {
        return haystack;
    }

    new_len = (haystack.len - replaceable * needle.len) + (replaceable * replacement.len);
    res = S8_alloc(new_len, a);

    while (i < haystack.len) {
        replaced = FALSE;
        if (haystack.data[i] == needle.data[0]) {
            if ((haystack.len - i) >= needle.len) {
                sub.data = haystack.data + i;
                sub.len = needle.len;      
                if (S8_eq(sub, needle)) {
                    mem_move(res.data + o, replacement.data, replacement.len);
                    replaced = TRUE;
                }
            }
        }

        if (replaced) {
            o += replacement.len;
            i += needle.len;
        } else {
            res.data[o] = haystack.data[i];
            o++;
            i++;
        }
    }

    return res;
}

ISize
S8_count(S8 haystack, S8 needle)
{
	ISize count = 0;
    ISize idx = 0;

    while (TRUE) {
        idx = S8_find(haystack, needle, idx);
        if (idx == -1)
            break;
        count++;
        idx++;
    }
    return count;
}

ISize
S8_find(S8 haystack, S8 needle, ISize offset)
{
	ISize i = -1;
    U8 c = needle.data[0];
    ISize one_past_last = haystack.len - needle.len + 1;
    S8 sub;

    if (needle.len > 0 && haystack.len >= needle.len) {
        i = offset;
        for (; i < one_past_last; i++) {
            if (haystack.data[i] == c) {
                if ((haystack.len - i) >= needle.len) {
                    sub.data = haystack.data + i;
                    sub.len = needle.len;
                    if (S8_eq(sub, needle)) {
                        break;
                    }
                }
            }
        } 
        if (i == one_past_last) {
            i = -1;
        }
    }

    return i;
}

S8Slice
S8_split(S8 s, S8 delim, mem_Arena *a)
{
	S8Slice slice = {0};
	ISize start = 0;
	S8 cloned;
	ISize i;

	for (i = 0; i < s.len; i++) {
		if (s.data[i] != delim.data[0]) {
			continue;
		}

		if (mem_compare(&s.data[i], delim.data, delim.len) == 0) {
			cloned = S8_sub(s, start, i, a);
			*mem_Slice_push(&slice, a) = cloned;
			start = i + delim.len;
		}
	}

	if (start + delim.len < s.len) {
		/* Last segment */
		cloned = S8_sub(s, start, s.len, a);
		*mem_Slice_push(&slice, a) = cloned;
	}

	return slice;
}

S8
S8_join(S8Slice s, S8 join, mem_Arena *a)
{
	ISize total_len;
	ISize i;
	U8 *mem;
	ISize offset = 0;
	S8 res = {0};

	total_len = (s.len - 1) * join.len;
	for (i = 0; i < s.len; i++) {
		total_len += s.data[i].len;
	}

	mem = mem_make(a, U8, total_len);
	for (i = 0; i < s.len; i++) {
		mem_copy(&mem[offset], s.data[i].data, s.data[i].len);
		offset += s.data[i].len;

		if (i == s.len - 1) {
			break;
		}

		mem_copy(&mem[offset], join.data, join.len);
		offset += join.len;
	}

	mem[total_len] = 0;

	res.len = total_len;
	res.data = mem;

	return res;
}

U32
S8_hash32(S8 s)
{
    U32 hash = 2166136261u;
    ISize i;
    for (i = 0; i < s.len; i++) {
        hash ^= s.data[i];
        hash *= 16777619;
    }
    return hash;
}

U64
S8_hash64(S8 s)
{
    U64 hash = 2166136261u;
    ISize i;
    for (i = 0; i < s.len; i++) {
        hash ^= s.data[i];
        hash *= 16777619;
    }
    return hash;
}

I64
S8_parseI64(S8 num_str)
{
	I64 result = 0;
	B32 is_negative = FALSE;
	U8 *c = num_str.data;

	if (S8_isNil(num_str)) {
		tctx_logAppend(tctx_MsgError, S8("Failed to parse NIL string"));
		return 0;
	}

	if (*c == '-') {
		is_negative = TRUE;
		c++;
	}

	while (c < num_str.data + num_str.len) {
		if (*c < '0' || *c > '9') {
			tctx_logAppend(tctx_MsgError, S8("Non-digit character '%c' found while parsing string `%s`"), *c, num_str);
			return 0;
		}
		result *= 10;
		result += *c - '0';
		c++;
	}

	if (is_negative) {
		result *= -1;
	}
	return result;
}