#ifndef BASTD_OS_C
#define BASTD_OS_C

FUNCTION void OS_Abort(S8 msg);
FUNCTION void *OS_Alloc(ISize cap);

#if defined(OS_WINDOWS)

#include "os_windows.c"

#elif defined(OS_LINUX)

#include "os_linux.c"

#endif//OS_WINDOWS

#endif//BASTD_OS_C