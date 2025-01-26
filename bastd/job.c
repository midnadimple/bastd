#ifndef BASTD_JOB_C
#define BASTD_JOB_C

typedef void (*JobProc)(void *args);

typedef struct Job Job;
struct Job {
	JobProc proc;
	void *args;
};
#define JOB(p, a) (Job){.proc = p, .args = (void *)a}

#define MAX_NUM_JOBS 256
typedef struct JobQueue JobQueue;
struct JobQueue {
	U64 next_read;
	U64 next_write;
	
	U64 complete_len;
	U64 complete_goal;

	Job jobs[MAX_NUM_JOBS];
	os_Semaphore semaphore;
};

FUNCTION B8
JobQueue__nextJob(JobQueue *queue)
{
	B8 done = FALSE;
	U64 next_read = queue->next_read;
	U64 new_next_read = (next_read + 1) % MAX_NUM_JOBS;	

	if (next_read != queue->next_write) {
		U64 index = os_atomic_compareExchange64((I64 volatile *)&queue->next_read, new_next_read, next_read);
		if (index == next_read) {
			done = TRUE;
			Job *job = queue->jobs + index;
			job->proc(job->args);
			os_atomic_increment64((I64 volatile *)&queue->complete_len);
		}
	}

	return done;
}

CALLBACK_EXPORT U32
JobQueue__threadProc(void *args)
{
	JobQueue *queue = (JobQueue *)args;
	for (;;) {
		if (!JobQueue__nextJob(queue)) {
			os_Semaphore_wait(queue->semaphore);
		}
	}
}

FUNCTION JobQueue *
JobQueue_start(int num_threads, m_Allocator *perm)
{
	JobQueue *res = m_MAKE(JobQueue, 1, perm);

	res->semaphore = os_Semaphore_create(0, num_threads);

	for (int i = 0; i < num_threads; i++) {
		os_Thread t = os_Thread_start(JobQueue__threadProc, res);
		os_Thread_detach(&t);
	}

	return res;
}

// Should be called on main thread for now. Maybe need to sync up later
FUNCTION void
JobQueue_dispatch(JobQueue *queue, Job job)
{
	U64 next_write = queue->next_write;
	U64 new_next_write = (next_write + 1) % MAX_NUM_JOBS;
	//ASSERT(new_next_write != queue->next_read, "This will override the current entry");
	Job *job_to_write = queue->jobs + next_write;
	*job_to_write = job;
	queue->complete_goal++;

	os_WRITE_BARRIER;

	queue->next_write = new_next_write;
	os_Semaphore_increment(queue->semaphore, 1);
}

FUNCTION void
JobQueue_completeAll(JobQueue *queue)
{
	while (queue->complete_len != queue->complete_goal) {
		JobQueue__nextJob(queue);
	}

	queue->complete_len = 0;
	queue->complete_goal = 0;
}

#endif//BASTD_JOB_C