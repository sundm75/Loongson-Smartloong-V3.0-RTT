#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_uint8_t *ptr[48];
static rt_uint8_t mempool[4096];
static struct rt_mempool mp; /* 静态内存池对象 */

/* 线程 1 入口 */
static void thread1_entry(void* parameter)
{
    int i;
    char *block;
    while(1)
    {
        for (i = 0; i < 48; i++)
        {
            /* 申请内存块 */
            rt_kprintf("allocate No.%d\n", i);
            if (ptr[i] == RT_NULL)
            {
                ptr[i] = rt_mp_alloc(&mp, RT_WAITING_FOREVER);
            }
        }

        /* 继续申请一个内存块，因为已经没有内存块，线程应该被挂起 */
        block = rt_mp_alloc(&mp, RT_WAITING_FOREVER);
        rt_kprintf("allocate the block mem\n");
        /* 释放这个内存块 */
        rt_mp_free(block);
        block = RT_NULL;
    }
}

/* 线程 2 入口*/
static void thread2_entry(void *parameter)
{
    int i;
    while(1)
    {
        rt_kprintf("try to release block\n");
        for (i = 0 ; i < 48; i ++)
        {
            /* 释放所有分配成功的内存块 */
            if (ptr[i] != RT_NULL)
        {
            rt_kprintf("release block %d\n", i);
            rt_mp_free(ptr[i]);
            ptr[i] = RT_NULL;
            }
        }

        /* 休眠 10 个 OS Tick */
        rt_thread_delay(10);
    }
}

void test_mem_01(void)
{
	int i;
	for (i = 0; i < 48; i ++) ptr[i] = RT_NULL;
	/* 初始化内存池对象 */
	rt_mp_init(&mp, "mp1", &mempool[0], sizeof(mempool), 80);

	tid1 = rt_thread_create("thread1",
			thread1_entry, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
			thread2_entry,   RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY + 1, THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
			rt_thread_startup(tid2);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_mem_01, comu test);