#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_regs.h"
#include "../libraries/ls1c_pwm.h"

#include "ledmatrix.h"

#define led2_gpio 53


/**
  * @brief  高中断 关显示 led更新 低中断 开显示
  * @note   该函数产生中断时调用
  * @param  htim : TIM handle
  * @retval None
  */
void pwm_timer_irqhandler(int irq, void *param)
{
    timer_info_t timer_info;
    static unsigned long cnt = 0;
    unsigned int timer_reg_base = 0;

    timer_info.timer = irq - LS1C_PWM0_IRQ;  
    timer_reg_base = LS1C_REG_BASE_PWM0 + 0x10 * timer_info.timer ;
    cnt = reg_read_32((volatile unsigned int *)(timer_reg_base + LS1C_PWM_CNTR));
    timer_int_clr(&timer_info);
    /*进入了低中断*/
    if (cnt>= reg_read_32((volatile unsigned int *)(timer_reg_base + LS1C_PWM_LRC)))
    {    
        gpio_set(led2_gpio, gpio_level_high); //熄灭
        timer_cnt_clr(&timer_info);
        timer_int_clr(&timer_info);
        OE_LED_DIS;
        led_scan();
    }
    else /*进入了高中断*/
    {
        gpio_set(led2_gpio, gpio_level_low ); //点亮
        OE_LED_EN;
    }
}


/* *************************************** */
/* MSH CMD */

/*0.8 ms周期 配置period为800 */
void Timer_Init(int pwm_n, int period, int time_width)
{
    int i;
    pwm_info_t pwm_info;
    timer_info_t timer_info;
    rt_uint32_t ram_addr;
    int pwm_timer_irq = LS1C_PWM0_IRQ + pwm_n; 

    gpio_init(led2_gpio, gpio_mode_output);
    gpio_set(led2_gpio, gpio_level_low );
    rt_thread_delay(10);

    timer_info.timer = pwm_n;  
    timer_info.time_ns = period * 1000;       //133152 us 周期最大 = 133ms
    timer_info.timer = pwm_n;  
    timer_info.time_h_ns = time_width * 1000;       
    timer_int_init(&timer_info, 1, 1); //
    
    rt_hw_interrupt_install(pwm_timer_irq, pwm_timer_irqhandler, RT_NULL, "pwm_timer"); 
    rt_hw_interrupt_umask(pwm_timer_irq); 
    ram_addr = LS1C_REG_BASE_PWM0 +  0x10 * timer_info.timer + LS1C_PWM_LRC;
    rt_kprintf("\r\n LRC 0x%x = 0x%x  \r\n", ram_addr, __REG32(ram_addr));
    ram_addr = LS1C_REG_BASE_PWM0 +  0x10 * timer_info.timer + LS1C_PWM_HRC;
    rt_kprintf("\r\n HRC 0x%x = 0x%x  \r\n", ram_addr, __REG32(ram_addr));

}

void pwm_testint(int argc, char **argv)
{
    rt_uint32_t pwm_n;
    rt_uint32_t period;
    rt_uint32_t time_width;
	pwm_n = strtoul(argv[1], NULL, 0);
    period = strtoul(argv[2], NULL, 0);
    time_width = strtoul(argv[3], NULL, 0);
    rt_kprintf("\r\n timer_int_init"); 
    Timer_Init(pwm_n, period, time_width);   
}
FINSH_FUNCTION_EXPORT(pwm_testint, pwm test);
MSH_CMD_EXPORT(pwm_testint, pwm  pwm_no period  low_width);// pwm 1 800 400


