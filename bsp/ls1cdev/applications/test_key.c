/*
 * File      : test_key.c
测试key接口在finsh中运行
1. test_key   
首先初始化打开按键中断，启动线程不断检测标志位；
按键按下，进入中断服务程序置位标志位；
线程检测到标志位后操作闪灯一下。
 */
#include <rtthread.h>  
#include <stdlib.h>  
#include <rthw.h>

#include "ls1c.h"  
#include "ls1c_timer.h"  
#include "ls1c_public.h"  
#include "ls1c_gpio.h"  
#include "mipsregs.h"  

// 测试用的线程    
#define THREAD_TEST_PRIORITY                    (25)    
#define THREAD_TEST_STACK_SIZE                  (4*1024)        // 4k    
#define THREAD_TEST_TIMESLICE                   (10)    

#define led_gpio 52
#define key_gpio 85

    
struct rt_thread thread_test;    
ALIGN(8)  
rt_uint8_t thread_test_stack[THREAD_TEST_STACK_SIZE];    
  
  
volatile rt_int32_t key_irq_flag = 0;  
  
void ls1c_test_key_irqhandler(int irq, void *param)  
{  
    key_irq_flag = 1;  
}  
    
// 测试用的线程的入口    
void thread_test_entry(void *parameter)    
{  
    int key_irq = LS1C_GPIO_TO_IRQ(key_gpio);  
  
    // 初始化按键中断  
    gpio_set_irq_type(key_gpio, IRQ_TYPE_EDGE_FALLING);  
    rt_hw_interrupt_install(key_irq, ls1c_test_key_irqhandler, RT_NULL, "Key1");  
    rt_hw_interrupt_umask(key_irq);  
    gpio_init(key_gpio, gpio_mode_input);  
    rt_kprintf("\r\nkey_irq = %d \r\n", key_irq);
  
    // 初始化led  
    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);       // 指示灯默认熄灭
      
    while (1)    
    {    
        if (1 == key_irq_flag)  
        {  
            // 延迟10ms，消抖  
            rt_thread_delay(RT_TICK_PER_SECOND/10);  
            key_irq_flag = 0;  
        if (0 == (gpio_get(led_gpio)))
            gpio_set(led_gpio, gpio_level_high); 
        else
            gpio_set(led_gpio, gpio_level_low); 

            rt_kprintf("[%s] Key1 press\n", __FUNCTION__);  
        }  
        rt_thread_delay(RT_TICK_PER_SECOND);    
    }    
}    
  
  
int test_key(void)  
{  
    rt_thread_t tid;  
    rt_err_t result;  
  
 
  
    
    // 初始化测试用的线程    
    result = rt_thread_init(&thread_test,     
                            "thread_test",    
                            thread_test_entry,    
                            RT_NULL,    
                            &thread_test_stack[0],    
                            sizeof(thread_test_stack),    
                            THREAD_TEST_PRIORITY,    
                            THREAD_TEST_TIMESLICE);    
    if (RT_EOK == result)    
    {    
        rt_thread_startup(&thread_test);    
    }    
    else    
    {    
        return -1;    
    }    
  
    return 0;  
}  
int test_print(void)  
{  
    printf("\r\nthis is test \r\n");
}  


 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_key, test_key  e.g.test_key());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_key, test_key);
MSH_CMD_EXPORT(test_print, test_key);