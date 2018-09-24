#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t worker = RT_NULL;
static rt_mutex_t mutex = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

rt_uint32_t worker_count, t1_count, t2_count;

static void thread1_entry(void* parameter)
{
    rt_err_t result;
    result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
    result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread1: got mutex\n");
    if (result != RT_EOK)
    {
        return;
    }
    for(t1_count = 0; t1_count < 5;t1_count ++)
    {
        rt_kprintf("thread1:count: %d\n", t1_count);
    }
    rt_kprintf("thread1: released mutex\n");
    /*判断语句用来验证thread1的优先级是否提升到与thread2一致，可去除*/
    if(tid2->current_priority == tid2->current_priority)
    {
        rt_mutex_release(mutex);
        rt_mutex_release(mutex);
    }
}

static void thread2_entry(void* parameter)
{
    rt_err_t result;
    rt_thread_delay(5);
    result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread2: got mutex\n");
    for(t2_count = 0; t2_count < 5;t2_count ++)
    {
        rt_kprintf("thread2: count: %d\n", t2_count);
    }
}

static void worker_thread_entry(void* parameter)
{
    rt_thread_delay(5);
    for(worker_count = 0; worker_count < 5; worker_count++)
    {
        rt_kprintf("worker:count: %d\n", worker_count);
        rt_thread_delay(5);
    }
}

void test_comu_08(void)
{
	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
	if (mutex == RT_NULL)
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
MSH_CMD_EXPORT(test_comu_08, comu test);