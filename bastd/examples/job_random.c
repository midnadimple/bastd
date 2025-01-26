#define BASTD_CLI
#include "../../bastd.c"

typedef struct DataToPass DataToPass;
struct DataToPass {	
	I64 job_num;
	I64 random_num;
};

CALLBACK_EXPORT void
printJob(void *args)
{
	DataToPass *dtp = (DataToPass *)args;

	U8 raw[256];
	Buffer buf = BUFFER(raw, 256);
	Buffer_appendS8(&buf, S8("Thread #"));
	Buffer_appendI64(&buf, dtp->job_num);
	Buffer_appendS8(&buf, S8(" | Random number is "));
	Buffer_appendI64(&buf, dtp->random_num);
	Buffer_appendU8(&buf, '\n');
	Buffer_standardOutput(&buf);
}

CALLBACK_EXPORT os_ErrorCode
os_entry(void)
{	
	m_Buddy buddy = m_Buddy_create(os_alloc(MEGA(2)), MEGA(2));
	m_Allocator perm = m_BUDDY_ALLOCATOR(buddy);
	JobQueue *queue = JobQueue_start(8, &perm);

	for (I64 i = 0; i < 255; i++) {
		DataToPass *dtp = m_MAKE(DataToPass, 1, &perm);
		dtp->job_num = i;
		dtp->random_num = rand_next() % 100;
		JobQueue_dispatch(queue, JOB(printJob, dtp));
	}

	JobQueue_completeAll(queue);

	return os_ErrorCode_success;
}