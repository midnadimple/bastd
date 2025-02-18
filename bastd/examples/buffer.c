#define BASTD_CLI
#include "../../bastd.c"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	Buffer buf = BUFFER(os_alloc(KILO(2)), KILO(2));
	Buffer_appendS8(&buf, S8("Hello, Buffered output! "));
	Buffer_appendI64(&buf, 10245);
	S8 buf_built = Buffer_buildS8(&buf, &perm);

	Buffer_appendS8(&buf, buf_built);
	Buffer_appendU8(&buf, '\n');
	Buffer_standardOutput(&buf);

	Buffer_appendS8(&buf, buf_built);
	Buffer_appendU8(&buf, '\n');
	Buffer_fileOutput(&buf, S8("test.txt"));

	Buffer_appendFile(&buf, S8("test.txt"), &perm);
	Buffer_appendS8(&buf, S8("We got this data from a file.\n"));
	Buffer_standardOutput(&buf);

	Buffer_appendS8(&buf, S8("USER INPUT:\n"));
	Buffer_standardOutput(&buf);

	Buffer_appendStandardInput(&buf, 1024, &perm);
	Buffer_fileOutput(&buf, S8("test.txt"));

	return os_ErrorCode_success;
}