#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 消息队列控制块 */
static struct rt_messagequeue mq;
/* 消息队列中用到的放置消息的内存池 */
static char msg_pool[2048];

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    char buf[128];
    while (1)
    {
        rt_memset(&buf[0], 0, sizeof(buf));
        /* 从消息队列中接收消息 */
        if (rt_mq_recv(&mq, &buf[0], sizeof(buf), RT_WAITING_FOREVER)
                == RT_EOK)
        {
            rt_kprintf("thread1: recv a msg, the content:%s\n", buf);
        }
        rt_thread_delay(10);
    }
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
{
    int i, result;
    char buf[] = "this is message No.x";
    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            buf[sizeof(buf) - 2] = '0' + i;
            rt_kprintf("thread2: send message - %s\n", buf);
            /* 发送消息到消息队列中 */
            result = rt_mq_send(&mq, &buf[0], sizeof(buf));
            if ( result == -RT_EFULL)
            {
                rt_kprintf("message queue full, delay 1s\n");
                rt_thread_delay(100);
            }
        }
        rt_thread_delay(10);
    }
}

/* 线程3入口 */
static void thread3_entry(void* parameter)
{
    char buf[] = "this is an urgent message!";
    while (1)
    {
        rt_kprintf("thread3: send an urgent message\n");
        /* 发送紧急消息到消息队列中 */
        rt_mq_urgent(&mq, &buf[0], sizeof(buf));
        rt_thread_delay(25);
    }
}

void test_comu_11(void)
{
	/* 初始化消息队列 */
	rt_mq_init(&mq, "mqt", 
		&msg_pool[0], /* 内存池指向msg_pool */
		128 - sizeof(void*), /* 每个消息的大小是 128 - void* */
		sizeof(msg_pool),  /* 内存池的大小是msg_pool的大小 */
		RT_IPC_FLAG_FIFO); /* 如果有多个线程等待，按照FIFO的方法分配消息 */

	tid1 = rt_thread_create("thread1",
			thread1_entry, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
			thread2_entry,   RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY , THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
			rt_thread_startup(tid2);

	tid3 = rt_thread_create("thread3",
			thread3_entry,   RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY , THREAD_TIMESLICE);
	if (tid3 != RT_NULL)
			rt_thread_startup(tid3);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_11, comu test);