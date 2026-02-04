#include "lib.h"

#include "lib/mem.c"
#include "lib/string.c"
#include "lib/bitset.c"
#include "lib/buffer.c"
#include "lib/thread.c"

#if defined(_WIN32)
# include "lib/os_windows.c"
#else
/* Unfinished */
# include "lib/os_posix.c"
#endif