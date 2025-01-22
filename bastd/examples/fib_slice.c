#define BASTD_CLI
#include "../../bastd.c"

/* The sl_TYPE macro defines the type of the generic. It is cleared after every
   include of "../slice.c"
*/
#define sl_TYPE I64
#include "../slice.c"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	// Slices can be created with a backing array to start with.
	I64 init[] = {0, 1};
	sl_I64 fib = sl_I64_create(init, 2);

	for (;;) {
		// The data in a slice can be accessed like a normal array
		I64 a = fib.elems[fib.len - 2];
		I64 b = fib.elems[fib.len - 1];
		if (a + b > 255) {
			break;
		}

		/* The sl_TYPE_push function automatically doubles the capacity of the
		   array if the slice is full and returns a pointer to a new element
		*/
		*sl_I64_push(&fib, &perm) = a + b;
	}

	Buffer output = BUFFER(m_MAKE(U8, KILO(2), &perm), KILO(2));
	for (int i = 0; i < fib.len; i++) {
		Buffer_appendI64(&output, fib.elems[i]);
		Buffer_appendU8(&output, '\n');
	}
	Buffer_standardOutput(&output);

	// Wait for you to press enter
	Buffer_appendS8(&output, S8("I've got a message for you. Press Enter to see it: "));
	Buffer_standardOutput(&output);
	Buffer_appendStandardInput(&output, 1024, &perm);

	sl_S8 msgs = sl_S8_create(NIL, 0);
	*sl_S8_push(&msgs, &perm) = S8("C standard library!");
	*sl_S8_push(&msgs, &perm) = S8("a Bravely Arranged ");
	*sl_S8_push(&msgs, &perm) = S8("This is ");
	*sl_S8_push(&msgs, &perm) = S8("World! ");
	*sl_S8_push(&msgs, &perm) = S8("Hello, ");

	for (; msgs.len > 0;) {
		Buffer_appendS8(&output, sl_S8_pop(&msgs));
	}
	Buffer_standardOutput(&output);

	return os_ErrorCode_success;
}