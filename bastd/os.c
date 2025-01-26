#ifndef BASTD_OS_C
#define BASTD_OS_C

FUNCTION void os_abort(char *msg);
FUNCTION void *os_alloc(U64 cap);
FUNCTION B8 os_write(int fd, U8 *buf, int len);
FUNCTION B8 os_read(int fd, U8 *buf, int len, U32 *bytes_read);
FUNCTION U64 os_getFileSize(int fd);
FUNCTION int os_openFile(U8 *filename, B8 always_create);
FUNCTION B8 os_closeFile(int fd);

typedef struct os_Thread os_Thread;
struct os_Thread; // Depends on the OS

typedef U32 (*os_ThreadProc)(void *);
FUNCTION os_Thread os_Thread_start(os_ThreadProc proc, void *ctx);
FUNCTION void os_Thread_detach(os_Thread *t);
FUNCTION void os_Thread_join(os_Thread *t);

FUNCTION I64 os_atomic_compareExchange64(I64 volatile *dst, I64 exchange, I64 compare);
FUNCTION I64 os_atomic_increment64(I64 volatile *dst);
FUNCTION I64 os_atomic_decrement64(I64 volatile *dst);
#define os_WRITE_BARRIER // Depends on the OS
#define os_READ_BARRIER // Depends on the OS

typedef struct os_Semaphore os_Semaphore;
struct os_Semaphore; // Depends on the OS
FUNCTION os_Semaphore os_Semaphore_create(I32 initial_count, I32 max_count);
FUNCTION B8 os_Semaphore_increment(os_Semaphore semaphore);
FUNCTION void os_Semaphore_wait(os_Semaphore semaphore);

typedef int os_ErrorCode;
enum {
	os_ErrorCode_success = 0,
	os_ErrorCode_fail,
};
CALLBACK_EXPORT os_ErrorCode os_entry(void);

FUNCTION U64 os_rdtsc(void);

#if defined(OS_WINDOWS)

#include "os_windows.c"

#elif defined(OS_LINUX)

#include "os_linux.c"

#endif


#endif//BASTD_OS_C