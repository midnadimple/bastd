/* Bravely Arranged Standard Library 
 * - Only depends upon C89
 * - Single-Header
 * - Well-Documented
 * - Memory Allocators
 * - Length-based Strings
 * - Dynamic Arrays
 * - File system access
 * - Buffered I/O
 * - Multi-threaded Job System
 * - Unified entry point
 * - Window Creation with Graphics Context
 * - Immediate-mode Rendering
 * - Custom HLSL shader support
 * - 2D Batch Rendering
 * - 3D Forward+ Rendering
 *
 * HOW TO USE:
 * 1. Copy this file into your project.
 * 2. Include this file in any source file where it's necessary.
 * 3. In ONE source file, `#define BASTD_IMPL`
 * 4. (OPTIONAL) `#define BASTD_NO_ASSERT` to disable asserts
 * 5. (OPTIONAL) `#define BASTD_NO_CONTEXT` to disable OS context detection
 *    NOTE: It will probably break most things
 *
 * OS Support Matrix:
 * - TODO Linux / Vulkan
 * - TODO Windows / DirectX 12
 * - TODO Web / WebGPU
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef BASTD_BASE_H
#define BASTD_BASE_H

/* CONTEXT CRACKING (you probably want to skip this part)
 * =====================================================================
 *
 * Credit goes to https://git.mr4th.com/mr4th-public/mr4th/src/branch/main/src/base/base_context.h
 * Lord knows I'm not writing this myself
 */
#if !defined(BASTD_NO_ASSERT)
# define BASTD_NO_ASSERT 0
#endif

// NOTE(allen): Untangle Compiler, OS & Architecture
#if defined(__clang__)
# define COMPILER_CLANG 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error missing OS detection
# endif

# if defined(__amd64__)
#  define ARCH_X64 1
// TODO(allen): verify this works on clang
# elif defined(__i386__)
#  define ARCH_X86 1
// TODO(allen): verify this works on clang
# elif defined(__arm__)
#  define ARCH_ARM 1
// TODO(allen): verify this works on clang
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# else
#  error missing ARCH detection
# endif

#elif defined(_MSC_VER)
# define COMPILER_CL 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error missing OS detection
# endif

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_I86)
#  define ARCH_X86 1
# elif defined(_M_ARM)
#  define ARCH_ARM 1
// TODO(allen): ARM64?
# else
#  error missing ARCH detection
# endif

#elif defined(__GNUC__)
# define COMPILER_GCC 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error missing OS detection
# endif

# if defined(__amd64__)
#  define ARCH_X64 1
# elif defined(__i386__)
#  define ARCH_X86 1
# elif defined(__arm__)
#  define ARCH_ARM 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# else
#  error missing ARCH detection
# endif

#else
# error no context cracking for this environment, define BASTD_NO_CONTEXT to ignore this
#endif

#if !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif


// NOTE(allen): Language
#if defined(__cplusplus)
# define LANG_CXX 1
#else
# define LANG_C 1
#endif

#if !defined(LANG_CXX)
# define LANG_CXX 0
#endif
#if !defined(LANG_C)
# define LANG_C 0
#endif


// NOTE(allen): Determine Intrinsics Mode
#if OS_WINDOWS
# if COMPILER_CL || COMPILER_CLANG
#  define INTRINSICS_MICROSOFT 1
# endif
#endif

#if !defined(INTRINSICS_MICROSOFT)
# define INTRINSICS_MICROSOFT 0
#endif


// NOTE(allen): Setup Pointer Size Macro
#if ARCH_X64 || ARCH_ARM64
# define ARCH_ADDRSIZE 64
#else
# define ARCH_ADDRSIZE 32
#endif

/* THE BASICS
 * =====================================================================
 */

/* Better Type Names */
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char I8;
typedef signed short I16;
typedef signed int I32;
typedef signed long long I64;
typedef float F32;
typedef double F64;

typedef void VoidFunc(void);

/* Size Types.
 * Prefer ISize where you can.
 * See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1428r0.pdf
 */
#if ARCH_ADDRSIZE == 64
typedef I64 ISize;
typedef U64 USize;
typedef U64 UPtr;
typedef I64 IPtr;
#else
typedef I32 ISize;
typedef U32 USize;
typedef U32 UPtr;
typedef I32 IPtr;
#endif

/* Boolean */
typedef I8 B8;
#define TRUE 1
#define FALSE 0

/* Invalid pointer */
#define NIL 0

/* Length Macros */
#define ISIZE_OF(x)  (ISize)(sizeof(x))
#define USIZE_OF(x)  (USize)(sizeof(x))
#define COUNT_OF(a)  (ISIZE_OF(a) / ISIZE_OF(*(a)))
#define LENGTH_OF(s) (COUNT_OF(s) - 1)

#define STMNT(S) do{ S }while(0)

/* Assertions */
#ifdef BASTD_NO_ASSERT

#define ASSERT(c)

#else

#define ASSERT_BREAK() (*(volatile int *)0 = 0)
#define ASSERT(c) STMNT( if (!(c)) ASSERT_BREAK(); )

#endif

/* Minmax */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(a,x,b) (MAX(MIN(x,a), b))

/* Clarifying what "static" means in each context
 *
 * - GLOBAL_VAR defines a global variable
 * - PERSIST_VAR defines a variable within a function that persists its state
 *   across multiple function calls
 * - INTERNAL_FUNC defines a private function that shouldn't be used by external
 *   programs*/
#define GLOBAL_VAR static 
#define PERSIST_VAR static 
#define INTERNAL_FUNC static

/* Byte sizes */
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)
#define TB(x) ((U64)(x) << 40llu)

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

/* Allocator interface
 *
 * The memory management strategy of a program should always be the
 * responsibility of the function caller, not the library's internal
 * decision. So, whenever any function (at least in bastd) will allocate
 * memory, it will ask for this structure instead of assuming a specific
 * allocator.
 *
 * In addition, using this struct also makes it clear that a function is
 * going to allocate memory, so there are no implicit allocations.
 *
 * The 'ctx' struct is always passed to the 'alloc' and 'free' functions,
 * which should do what their name implies. The 'size' in the 'free'
 * function is the size of the existing allocation.
 */
typedef struct mem_Allocator mem_Allocator;
struct mem_Allocator {
    void *(*alloc)(ISize size, void *ctx);
    void (*free)(ISize size, void *ptr, void *ctx);
    void *ctx;
};

/* Nice macro wrappers for allocating and freeing memory. */
#define mem_MAKE(T, n, a)  ((T *)((a).alloc(ISIZE_OF(T) * n, (a).ctx)))
#define mem_RELEASE(s, p, a)  ((a).free(s, p, (a).ctx))

/* Arena allocator
 *
 * Linearly allocates memory within a fixed buffer using an offset.
 * Typically this buffer is either an array on the stack or an OS allocated
 * heap pointer.
 *
 * Individual allocations cannot be freed, only a whole block of memory at once OR
 * the newest allocation.
 *
 * Functions that use arenas can either ask for a pointer to it, meaning that
 * allocations will persist in the caller's scope, or they ask for a copy of it,
 * meaning that the arena is used a scratch arena and allocations will not persist
 * outside the function.
 */
typedef struct mem_Arena mem_Arena;
struct mem_Arena {
    void *base;
    ISize size;
    ISize offset;
    ISize committed;
};


/* Create an arena */
mem_Arena mem_Arena_create(void *buf, ISize cap);
/* Wrap the arena in an allocator */
#define mem_ARENA_ALLOCATOR(a) (mem_Allocator){mem_Arena_alloc, mem_Arena_free, a}
/* Allocate memory within an arena and zeroes out the memory given
 *
 * The returned value is a pointer to the allocated memory or NIL if the arena is
 * out of memory.
 */
void *mem_Arena_alloc(ISize size, void *ctx);
/* Does nothing (you can only free all memory at once) */
void mem_Arena_free(ISize size, void *ptr, void *ctx);
/* Frees all memory in the arena */
void mem_Arena_freeAll(void *ctx);

/* LENGTH-BASED STRINGS
 * =====================================================================
 *
 * Using null-terminated strings makes almost all string-based operations more
 * annoying. Encoding the length in the string allows for easier manipulation.
 */
#define S8(s) (S8){(U8 *)s, LENGTH_OF(s)}
typedef struct S8 S8;
struct S8 {
    U8 *data; /* Raw null-terminated string */
    ISize len; /* Length of the string (including the null terminator) */
};

/* Allocate a new string within the given arena. It will be initialized to 0 and
 * a null terminator is included in the length
 */
S8 S8_alloc(ISize len, mem_Allocator a);
/* Allocate a new copy of the src string within the given arena. */
S8 S8_copy(S8 src, mem_Allocator a);
/* Allocate a new string that is the concatenation of the given strings within
 * the given arena.
 */
S8 S8_cat(S8 a, S8 b, mem_Allocator a);
/* Checks if a string is equal to another */
B8 S8_eq(S8 a, S8 b);
/* Checks if a string is NIL */
B8 S8_isNil(S8 k);
/* Allocate a new string where all instances of the needle string in the haystack string
 * are replaced with the replacement string
 */
S8 S8_replace(S8 haystack, S8 needle, S8 repalcement, mem_Allocator a);
/* Count the instances of the needle string in the haystack string */
ISize S8_count(S8 haystack, S8 needle);
/* Find the offset of the first instance of the needle string in the haystack
 * string after the given offset. If offset is less than 0, the whole string is
 * searched. Returns -1 if the needle isn't in the haystack.
 */
ISize S8_find(S8 haystack, S8 needle, ISize offset);
/* Returns an unsigned 32-bit hash of the given string */
U32 S8_hash32(S8 s);
/* Returns an unsigned 64-bit hash of the given string */
U64 S8_hash64(S8 s);

/* Strings are assumed to be immutable. Each function that uses them does not
 * directly mutate the struct. 
 *
 * Instead, you can use a S8List to construct a new string from a linked list
 * of S8Nodes
 */
typedef struct S8Node S8Node;
struct S8Node {
    S8Node *next;
    S8 str;
};

typedef struct S8List S8List;
struct S8List {
    S8Node *first;
    S8Node *last;
    ISize node_count;
    ISize total_size;
};

/* Push an S8 to an S8List. An S8Node is allocated with the given arena */
void S8List_pushS8(S8List *list, S8 str, mem_Allocator a);
/* Push an S8Node to an S8List */
void S8List_pushS8Node(S8List *list, S8Node node);
/* Check if every S8Node in the given S8Lists is every */
B8 S8List_eq(S8List *a, S8List *b);
/* Check if the needle S8 is in the haystack S8List */
B8 S8List_contains(S8List *haystack, S8 needle);
/* Flatten the given S8List into a S8 allocated into the given arena */
S8 S8List_flatten(S8List *list, mem_Allocator a);

/* Windows uses UTF-16 for some depraved reason. So this is here for completeness */
#define S16(s) (S16){u##s, LENGTH_OF(u##s)}
typedef struct S16 S16;
struct S16 {
    U16 *data;
    ISize len;
};
/* TODO: Unicode conversions */

/* DATA STRUCTURES
 * =====================================================================
 *
 * A notable exclusion from here is the hash map. A suitable hash map
 * implementation requires specified types and doesn't work with void pointers.
 *
 * See https://nullprogram.com/blog/2023/09/30/ for an implementation that's
 * very compatible with this base layer.
 */

/* Dynamic array implementation taken from
 * https://www.bytesbeneath.com/p/dynamic-arrays-in-c */

/* Header for dynamic arrays (henceforth called slices).
 *
 * This is allocated at the beginning of each slice and contains key
 * information
 */
typedef struct Slice_Header Slice_Header;
struct Slice_Header {
    ISize len;
    ISize cap;
    ISize __padding; /* Added so that the header is 32 bytes */
    mem_Allocator *allocator;
};

/* Convenience macros for accessing header elements */
#define Slice_HEADER(s) ((Slice_Header *)(s) - 1)
#define Slice_LEN(s) (SLICE_HEADER(s)->len)
#define Slice_CAP(s) (SLICE_HEADER(s)->cap)

/* Macro that initializes a slice, with an initial capacity of 16 */
#define Slice_INITIAL_CAPACITY 16
#define Slice(T, a) (T *)Slice_init(ISIZE_OF(T), Slice_INITIAL_CAPACITY, a)

/* Initializes a slice with a given size and proper header */
void *Slice_init(ISize item_size, ISize capacity, mem_Allocator *a);

/* Macro that appends to a slice, growing it if necessary */
#define Slice_APPEND(s, v) ( \
        (s) = Slice_ensureCap(s, 1, ISIZE_OF(v)), \
        (s)[Slice_header(s)->len] = (v), \
        &(s)[Slice_header(s)->len++])

/* Ensure the given slice has enough space for n items of item_size, resizing
 * the slice if there is not enough */
void *Slice_ensureCap(void *slice, ISize n, ISize item_size);

/* OS MISC.
 * =====================================================================
 */

void os_abort(S8 msg);
/* Allocate memory pages from the operating system.
 *
 * Returns NIL on failure, or the base pointer of the allocated page on
 * success.
 *
 * By default, the pages are read-write. Use os_protect to change that.
 *
 * TODO support mmap extra flags (i.e. shared memory) ?
 */
void *os_alloc(ISize size);
/* Free memory pages from the operating system.
 *
 * Returns TRUE on success, or FALSE on failure
 */
B8 os_free(ISize size, void *ptr);

/* Change protection strategy on memory pages */
typedef enum os_ProtectMode os_ProtectMode;
enum os_ProtectMode {
    os_ProtectMode_Read = 0,
    os_ProtectMode_ReadWrite,
    os_ProtectMode_Execute,
};
B8 os_protect(void *ptr, ISize size, os_ProtectMode mode);

/* Get OS error as a string */
S8 os_error(void);

/* TODO OS FILE SYSTEM
 * =====================================================================
 */

/* IMPLEMENTATION
 * =====================================================================
 */
#ifdef BASTD_IMPL

void *
mem_copy(void *dst, void *src, ISize n)
{
    U8 *csrc = (U8 *)src;
    U8 *cdst = (U8 *)dst;
    int i;

    for (i = 0; i < n; i++)
        cdst[i] = csrc[i];

    return dst;
}

ISize
mem_compare(void *a, void *b, ISize n)
{
    U8 *ca = a;
    U8 *cb = b;
    ISize count = 0;

    if (a == b)
        return 0;

    while (n > 0) {
        if (*ca != *cb) {
            count = (*ca > *cb) ? 1 : -1;
            break;
        }
        n--;
        ca++;
        cb++;
    }
    return count;
}

void *
mem_move(void *dst, void *src, ISize n)
{
    U8 *csrc = (U8 *)src;
    U8 *cdst = (U8 *)dst;

    if (cdst > csrc && cdst < csrc + n) {
        cdst += n;
        csrc += n;
        while (n--) {
            *cdst-- = *csrc--;
        }
    } else {
        while (n--) {
            *cdst++ = *csrc++;
        }
    }
    return dst;
}

void *
mem_set(void *buf, int val, ISize size)
{
    U8 *p = buf;
    
    while (size > 0) {
        *p = val;
        p++;
        size--;
    }
}

mem_Arena
mem_Arena_create(void *buf, ISize size)
{
    mem_Arena a = {0};
    a.base = buf;
    a.size = size;
    return a;
}

#define IS_POWER_OF_TWO(x) ((x != 0) && ((x & (x - 1)) == 0))

static IPtr
mem__alignForward(IPtr ptr, ISize alignment) {
    uintptr_t p, a, modulo;
    if (!IS_POWER_OF_TWO(alignment)) {
        return 0;
    }

    p = ptr;
    a = (IPtr)alignment;
    modulo = p & (a - 1);

    if (modulo) {
        p += a - modulo;
    }

    return p;
}

// TODO support different alignments?
void *
mem_Arena_alloc(ISize size, void *ctx)
{
    mem_Arena *a = ctx;
    IPtr curr_ptr = (IPtr)a->base + (IPtr)a->offset;
    IPtr offset = mem__alignForward(curr_ptr, ISIZE_OF(2 * sizeof(void *)));
    void *p;

    offset -= (IPtr)a->base;
    if (size <= 0 || offset + size > a->size) {
        return NIL;
    }

    a->committed += size;
    p = (U8 *)a->base + offset;
    a->offset = offset + size;

    return p;
}

void
mem_Arena_free(ISize size, void *ptr, void *ctx)
{
    (void)size; (void)ptr; (void)ctx;
}

void
mem_Arena_freeAll(void *ctx)
{
    mem_Arena *a = ctx;
    a->offset = 0;
    a->committed = 0;
}

S8
S8_alloc(ISize len, mem_Allocator a)
{
    S8 str = {0};
    str.data = mem_MAKE(U8, len + 1, a);
    str.data[size] = '\0';
    str.len = len;
    return str;
}

S8
S8_copy(S8 src, mem_Allocator a)
{
    S8 dst = {0};
    dst.data = mem_MAKE(U8, src.len + 1, a);
    dst.len = src.len;
    mem_copy(dst.data, src.data, src.len);
    dst.data[src.len] = '\0';
    return dst;
}

S8
S8_cat(S8 a, S8 b, mem_Allocator a)
{
    S8 res = {0};
    res.len = a.len + b.len;
    res.data = mem_MAKE(U8, res.len + 1, a);
    mem_copy(res.data, a.data, a.len);
    mem_copy(res.data + a.len, b.data, b.len);
    res.data[res.len] = '\0';
    return res;
}

B8
S8_eq(S8 a, S8 b)
{
    if (a.len != b.len)
        return FALSE;
    return mem_compare(a.data, b.data, b.len) == 0;
}

B8
S8_isNil(S8 k)
{
    return k.data == NIL && k.len == NIL;
}

S8
S8_replace(S8 haystack, S8 needle, S8 repalcement, mem_Allocator a)
{
    ISize replaceable;
    ISize new_len;
    S8 res;
    B8 replaced;
    ISize o = 0;
    ISize i = 0;
    S8 sub;

    if (needle.len == 0)
        return haystack;

    replaceable = S8_count(haystack, needle);
    if (replaceable == 0)
        return haystack;

    new_len = (haystack.len - replaceable * needle.len) + (replaceable * replacement.len);
    res = S8_alloc(new_len, a);

    while (i < haystack.len) {
        replaced = FALSE;
        if (haystack.data[i] == needle.data[0]) {
            if ((haystack.len - i) >= needle.len) {
                sub.data = haystack.data + i;
                sub.len = needle.len;      
                if (S8_eq(sub, needle)) {
                    mem_move(res.data + o, replacement.data, replacement.len);
                    replaced = TRUE;
                }
            }
        }

        if (replaced) {
            o += replacement.len;
            i += needle.len;
        } else {
            res.data[o] = haystack.data[i];
            o++;
            i++;
        }
    }

    return res;
}

ISize
S8_count(S8 haystack, S8 needle)
{
    ISize count = 0;
    ISize idx = 0;

    while (TRUE) {
        idx = S8_find(haystack, needle, idx);
        if (idx == -1)
            break;
        count++;
        idx++;
    }
    return count;
}

ISize
S8_find(S8 haystack, S8 needle, ISize offset)
{
    ISize i = -1;
    U8 c = needle.data[0];
    ISize one_past_last = haystack.len - needle.len + 1;
    S8 sub;

    if (needle.len > 0 && haystack.len >= needle.len) {
        i = offset;
        for (; i < one_past_last; i++) {
            if (haystack.data[i] == c) {
                if ((haystack.len - i) >= needle.len) {
                    sub.data = haystack.data + i;
                    sub.len = needle.len;
                    if (S8_eq(sub, needle))
                        break;
                }
            }
        } 
        if (i == one_past_last)
            i = -1;
    }

    return i;
}

U32
S8_hash32(S8 s)
{
    U32 hash = 2166136261u;
    ISize i;
    for (i = 0; i < s.len; i++) {
        hash ^= s.data[i];
        hash *= 16777619;
    }
    return hash;
}

U64
S8_hash64(S8 s)
{
    U64 hash = 2166136261u;
    ISize i;
    for (i = 0; i < s.len; i++) {
        hash ^= s.data[i];
        hash *= 16777619;
    }
    return hash;
}

void
S8List_pushS8(S8List *list, S8 str, mem_Allocator a)
{
    S8Node *node = mem_MAKE(S8Node, 1, a);
    node->str = str;
    S8List_pushS8Node(list, node);
}

void
S8List_pushS8Node(S8List *list, S8Node node)
{
    if (!list->first && !list->last) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        list->last = node;
    }
    list->node_count += 1;
    list->total_size += node->str.len;
}

B8
S8List_eq(S8List *a, S8List *b)
{
    S8Node *curr_a = a->first;
    S8Node *curr_b = b->first;

    if (a->total_size != b->total_size)
        return FALSE;
    if (a->node_count != b->node_count)
        return FALSE;

    while (curr_a != NIL || curr_b != NIL) {
        if (!S8_eq(curr_a->str, curr_b->str))
            return FALSE;

        curr_a = curr_a->next;
        curr_b = curr_b->next;
    }

    return TRUE;
}

B8
S8List_contains(S8List *haystack, S8 needle)
{
    S8Node *curr = haystack->first;
    while (curr != NIL) {
        if (S8_eq(needle, curr->str))
            return TRUE;
        curr = curr->next;
    }
    return FALSE;
}

S8
S8List_flatten(S8List *list, mem_Allocator a)
{
    S8 res = S8_alloc(list->total_size, a);
    ISize curr_offset = 0;
    S8Node node;
    
    for (node = list->first; node != NIL; node = node->next) {
        mem_copy(res.data + curr_offset, node->str.data, node->str.len);
        curr_offset += node->str.len;
    }

    return res;
}

void *
Slice_init(ISize item_size, ISize capacity, mem_Allocator a)
{
    void *ptr = NIL;
    ISize size = item_size * capacity + ISIZE_OF(Slice_Header);
    Slice_Header *h = a.alloc(size, a.ctx);

    if (h) {
        h->cap = capacity;
        h->len = 0;
        h->allocator = a;
        ptr = h + 1;
    }

    return ptr;
}

void *
Slice_ensureCap(void *slice, ISize n, ISize item_size)
{
    Slice_Header *h = Slice_HEADER(slice);
    ISize desired_cap = h->len + n;
    ISize new_cap = h->cap * 2;
    ISize new_size;
    Slice_Header *new_h;
    ISize old_size;

    if (h->cap < desired_cap) {
        while (new_cap < desired_cap) {
            new_cap *= 2;
        }

        new_size = ISIZE_OF(Slice_Header) + new_cap * item_size;
        new_h = h->allocator.alloc(new_size, h->allocator.ctx);

        if (new_h) {
            old_size = ISIZE_OF(*h) + h->len * item_size;
            mem_copy(new_h, h, old_size);

            new_h->cap = new_cap;
            h = new_h + 1;
        } else {
            h = NIL;
        }
    } else {
        h += 1;
    }

    return h;
}

// Linux specifcs
#ifdef OS_LINUX

#include <stdlib.h>
#include <sys/mman.h>

void
os_abort(S8 msg)
{
    // TODO log the error
    (void)msg;
    abort();
}

void *
os_alloc(ISize size)
{
    return mmap(NIL, (USize)size, PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

B8
os_free(ISize size, void *ptr)
{
    int result = munmap(ptr, (USize)size);
    return (B8)(result == 0);
}

B8
os_protect(void *ptr, ISize size, os_ProtectMode mode)
{
    int prot;
    int result;

    switch (mode) {
        case os_ProtectMode_Read:
            prot = PROT_READ;
        break; 
        case os_ProtectMode_ReadWrite:
            prot = PROT_READ | PROT_WRITE;
        break;
        case os_ProtectMode_Execute:
            prot = PROT_EXEC;
        break;
    }

    result = mprotect(ptr, (USize)size, prot);
    return (B8)(result == 0);
}

#endif//OS_LINUX

#endif//BASTD_IMPL

#endif//BASTD_BASE_H
