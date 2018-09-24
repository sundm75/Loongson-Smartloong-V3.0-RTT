#include <rthw.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5   

/* 指向生产者和消费者线程控制块的指针 */
static rt_thread_t producer_tid = RT_NULL;
static rt_thread_t consumer_tid = RT_NULL;

/* 定义能够产生的最大元素个数为5 */
#define MAXSEM     5

/* 用于放置生产的整数数组 */
rt_uint32_t array[MAXSEM];

/* 指向生产者、消费者在array数组中的读写位置 */
static rt_uint32_t set, get;

struct rt_semaphore sem_lock;
struct rt_semaphore sem_empty, sem_full;

/* 生产者线程入口 */
static void producer_thread_entry(void* parameter)
{
    rt_int32_t cnt = 0;

    /* 运行100次 */
    while( cnt < 100)
    {
        /* 获取一个空位 */
        rt_sem_take(&sem_empty, RT_WAITING_FOREVER);

        /* 修改array内容，上锁 */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        array[set%MAXSEM] = cnt + 1;
        rt_kprintf("the producer generates a number: %d\n", 
            array[set%MAXSEM]);
        set++;
        rt_sem_release(&sem_lock);

        /* 发布一个满位 */
        rt_sem_release(&sem_full);
        cnt++;

        /* 暂停一段时间 */
        rt_thread_delay(50);
    }

    rt_kprintf("the producer exit!\n");
}

/* 消费者线程入口 */
static void consumer_thread_entry(void* parameter)
{
    rt_uint32_t no;
    rt_uint32_t sum = 0;

    /* 第n个线程，由入口参数传进来 */
    no = (rt_uint32_t)parameter;

    while(1)
    {
        /* 获取一个满位 */
        rt_sem_take(&sem_full, RT_WAITING_FOREVER);

        /* 临界区，上锁进行操作 */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        sum += array[get%MAXSEM];
        rt_kprintf("the consumer[%d] get a number:%d\n", no, array[get%MAXSEM]);
        get++;
        rt_sem_release(&sem_lock);

        /* 释放一个空位 */
        rt_sem_release(&sem_empty);

        /* 生产者生产到100个数目时，消费者线程相应停止 */
        if (get == 100) break;

        /* 暂停一小会时间 */
        rt_thread_delay(10);
    }

    rt_kprintf("the consumer[%d] sum is %d \n ", no, sum);
    rt_kprintf("the consumer[%d] exit!\n");
}

void test_comu_05(void)
{
	/* 创建生产者线程 */
	producer_tid = rt_thread_create("producer",
		producer_thread_entry, /* 线程入口是producer_thread_entry */
		RT_NULL, /* 入口参数是RT_NULL */
		THREAD_STACK_SIZE, THREAD_PRIORITY - 1, THREAD_TIMESLICE);
	if (producer_tid != RT_NULL)
		rt_thread_startup(producer_tid);

	/* 创建消费者线程 */
	consumer_tid = rt_thread_create("consumer",
	   consumer_thread_entry,/* 线程入口是consumer_thread_entry */
		RT_NULL, /* 入口参数是RT_NULL */
		THREAD_STACK_SIZE, THREAD_PRIORITY + 1, THREAD_TIMESLICE);
	if (consumer_tid != RT_NULL)
		rt_thread_startup(consumer_tid);

	/* 初始化3个信号量 */
	rt_sem_init(&sem_lock , "lock",   1,    RT_IPC_FLAG_FIFO);
	rt_sem_init(&sem_empty,"empty", MAXSEM, RT_IPC_FLAG_FIFO);
	rt_sem_init(&sem_full , "full",   0,   RT_IPC_FLAG_FIFO);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_05, comu test);