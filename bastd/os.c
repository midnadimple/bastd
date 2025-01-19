#ifndef BASTD_OS_C
#define BASTD_OS_C

FUNCTION void os_abort(char *msg);
FUNCTION void *os_alloc(ISize cap);

#if defined(OS_WINDOWS)

#include "os_windows.c"

#elif defined(OS_LINUX)

#include "os_linux.c"

#endif//OS_WINDOWS

#endif//BASTD_OS_C