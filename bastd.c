#include "bastd.h"

#include "bastd/mem.c"
#include "bastd/string.c"
#include "bastd/buffer.c"
#include "bastd/thread.c"

#if defined(_WIN32)
# include "bastd/os_windows.c"
#else
# include "bastd/os_posix.c"
#endif