/*
file: test_pin.c
测试uart驱动， 在finsh 中运行
1. test_uart(0)   GPIO74,75  uart00 第2 复用
2. test_uart(1)   GPIO74,75  uart01 第2 复用 接收键盘输入后再发出去
3. test_uart(2)   GPIO2,3      uart1 第4 复用
4. test_uart(3)   GPIO36,37  uart2 第2 复用
5. test_uart(4)   GPIO0,1      uart3 第4 复用 
6. test_uart(5)   GPIO58,59  uart4 第5 复用 
7. test_uart(6)   GPIO60,61  uart5 第5 复用  
8. test_uart(7)   GPIO62,63  uart6 第5 复用  
9. test_uart(8)   GPIO64,65  uart7 第5 复用  
10. test_uart(9)   GPIO66,67  uart8 第5 复用  
11. test_uart(10)   GPIO68,69  uart9 第5 复用  
12. test_uart(11)   GPIO70,71  uart10 第5 复用  
13. test_uart(12)   GPIO72,73  uart11 第5 复用  
*/

#include <rtthread.h>
#include <stdlib.h>  
#include <rthw.h>

#include "ls1c.h"
#include <drivers/pin.h>
#include "ls1c_public.h"
#include "ls1c_uart.h"
#include "ls1c_pin.h"  

void test_uart_irqhandler(int IRQn, void *param)
{
    ls1c_uart_t uartx = uart_irqn_to_uartx(IRQn);
    void *uart_base = uart_get_base(uartx);
   unsigned char iir = reg_read_8(uart_base + LS1C_UART_IIR_OFFSET);
    // 判断是否为接收超时或接收到有效数据
    if ((IIR_RXTOUT & iir) || (IIR_RXRDY & iir))
    {
        // 是，则读取数据，并原样发送回去
        while (LSR_RXRDY & reg_read_8(uart_base + LS1C_UART_LSR_OFFSET))
        {
            uart_putc(uartx, reg_read_8(uart_base + LS1C_UART_DAT_OFFSET));
        }
    }    
    return ;
}

void test_uart3_irqhandler(int IRQn, void *param)
{
    char ch;
    ls1c_uart_t uartx = uart_irqn_to_uartx(IRQn);
    void *uart_base = uart_get_base(uartx);
   unsigned char iir = reg_read_8(uart_base + LS1C_UART_IIR_OFFSET);
    // 判断是否为接收超时或接收到有效数据
    if ((IIR_RXTOUT & iir) || (IIR_RXRDY & iir))
    {
        // 是，则读取数据，并原样发送回去
        while (LSR_RXRDY & reg_read_8(uart_base + LS1C_UART_LSR_OFFSET))
        {
            ch = reg_read_8(uart_base + LS1C_UART_DAT_OFFSET);
            uart_putc(uartx, ch);
            uart_putc(LS1C_UART2, ch);
        }
    }    
    return ;
}


void test_uart(rt_uint8_t uart_num)  
{
    // 调试串口信息
    ls1c_uart_info_t uart_info = {0};	
    unsigned int rx_gpio ;
    unsigned int tx_gpio ;
    int dat;
    switch( uart_num)
    {
    case LS1C_UART00:
        rx_gpio = 74;
        tx_gpio = 75;
        pin_set_remap(tx_gpio, PIN_REMAP_SECOND);
        pin_set_remap(rx_gpio, PIN_REMAP_SECOND);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart00 sending string.\r\n");
    break;
    case LS1C_UART01://测试不成功
        reg_set_one_bit((volatile unsigned int *)0xbfd00420, 30);//当 UART_split = 1 时，UART0 分割成两个控制器 UART00 和 UART01
        rx_gpio = 60;
        tx_gpio = 61;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart01 sending string.\r\n");
    break;   
    case LS1C_UART1:
        rx_gpio = 2;
        tx_gpio = 3;
        pin_set_remap(tx_gpio, PIN_REMAP_FOURTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FOURTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = TRUE;  
        uart_init(&uart_info);
        rt_hw_interrupt_umask(LS1C_UART1_IRQ);		
        uart_print((ls1c_uart_t)uart_num, "\r\nThis is uart1 receive string:");
        rt_hw_interrupt_install(LS1C_UART1_IRQ, test_uart_irqhandler, RT_NULL, "UART1");
    break;
    case LS1C_UART2:
        uart_print(uart_num, "\r\nthis is uart2 sending  string.\r\n");
    break;
    case LS1C_UART3:
        rx_gpio = 0;
        tx_gpio = 1;
        pin_set_remap(tx_gpio, PIN_REMAP_FOURTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FOURTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = TRUE;  
        uart_init(&uart_info);
        rt_hw_interrupt_umask(LS1C_UART3_IRQ);		
        uart_print(uart_num, "\r\nThis is uart3 sending string.\r\n");
        rt_hw_interrupt_install(LS1C_UART3_IRQ, test_uart3_irqhandler, RT_NULL, "UART3");
    break;
    case LS1C_UART4:
        rx_gpio = 58;
        tx_gpio = 59;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart4 sending string.\r\n");
    break;
    case LS1C_UART5:
        rx_gpio = 60;
        tx_gpio = 61;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart5 sending string.\r\n");
    break;
    case LS1C_UART6:
        rx_gpio = 62;
        tx_gpio = 63;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart6 sending string.\r\n");
    break;
    case LS1C_UART7:
        rx_gpio = 64;
        tx_gpio = 65;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart7 sending string.\r\n");
    break;
    case LS1C_UART8:
        rx_gpio = 66;
        tx_gpio = 67;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart8 sending string.\r\n");
    break;
    case LS1C_UART9:
        rx_gpio = 68;
        tx_gpio = 69;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart9 sending string.\r\n");
    break;
    case LS1C_UART10:
        rx_gpio = 70;
        tx_gpio = 71;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart10 sending string.\r\n");
    break;
    case LS1C_UART11:
        rx_gpio = 72;
        tx_gpio = 73;
        pin_set_remap(tx_gpio, PIN_REMAP_FIFTH);
        pin_set_remap(rx_gpio, PIN_REMAP_FIFTH);
        uart_info.UARTx = uart_num;
        uart_info.baudrate = 115200;
        uart_info.rx_enable = FALSE;  
        uart_init(&uart_info);
        uart_print(uart_num, "\r\nThis is uart11 sending string.\r\n");
    break;
   default:
    break;
    }
}  

void test_uart_msh(int argc, char** argv)
{
    unsigned int num;
	num = strtoul(argv[1], NULL, 0);
	test_uart(num);
}

 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_uart , test_uart 0-12 e.g.test_uart(0));
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_uart_msh, test_uart_msh 0);
