/*
 * file: test_rtt_uart.c
 * 基于uart设备框架测试uart驱动， 在finsh 中运行
 
 * 程序清单：串口设备操作例程
 *
 * 在这个例程中，将启动一个devt线程，然后打开串口1和2
 * 当串口1有输入时，将读取其中的输入数据然后写入到
 * 串口1和 串口2设备中。
 *
 */

#include <rtthread.h>
#include <stdlib.h>  
#include <drivers/pin.h>
#include <drivers/serial.h>
#include "../drivers/drv_uart.h"

/* UART接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 用于接收消息的消息队列*/
static struct rt_messagequeue rx_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[64];

/* 数据到达回调函数*/
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    msg.dev = dev;
    msg.size = size;

    /* 发送消息到消息队列中*/
    rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));

    return RT_EOK;
}

static void device_thread_entry(void* parameter)
{
    struct rx_msg msg;
    int count = 0;
    rt_device_t device, write_device;
    rt_err_t result = RT_EOK;

	result = rt_mq_init(&rx_mq, "rx_mq", &msg, sizeof(struct rx_msg), sizeof(struct rx_msg)*20, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK) 
	{ 
	  rt_kprintf("init message queue failed.\n"); 
	} 
	
    /* 查找系统中的串口1设备 */
    device = rt_device_find("uart1");
	write_device = device;
    if (device!= RT_NULL)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(device, uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }

    while (1)
    {
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(struct rx_msg), 1000);
        if (result == -RT_ETIMEOUT)
        {
            /* 接收超时*/
            rt_kprintf("\r\ntimeout count:%d\n", ++count);
        }

        /* 成功收到消息*/
        if (result == RT_EOK)
        {
            rt_uint32_t rx_length;
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ?
                    msg.size : sizeof(uart_rx_buffer) - 1;

            /* 读取消息*/
            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
                    rx_length);
            uart_rx_buffer[rx_length] = '\0';

            /* 写到写设备中*/
            if (write_device != RT_NULL)
                rt_device_write(write_device, 0, &uart_rx_buffer[0],
                        rx_length);
            rt_kprintf("\r\n uart1 rev %d bytes: %s \r\n", rx_length,  &uart_rx_buffer[0]);
        }
    }
}

void test_rtt_uart(void)
{

    /* 创建devt线程*/
    rt_thread_t thread = rt_thread_create("devt",
        device_thread_entry, RT_NULL,
        1024, 25, 7);
    /* 创建成功则启动线程*/
    if (thread!= RT_NULL)
        rt_thread_startup(thread);}

 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_rtt_uart , test uart 1-2 e.g.test_rtt_uart());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_uart, test_rtt_uart);
