/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *

*/

#include <rtthread.h>

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/*定义共享变量*/
static int share_var;

static void thread1_entry(void* parameter)
{
    int i;
    share_var = 0;
    rt_kprintf("share_var = %d\n", share_var);
    for(i=0; i<100000; i++)
    {
        share_var ++;
    }
    rt_kprintf("\r\nshare_var = %d\n", share_var);
}

static void thread2_entry(void* parameter)
{
    /*延时修改为1000后，就不会打断线程1的share_var累加*/
    rt_thread_delay(1);
    share_var ++;
}

void test_comu_01(void)
{
	tid1 = rt_thread_create("thread1",
		thread1_entry, RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
		thread2_entry,   RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY - 1 , THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_comu_01, comu test);