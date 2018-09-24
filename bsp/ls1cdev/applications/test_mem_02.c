#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 线程1入口*/
static void thread1_entry(void* parameter)
{
    int i;
    char *ptr[20]; /* 用于放置20个分配内存块的指针*/

    /* 对指针清零*/
    for (i = 0; i < 20; i ++) ptr[i] = RT_NULL;

    while(1)
    {
        for (i = 0; i < 20; i++)
        {
            /* 每次分配(1 << i)大小字节数的内存空间*/
            ptr[i] = rt_malloc(1 << i);

            /* 如果分配成功*/
            if (ptr[i] != RT_NULL)
            {
                rt_kprintf("get memory: 0x%x\n", ptr[i]);
                /* 释放内存块*/
                rt_free(ptr[i]);
                ptr[i] = RT_NULL;
            }
        }
    }
}

void test_mem_02(void)
{
	tid1 = rt_thread_create("thread1",
			thread1_entry, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_mem_02, comu test);