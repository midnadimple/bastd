#ifndef BASTD_OS_C
#define BASTD_OS_C

FUNCTION void os_abort(char *msg);
FUNCTION void *os_alloc(U64 cap);
FUNCTION B8 os_write(int fd, U8 *buf, int len);
FUNCTION B8 os_read(int fd, U8 *buf, int len, U32 *bytes_read);
FUNCTION U64 os_getFileSize(int fd);
FUNCTION int os_openFile(U8 *filename, B8 always_create);
FUNCTION B8 os_closeFile(int fd);

typedef int os_ErrorCode;
enum {
	os_ErrorCode_success = 0,
	os_ErrorCode_fail,
};
CALLBACK_EXPORT os_ErrorCode os_entry(void);

#if defined(OS_WINDOWS)

#include "os_windows.c"

#elif defined(OS_LINUX)

#include "os_linux.c"

#endif


#endif//BASTD_OS_C