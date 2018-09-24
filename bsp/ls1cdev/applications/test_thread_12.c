#include <rtthread.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    rt_uint8_t i;
    for(i = 0; i < 6; i ++)
    {
        rt_kprintf("THREAD1:%d\n\r",i);
        rt_kprintf("THIS IS \n");
        rt_kprintf("A\n");
        rt_kprintf("DEMO\n");       
        rt_thread_delay(1);
    }
}
/* 线程2入口 */
static void thread2_entry(void* parameter)
{
    rt_uint8_t i;
    for(i = 0; i < 60; i ++)
    {
        rt_kprintf("thread2:%d\n\r",i);
        rt_kprintf("this is \n");
        rt_kprintf("a\n");
        rt_kprintf("demo\n");       
    }
}
void test_thread_12(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE*2);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
		thread2_entry,   RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_12, thread test);