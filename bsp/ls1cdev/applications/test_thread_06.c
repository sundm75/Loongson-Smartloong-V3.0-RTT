#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

/* 线程入口 */
static void thread_entry(void* parameter)
{
    rt_uint32_t count = 0;
    rt_uint32_t no = (rt_uint32_t) parameter; /* 获得线程的入口参数 */

    while (1)
    {
        /* 打印线程计数值输出 */
        rt_kprintf("thread%d count: %d\n", no, count ++);

        /* 休眠10个OS Tick */
        rt_thread_delay(10);
    }
}

void test_thread_06(void)
{
	tid1 = rt_thread_create("thread1",
		thread_entry, (void*)1, /* 线程入口是thread_entry, 入口参数是1 */
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);
	else
		return ;

	tid2 = rt_thread_create("thread2",
		thread_entry, (void*)2, /* 线程入口是thread_entry, 入口参数是2 */
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
	else
		return ;
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_06, thread test);