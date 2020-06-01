/*
 * File      : test_beep.c   
测试硬件pwm， 在finsh中运行
1. test_beep()    产生连续的音调 1234567
音阶 1 2 3 4 5 6 7 1（高音）
520 585 650 693 780 867 975 1040
 */
#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_pwm.h"

static rt_thread_t tid1 = RT_NULL;
int tong[8] = {520, 585, 650, 693, 780, 867, 975, 1040};// 音调 中音DO ~ 高音DO 对应频率 
int tong1[8] = {1923, 1709, 1538, 1443, 1282, 1153, 1026, 962}; // 周期 us

unsigned int gpio, duty;

void test_beep_thread(void* parameter)
{
	
    pwm_info_t pwm_info;
    pwm_info.gpio = gpio;                           // pwm引脚位gpio
    pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
    pwm_info.duty = (float)duty/100;                           // pwm占空比

while(1)
{
    for(int i=0 ; i<8; i++)
    {
        pwm_info.period_ns = tong1[i]*1000;               // pwm周期5ms

        // pwm初始化，初始化后立即产生pwm波形
        pwm_init(&pwm_info);   
        {        // 使能pwm
            pwm_enable(&pwm_info);
        }   
        rt_thread_delay(RT_TICK_PER_SECOND*1)   ;
    }
}

    return ;
}

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5
#include  <finsh.h> 

void test_beep(int argc, char** argv)
{
    if(argc != 3)
    return;

	gpio = strtoul(argv[1], NULL, 0);
	duty = strtoul(argv[2], NULL, 0);

	tid1 = rt_thread_create("thread1",
			test_beep_thread, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);
}

MSH_CMD_EXPORT(test_beep, test_beep e.g.test_beep 46 50);