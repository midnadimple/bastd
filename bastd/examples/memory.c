#define BASTD_CLI
#include "..\..\bastd.c"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	U32 buffer_size = KILO(2);

	/* Arena allocator allocates new values at an incrementing offset, and can
	   only free by resetting the beginning pointer to a previous offset
	*/
	m_Arena arena = m_Arena_create(os_alloc(buffer_size), buffer_size);
	m_Allocator arena_allocator = m_ARENA_ALLOCATOR(arena);

	{
		/* By cloning an arena at the start of the scope, all changes to the
		   arena are now local to the scope, so all allocations with be freed
		   at the end automatically.
		*/
		m_Arena scratch_arena = arena;
		m_Allocator scratch_allocator = m_ARENA_ALLOCATOR(scratch_arena);

		U8* array = m_MAKE(U8, 20, &scratch_allocator);
		for (int i = 0; i < 20; i++) {
			array[i] = i;
		}
	} // Auto clears here
	// The same can be accomplished by saving the arena's offset, then restoring it
	m_ArenaOffset savepoint = m_Arena_getOffset(arena);

	U8* temp_array = m_MAKE(U8, 20, &arena_allocator);
	for (int i = 0; i < 20; i++) {
		temp_array[i] = i;
	}

	m_Arena_setOffset(&arena, savepoint); // Clears here

	U8* array = m_MAKE(U8, 10, &arena_allocator);
	for (int i = 10; i-- > 0;) {
		array[10 - i] = i;
	}

	// Arenas allow for the resizing of blocks.
	array = m_RESIZE(array, 10, 20, &arena_allocator);
	for (int i = 20; i-- > 0;) {
		array[20 - i] = i;
	}


	/* For an arena allocator, you can deallocate the last block allcoated on
	   the arena.
	*/
	m_RELEASE(array, 20, &arena_allocator);

	/* Buddy allocator is more general-purpose than arena allocator, but is
	   slower and should preferrably be used for long-lasting allocations, 
	   rather than temporary ones.
	*/
	m_Buddy buddy = m_Buddy_create(os_alloc(buffer_size), buffer_size);
	m_Allocator buddy_allocator = m_BUDDY_ALLOCATOR(buddy);

	// You can allocate and deallocate any sized object in any order
	U8 *file = m_MAKE(U8, 20, &buddy_allocator);
	F32 *vec2 = m_MAKE(F32, 2, &buddy_allocator);
	char *raw_str = m_MAKE(char, 50, &buddy_allocator);
	
	m_RELEASE(vec2, 2, &buddy_allocator);
	m_RELEASE(raw_str, 50, &buddy_allocator);
	m_RELEASE(file, 20, &buddy_allocator);

	return os_ErrorCode_success;
}