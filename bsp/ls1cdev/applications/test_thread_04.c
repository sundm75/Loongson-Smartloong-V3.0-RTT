#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/*  静态线程1 的对象和运行时用到的栈 */  
static struct rt_thread thread1;
static rt_uint8_t thread1_stack[THREAD_STACK_SIZE]; 

/*  动态线程2 的对象 */  
static rt_thread_t thread2 = RT_NULL;

/*  线程 1入口  */  
static void thread1_entry (void* parameter) 
{ 
    int  count = 0; 

    while (1) 
    { 
				rt_kprintf( "%d\n" , ++count);

				/*  延时100 个OS Tick */  
				rt_thread_delay(100); 
    } 
} 

/*  线程 2入口  */  
static void thread2_entry (void* parameter) 
{ 
    int  count = 0; 
    while (1) 
    { 
        rt_kprintf( "Thread2 count:%d\n" , ++count); 

        /*  延时50个OS Tick */  
        rt_thread_delay(50); 
    } 
}

void test_thread_04(void)
{
	rt_err_t result; 

	/*  初始化线程1 */ 
	/*  线程的入口是thread1_entry ，参数是RT_NULL 
	 *  线程栈是thread1_stack 栈空间是512 ，  
	 *  优先级是25 ，时间片是5个OS Tick 
	 */  
	result = rt_thread_init(&thread1, "thread1", 
			thread1_entry, RT_NULL, 
			&thread1_stack[0], sizeof(thread1_stack), 
			THREAD_PRIORITY, THREAD_TIMESLICE); 

	/*  启动线程1 */  
	if (result == RT_EOK) rt_thread_startup(&thread1); 

	/*  创建线程2 */ 
	/*  线程的入口是thread2_entry,  参数是RT_NULL 
	 *  栈空间是512 ，优先级是25 ，时间片是5个OS Tick 
	 */  
	thread2 = rt_thread_create( "thread2", thread2_entry, RT_NULL, 
			THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE); 

	/*  启动线程2 */  
	if (thread2 != RT_NULL) rt_thread_startup(thread2);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_04, thread test);