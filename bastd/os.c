#ifndef BASTD_OS_C
#define BASTD_OS_C

FUNCTION void os_abort(char *msg);
FUNCTION void *os_alloc(U64 cap);

typedef struct os_File os_File;
struct os_File {
    U8 *name;
	void *raw; // Depends on the OS
	U64 size;
    B32 already_exists;
};
FUNCTION os_File os_openFile(U8 *filename);
FUNCTION os_File os_getStdout(void);
FUNCTION os_File os_getStdin(void);

FUNCTION B32 os_deleteFile(os_File file);
FUNCTION B32 os_closeFile(os_File file);

FUNCTION B32 os_readFile(os_File file, U8 *buf, int len, U32 *bytes_read);
FUNCTION B32 os_writeFile(os_File file, U8 *buf, int len);

typedef struct os_Thread os_Thread;
struct os_Thread {
	void *raw; // Depends on the OS
};
typedef U32 (*os_ThreadProc)(void *);
FUNCTION os_Thread os_Thread_start(os_ThreadProc proc, void *ctx);
FUNCTION void os_Thread_detach(os_Thread *t);
FUNCTION void os_Thread_join(os_Thread *t);

FUNCTION I64 os_atomic_compareExchange64(I64 volatile *dst, I64 exchange, I64 compare);
FUNCTION I64 os_atomic_increment64(I64 volatile *dst);
FUNCTION I64 os_atomic_decrement64(I64 volatile *dst);
//os_WRITE_BARRIER // Depends on the OS
//os_READ_BARRIER // Depends on the OS

typedef struct os_Semaphore os_Semaphore;
struct os_Semaphore {
	void *raw; // Depends on the OS
};
FUNCTION os_Semaphore os_Semaphore_create(I32 initial_count, I32 max_count);
FUNCTION B32 os_Semaphore_increment(os_Semaphore semaphore);
FUNCTION void os_Semaphore_wait(os_Semaphore semaphore);

// Handle microseconds using U64
FUNCTION U64 os_wallclock(void);

FUNCTION void os_debugPrint(S8 msg);

#ifdef BASTD_GUI
// windowing stuff

#include "os_key.c"

typedef struct os_Input os_Input;
struct os_Input {
	B32 key_down[os_Key_len];
	B32 key_was_down[os_Key_len];
	I32 mouse_x, mouse_y; // relative to top-left of screen
	// TODO repeat event
	// TODO gamepad
};

FUNCTION B32
os_Input_keyPressed(os_Input inp, os_Key key)
{
	return inp.key_down[key] && !inp.key_was_down[key];
}

FUNCTION B32
os_Input_keyDown(os_Input inp, os_Key key)
{
	return inp.key_down[key];
}

FUNCTION B32
os_Input_keyReleased(os_Input inp, os_Key key)
{
	return !inp.key_down[key] && inp.key_was_down[key];
}

typedef struct os_Window os_Window;
struct os_Window {
	S8 title;
	U64 width;
	U64 height;
	U64 x; // leftmost
	U64 y; // topmost
	B32 resizeable;
	void *raw; // Depends on the OS
};

FUNCTION os_Window os_Window_create(S8 title, U64 width, U64 height, B32 resizeable);
FUNCTION B32 os_Window_update(os_Window* window, os_Input *input);
FUNCTION void os_Window_close(os_Window window);

#include "webgpu.h"
FUNCTION WGPUSurfaceDescriptor os_Window_getWGPUSurfaceDesc(os_Window window);

FUNCTION void os_errorBoxGui(S8 msg);

#endif

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