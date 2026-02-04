#ifndef BASTD_H
#define BASTD_H

/* Better type names */
#include <stdint.h>

typedef uint8_t   U8;
typedef uint16_t  C16;
typedef int32_t   B32;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef float     F32;
typedef double    F64;
typedef uintptr_t UPtr;
typedef char      Byte;
typedef ptrdiff_t ISize;
typedef size_t    USize;

#define TRUE 1
#define FALSE 0

/* Useful macros */
#define isizeOf(o)  (ISize)(sizeof(o))
#define countOf(a)  (isizeOf(a) / isizeOf(*(a)))
#define lengthOf(s) (countOf(s) - 1)

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#if defined(_MSC_VER)
#define debugBreak() __debugbreak()
#define isDebuggerPresent() IsDebuggerPresent()
#elif defined(__GNUC__) || defined(__clang__)
#define debugBreak() __builtin_unreachable()
/* checking this is more annoying than it should be so im leaving it for now */
#define isDebuggerPresent()
#else
#error "debugBreak undefined for current compiler"
#endif

#define assert(c)  while(!(c)) debugBreak()

#if defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL __thread
#else
#error "THREAD_LOCAL undefined for current compiler"
#endif

#if defined(_MSC_VER)
#pragma section(".roglob", read)
#define READ_ONLY __declspec(allocate(".roglob"))
#elif defined(__GNUC__) || defined(__clang__)
/* TODO: I'm 90% sure this doesnt work as intended */
#define READ_ONLY volatile const
#else
#error "READ_ONLY undefined for current compiler"
#endif

#ifndef DEBUG_BUILD
#define DEBUG_BUILD FALSE
#endif

/* NULL pointer */
#define NIL 0

/* Symbolic constants */
#define KiB(n) ((ISize)(n) << 10)
#define MiB(n) ((ISize)(n) << 20)
#define GiB(n) ((ISize)(n) << 30)

/* MEMORY MANAGEMENT
 * =====================================================================
 */


/* Copy memory from one place to another */
void *mem_copy(void *dst, void *src, ISize n);
/* Compare the first n bytes of the dst memory block to the first n bytes of
 * the src memory block. Returns 0 if they are all equal or the number of
 * different bytes
 */
ISize mem_compare(void *a, void *b, ISize n);
/* Move memory from one place to another */
void *mem_move(void *dst, void *src, ISize n);
/* Set memory to given value */
void *mem_set(void *buf, int val, ISize size);

/* Arena allocator
 *
 * Linearly allocates memory within a fixed buffer using an offset.
 * Typically this buffer is either an array on the stack or an OS allocated
 * heap pointer.
 *
 * Individual allocations cannot be freed, only a whole block of memory at once.
 *
 * NOTE: Unlike the majority of data structures, zero-initialization is not supported,
 * you must define an arena with a pre-defined size.
 */
typedef struct mem_Arena mem_Arena;
struct mem_Arena {
	ISize reserve_size;
	ISize commit_size;

	ISize pos;
	ISize commit_pos;
};

/* Create an arena using OS functions */
mem_Arena *mem_Arena_create(ISize reserve_size, ISize commit_size);

/* Destroy an arena using OS functions */
void mem_Arena_destroy(mem_Arena *a);

/* Allocate memory within an arena and zeroes out the memory given
 *
 * The returned value is a pointer to the allocated memory or NIL if the arena is
 * out of memory.
 */
void *mem_Arena_push(mem_Arena *a, ISize size);

/* a = arena
 * t = type you want to make
 * n = number of objects to allocate
 */
#define mem_make(a, T, n) (T *)mem_Arena_push((a), isizeOf(T) * (n))

/* Free a given number of bytes from the top of the arena.
 *
 * This makes the memory available for further allocations in the arena.
 */
void mem_Arena_pop(mem_Arena *a, ISize size);

/* Jump back to a certain position in the arena.
 */
void mem_Arena_popTo(mem_Arena *a, ISize pos);

/* Clear the whole arena */
void mem_Arena_clear(mem_Arena *a);

/* Temporary buffer for an arena */
typedef struct mem_ArenaTemp mem_ArenaTemp;
struct mem_ArenaTemp {
	mem_Arena *arena;
	ISize pos;
};

mem_ArenaTemp mem_ArenaTemp_begin(mem_Arena *a);
void mem_ArenaTemp_end(mem_ArenaTemp at);

/* DATA STRUCTURES
 * =====================================================================
 *
 * A notable exclusion from here is the hash map. A suitable hash map
 * implementation requires specified types and doesn't work with void pointers.
 *
 * See https://nullprogram.com/blog/2023/09/30/ for an implementation that's
 * very compatible with this base layer.
 */

/* Dynamic Arrays / Slice (https://nullprogram.com/blog/2023/10/05/) */

/* Macro abuse for defining a slice type */
#define mem_Slice_TYPE(T)   \
	struct {  \
		ISize len; \
		ISize cap; \
		T   *data; \
	}

#define mem_Slice_push(s, arena) \
    ((s)->len >= (s)->cap \
        ? mem_Slice_grow(s, sizeof(*(s)->data), arena), \
          (s)->data + (s)->len++ \
        : (s)->data + (s)->len++)

#define mem_Slice_remove(s, i) do { \
    	if (i == (s)->len) { \
    		(s)->len--; \
    	} else if ((s)->len > 1) { \
 			(s)->len--; \
 			mem_copy((s)->data + i, (s)->data + (s)->len, isizeOf(*(s))); \
    	} \
	} while (0);

#define mem_Slice_pop(s) ((s)->data[--(s)->len - 1])

void mem_Slice_grow(void *slice, ISize size, mem_Arena *a);

/* LENGTH-BASED STRINGS
 * =====================================================================
 *
 * Using null-terminated strings makes almost all string-based operations more
 * annoying. Encoding the length in the string allows for easier manipulation.
 */
#define S8(s) (S8){lengthOf(s), (U8 *)s}
typedef struct S8 S8;
struct S8 {
    ISize len; /* Length of the string (excluding the null terminator) */
    U8  *data; /* Raw null-terminated string */
};

/* Allocate a new string within the given arena. It will be initialized to 0 and
 * the null terminator is excluded from its length
 */
S8 S8_alloc(ISize len, mem_Arena *a);
/* Allocate a new copy of the src string within the given arena. */
S8 S8_clone(S8 src, mem_Arena *a);
/* Allocate a new string that contains the given range of the given string */
S8 S8_sub(S8 s, ISize start, ISize end, mem_Arena *a);
/* Allocate a new string that is the concatenation of the given strings within
 * the given arena.
 */
S8 S8_concat(S8 s1, S8 s2, mem_Arena *a);
/* Checks if a string is equal to another */
B32 S8_eq(S8 a, S8 b);
/* Checks if a string is empty */
B32 S8_isNil(S8 k);
/* Allocate a new string where all instances of the needle string in the haystack string
 * are replaced with the replacement string
 */
S8 S8_replace(S8 haystack, S8 needle, S8 replacement, mem_Arena *a);
/* Count the instances of the needle string in the haystack string */
ISize S8_count(S8 haystack, S8 needle);
/* Find the offset of the first instance of the needle string in the haystack
 * string after the given offset. If offset is less than 0, the whole string is
 * searched. Returns -1 if the needle isn't in the haystack.
 */
ISize S8_find(S8 haystack, S8 needle, ISize offset);
/* Parse a string into an I64 */
I64 S8_parseI64(S8 num_str);

typedef mem_Slice_TYPE(S8) S8Slice;

/* Split an S8 into an S8Slice at the delimeter.
 *
 * The S8Slice is allocated in the given arena.
 */
S8Slice S8_split(S8 s, S8 delim, mem_Arena *a);
/* Join an S8Slice into an S8, appending a join string in between
 *
 * The S8 is allocated in the given arena;
 */
S8 S8_join(S8Slice s, S8 join, mem_Arena *a);

/* Returns an unsigned 32-bit hash of the given string */
U32 S8_hash32(S8 s);
/* Returns an unsigned 64-bit hash of the given string */
U64 S8_hash64(S8 s);

/* BITSET */

typedef struct BitSet BitSet;
struct BitSet {
	/* we're using the native int size for the bitarray */
	unsigned int *ba;
};
BitSet BitSet_alloc(mem_Arena *a, ISize len);
void BitSet_setIdx(BitSet *ba, ISize idx);

/* OS functions */

typedef struct os_File os_File;
struct os_File {
	S8 name;
	void *raw; /* depends on the os */
	ISize size;
	B32 already_exists;
};

typedef enum os_FileOpenMode os_FileOpenMode;
enum os_FileOpenMode {
	os_FileOpenExisting = 0,
	os_FileCreate,
	os_FileCreateIfNotExists,
};
os_File os_File_open(S8 filename, os_FileOpenMode mode);
B32 os_File_delete(os_File file);
void os_File_close(os_File file);
/* reads U8 bytes between start_offset and end_offset from given file
 *
 * if start_offset < 0, will read from the start of the file
 * if end_offset < 0, will read until the end of the file
 */
S8 os_File_read(os_File file, mem_Arena *arena, ISize start_offset, ISize end_offset);
/* if offset < 0, will write to end of file */
B32 os_File_write(os_File file, S8 str, ISize offset);

ISize os_getPageSize(void);
void *os_memReserve(ISize size);
B32 os_memCommit(void *p, ISize size);
B32 os_memDecommit(void *p, ISize size);
B32 os_memRelease(void *p, ISize size);
void os_exit(U32 err_code);

/* Buffered output
 *
 * Me no like printf, so me make own printf
 */
typedef struct Buffer Buffer;
struct Buffer {
	U8 *buf;
	ISize cap;
	ISize len;
	B32 error;
	os_File file;
};
#define Buffer(buf, cap) (Buffer){buf, cap, 0, FALSE, {0}}
#define Buffer_FILE(buf, cap, file) (Buffer){buf, cap, 0, FALSE, file}

void Buffer_append(Buffer *b, U8 *src, ISize len);
void Buffer_appendS8(Buffer *b, S8 s);
void Buffer_appendU8(Buffer *b, U8 c);
void Buffer_appendI64(Buffer *b, I64 x);
void Buffer_appendPtr(Buffer *b, void *p);
void Buffer_appendF64(Buffer *b, F64 x);
void Buffer_flush(Buffer *b);
S8 Buffer_buildS8(Buffer *b, mem_Arena *a);

/* Append a formatted string to stdout, stderr and a given buffer respectively,
 * and flush the buffer.
 *
 * These functions do not implement the full format specification
 *
 * Supported conversions:
 * - %%
 * - %c
 * - %s (with the S8 type)
 * - %d
 * - %f
 * - %p
 *
 * Modifiers and flags are NOT supported
 */
void print(S8 fmt, ...);
void printError(S8 fmt, ...);
void printBuffer(Buffer *b, S8 fmt, ...);

/* print to stderr then abort */
void printAndAbort(S8 fmt, ...);

typedef enum tctx_MsgKind tctx_MsgKind;
enum tctx_MsgKind {
	tctx_MsgInfo,
	tctx_MsgWarn,
	tctx_MsgError,
	tctx_MsgDebug,
	tctx_MsgKind_LEN,
};

typedef struct tctx_Msg tctx_Msg;
struct tctx_Msg {
	tctx_MsgKind kind;
	S8 msg;
	/* todo thread name */
	ISize line;
	S8 filename;
};

#define tctx_SCRATCH_ARENA_COUNT 2

typedef struct ThreadContext ThreadContext;
struct ThreadContext {
	Buffer console_out;
	Buffer console_error;
	mem_Arena *scratch_arenas[tctx_SCRATCH_ARENA_COUNT];
	/* TODO stdin */

	mem_Arena *log_arena;
	/* TODO custom category names */
	mem_Slice_TYPE(tctx_Msg) log_msg_list;
};
extern THREAD_LOCAL ThreadContext tctx;

/* Get a scratch arena that doesn't conflict with any given arenas */
mem_ArenaTemp tctx_getScratchArena(mem_Arena **conflicts, ISize num_conflicts);

#define tctx_logAppend(kind, msg, ...) tctx_logAppendFileLine(__LINE__, S8(__FILE__), (kind), (msg), __VA_ARGS__)
void tctx_logAppendFileLine(ISize line, S8 filename, tctx_MsgKind kind, S8 msg, ...);
void tctx_logOutputToConsole(void);

/* Entry Point */
int entry(S8Slice args);

#endif//BASTD_H