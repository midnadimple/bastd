// GENERIC FILE, REDEFINED EVERY INCLUDE
#ifndef hm_KEY_TYPE
#error "No hm_KEY_TYPE given"
#endif

// hm_KEY_HASHPROC need signature "U64 procname(hm_KEY_TYPE)"
#ifndef hm_KEY_HASHPROC
#error "No hm_KEY_HASHPROC given"
#endif

#ifndef hm_KEY_EQUALSPROC
#error "No hm_KEY_EQUALSPROC given"
#endif

#ifndef hm_VAL_TYPE
#error "No hm_VAL_TYPE given"
#endif

#define hm_NAME CONCAT(CONCAT(hm_, hm_KEY_TYPE), CONCAT(_, hm_VAL_TYPE))

typedef struct hm_NAME hm_NAME;
struct hm_NAME {
	hm_NAME *child[4];
	hm_KEY_TYPE key;
	hm_VAL_TYPE val;
};

// always store a pointer to the hashmap, not the actual value
#define hm_EMPTY NIL

FUNCTION hm_VAL_TYPE *
CONCAT(hm_NAME, _upsert) (hm_NAME **m, hm_KEY_TYPE key, m_Allocator *perm)
{
	for (U64 h = hm_KEY_HASHPROC(key); *m; h <<= 2) {
		if (hm_KEY_EQUALSPROC(key, (*m)->key)) {
			return &(*m)->val;
		}
		m = &(*m)->child[h>>62];
	}
	if (perm == NIL) {
		// just searching, dont insert a new value
		return NIL;
	}
	*m = m_MAKE(hm_NAME, 1, perm);
	(*m)->key = key;
	return &(*m)->val;
}

#undef hm_NAME
#undef hm_VAL_TYPE
#undef hm_KEY_EQUALSPROC
#undef hm_KEY_HASHPROC
#undef hm_KEY_TYPE