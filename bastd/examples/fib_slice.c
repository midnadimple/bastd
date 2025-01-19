#include "../../bastd.c"

/* The sl_TYPE macro defines the type of the generic. It is cleared after every
   include of "../slice.c"
*/
#define sl_TYPE I32
#include "../slice.c"

int
main(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(2 * GIGA), 2 * GIGA);
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	// Slices can be created with a backing array to start with.
	I32 init[] = {0, 1};
	sl_I32 fib = sl_I32_create(init, 2);

	os_DEBUGBREAK();

	for (;;) {
		// The data in a slice can be accessed like a normal array
		I32 a = fib.elems[fib.len - 2];
		I32 b = fib.elems[fib.len - 1];
		if (a + b > 255) {
			break;
		}

		/* The sl_TYPE_push function automatically doubles the capacity of the
		   array if the slice is full and returns a pointer to a new element
		*/
		*sl_I32_push(&fib, &perm) = a + b;
	}

	os_DEBUGBREAK();

	sl_S8 msgs = sl_S8_create(NIL, 0);
	*sl_S8_push(&msgs, &perm) = S8("C standard library!");
	*sl_S8_push(&msgs, &perm) = S8("a Bravely Arranged ");
	*sl_S8_push(&msgs, &perm) = S8("This is ");
	*sl_S8_push(&msgs, &perm) = S8("World! ");
	*sl_S8_push(&msgs, &perm) = S8("Hello, ");
	
	os_DEBUGBREAK();

	for (; msgs.len > 0;) {
		S8 check_debugger = sl_S8_pop(&msgs);
		os_DEBUGBREAK();
	}

	return 0;
}