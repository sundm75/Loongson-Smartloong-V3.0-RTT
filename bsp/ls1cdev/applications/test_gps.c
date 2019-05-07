/*
file: test_gps.c
使用 AT ESP8266模块联网，测试 gps 模块的数据上传， 在finsh 中运行
1. AT ESP8266模块 连接到 UART1, GPIO2、GPIO3。
2. GPS模块 连接到 UART3， GPIO0、GPIO1。
3. 在 finsh 中运行命令 gps_net <ip address> <port> ，
ip address 为要连接的IP地址，port 为端口号，
联网成功后，在串口收到数据后向该IP地址发送GPS的数据。
*/
#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#include <rthw.h>

#define BUFSZ   1024

/* 全局变量 */
static int send_data_flag = 0;
static int send_length = 0;

static char send_data[128] = "This is TCP Client from RT-Thread."; /* 发送用到的数据 */

#include <rtthread.h>
#include <stdlib.h>  
#include <rthw.h>

#include "ls1c.h"
#include <drivers/pin.h>
#include "ls1c_public.h"
#include "ls1c_uart.h"
#include "ls1c_pin.h"  

void uart3_irqhandler(int IRQn, void *param)
{
    char ch;
    ls1c_uart_t uartx = uart_irqn_to_uartx(IRQn);
    void *uart_base = uart_get_base(uartx);
   unsigned char iir = reg_read_8(uart_base + LS1C_UART_IIR_OFFSET);
    // 判断是否为接收超时或接收到有效数据
    if ((IIR_RXTOUT & iir) || (IIR_RXRDY & iir))
    {
        while (LSR_RXRDY & reg_read_8(uart_base + LS1C_UART_LSR_OFFSET))
        {
            ch = reg_read_8(uart_base + LS1C_UART_DAT_OFFSET);
            send_data[send_length++] = ch;
            if((send_data[send_length-2]==0x0D) && (send_data[send_length-1]==0x0A))
            {
                rt_kprintf(send_data);
                while(send_data_flag == 2) //正在发送数据时不能修改
                rt_memset(send_data, 0x00, sizeof(send_data));
                send_length = 0;
                send_data_flag = 1; // 写完数据
            }
        }
    }    
    return ;
}

void gps_uart(void)  
{
    ls1c_uart_info_t uart_info = {0};	
    rt_err_t ret = RT_EOK;

    uart_info.UARTx = 4; //配置 UART3
    uart_info.baudrate = 115200;
    uart_info.rx_enable = TRUE;  
    uart_init(&uart_info);
    rt_hw_interrupt_install(LS1C_UART3_IRQ, uart3_irqhandler, RT_NULL, "UART3");
    rt_hw_interrupt_umask(LS1C_UART3_IRQ);		

}  

void gps_net(int argc, char **argv)
{
    int ret;
    struct hostent *host;
    int sock, bytes_received;
    struct sockaddr_in server_addr;
    const char *url;
    int port;
    rt_err_t result;

    if (argc < 3)
    {
        rt_kprintf("Usage: tcpclient URL PORT\n");
        rt_kprintf("Like: tcpclient 192.168.12.44 5000\n");
        return ;
    }

    url = argv[1];
    port = strtoul(argv[2], 0, 10);

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(url);

    /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建socket失败 */
        rt_kprintf("Socket error\n");

        return;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 连接到服务端 */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 连接失败 */
        rt_kprintf("Connect fail!\n");
        closesocket(sock);

        return;
    }
    else
    {
        /* 连接成功 */
        rt_kprintf("Connect successful\n");
        gps_uart();
    }

    while (1)
    {
        
        if (send_data_flag ==1 )
        {
            send_data_flag = 2; //正在发送数据
            send_data_flag = 0; // 数据空

            /* 发送数据到sock连接 */
            ret = send(sock, send_data, strlen(send_data), 0);
            if (ret < 0)
            {
                /* 接收失败，关闭这个连接 */
                closesocket(sock);
                rt_kprintf("\nsend error,close the socket.\r\n");

                break;
            }
            else if (ret == 0)
            {
                /* 打印send函数返回值为0的警告信息 */
                rt_kprintf("\n Send warning,send function return 0.\r\n");
            }
            rt_kprintf("send successful\n");

        }
        else
            rt_thread_delay(RT_TICK_PER_SECOND/100);
    }
    return;
}
 #include  <finsh.h> 
MSH_CMD_EXPORT(gps_net, tcp client gps data send);


