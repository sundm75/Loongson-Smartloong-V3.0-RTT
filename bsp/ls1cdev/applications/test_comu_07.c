#include <rthw.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;
static rt_mutex_t mutex = RT_NULL;

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
        /* 先让低优先级线程运行 */
        rt_thread_delay(10);

        /* 此时thread3持有mutex，并且thread2等待持有mutex */

        /* 检查thread2与thread3的优先级情况 */
        if (tid2->current_priority != tid3->current_priority)
        {
            /* 优先级不相同，测试失败 */
            rt_kprintf("\r\ntest1 failed！\r\n");
            return;
        }
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
{
        rt_err_t result;

        /* 先让低优先级线程运行 */
        rt_thread_delay(5);

        while (1)
        {
                /*
                 * 试图持有互斥锁，此时thread3持有，应把thread3的优先级提升
                 * 到thread2相同的优先级
                 */
                result = rt_mutex_take(mutex, RT_WAITING_FOREVER);

                if (result == RT_EOK)
                {
                        /* 释放互斥锁 */
                        rt_mutex_release(mutex);
                }
        }
}

/* 线程3入口 */
static void thread3_entry(void* parameter)
{
        rt_tick_t tick;
        rt_err_t result;

        while (1)
        {
                result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
                result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
                if (result != RT_EOK)
                {
                    rt_kprintf("\r\ntest3 failed！\r\n");
                    return ;
                }

                /* 做一个长时间的循环，总共50个OS Tick */
                tick = rt_tick_get();
                while (rt_tick_get() - tick < 50) ;

                rt_mutex_release(mutex);
                rt_mutex_release(mutex);
        }
}

void test_comu_07(void)
{
    /* 创建互斥锁 */
    mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
    if (mutex == RT_NULL)
    {
      rt_kprintf("\r\ntest0 failed！\r\n");  
			return ;
    }

    tid1 = rt_thread_create("t1",
        thread1_entry, RT_NULL, 
        THREAD_STACK_SIZE, THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    tid2 = rt_thread_create("t2",
        thread2_entry, RT_NULL, 
        THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    tid3 = rt_thread_create("t3",
        thread3_entry,  RT_NULL, 
        THREAD_STACK_SIZE, THREAD_PRIORITY + 1, THREAD_TIMESLICE);
    if (tid3 != RT_NULL)
        rt_thread_startup(tid3);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_07, comu test);