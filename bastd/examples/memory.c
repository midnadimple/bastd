#include "bastd.c"

int
main(void)
{
	ISize buffer_size = 100 * sizeof(U8);
	M_Arena arena = M_Arena_create(OS_Alloc(buffer_size), buffer_size);
	M_Allocator permanent_allocator = M_ARENA_ALLOCATOR(arena);

	{
		/* By cloning an arena at the start of the scope, all changes to the
		   arena are now local to the scope, so all allocations with be freed
		   at the end automatically.
		*/
		M_Arena scratch_arena = arena;
		M_Allocator scratch_allocator = M_ARENA_ALLOCATOR(scratch_arena);

		U8* array = M_MAKE(U8, 20, scratch_allocator);
		for (int i = 0; i < 20; i++) {
			array[i] = i;
		}
	} // Auto clears here

	OS_DEBUGBREAK();

	U8* array = M_MAKE(U8, 10, permanent_allocator);
	for (int i = 10; i-- > 0;) {
		array[10 - i] = i;
	}
	OS_DEBUGBREAK();

	array = M_RESIZE(array, 10, 20, permanent_allocator);
	for (int i = 20; i-- > 0;) {
		array[20 - i] = i;
	}
	OS_DEBUGBREAK();

	M_RELEASE(array, 20, permanent_allocator);
	OS_DEBUGBREAK();
}