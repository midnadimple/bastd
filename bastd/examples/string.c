#include "../../bastd.c"

int
main(void)
{
	m_Buddy buddy = m_Buddy_create(os_alloc(2 * MEGA), 2 * MEGA);
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);

	S8 init[] = {S8("Hello"), S8("World")};
	sl_S8 str_list = sl_S8_create(init, 2);
	S8 greeting = s8Join(str_list, S8(", "), &perm);
	os_DEBUGBREAK();

	S8 hello = s8Sub(greeting, 0, 5, &perm);
	os_DEBUGBREAK();

	sl_S8 bye_bye = s8Split(S8("Goodbye! Sayonara! Ciao!"), S8(" "), &perm);
	os_DEBUGBREAK();
}