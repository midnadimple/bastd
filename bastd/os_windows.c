#ifndef BASTD_OS_WINDOWS_C
#define BASTD_OS_WINDOWS_C

#include <windows.h>

#define OS_DEBUGBREAK() __debugbreak();

FUNCTION void
OS_Abort(S8 msg)
{
    HANDLE stderr = GetStdHandle(-12);
    U32 dummy;
    WriteFile(stderr, msg.raw, msg.length, &dummy, 0);
    ExitProcess(101);
}

FUNCTION void *
OS_Alloc(ISize cap)
{
	return VirtualAlloc(NIL, cap, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

#endif//BASTD_OS_WINDOWS_C