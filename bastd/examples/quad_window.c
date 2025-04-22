#define BASTD_GUI // must be defined in order to import all the necessary code
#include "../../bastd.c"
#include "../quad.c"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	m_Arena arena = m_Arena_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator allocator = m_ARENA_ALLOCATOR(arena);

	os_Input inp = {0}; // Can store an input per window
	os_Window win = os_Window_create(S8("bastd - Quad Window"), 1280, 720, FALSE); // Last boolean = "resizeable?"
	quad_Renderer *qr = quad_Renderer_create(&win, &allocator);

	// os_Window_update returns false when OS sends quit message
	while (os_Window_update(&win, &inp)) {


		// Immediate mode keyboard input
		if (os_Input_keyReleased(inp, 'W')) {
			OutputDebugStringA("W key released\n"); // Win32 Debugger only, will fix once renderer is done
		} else if (os_Input_keyPressed(inp, os_Key_LeftMouseBtn)) {
			OutputDebugStringA("Left mouse button pressed\n"); // Win32 Debugger only, will fix once renderer is done
		}

		quad_Renderer_begin(qr);

		// Projection: Orthographic, top-left = (0, 0)
		quad_Color col = {1.0, 0.0, 0.0, 1.0};
		if (os_Input_keyDown(inp, 'W')) {
			col[0] = 0.0;
			col[1] = 1.0;
		}

		quad_Renderer_pushQuad(qr,
			math_V2(inp.mouse_x - 50.0f, inp.mouse_y - 50.0f),
			math_V2(inp.mouse_x + 50.0f, inp.mouse_y + 50.0f),
			math_V2(0.0, 0.0), math_V2(1.0, 1.0),
			col);
		
			
		quad_Renderer_end(qr);
		
		quad_Renderer_begin(qr);
		
		// One texture per batch
		U64 x,y,n;
		U8 *pixels = stbi_load("assets/quad_window.texture.png", &x, &y, &n, 4);
		quad_Renderer_setTexture(qr, pixels, (WGPUExtent2D){x, y});

		quad_Renderer_pushQuad(qr,
			math_V2(0.0f, 0.0f),
			math_V2(100.0f, 100.0f),
			math_V2(0.0, 0.0), math_V2(1.0, 1.0),
			(quad_Color){1.0, 1.0, 1.0, 1.0});

		quad_Renderer_end(qr);
	}
	os_Window_close(win);
	return os_ErrorCode_success;
}