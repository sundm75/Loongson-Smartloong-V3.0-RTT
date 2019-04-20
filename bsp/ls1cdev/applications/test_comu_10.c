#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    unsigned char* str;

    while (1)
    {
        rt_kprintf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&mb, (rt_ubase_t*)&str, RT_WAITING_FOREVER)
                == RT_EOK)
        {
            /* 显示邮箱内容 */
            rt_kprintf("thread1: get a mail, the content:%s\n", str);

            /* 延时10个OS Tick */
            rt_thread_delay(10);
        }
    }
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
{
    rt_uint8_t count;

    count = 0;
    while (1)
    {
        count ++;
         /*轮流发送2个字符串的地址*/
        if (count & 0x1)
        {
            /* 发送mb_str1地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str1[0]);
        }
        else
        {
            /* 发送mb_str2地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str2[0]);
        }

        /* 延时20个OS Tick */
        rt_thread_delay(20);
    }
}

void test_comu_10(void)
{

    /* 初始化一个mailbox */
    rt_mb_init(&mb,
        "mbt",             /* 名称是mbt */
        &mb_pool[0],       /* 邮箱用到的内存池是mb_pool */
        sizeof(mb_pool)/4, /* 大小是mb_pool/4，因为每封邮件的大小是4字节 */
        RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */

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
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_10, comu test);