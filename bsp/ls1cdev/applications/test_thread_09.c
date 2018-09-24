#include <rtthread.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

 /* 线程1入口*/ 
static void thread1_entry(void* parameter) 
{
  rt_uint32_t count ; 
 for(count = 0;count<4;count++) 
 { 
        rt_thread_delay(RT_TICK_PER_SECOND*3); 
        rt_kprintf("count = %d\n", count); 
 } 
} 

 /* 线程2入口*/
static void thread2_entry(void* parameter) 
{ 
    rt_tick_t tick; 
    rt_uint32_t i; 

    for(i=0; i<10 ; ++i) 
    { 
        tick = rt_tick_get(); 
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("tick = %d\n",tick++); 
    } 
} 

void test_thread_09(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, RT_NULL, 
		THREAD_STACK_SIZE, THREAD_PRIORITY - 1, THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
		thread2_entry, RT_NULL,
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_09, thread test);