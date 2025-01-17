#ifndef BASTD_MEM_C
#define BASTD_MEM_C

FUNCTION void *
m_memorySet(void *buffer, U8 value, ISize length)
{
	U8* p = buffer;
	while (length-- > 0) {
		*p++ = value;
	}
	return buffer;
}

FUNCTION void *
m_memoryCopy(void *dst, void *src, size_t n) 
{ 
	U8 *s = (U8 *)src; 
	U8 *d = (U8 *)dst; 

	for (I32 i = 0; i < n; i++) {
		d[i] = s[i]; 
	}

	return dst;
}

/* Alloc function taken from Lua.
  If ptr == NIL, old_size irrelevant
  If ptr != NIL, old_size = size of block of memory at ptr
  If new_size > 0, function like realloc and either resize or create
  If new_size <= 0, function like free and clear the memory
*/
typedef void *(*m_AllocFunc)(void *ctx, void *ptr, ISize old_size, ISize new_size);

typedef struct m_Allocator m_Allocator; 
struct m_Allocator {
	m_AllocFunc alloc;
	void* ctx;
};

#define m_MAKE(T, n, a) ((T *)((a).alloc((a).ctx, NIL, 0, sizeof(T) * n)))
#define m_RESIZE(p, o, n, a) ((a).alloc((a).ctx, p, sizeof(*(p)) * o, sizeof(*(p)) * n))
#define m_RELEASE(p, s, a) ((a).alloc((a).ctx, p, sizeof(*(p)) * s, 0))

/* Linear Allocator (Arena)
   Allocates variably-sized regions from a fixed-size block of memory.
   A set of regions is freed by setting the allocator's offset to an earlier
   value.
*/
typedef struct m_Arena m_Arena;
struct m_Arena {
	U8* beg;
	U8* end;
};

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

FUNCTION void *
m_Arena_alloc(void *ctx, void *ptr, ISize old_size, ISize new_size)
{
	m_Arena *a = (m_Arena *)ctx;

	if (new_size <= 0) {
		/*  Arena can only free the most recent block. This allows to follow stack pattern.
			What we do is move the end pointer forward by the size of the block
			Then, because there is now more available space, the data will be 
			ignored following the next allocation.
		*/
		ISize padding = -old_size & (DEFAULT_ALIGNMENT - 1);
		a->end += old_size + padding;
		return NIL;
	} else {
		// Allocate a block
		ASSERT(new_size > old_size, "Can't reallocate to a smaller block");

		ISize padding = -(UPtr)a->beg & (DEFAULT_ALIGNMENT - 1);
		ISize available = a->end - a->beg - padding;

		if (available < 0 || new_size > available) {
			OS_Abort(S8("Out of Memory!"));
		}

		void *p = NIL;

		if (a->beg == (U8 *)ptr + old_size) {
			// This was the last allocated block, can extend allocation for realloc
			p = ptr;
			a->beg += padding + new_size;
		} else {
			// New allocation;
			p = a->beg + padding;
			a->beg += padding + new_size;
			p = m_memorySet(p, 0, new_size);
			
			if (ptr != NIL) {
				// Arbitrary block, copy data from old pointer
				p = m_memoryCopy(p, ptr, old_size);
			}
		}

		return p;
	}
}

FUNCTION m_Arena
m_Arena_create(void* buffer, ISize capacity)
{
	m_Arena arena = {0};
	arena.beg = (U8 *)buffer;
	arena.end = arena.beg ? arena.beg + capacity : 0;
	return arena;
}

#define m_ARENA_ALLOCATOR(a) (m_Allocator){m_Arena_alloc, (void *)&(a)}

#endif//BASTD_MEM_C