/*
 * File      : test_led.c
测试led接口在finsh/msh 中运行
1. test_led() / test_led
*/
#include <rtthread.h>
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#define led_gpio 53

/*
 * 测试库中gpio作为输出时的相关接口
 * led闪烁10次
 */
void test_led(void)
{
    int i;

    // 初始化
    rt_kprintf("Init led! \n");
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

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_led, test_led  e.g.test_led());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_led, test led flash );
