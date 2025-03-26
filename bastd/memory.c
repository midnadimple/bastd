#ifndef BASTD_MEM_C
#define BASTD_MEM_C

FUNCTION void *
m_memorySet(void *buffer, U8 value, U64 length)
{
	U8* p = buffer;
	while (length-- > 0) {
		*p++ = value;
	}
	return buffer;
}

FUNCTION void *
m_memoryCopy(void *dst, void *src, U64 n) 
{ 
	U8 *s = (U8 *)src; 
	U8 *d = (U8 *)dst; 

	for (U64 i = 0; i < n; i++) {
		d[i] = s[i]; 
	}

	return dst;
}

FUNCTION I64
m_memoryDifference(void *s1, void *s2)
{
    U8 *p1 = (U8 *)s1;
    U8 *p2 = (U8 *)s2;

    while (*p1 && *p1 == *p2) ++p1, ++p2;

    return (*p1 > *p2) - (*p2  > *p1);
}

/* Alloc function taken from Lua.
  If ptr == NIL, old_size irrelevant
  If ptr != NIL, old_size = size of block of memory at ptr
  If new_size > 0, function like realloc and either resize or create
  If new_size <= 0, function like free and clear the memory
*/
typedef void *(*m_AllocFunc)(void *ctx, void *ptr, U64 old_size, U64 new_size);

typedef struct m_Allocator m_Allocator; 
struct m_Allocator {
	m_AllocFunc alloc;
	void* ctx;
};

#define m_MAKE(T, n, a) ((T *)((a)->alloc((a)->ctx, NIL, 0, sizeof(T) * n)))
#define m_RESIZE(p, o, n, a) ((a)->alloc((a)->ctx, p, sizeof(*(p)) * o, sizeof(*(p)) * n))
#define m_RELEASE(p, s, a) ((a)->alloc((a)->ctx, p, sizeof(*(p)) * s, 0))

/* Linear Allocator (Arena)
   Allocates variably-sized regions from a fixed-size block of memory.
   A set of regions is freed by setting the allocator's offset to an earlier
   value. This should be used as an allocator for tasks where the lifetime of
   the data isn't important to its implementation. This would be an update loop
   in a game, or a threaded web request, for example.
*/
typedef struct m_Arena m_Arena;
struct m_Arena {
	U8* beg;
	U8* end;
};

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

FUNCTION void *
m_Arena_alloc(void *ctx, void *ptr, U64 old_size, U64 new_size)
{
	m_Arena *a = (m_Arena *)ctx;

	if (new_size <= 0) {
		/*  Arena can only free the most recent block. This allows to follow stack pattern.
			All we do is move the beginning pointer back the size of the allocation.
		*/
		U64 padding = -old_size & (DEFAULT_ALIGNMENT - 1);
		a->beg -= (old_size + padding);
		return NIL;
	} else {
		// Allocate a block
		ASSERT(new_size > old_size, "Can't reallocate to a smaller block");

		U64 padding = (UPtr)a->beg & (DEFAULT_ALIGNMENT - 1);
		U64 available = a->end - a->beg - padding;

		if (available < 0 || new_size > available) {
			os_abort("Out of Memory!");
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
m_Arena_create(void* buffer, U64 capacity)
{
	m_Arena arena = {0};
	arena.beg = (U8 *)buffer;
	arena.end = arena.beg ? arena.beg + capacity : 0;
	return arena;
}

// Just to make it clear what you're saving
typedef U8 *m_ArenaOffset;

FUNCTION m_ArenaOffset
m_Arena_getOffset(m_Arena arena)
{
	return arena.beg;
}

FUNCTION void
m_Arena_setOffset(m_Arena *arena, m_ArenaOffset offset)
{
	arena->beg = offset;
}

#define m_ARENA_ALLOCATOR(a) (m_Allocator){m_Arena_alloc, (void *)&(a)}

/* Buddy Allocator (General-Purpose)
   Allocates any size data in any order, by splitting a fixed block of memory
   into half-size "buddy" blocks recursively until the allocation fits in the
   smallest possible block. This is a general purpose allocator, so you have to
   keep track of individual object lifetimes. THe best use-case for this is for
   objects that live for the entirety of a program, although static global
   variables / function parameters should be preferred.
*/
typedef struct m_BuddyBlock m_BuddyBlock;
struct m_BuddyBlock {
	U64 size;
	B32 is_free;
};

FUNCTION m_BuddyBlock *
m_BuddyBlock_next(m_BuddyBlock *block)
{
	return (m_BuddyBlock *)((char *)block + block->size);
}

FUNCTION m_BuddyBlock *
m_BuddyBlock_split(m_BuddyBlock *block, U64 size)
{
	if (block != NIL && size != 0) {
		while (size < block->size) {
			U64 sz = block->size >> 1;
			block->size = sz;
			block = m_BuddyBlock_next(block);
			block->size = sz;
			block->is_free = TRUE;
		}

		if (size <= block->size) {
			return block;
		}
	}

	return NIL;
}

FUNCTION m_BuddyBlock *
m_BuddyBlock_findBest(m_BuddyBlock *head, m_BuddyBlock *tail, U64 size)
{
	m_BuddyBlock *best_block = NIL;
	m_BuddyBlock *block = head;
	m_BuddyBlock *buddy = m_BuddyBlock_next(block);

	if (buddy == tail && block->is_free) {
		return m_BuddyBlock_split(block, size);
	}

	while (block < tail && buddy < tail) {
		// Coalesce to reduce fragmentation
		if (block->is_free && buddy->is_free && block->size == buddy->size) {
            block->size <<= 1;
            if (size <= block->size && (best_block == NIL || block->size <= best_block->size)) {
                best_block = block;
            }
            
            block = m_BuddyBlock_next(buddy);
            if (block < tail) {
                // Delay the buddy block for the next iteration
                buddy = m_BuddyBlock_next(block);
            }
            continue;
        }

		if (block->is_free && size <= block->size && 
            (best_block == NIL || block->size <= best_block->size)) {
            best_block = block;
        }
        
        if (buddy->is_free && size <= buddy->size &&
            (best_block == NIL || buddy->size < best_block->size)) { 
            // If each buddy are the same size, then it makes more sense 
            // to pick the buddy as it "bounces around" less
            best_block = buddy;
        }
        
        if (block->size <= buddy->size) {
            block = m_BuddyBlock_next(buddy);
            if (block < tail) {
                // Delay the buddy block for the next iteration
                buddy = m_BuddyBlock_next(block);
            }
        } else {
            // Buddy was split into smaller blocks
            block = buddy;
            buddy = m_BuddyBlock_next(buddy);
        }
	}
	
	if (best_block != NIL) {
		return m_BuddyBlock_split(best_block, size);
	}

	os_abort("Failed to buddy allocate, possibly out of memory");
	return NIL;
}

typedef struct m_Buddy m_Buddy;
struct m_Buddy {
	m_BuddyBlock *head;
	m_BuddyBlock *tail;
	U64 alignment;
};

FUNCTION m_Buddy
m_Buddy_create(void *data, U64 size)
{
	m_Buddy res = {0};
	res.alignment = DEFAULT_ALIGNMENT;
	if (res.alignment < sizeof(m_BuddyBlock)) {
		res.alignment = sizeof(m_BuddyBlock);
	}

	ASSERT(((UPtr)data & res.alignment) == 0, "Data not aligned to minimum alignment");

	res.head = (m_BuddyBlock *)data;
	res.head->size = size;
	res.head->is_free = TRUE;

	res.tail = m_BuddyBlock_next(res.head);

	return res;
}

FUNCTION U64
m_Buddy_requiredSize(m_Buddy *b, U64 size) {
    U64 actual_size = b->alignment;
    
    size += sizeof(m_BuddyBlock);
    U64 padding = -size & (b->alignment - 1); 
	size += b->alignment + padding;
    
    while (size > actual_size) {
        actual_size <<= 1;
    }
    
    return actual_size;
}

FUNCTION void
m_Buddy_coalesce(m_BuddyBlock *head, m_BuddyBlock *tail) {
    for (;;) { 
        // Keep looping until there are no more buddies to coalesce
        
        m_BuddyBlock *block = head;   
        m_BuddyBlock *buddy = m_BuddyBlock_next(block);   
        
        B32 no_coalescence = TRUE;
        while (block < tail && buddy < tail) { // make sure the buddies are within the range
            if (block->is_free && buddy->is_free && block->size == buddy->size) {
                // Coalesce buddies into one
                block->size <<= 1;
                block = m_BuddyBlock_next(block);
                if (block < tail) {
                    buddy = m_BuddyBlock_next(block);
                    no_coalescence = FALSE;
                }
            } else if (block->size < buddy->size) {
                // The buddy block is split into smaller blocks
                block = buddy;
                buddy = m_BuddyBlock_next(buddy);
            } else {
                block = m_BuddyBlock_next(buddy);
                if (block < tail) {
                    // Leave the buddy block for the next iteration
                    buddy = m_BuddyBlock_next(block);
                }
            }
        }
        
        if (no_coalescence) {
            return;
        }
    }
}

FUNCTION void *
m_Buddy_alloc(void *ctx, void *ptr, U64 old_size, U64 new_size) {
	m_Buddy *b = (m_Buddy *)ctx;

	if (new_size <= 0 && ptr != NIL) {
		m_BuddyBlock *block;
		
		// ASSERT((void *)b->head <= ptr, "Head is greater than pointer to free");
		// ASSERT(ptr < (void *)b->tail, "Tail is less than pointer to free");
		
		block = (m_BuddyBlock *)((char *)ptr - b->alignment);
		block->is_free = TRUE;
		
		m_Buddy_coalesce(b->head, b->tail);

		return NIL;
	} else {    
        size_t actual_size = m_Buddy_requiredSize(b, new_size);
        
        m_BuddyBlock *found = m_BuddyBlock_findBest(b->head, b->tail, actual_size);
        if (found == NIL) {
            // Try to coalesce all the free buddy blocks and then search again
            m_Buddy_coalesce(b->head, b->tail);
            found = m_BuddyBlock_findBest(b->head, b->tail, actual_size);
        }
            
        if (found != NIL) {
            found->is_free = FALSE;
			U8 *p = (U8 *)found + b->alignment;
			m_memorySet(p, 0, new_size);
			if (ptr != NIL) {
				m_memoryCopy(p, ptr, old_size);
			}
            return (void *)(p);
        }
        
		os_abort("Buddy Allcoator out of memory, possibly too much internal fragmentation");
    }
    
    return NIL;
}

#define m_BUDDY_ALLOCATOR(a) (m_Allocator){m_Buddy_alloc, (void *)&(a)}

#endif//BASTD_MEM_C