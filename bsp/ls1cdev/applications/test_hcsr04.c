/*
 * File      : test_hcsr04.c
测试test_hcsr04, 在 msh  中运行 test_hcsr04 
 2 * 距离 = 时间 * 速度 = 纳秒 * 340*1000 cm/ns
由 test_key修改而来
 */
#include <rtthread.h>  
#include <stdlib.h>  
#include <rthw.h>

#include "ls1c.h"  
#include "ls1c_timer.h"  
#include "ls1c_public.h"  
#include "ls1c_gpio.h"  
#include "mipsregs.h"  
#include "ls1c_pin.h"
// 测试用的线程    
#define THREAD_TEST_PRIORITY                    (25)    
#define THREAD_TEST_STACK_SIZE                  (4*1024)        // 4k    
#define THREAD_TEST_TIMESLICE                   (10)    

#define trig_gpio 50
#define echo_gpio 51


#define MY_PRINTF(value) \
	if(value>=0)\
	rt_kprintf (" %d.%04d", (int)value,(int)((value- (int)value)*10000));\
	if(value<0)\
	rt_kprintf (" -%d.%04d", (-1)*(int)value,(-1)*(int)((value- (int)value)*10000));
   
struct rt_thread thread_test_hcsr04;  
void ls1c_test_echo_down_irqhandler(int irq, void *param) ;
void ls1c_test_echo_up_irqhandler(int irq, void *param) ;
rt_uint8_t thread_testkeydownup_stack[THREAD_TEST_STACK_SIZE]; 
int echo_irq ; 
float distance;

timer_info_t timer_info ;
unsigned long timer_echo_rising,timer_echo_falling;

void ls1c_test_echo_up_irqhandler(int irq, void *param)  
{  
    if(1 == gpio_get(echo_gpio))
    {
        rt_hw_interrupt_mask(echo_irq);
        timer_echo_rising=timer_get_time_ns(&timer_info);
        gpio_set_irq_type(echo_gpio, IRQ_TYPE_EDGE_FALLING);
        rt_hw_interrupt_install(echo_irq, ls1c_test_echo_down_irqhandler, RT_NULL, "echo");    
        rt_hw_interrupt_umask(echo_irq);  
        
    }        
}  

  
void ls1c_test_echo_down_irqhandler(int irq, void *param)  
{  
    if(0 == gpio_get(echo_gpio))
    {
        rt_hw_interrupt_mask(echo_irq);
        timer_echo_falling=timer_get_time_ns(&timer_info); 
        timer_stop(&timer_info);

		distance=((double)(timer_echo_falling-timer_echo_rising)*170)/10000000; //cm
        rt_kprintf("\r\nt= %d ns, distance= " , timer_echo_falling-timer_echo_rising);
        MY_PRINTF(distance);
        rt_kprintf("cm\r\n " );
    }        
}  

// 测试用的线程的入口    
void thread_test_echo_down_up_entry(void *parameter)    
{  
 
    echo_irq = LS1C_GPIO_TO_IRQ(echo_gpio);

        pin_set_purpose(echo_gpio, PIN_PURPOSE_GPIO);
        pin_set_remap(echo_gpio, PIN_REMAP_DEFAULT);
        pin_set_purpose(trig_gpio, PIN_PURPOSE_GPIO);
        pin_set_remap(trig_gpio, PIN_REMAP_DEFAULT);

        // 初始化 echo 中断  
        gpio_set_irq_type(echo_gpio, IRQ_TYPE_EDGE_RISING);  
        rt_hw_interrupt_install(echo_irq, ls1c_test_echo_up_irqhandler, RT_NULL, "echo");  
        rt_hw_interrupt_umask(echo_irq);  
        gpio_init(echo_gpio, gpio_mode_input);  
        rt_kprintf("\r\necho_irq = %d \r\n", echo_irq);
        
        // 初始化 trig
        gpio_init(trig_gpio, gpio_mode_output);
        gpio_set(trig_gpio, gpio_level_low);    

        
        timer_info.timer=TIMER_PWM0;
        timer_info.time_ns=100*1000*1000;
        timer_init(&timer_info);    
                
        {  
            gpio_set(trig_gpio, gpio_level_high);         
            rt_thread_delay(1);    
        } 
        gpio_set(trig_gpio, gpio_level_low);    
}    

  
int test_hcsr04(void)  
{  
    rt_thread_t tid;  
    rt_err_t result;  
    // 初始化测试用的线程    
    result = rt_thread_init(&thread_test_hcsr04,     
                            "test_hcsr04",    
                            thread_test_echo_down_up_entry,    
                            RT_NULL,    
                            &thread_testkeydownup_stack[0],    
                            sizeof(thread_testkeydownup_stack),    
                            THREAD_TEST_PRIORITY,    
                            THREAD_TEST_TIMESLICE);    
    if (RT_EOK == result)    
    {    
        rt_thread_startup(&thread_test_hcsr04);    
    }    
    else    
    {    
        return -1;    
    }    
  
    return 0;  
}  
 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_hcsr04, test_hcsr04  e.g.test_hcsr04());
MSH_CMD_EXPORT(test_hcsr04, test_hcsr04  e.g.test_hcsr04());