#include <rtthread.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5
/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    rt_uint32_t count = 0;
    int i=0;

    for(i = 0 ; i < 10 ; i++)
    {
        /* 执行yield后应该切换到thread2执行*/
        rt_thread_yield();
        /* 打印线程1的输出*/
        rt_kprintf("thread1: count = %d\n", count ++);
    }
}
/* 线程2入口 */
 static void thread2_entry(void* parameter)
 {
        rt_uint32_t count = 0;
    int i=0;

    for(i = 0 ; i < 10 ; i++)
    {
        /* 打印线程2的输出*/
        rt_kprintf("thread2: count = %d\n", count ++);
        /* 执行yield后应该切换到thread1执行*/
        rt_thread_yield();
    }
}

void test_thread_08(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, RT_NULL, 
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
MSH_CMD_EXPORT(test_thread_08, thread test);