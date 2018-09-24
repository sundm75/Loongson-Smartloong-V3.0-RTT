#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t worker = RT_NULL;
static rt_sem_t sem = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

rt_uint32_t worker_count, t1_count, t2_count;

static void thread1_entry(void* parameter)
{
    rt_err_t result;
    result = rt_sem_take(sem, RT_WAITING_FOREVER);
    for(t1_count = 0; t1_count < 10; t1_count ++)
    {
        rt_kprintf("thread1: got semaphore, count: %d\n",   t1_count);
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
    rt_kprintf("thread1: release semaphore\n");
    rt_sem_release(sem);
}

static void thread2_entry(void* parameter)
{
    rt_err_t result;
    while (1)
    {
        result = rt_sem_take(sem, RT_WAITING_FOREVER);
        rt_kprintf("thread2: got semaphore\n");
        if (result != RT_EOK)
        {
            return;
        }
        rt_kprintf("thread2: release semaphore\n");
        rt_sem_release(sem);
        rt_thread_delay(5);
        result = rt_sem_take(sem, RT_WAITING_FOREVER);
        t2_count ++;
        rt_kprintf("thread2: got semaphore, count: %d\n",   t2_count);
    }
}
static void worker_thread_entry(void* parameter)
{
    rt_thread_delay(5);
    for(worker_count = 0; worker_count < 10; worker_count++)
    {
        rt_kprintf("worker: count: %d\n", worker_count);
    }
    rt_thread_delay(RT_TICK_PER_SECOND);
}

void test_comu_04(void)
{
	sem = rt_sem_create("sem", 1, RT_IPC_FLAG_PRIO);
	if (sem == RT_NULL)
	{
		return ;
	}
	
	tid1 = rt_thread_create("thread1",
	thread1_entry, RT_NULL,       
	THREAD_STACK_SIZE, THREAD_PRIORITY + 2, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    tid2 = rt_thread_create("thread2",
        thread2_entry,   RT_NULL,       
        THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
		
		worker = rt_thread_create("worker",
        worker_thread_entry,   RT_NULL,       
        THREAD_STACK_SIZE, THREAD_PRIORITY+1, THREAD_TIMESLICE);
    if (worker != RT_NULL)
        rt_thread_startup(worker);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_04, comu test);