/*
 * File      : test_gpio.c
测试gpio接口在finsh中运行
1. test_output()
2. test_delay_1us()
3. test_delay_1s()
4. print_clk()  将当前的分频的所有时钟都打印出来
5. test_rtdelay_1s 调用RT_TICK_PER_SECOND实现1s 延时测试
6. mem_read(***) 读取***寄存器中32位数据并打印
 */


#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#define led_gpio 52
#define key_gpio 85

/*
 * 测试库中gpio作为输出时的相关接口
 * led闪烁10次
 */
void test_output(void)
{
    int i;

    // 初始化
    rt_kprintf("Init gpio! \n");
    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);       // 指示灯默认熄灭

    // 输出10个矩形波，如果gpio50上有led，则可以看见led闪烁10次
    for (i=0; i<10; i++)
    {
        gpio_set(led_gpio, gpio_level_low);
        delay_ms(500);
        gpio_set(led_gpio, gpio_level_high);
        delay_ms(500);
    rt_kprintf("current time: %d \n", i);
    }
    return ;
}

/*
 * 测试库中gpio作为输入时的相关接口
 * 按键按下时，指示灯点亮，否则，熄灭
 */
void test_input(void)
{

    // 初始化
    gpio_init(led_gpio, gpio_mode_output);
    gpio_init(key_gpio, gpio_mode_input);
    gpio_set(led_gpio, gpio_level_high);        // 指示灯默认熄灭

    while (1)
    {
        if (gpio_level_low != gpio_get(key_gpio))
            continue;       // 按键没有按下

        // 延时(软件消抖)后再次确认按键是否按下
        delay_ms(10);
        if (gpio_level_low != gpio_get(key_gpio))
            continue;       // 按键没有按下

        // 点亮指示灯
        gpio_set(led_gpio, gpio_level_low);

        // 等待释放按键
        while (gpio_level_high != gpio_get(key_gpio))
            ;
        delay_ms(10);

        // 熄灭指示灯
        gpio_set(led_gpio, gpio_level_high);
    }
}

 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_output, test_output  e.g.test_output());
FINSH_FUNCTION_EXPORT(test_input, test_input  e.g.test_input());

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_output, gpio output  sample);
MSH_CMD_EXPORT(test_input, gpio input sample);

