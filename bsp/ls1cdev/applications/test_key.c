/*
 * File      : test_key.c
测试key接口在finsh中运行
1. test_key1 测试 key1 下降沿中断   
首先初始化打开按键中断，启动线程不断检测标志位；
按键按下，进入中断服务程序置位标志位；
线程检测到标志位后操作蓝灯 LED1闪灯一下。
2. test_key2 测试 key2 上升沿 和下降沿中断  
首先初始化打开按键下降中断，启动线程不断检测标志位，打印相关信息；
按键按下，检测到下降沿后进入中断服务程序置位标志位，更换中断为上升沿；
按键抬起，检测到上升沿后进入中断服务程序置位标志位，更换中断为下降沿；
中断检测到上、下边沿后操作橙灯 LED2闪灯一下。
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
#define key1_gpio 85
    
struct rt_thread thread_testkey1;    
ALIGN(8)  
rt_uint8_t thread_test_stack[THREAD_TEST_STACK_SIZE];    
  
  
volatile rt_int32_t key1_irq_flag = 0;  
  
void ls1c_test_key_down_irqhandler(int irq, void *param)  
{  
    // 延迟20ms，消抖  
    rt_thread_delay(RT_TICK_PER_SECOND/50);
    if(0 == gpio_get(key1_gpio))
    {
        key1_irq_flag = 1;  
        while(0 == gpio_get(key1_gpio));
    }        
}  

// 测试用的线程的入口    
void thread_test_key1_entry(void *parameter)    
{  
    int key_irq = LS1C_GPIO_TO_IRQ(key1_gpio);  
  
    // 初始化按键中断  
    gpio_set_irq_type(key1_gpio, IRQ_TYPE_EDGE_FALLING);  
    rt_hw_interrupt_install(key_irq, ls1c_test_key_down_irqhandler, RT_NULL, "Key1");  
    rt_hw_interrupt_umask(key_irq);  
    gpio_init(key1_gpio, gpio_mode_input);  
    rt_kprintf("\r\nkey_irq = %d \r\n", key_irq);
  
    // 初始化led  
    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);       // 指示灯默认熄灭
      
    while (1)    
    {    
        if (1 == key1_irq_flag)  
        {  
            key1_irq_flag = 0;  
        if (0 == (gpio_get(led_gpio)))
            gpio_set(led_gpio, gpio_level_high); 
        else
            gpio_set(led_gpio, gpio_level_low); 

            rt_kprintf("[%s] Key1 down press\n", __FUNCTION__);  
        }  
        rt_thread_delay(RT_TICK_PER_SECOND/2);    
    }    
}    
  
  
int test_key1(void)  
{  
    rt_thread_t tid;  
    rt_err_t result;  
    // 初始化测试用的线程    
    result = rt_thread_init(&thread_testkey1,     
                            "thread_testkey1",    
                            thread_test_key1_entry,    
                            RT_NULL,    
                            &thread_test_stack[0],    
                            sizeof(thread_test_stack),    
                            THREAD_TEST_PRIORITY,    
                            THREAD_TEST_TIMESLICE);    
    if (RT_EOK == result)    
    {    
        rt_thread_startup(&thread_testkey1);    
    }    
    else    
    {    
        return -1;    
    }    
  
    return 0;  
}  
int test_print(void)  
{  
    printf("\r\nthis is test printf ");
}  


 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_key1, test_key1  e.g.test_key1());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_key1, test_key1);
MSH_CMD_EXPORT(test_print, test_print);

/*-----------------key 2 up interupt------------------------------*/
#define key2_gpio 86
#define led2_gpio 53
    
struct rt_thread thread_testkey2;    
ALIGN(8)  
rt_uint8_t thread_testkey2_stack[THREAD_TEST_STACK_SIZE];    
  
  
volatile rt_int32_t key2_irq_flag = 0;  
volatile rt_int32_t key2_state = 0;  
  
void ls1c_test_key2_irqhandler(int irq, void *param)  
{  
    // 延迟20ms，消抖  
    rt_thread_delay(RT_TICK_PER_SECOND/50);
    if(1 == gpio_get(key2_gpio))
    {
        key2_irq_flag = 1;  
        while(0 == gpio_get(key2_gpio));
        key2_state = 1;  //key2 up
        gpio_set_irq_type(key2_gpio, IRQ_TYPE_EDGE_FALLING);  
    } 
    else
    {
        key2_irq_flag = 1; 
        while(1 == gpio_get(key2_gpio)); 
        key2_state = 0;  //key2 down
        gpio_set_irq_type(key2_gpio, IRQ_TYPE_EDGE_RISING);  
    }    
}  

void gpio_toggle(unsigned int gpio)
{
    if (0 == (gpio_get(gpio)))
        gpio_set(gpio, gpio_level_high); 
    else
        gpio_set(gpio, gpio_level_low); 
}

// 测试用的线程的入口    
void thread_test_key2_entry(void *parameter)    
{  
    int key_irq = LS1C_GPIO_TO_IRQ(key2_gpio);  
  
    // 初始化按键中断  
    gpio_set_irq_type(key2_gpio, IRQ_TYPE_EDGE_FALLING);  
    rt_hw_interrupt_install(key_irq, ls1c_test_key2_irqhandler, RT_NULL, "Key2");  
    rt_hw_interrupt_umask(key_irq);  
    gpio_init(key2_gpio, gpio_mode_input);  
    rt_kprintf("\r\nkey_irq = %d \r\n", key_irq);
  
    // 初始化led  
    gpio_init(led2_gpio, gpio_mode_output);
    gpio_set(led2_gpio, gpio_level_high);       // 指示灯默认熄灭
      
    while (1)    
    {    
        if (1 == key2_irq_flag)  
        {  
            key2_irq_flag = 0;  
            gpio_toggle(led2_gpio);
            if(1 == key2_state)
            {
                rt_kprintf("[%s] Key2 up press\n", __FUNCTION__);  
            }
            else
            {
                rt_kprintf("[%s] Key2 down press\n", __FUNCTION__); 
            }            
        }
        rt_thread_delay(RT_TICK_PER_SECOND/100);  
    }    
}      
  
int test_key2(void)  
{  
    rt_thread_t tid;  
    rt_err_t result;  
    // 初始化测试用的线程    
    result = rt_thread_init(&thread_testkey2,     
                            "thread_testkey2",    
                            thread_test_key2_entry,    
                            RT_NULL,    
                            &thread_testkey2_stack[0],    
                            sizeof(thread_testkey2_stack),    
                            THREAD_TEST_PRIORITY,    
                            THREAD_TEST_TIMESLICE);    
    if (RT_EOK == result)    
    {    
        rt_thread_startup(&thread_testkey2);    
    }    
    else    
    {    
        return -1;    
    }    
  
    return 0;  
}  

 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_key2, test_key1  e.g.test_key2());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_key2, test_key2);