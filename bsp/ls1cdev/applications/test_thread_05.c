#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

/* 线程1入口 */ 
static void thread1_entry(void* parameter)
{
        rt_uint32_t count = 0;
        rt_kprintf("thread1 dynamicly created ok\n");
        while(1)
        {
                rt_kprintf("thread1 count: %d\n",count++);
                rt_thread_delay(RT_TICK_PER_SECOND);
        }
}
/* 线程2入口 */ 
static void thread2_entry(void* parameter)
{
        rt_kprintf("thread2 dynamicly created ok\n");

        rt_thread_delay(RT_TICK_PER_SECOND * 4);

        rt_thread_delete(tid1);
        rt_kprintf("thread1 deleted ok\n");
}

void test_thread_05(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, 
		RT_NULL,
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE); 
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
		thread2_entry, RT_NULL,
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE); 
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_05, thread test);