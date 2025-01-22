#ifndef BASTD_C
#define BASTD_C

////////////////////////////////
// NOTE(allen): Context Cracking

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
# error no context cracking for this compiler
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


// NOTE(allen): Profiler
#if !defined(PROFILER_SPALL)
# define PROFILER_SPALL 0
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

// Types
#include <stdint.h>

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

typedef uintptr_t UPtr;
typedef ptrdiff_t ISize;
typedef size_t USize;

typedef U8 B8;
#define TRUE 1
#define FALSE 0

// Macros
#define CALLBACK_EXPORT

#define FUNCTION static
#define GLOBAL_VAR static
#define LOCAL_PERSIST static

#define STMNT(s) do{ s }while(0)

#define ASSERT(x, msg) STMNT(if (!x) os_DEBUGBREAK();)

#define CONCAT_(a,b) a##b
#define CONCAT(a,b) CONCAT_(a,b)

#define NIL 0

#define COUNT_OF(a) (ISize)(sizeof(a) / sizeof(*(a)))
#define LENGTH_OF(s) (COUNT_OF(s) - 1)

#define KILO(n) (n << 10)
#define MEGA(n) (n << 20)
#define GIGA(n) (n << 30)
#define TERA(n) (n << 40)
#define PETA(n) (n << 50)

// User Config Flags
#if !defined(BASTD_CLI) && !defined(BASTD_GUI)
#error "BASTD_CLI or BASTD_GUI must be defined to set the approriate entry point
#endif

// Includes
#include "bastd/os.c"
#include "bastd/memory.c"
#include "bastd/string.c"
#include "bastd/buffer.c"
#include "bastd/args.c"

#endif //BASTD_C