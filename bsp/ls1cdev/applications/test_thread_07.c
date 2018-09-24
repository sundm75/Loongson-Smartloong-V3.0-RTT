#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static struct rt_thread thread1;
static rt_uint8_t thread1_stack[THREAD_STACK_SIZE];
static struct rt_thread thread2;
static rt_uint8_t thread2_stack[THREAD_STACK_SIZE];
/* 线程1入口 */
static void thread1_entry(void* parameter)
{
	rt_uint32_t count = 0;

	while (1)
	{
		/* 线程1采用低优先级运行，一直打印计数值 */
		rt_kprintf("thread count: %d\n", count ++);
		rt_thread_delay(RT_TICK_PER_SECOND);
	}
}
/* 线程2入口 */	
static void thread2_entry(void* parameter)
{
	/* 线程2拥有较高的优先级，以抢占线程1而获得执行 */	
	/* 线程2启动后先睡眠10个OS Tick */
	rt_thread_delay(RT_TICK_PER_SECOND*10);

	/*线程2唤醒后直接执行线程1脱离，线程1将从就绪线程队列中删除*/
	rt_thread_detach(&thread1);

	/*线程2继续休眠10个OS Tick然后退出*/
	rt_thread_delay(10);	
}

void test_thread_07(void)
{
	rt_err_t result; 
	result = rt_thread_init(&thread1, "thread1", 
			thread1_entry, RT_NULL, 
			&thread1_stack[0], sizeof(thread1_stack), 
			THREAD_PRIORITY + 1, THREAD_TIMESLICE);
	if (result == RT_EOK) 
			rt_thread_startup(&thread1);

	result = rt_thread_init(&thread2, "thread2", 
			thread2_entry, RT_NULL, 
			&thread2_stack[0], sizeof(thread2_stack), 
			THREAD_PRIORITY, THREAD_TIMESLICE);
	if (result == RT_EOK) 
			rt_thread_startup(&thread2);

}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_07, thread test);