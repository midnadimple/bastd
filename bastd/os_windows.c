#ifndef BASTD_OS_WINDOWS_C
#define BASTD_OS_WINDOWS_C

#include <windows.h>
#include <io.h>

#define os_DEBUGBREAK() __debugbreak();

FUNCTION void
os_abort(char *msg)
{
    HANDLE stderr = GetStdHandle(-12);
    U32 dummy;
    WriteFile(stderr, msg, LENGTH_OF(msg), &dummy, 0);
    ExitProcess(101);
}

FUNCTION void *
os_alloc(ISize cap)
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

FUNCTION ISize
os_getFileSize(int fd)
{
    HANDLE h = GetStdHandle(-10 - fd); 
    if (h == INVALID_HANDLE_VALUE) {
        // This is a real file
        h = (HANDLE)_get_osfhandle(fd);
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(h, &file_size);
    return (ISize)file_size.QuadPart;
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