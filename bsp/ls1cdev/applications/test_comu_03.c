#include <rthw.h>

static rt_thread_t tid1 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/*  静态信号量控制块 */
static struct rt_semaphore static_sem; 
/*  指向动态信号量的指针 */ 
static rt_sem_t dynamic_sem = RT_NULL;

static void thread1_entry(void* parameter) 
{ 
    rt_err_t result; 
    rt_tick_t tick; 

    /* 1. staic semaphore demo */ 
    /*  获得当前的OS Tick */ 
    tick = rt_tick_get(); 

    /*  试图持有信号量，最大等待 10个OS Tick后返回 */ 
    result = rt_sem_take(&static_sem, 10); 
    if (result == -RT_ETIMEOUT) 
    { 
        /*  超时后判断是否刚好是 10个OS Tick */ 
        if (rt_tick_get() - tick != 10) 
        { 
            rt_sem_detach(&static_sem); 
            return; 
        } 
        rt_kprintf("take semaphore timeout\n"); 
    } 
    else 
    {  /*  因为没有其他地方释放信号量，所以不应该成功持有信号量 */ 
        rt_kprintf("take a static semaphore, failed.\n"); 
        rt_sem_detach(&static_sem); 
        return; 
    } 
  /*  释放一次信号量 */ 
    rt_sem_release(&static_sem); 

    /*  永久等待方式持有信号量 */ 
    result = rt_sem_take(&static_sem, RT_WAITING_FOREVER); 
    if (result != RT_EOK) 
    { 
        /*  不成功则测试失败 */ 
        rt_kprintf("take a static semaphore, failed.\n"); 
        rt_sem_detach(&static_sem); 
        return; 
    } 

    rt_kprintf("take a staic semaphore, done.\n"); 

    /*  脱离信号量对象 */ 
    rt_sem_detach(&static_sem);
    tick = rt_tick_get(); 

    /*  试图持有信号量，最大等待 10个OS Tick后返回 */ 
    result = rt_sem_take(dynamic_sem, 10); 
    if (result == -RT_ETIMEOUT) 
    { 
        /*  超时后判断是否刚好是 10个OS Tick */ 
        if (rt_tick_get() - tick != 10) 
        { 
            rt_sem_delete(dynamic_sem); 
            return; 
        } 
        rt_kprintf("take semaphore timeout\n"); 
    } 
    else 
    { 
        /*  因为没有其他地方释放信号量，所以不应该成功持有信号量，否则测试失败*/ 
        rt_kprintf("take a dynamic semaphore, failed.\n"); 
        rt_sem_delete(dynamic_sem); 
        return; 
        }  /*  释放一次信号量 */ 
    rt_sem_release(dynamic_sem); 

    /*  永久等待方式持有信号量 */ 
    result = rt_sem_take(dynamic_sem, RT_WAITING_FOREVER); 
    if (result != RT_EOK) 
    { 
        /*  不成功则测试失败 */ 
        rt_kprintf("take a dynamic semaphore, failed.\n"); 
        rt_sem_delete(dynamic_sem); 
        return; 
    } 

    rt_kprintf("take a dynamic semaphore, done.\n"); 
    /*  删除信号量对象 */ 
    rt_sem_delete(dynamic_sem); 
} 
void test_comu_03(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, (void*)10,       
		THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	rt_err_t result;    
	/*  初始化静态信号量，初始值是 0 */ 
	result = rt_sem_init(&static_sem, "ssem", 0, RT_IPC_FLAG_FIFO); 
	if (result != RT_EOK) 
	{ 
			rt_kprintf("init dynamic semaphore failed.\n"); 
			return ; 
	} 
	/*  创建一个动态信号量，初始值是 0 */ 
	dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO); 
	if (dynamic_sem == RT_NULL) 
	{ 
			rt_kprintf("create dynamic semaphore failed.\n"); 
			return ; 
	}
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_03, comu test);