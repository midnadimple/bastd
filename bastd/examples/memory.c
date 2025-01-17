#include "..\..\bastd.c"

int
main(void)
{
	ISize buffer_size = 100 * sizeof(U8);
	m_Arena arena = m_Arena_create(OS_Alloc(buffer_size), buffer_size);
	m_Allocator permanent_allocator = m_ARENA_ALLOCATOR(arena);

	{
		/* By cloning an arena at the start of the scope, all changes to the
		   arena are now local to the scope, so all allocations with be freed
		   at the end automatically.
		*/
		m_Arena scratch_arena = arena;
		m_Allocator scratch_allocator = m_ARENA_ALLOCATOR(scratch_arena);

		U8* array = m_MAKE(U8, 20, scratch_allocator);
		for (int i = 0; i < 20; i++) {
			array[i] = i;
		}
	} // Auto clears here

	os_DEBUGBREAK();

	U8* array = m_MAKE(U8, 10, permanent_allocator);
	for (int i = 10; i-- > 0;) {
		array[10 - i] = i;
	}
	os_DEBUGBREAK();

	array = m_RESIZE(array, 10, 20, permanent_allocator);
	for (int i = 20; i-- > 0;) {
		array[20 - i] = i;
	}
	os_DEBUGBREAK();

	m_RELEASE(array, 20, permanent_allocator);
	F64 *vector2 = m_MAKE(F32, 2, permanent_allocator);
	vector2[0] = 0x1.921fb6p+1f;
	vector2[1] = 490875;
	os_DEBUGBREAK();
}