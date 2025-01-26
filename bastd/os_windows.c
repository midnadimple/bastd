#ifndef BASTD_OS_WINDOWS_C
#define BASTD_OS_WINDOWS_C

#include <windows.h>
#include <intrin.h>
#include <shellapi.h>
#include <io.h>

#define os_DEBUGBREAK() __debugbreak();
#define os_WRITE_BARRIER _WriteBarrier()
#define os_READ_BARRIER _ReadBarrier()

FUNCTION void
os_abort(char *msg)
{
    HANDLE stderr = GetStdHandle(-12);
    U32 dummy;
    WriteFile(stderr, msg, LENGTH_OF(msg), &dummy, 0);
    ExitProcess(101);
}

FUNCTION void *
os_alloc(U64 cap)
{
	return VirtualAlloc(NIL, cap, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

FUNCTION B8
os_write(int fd, U8 *buf, int len)
{
    HANDLE h = GetStdHandle(-10 - fd); 
    if (h == INVALID_HANDLE_VALUE) {
        h = (HANDLE)_get_osfhandle(fd);
    }
    if (h == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    U32 dummy;
    return WriteFile(h, buf, len, &dummy, 0);
}

FUNCTION B8
os_read(int fd, U8 *buf, int len, U32 *bytes_read)
{
    HANDLE h = GetStdHandle(-10 - fd); 
    if (h != INVALID_HANDLE_VALUE) {
        // This *should be* stdin
        ASSERT(fd == 0, "fd is 0 for stdin");

        B8 res = ReadConsole(h, buf, len, bytes_read, NIL);
        return res;
    } else {
        h = (HANDLE)_get_osfhandle(fd);
        return ReadFile(h, buf, len, bytes_read, 0);
    }
}

FUNCTION U64
os_getFileSize(int fd)
{
    HANDLE h = GetStdHandle(-10 - fd); 
    if (h == INVALID_HANDLE_VALUE) {
        // This is a real file
        h = (HANDLE)_get_osfhandle(fd);
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(h, &file_size);
    return (U64)file_size.QuadPart;
}

FUNCTION int
os_openFile(U8 *filename, B8 always_create)
{
    U32 open_style = OPEN_EXISTING;
    if (always_create) {
        open_style = CREATE_ALWAYS;
    }
    HANDLE file = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ, 0, open_style, 0, 0);
    return _open_osfhandle((intptr_t)file, 0);
}

FUNCTION B8
os_closeFile(int fd)
{
    return _close(fd);
}

FUNCTION char **
os_getArgcAndArgv(int *argc)
{
    char** wargv = CommandLineToArgvW(GetCommandLineW(), argc);

    int n = 0;
    for (int i = 0; i < *argc;  i++)
        n += WideCharToMultiByte( CP_UTF8, 0, wargv[i], -1, NIL, 0, NIL, NIL ) + 1;
    
    char **argv = os_alloc( (*argc + 1) * sizeof(char *) + n );

    char *arg = (char *)&(argv[*argc + 1]);
    for (int i = 0; i < *argc; i++) {
        argv[i] = arg;
        arg += WideCharToMultiByte( CP_UTF8, 0, wargv[i], -1, arg, n, NIL, NIL ) + 1;
    }
    argv[*argc] = NIL;

    return argv;
}

struct os_Thread {
    HANDLE raw;
};

FUNCTION os_Thread
os_Thread_start(os_ThreadProc proc, void *ctx)
{
    os_Thread res = {0};
    res.raw = CreateThread(NIL, 0, proc, ctx, 0, NIL);
    return res;
}

FUNCTION void
os_Thread_detach(os_Thread *t)
{
    if (t->raw == NIL) return;
    CloseHandle(t->raw);
    t->raw = NIL;
}

FUNCTION void
os_Thread_join(os_Thread *t)
{
    if (t->raw == NIL) return;
    WaitForSingleObject(t->raw, INFINITE);
    os_Thread_detach(t);
}

FUNCTION I64
os_atomic_compareExchange64(I64 volatile *dst, I64 exchange, I64 compare)
{
    return InterlockedCompareExchange64(dst, exchange, compare);
}

FUNCTION I64
os_atomic_increment64(I64 volatile *dst)
{
    return InterlockedIncrement64(dst);
}

FUNCTION I64
os_atomic_decrement64(I64 volatile *dst)
{
    return InterlockedDecrement64(dst);
}

struct os_Semaphore {
    HANDLE raw;
};

FUNCTION os_Semaphore
os_Semaphore_create(I32 initial_count, I32 max_count)
{
    os_Semaphore res = {0};

    res.raw = CreateSemaphoreExA(NIL, initial_count, max_count, NIL, 0, SEMAPHORE_ALL_ACCESS);
    
    return res;
}

FUNCTION B8
os_Semaphore_increment(os_Semaphore semaphore, I32 count)
{
    return ReleaseSemaphore(semaphore.raw, count, NIL);
}

FUNCTION void
os_Semaphore_wait(os_Semaphore semaphore)
{
    WaitForSingleObjectEx(semaphore.raw, INFINITE, FALSE);
}

FUNCTION U64
os_rdtsc(void)
{
    return __rdtsc();
}

#if defined(BASTD_CLI)

CALLBACK_EXPORT int
main(int argc, char *argv[])
{
    return os_entry();
}

#elif defined(BASTD_GUI)

CALLBACK_EXPORT int
WinMain(HINSTANCE instance,
    HINSTANCE prev_instance,
    LPSTR cmd_line,
    int show_code)
{
    return os_entry();
}

#endif

#endif//BASTD_OS_WINDOWS_C