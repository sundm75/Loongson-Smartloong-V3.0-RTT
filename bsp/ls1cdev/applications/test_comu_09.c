#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 事件控制块 */
static struct rt_event event;
rt_err_t result;

/* 线程 1 入口 */
static void thread1_entry(void *param)
{
    rt_uint32_t event_rev;
    /* receive first event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &event_rev) == RT_EOK)
    {
        rt_kprintf("thread1: AND recv event 0x%x\n", event_rev);
    }
    rt_kprintf("thread1: delay 1s to prepare second event\n");
    rt_thread_delay(RT_TICK_PER_SECOND);
    /* receive second event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &event_rev) == RT_EOK)
    {
        rt_kprintf("thread1: OR recv event 0x%x\n", event_rev);
    }
    rt_kprintf("thread1 leave.\n");
}

/* 线程 2 入口 */
static void thread2_entry(void *param)
{
rt_kprintf("thread2: send event1\n");
rt_event_send(&event, (1 << 3));
rt_kprintf("thread2 leave.\n");
}

/* 线程 3 入口 */
static void thread3_entry(void *param)
{
    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));
    rt_thread_delay(20);
    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));
    rt_kprintf("thread3 leave.\n");
}

void test_comu_09(void)
{
	/* 初始化事件对象 */
	rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
	{
		rt_kprintf("init event failed.\n");
		return ;
	}
	tid1 = rt_thread_create("thread1",
			thread1_entry, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY - 1 , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
			thread2_entry,   RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY , THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
			rt_thread_startup(tid2);
	
	tid3 = rt_thread_create("thread3",
			thread3_entry,   RT_NULL,        
			THREAD_STACK_SIZE, THREAD_PRIORITY + 1, THREAD_TIMESLICE);
	if (tid3 != RT_NULL)
			rt_thread_startup(tid3);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_09, comu test);