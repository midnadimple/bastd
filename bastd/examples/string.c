#define BASTD_CLI
#include "../../bastd.c"

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	Buffer output = BUFFER(m_MAKE(U8, KILO(2), &perm), KILO(2));

	S8 init[] = {S8("Hello"), S8("World")};
	sl_S8 str_list = sl_S8_create(init, 2);
	S8 greeting = S8_join(str_list, S8(", "), &perm);
	
	Buffer_append_S8(&output, greeting);
	Buffer_append_S8(&output, S8("!\n"));
	Buffer_standardOutput(&output);

	S8 hello = S8_sub(greeting, 0, 5, &perm);
	Buffer_append_S8(&output, hello);
	Buffer_append_S8(&output, S8("!\n"));
	Buffer_standardOutput(&output);

	sl_S8 bye_bye = S8_split(S8("Goodbye! Sayonara! Ciao!"), S8(" "), &perm);
	for (; bye_bye.len > 0;) {
		Buffer_append_S8(&output, sl_S8_pop(&bye_bye));
		Buffer_append_U8(&output, '\n');
	}
	Buffer_standardOutput(&output);

	return os_ErrorCode_success;
}