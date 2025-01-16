#include "bastd.c"

// Single line
/* Multi
Line */

// "GLOBAL_VAR" macro expands to "static"
GLOBAL_VAR F32 evil_number = 666 // variables names use snake_case

typedef struct RenderVertex RenderVertex; // typedefs come before structs
struct RenderVertex { // struct names use PascalCase
	F32 x, y, z
}

// Generic pattern. the output type will be "Slice_RenderVertex"
#define SLICE_TYPE RenderVertex
#include "slice.h"

// all functions prefixed with "FUNCTION" macro, which expands to "static"
FUNCTION I8 // primitive types follow rust style
addTwo(I8 a, I8 b) // Function names use camelCase
{
	I8 res = a + b;
	ASSERT(res != 666, "Ah! The Devil!"); // All macros, regardless of usage, are MACRO_CASE
	return res;
}

/* Functions called by other functions are prefixed with CALLBACK, which is not
   defined as "static" , incase you decide to move it to a different
   compilation unit
*/
CALLBACK Error
start(Slice_S8 args, Buffer stdin, Buffer stdout)
{
	/* all functions that belong to a type or a module are styled
	   "m_TypeName_functionName". "m" is a lower-case shortened version of the
	   module's name (usually 1-3 letters).
	*/
	Buffer_appendS8(&stdout, args[0]);
	Buffer_flush(&stdout);

	r_Window window = r_Window_create(1280, 720, "Bastd Example");
	I32 frame_number = 0;

	for (!window.should_close) {
		frame_number += 1;

		// if there is no module, function is styled "TypeName_functionName"
		Buffer_appendS8(&stdout, "Current frame: ");
		Buffer_appendI32(&stdout, frame_number);
		Buffer_appendChar(&stdout, '\n');
		Buffer_flush(&stdout);

		r_Window_tick(&window);
	}

	/* Enum names are styled with PascalCase. Enum values are styled
	   "TypeName_value_name"*/
	return Error_success;
}