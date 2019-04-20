/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * File      : test_ ringbuffer.c
 在finsh 或 msh中运行 test_ringbuffer
 1. 先初始化一段ringbuffer，初始化信号量sem 并启动2个线程 。
 2. 线程1写入环形缓冲区，线程2从环形缓冲区中读取数据。
*/ 

 
#include <rtthread.h>
#include <rtdevice.h>

#define CYCLE_TIMES 2
#define RINGBUFER_LENGTH 256

static rt_sem_t sem = RT_NULL;
static rt_uint8_t working_buffer[RINGBUFER_LENGTH];
struct rt_ringbuffer rb;

static void thread1_entry(void *parameter)
{
    rt_bool_t result;
    rt_uint32_t index,setchar;
    rt_uint32_t i = CYCLE_TIMES;
    rt_uint8_t data_buffer[33];
    setchar = 0x21;
    while(i--)
    {
        for (index = 0; index < 32; index++)
        {
            data_buffer[index] = setchar;
            if (++setchar == 0x7f)
            {
                setchar = 0x21;
            }
        }
        data_buffer[32] = '\0';
        result = rt_ringbuffer_put(&rb,&data_buffer[0],33);
        rt_sem_release(sem);
        rt_kprintf("write buffer success!\n");
    }
}

static void thread2_entry(void* parameter)
{
    rt_bool_t result;
    rt_uint8_t data_buffer[33];
    rt_uint32_t i = CYCLE_TIMES;
    while(i--)
    {
        rt_sem_take (sem,RT_WAITING_FOREVER);
        result = rt_ringbuffer_get(&rb,&data_buffer[0],33);
        rt_sem_release(sem);
        rt_kprintf("%s\n",data_buffer);
    }
    rt_sem_delete(sem); 
}

void test_ringbuffer(void)
{
   rt_thread_t thread1, thread2;
	/*  创建一个动态信号量，初始值是 0 */ 
	sem = rt_sem_create("sem", 0, RT_IPC_FLAG_FIFO); 
	if (sem == RT_NULL) 
	{ 
			rt_kprintf("create dynamic semaphore failed.\n"); 
			return ; 
	}
    rt_ringbuffer_init(&rb,working_buffer,RINGBUFER_LENGTH);

    thread1 = rt_thread_create("ring_buf1", thread1_entry, RT_NULL,
        1024, 20, 200);
    if (thread1 != RT_NULL)
        rt_thread_startup(thread1);

    thread2 = rt_thread_create("ring_buf2", thread2_entry, RT_NULL,
        1024, 21, 200);
    if (thread2 != RT_NULL)
        rt_thread_startup(thread2);
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_ringbuffer, test_ringbuffer e.g.test_ringbuffer())
MSH_CMD_EXPORT(test_ringbuffer, test_ringbuffer);