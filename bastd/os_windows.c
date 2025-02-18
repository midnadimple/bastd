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

struct os_File {
    U8 *name;
    HANDLE raw;
    U64 size;
    B8 already_exists;
};

FUNCTION os_File
os_openFile(U8 *filename)
{
    os_File res = {0};
    
    res.name = filename;
    
    U32 attribs = GetFileAttributes(res.name);
    res.already_exists = (attribs != INVALID_FILE_ATTRIBUTES &&
        !(attribs & FILE_ATTRIBUTE_DIRECTORY));
        
    DWORD open_style = CREATE_NEW;
    if (res.already_exists) {
        open_style = OPEN_EXISTING;
    }
    res.raw = CreateFileA(res.name, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ, 0, open_style, FILE_ATTRIBUTE_NORMAL, 0);
            
    LARGE_INTEGER file_size;
    GetFileSizeEx(res.raw, &file_size);
    res.size = (U64)file_size.QuadPart;
    
    return res;
}

FUNCTION os_File
os_getStdout(void)
{
    os_File res = {0};
    res.already_exists = TRUE;
    res.name = "stdout";
    res.size = KILO(1);
    res.raw = GetStdHandle((U32)(-11));
    return res;
}

FUNCTION os_File
os_getStdin(void)
{
    os_File res = {0};
    res.already_exists = TRUE;
    res.name = "stdin";
    res.size = KILO(1);
    res.raw = GetStdHandle((U32)(-10));
    return res;
}
        
FUNCTION B8
os_deleteFile(os_File file)
{
    return DeleteFileA(file.name);
}

FUNCTION B8
os_closeFile(os_File file)
{
    return CloseHandle(file.raw);
}

FUNCTION B8
os_writeFile(os_File file, U8 *buf, int len)
{
    U32 dummy;
    return WriteFile(file.raw, buf, len, &dummy, 0);
}

FUNCTION B8
os_readFile(os_File file, U8 *buf, int len, U32 *bytes_read)
{
    return ReadFile(file.raw, buf, len, bytes_read, 0);
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
os_wallclock(void)
{
    LARGE_INTEGER count, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    
    U64 res = (count.QuadPart * 1000000) / freq.QuadPart;
    return res;
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