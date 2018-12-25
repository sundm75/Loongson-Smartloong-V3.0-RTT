/*
 * File      : test_delay.c
测试延时函数， 在finsh 中运行 
1. test_delay_1ms()
2. test_delay_1us()
3. test_delay_1s()
4. print_clk()  将当前的分频的所有时钟都打印出来
5. test_rtdelay_1s 调用RT_TICK_PER_SECOND实现1s 延时测试
6. mem_read(***) 读取***寄存器中32位数据并打印
 */

#include "rtthread.h"
#include "ls1c.h"
#include <stdlib.h>  
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_clock.h"

#define led_gpio 52

// 测试延时函数delay_1ms()
void test_delay_1ms(void)
{
    int time = 0;

    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);

    // 产生不同宽度的高低电平，用示波器观察高低电平宽度是否正确
    {
        // 2ms
        time = 2/2;
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_high);

        // 10ms
        time = 10/2;
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_high);

        // 100ms
        time = 100/2;
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_high);

        // 500ms
        time = 500/2;
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_ms(time);
        gpio_set(led_gpio, gpio_level_high);        
    }
}



// 测试延时函数delay_1us()
void test_delay_1us(void)
{
    int time,i;

    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);

    // 产生不同宽度的高低电平，用示波器观察高低电平宽度是否正确
    for(i = 0; i<100; i++)
    {
        // 2us
        time = 2/2;
        delay_us(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_us(time);
        gpio_set(led_gpio, gpio_level_high);

        // 10us
        time = 10/2;
        delay_us(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_us(time);
        gpio_set(led_gpio, gpio_level_high);

        // 50us
        time = 50/2;
        delay_us(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_us(time);
        gpio_set(led_gpio, gpio_level_high);

        // 100us
        time = 100/2;
        delay_us(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_us(time);
        gpio_set(led_gpio, gpio_level_high);

        // 500us
        time = 500/2;
        delay_us(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_us(time);
        gpio_set(led_gpio, gpio_level_high);
    }
}


// 测试延时函数delay_1s()
void test_delay_1s(void)
{
    int time;

    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);

    {
        // 2s
        time = 2/2;
        delay_s(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_s(time);
        gpio_set(led_gpio, gpio_level_high);

        time = 2/2;
        delay_s(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_s(time);
        gpio_set(led_gpio, gpio_level_high);

        time = 2/2;
        delay_s(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_s(time);
        gpio_set(led_gpio, gpio_level_high);

        time = 2/2;
        delay_s(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_s(time);
        gpio_set(led_gpio, gpio_level_high);

        time = 2/2;
        delay_s(time);
        gpio_set(led_gpio, gpio_level_low);
        delay_s(time);
        gpio_set(led_gpio, gpio_level_high);
    }
}

void print_clk(void)
{
    rt_kprintf("current pll: %d \n", clk_get_pll_rate());
    rt_kprintf("current cpu_rate: %d \n", clk_get_cpu_rate());
    rt_kprintf("current addr_rate: %d \n", clk_get_ddr_rate());
    rt_kprintf("current apb_rate: %d \n", clk_get_apb_rate());
    rt_kprintf("current dc_rate: %d \n", clk_get_dc_rate());
}

void test_rtdelay_1s(void)
{
    int time;

    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);

    while (1)
    {
        // 2s
        time = 2/2;
        rt_thread_delay(RT_TICK_PER_SECOND);
        gpio_set(led_gpio, gpio_level_low);
        rt_thread_delay(RT_TICK_PER_SECOND);
        gpio_set(led_gpio, gpio_level_high);
     }
}

void mem_read(rt_uint32_t ram_addr)
{
    rt_kprintf("\r\n 0x%x = 0x%x   \r\n", ram_addr, __REG32(ram_addr));
}

void mem_read_msh(int argc, char** argv)
{
    rt_uint32_t ram_addr;
	ram_addr = strtoul(argv[1], NULL, 0);
	rt_kprintf("\r\n 0x%x = 0x%x   \r\n", ram_addr, __REG32(ram_addr));
}

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif
FINSH_FUNCTION_EXPORT(test_delay_1ms, test_delay_1ms  e.g.test_delay_1ms());
FINSH_FUNCTION_EXPORT(test_delay_1us, test_delay_1us  e.g.test_delay_1us());
FINSH_FUNCTION_EXPORT(test_delay_1s, test_delay_1s  e.g.test_delay_1s());
FINSH_FUNCTION_EXPORT(test_rtdelay_1s, test_rtdelay_1s  e.g.test_rtdelay_1s());
FINSH_FUNCTION_EXPORT(print_clk, print_clk  e.g.print_clk());
FINSH_FUNCTION_EXPORT(mem_read, mem read e.g. mem_read(0xbfe78030));
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_delay_1ms, test_delay_1ms );
MSH_CMD_EXPORT(test_delay_1us, test_delay_1us  );
MSH_CMD_EXPORT(test_delay_1s, test_delay_1s  );
MSH_CMD_EXPORT(test_rtdelay_1s, test_rtdelay_1s  );
MSH_CMD_EXPORT(print_clk, print_clk );
MSH_CMD_EXPORT(mem_read_msh, mem_read_msh   0xbfe78030);




