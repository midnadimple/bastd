#define BASTD_GUI // must be defined in order to import all the necessary code
#include "../../bastd.c"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	os_Input inp = {0}; // Can store an input per window
	os_Window win = os_Window_create(S8("bastd - Window"), 1280, 720, TRUE); // Last boolean = "resizeable?"

	// os_Window_update returns false when OS sends quit message
	while (os_Window_update(&win, &inp)) {
		// Immediate mode keyboard input
		if (os_Input_keyReleased(inp, 'W')) {
			OutputDebugStringA("W key released\n"); // Win32 Debugger only, will fix once renderer is done
		} else if (os_Input_keyPressed(inp, os_Key_LeftMouseBtn)) {
			OutputDebugStringA("Left mouse button pressed\n"); // Win32 Debugger only, will fix once renderer is done
		}

		// TODO mouse pos example
	}
	os_Window_close(win);
	return os_ErrorCode_success;
}