#define BASTD_CLI
#include "../../bastd.c"

/* The sl_TYPE macro defines the type of the generic. It is cleared after every
   include of "../slice.c"
*/
#define sl_TYPE I64
#include "../slice.c"


// SLICE types
FUNCTION void
fib_slice(Buffer *output, m_Allocator *perm)
{
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
		*sl_I64_push(&fib, perm) = a + b;
	}

	for (int i = 0; i < fib.len; i++) {
		Buffer_appendI64(output, fib.elems[i]);
		Buffer_appendU8(output, '\n');
	}
	Buffer_standardOutput(output);
}

/* These generic structures may have more than one required macro */
#define hm_KEY_TYPE S8
#define hm_KEY_HASHPROC S8_hash_prime19
#define hm_KEY_EQUALSPROC S8_equal
#define hm_VAL_TYPE U64
#include "../hashmap.c" // will define hm_S8_U64

// HASHMAPS
FUNCTION void
citypopulus_hashmap(Buffer *output, m_Allocator *perm)
{
	// We always store a hashmap pointer, NOT the actual hashmap
	hm_S8_U64 *database = hm_EMPTY;

	*hm_S8_U64_upsert(&database, S8("London"), perm) = 8800000;
	*hm_S8_U64_upsert(&database, S8("Lahore"), perm) = 13000000;
	*hm_S8_U64_upsert(&database, S8("Dhaka"), perm) = 21280000;
	*hm_S8_U64_upsert(&database, S8("Washington D.C."), perm) = 679000;
	*hm_S8_U64_upsert(&database, S8("Taxila"), perm) = 739000;

	// Can't iterate through hashmap, but in exchange, accessing values is fast
	// NOTE: passing no allocator implies that you should NOT insert a value if it doesnt exist
	U64 dc_population = *hm_S8_U64_upsert(&database, S8("Washington D.C."), NIL);
	U64 lahore_population = *hm_S8_U64_upsert(&database, S8("Lahore"), NIL);
	U64 london_population = *hm_S8_U64_upsert(&database, S8("London"), NIL);
	
	Buffer_appendI64(output, dc_population);
	Buffer_appendU8(output, '\n');
	Buffer_appendI64(output, lahore_population);
	Buffer_appendU8(output, '\n');
	Buffer_appendI64(output, london_population);
	Buffer_appendU8(output, '\n');

	Buffer_standardOutput(output);
}

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	Buffer output = BUFFER(m_MAKE(U8, KILO(2), &perm), KILO(2));

	fib_slice(&output, &perm);
	citypopulus_hashmap(&output, &perm);


	return os_ErrorCode_success;
}