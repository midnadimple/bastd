#ifndef BASTD_OS_WINDOWS_C
#define BASTD_OS_WINDOWS_C

#include <windows.h>
#include <intrin.h>
#include <shellapi.h>
#include <io.h>

#pragma comment (lib, "shell32")
#pragma comment (lib, "gdi32")
#pragma comment (lib, "user32")
#pragma comment (lib, "webgpu_dawn")

#define os_DEBUGBREAK() __debugbreak();
#define os_WRITE_BARRIER _WriteBarrier()
#define os_READ_BARRIER _ReadBarrier()

GLOBAL_VAR struct {
    HINSTANCE instance;    
} os_win32_priv;

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
    res.raw = (void*)CreateFileA(res.name, GENERIC_READ | GENERIC_WRITE,
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
    res.raw = (void*)GetStdHandle((U32)(-11));
    return res;
}

FUNCTION os_File
os_getStdin(void)
{
    os_File res = {0};
    res.already_exists = TRUE;
    res.name = "stdin";
    res.size = KILO(1);
    res.raw = (void*)GetStdHandle((U32)(-10));
    return res;
}
        
FUNCTION B32
os_deleteFile(os_File file)
{
    return DeleteFileA(file.name);
}

FUNCTION B32
os_closeFile(os_File file)
{
    return CloseHandle((HANDLE)file.raw);
}

FUNCTION B32
os_writeFile(os_File file, U8 *buf, int len)
{
    U32 dummy;
    return WriteFile((HANDLE)file.raw, buf, len, &dummy, 0);
}

FUNCTION B32
os_readFile(os_File file, U8 *buf, int len, U32 *bytes_read)
{
    return ReadFile((HANDLE)file.raw, buf, len, bytes_read, 0);
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

FUNCTION os_Thread
os_Thread_start(os_ThreadProc proc, void *ctx)
{
    os_Thread res = {0};
    res.raw = (void*)CreateThread(NIL, 0, proc, ctx, 0, NIL);
    return res;
}

FUNCTION void
os_Thread_detach(os_Thread *t)
{
    if (t->raw == NIL) return;
    CloseHandle((HANDLE)t->raw);
    t->raw = NIL;
}

FUNCTION void
os_Thread_join(os_Thread *t)
{
    if (t->raw == NIL) return;
    WaitForSingleObject((HANDLE)t->raw, INFINITE);
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

FUNCTION os_Semaphore
os_Semaphore_create(I32 initial_count, I32 max_count)
{
    os_Semaphore res = {0};

    res.raw = (void*)CreateSemaphoreExA(NIL, initial_count, max_count, NIL, 0, SEMAPHORE_ALL_ACCESS);
    
    return res;
}

FUNCTION B32
os_Semaphore_increment(os_Semaphore semaphore, I32 count)
{
    return ReleaseSemaphore((HANDLE)semaphore.raw, count, NIL);
}

FUNCTION void
os_Semaphore_wait(os_Semaphore semaphore)
{
    WaitForSingleObjectEx((HANDLE)semaphore.raw, INFINITE, FALSE);
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

FUNCTION void
os_debugPrint(S8 msg)
{
    msg.raw[msg.len - 1] = '\0';
    OutputDebugStringA(msg.raw);
}

#if defined(BASTD_CLI)

CALLBACK_EXPORT int
main(int argc, char *argv[])
{
    os_win32_priv.instance = GetModuleHandle(0);
    return os_entry();
}

#elif defined(BASTD_GUI)

#pragma comment (linker, "/subsystem:windows")

CALLBACK_EXPORT LRESULT
os_Window_win32Callback(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(wnd, msg, wparam, lparam);
}


FUNCTION os_Window
os_Window_create(S8 title, U64 width, U64 height, B32 resizeable)
{
    os_Window res = {0};
    res.title = title;
    res.width = width;
    res.height = height;

    // register window class to have custom WindowProc callback
    WNDCLASSEXW wc =
    {
        .cbSize = sizeof(wc),
        .lpfnWndProc = &os_Window_win32Callback,
        .hInstance = os_win32_priv.instance,
        .hIcon = LoadIcon(NULL, IDI_APPLICATION), // custom icon support
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = L"bastd_webgpu_class",
    };
    ATOM atom = RegisterClassExW(&wc);
    ASSERT(atom, "Failed to register window class");

    // window properties - width, height and style
    DWORD exstyle = WS_EX_APPWINDOW;
    DWORD style = WS_OVERLAPPEDWINDOW;

    RECT rect = { 0, 0, res.width, res.height };
    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    if (!resizeable) {
        style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    }

    wchar_t title_wchar[1024];
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, res.title.raw, -1, title_wchar, 1024);

    // create window
    res.raw = (void *)CreateWindowExW(
        exstyle, wc.lpszClassName, title_wchar, style,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, wc.hInstance, NULL);
    ASSERT(res.raw, "Failed to create window");

    ShowWindow(res.raw, SW_SHOWDEFAULT);

    return res;
}

FUNCTION B32
os_Window_update(os_Window *window, os_Input *input)
{
    RECT rect;
    // get current size and pos for window client area
    if (window->resizeable) {
        GetClientRect(window->raw, &rect);
        window->width = rect.right - rect.left;
        window->height = rect.bottom - rect.top;
    }

    GetWindowRect(window->raw, &rect);
    window->x = rect.left;
    window->y = rect.top;
    
    if (input) {
        for (U64 i = 0; i < os_Key_len; i++) {
            input->key_was_down[i] = input->key_down[i]; 
        }
        
        POINT cursor;
        GetCursorPos(&cursor);
        input->mouse_x = cursor.x - rect.left;
        // have to do this to avoid the stupid title bars
        int tb_height = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
        GetSystemMetrics(SM_CXPADDEDBORDER));
        input->mouse_y = cursor.y - (rect.top + tb_height);
    }

    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        switch (msg.message)
        {
            case WM_QUIT:
                return FALSE;
                break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
                if (input) {
                    U32 key_code = (U32)msg.wParam;
                    B32 is_down = ((msg.lParam & (1 << 31)) == 0);
                    input->key_down[key_code] = is_down;
                }
                break;
            // TODO higher-order clicks
            // TODO other mouse buttons
            case WM_LBUTTONDOWN:
                input->key_down[os_Key_LeftMouseBtn] = TRUE;
                break;
            case WM_LBUTTONUP:
                input->key_down[os_Key_LeftMouseBtn] = FALSE;
                break;
            case WM_RBUTTONDOWN:
                input->key_down[os_Key_RightMouseBtn] = TRUE;
                break;
            case WM_RBUTTONUP:
                input->key_down[os_Key_RightMouseBtn] = FALSE;
                break;
            case WM_MBUTTONDOWN:
                input->key_down[os_Key_MidMouseBtn] = TRUE;
                break;
            case WM_MBUTTONUP:
                input->key_down[os_Key_MidMouseBtn] = FALSE;
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
                break;
        }
    }

    return TRUE;
}

FUNCTION void
os_Window_close(os_Window window)
{
    DestroyWindow(window.raw);
}

FUNCTION WGPUSurfaceDescriptor
os_Window_getWGPUSurfaceDesc(os_Window window)
{
    WGPUSurfaceSourceWindowsHWND *source = VirtualAlloc(0, sizeof(WGPUSurfaceSourceWindowsHWND), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    source->chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
    source->hinstance = os_win32_priv.instance;
    source->hwnd = window.raw;
    return (WGPUSurfaceDescriptor){
        .nextInChain = &source->chain,
    };
}

FUNCTION void
os_errorBoxGui(S8 msg)
{
    msg.raw[msg.len] = '\0';
    MessageBoxA(NIL, msg.raw, "bastd - Error", MB_ICONEXCLAMATION);
    ExitProcess(0);
}

CALLBACK_EXPORT int
WinMain(HINSTANCE instance,
    HINSTANCE prev_instance,
    LPSTR cmd_line,
    int show_code)
{
    os_win32_priv.instance = instance;
    return os_entry();
}

#endif

#endif//BASTD_OS_WINDOWS_C