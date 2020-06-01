#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_pwm.h"
#include "../libraries/ls1c_pin.h"
/*
数码管段	PWM信号	GPIO	复用
a	0	6	无
b	0	4	三
c	1	92	无
d	1	5	三
e	2	52	四
f	3	47	四
g	3	53	四

group1: a c e f
group2: b d g

测试命令：  seg_dis a b c d e f g
例如：
seg_dis 1 1 1 1 1 1 1 //全部翻到 0.075位置
seg_dis 0 0 0 0 0 0 0 //全部翻到 0.125位置

函数调用 ：
seg_group(a,b,c,d,e,f,g) // a~g 取值 0 或者 1

*/

void group1(int a_sta, int c_sta, int e_sta, int f_sta) //X_sta 0为go 1为return
{
    float Duty_steering = 0;
	pwm_info_t pwm_info;
	/*这是a的翻转动作*/
	pin_set_remap(LS1C_PWM0_GPIO04, PIN_REMAP_DEFAULT); //解除 GPIO04 的PWM功能
	pin_set_purpose(06, PIN_PURPOSE_OTHER); //设置 GPIO06 的复用功能
	pin_set_remap(LS1C_PWM0_GPIO06, PIN_REMAP_DEFAULT);
    if(a_sta)
	{
		pwm_info.gpio= LS1C_PWM0_GPIO06;          // pwm引脚位gpio06
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
    }
	else
	{
		pwm_info.gpio= LS1C_PWM0_GPIO06;          // pwm引脚位gpio06
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	
	/*这是c的翻转动作*/
	pin_set_remap(LS1C_PWM1_GPIO05, PIN_REMAP_DEFAULT);  //解除 GPIO05 的PWM功能
	pin_set_purpose(92, PIN_PURPOSE_OTHER); //设置 GPIO92 的复用功能
	pin_set_remap(LS1C_PWM1_GPIO92, PIN_REMAP_DEFAULT);
    if(c_sta)
	{
		pwm_info.gpio= LS1C_PWM1_GPIO92;          // pwm引脚位gpio92
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM1_GPIO92;          // pwm引脚位gpio92
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	
	/*这是e的翻转动作*/
	pin_set_purpose(52, PIN_PURPOSE_OTHER); //设置 GPIO52 的复用功能
	pin_set_remap(LS1C_PWM2_GPIO52, PIN_REMAP_FOURTH);
    if(e_sta)
	{
		pwm_info.gpio= LS1C_PWM2_GPIO52;          // pwm引脚位gpio52
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM2_GPIO52;          // pwm引脚位gpio52
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	/*这是f的翻转动作*/
    pin_set_remap(LS1C_PWM3_GPIO53, PIN_REMAP_DEFAULT);
    gpio_init(53, gpio_mode_output);
    gpio_set(53, gpio_level_high); 
    pin_set_purpose(47, PIN_PURPOSE_OTHER); //设置 GPIO47 的复用功能
	pin_set_remap(LS1C_PWM3_GPIO47, PIN_REMAP_FOURTH);
    //pin_set_remap(LS1C_PWM3_GPIO53, PIN_REMAP_FIFTH);//解除 GPIO53 的PWM功能
    if(f_sta)
	{
		pwm_info.gpio= LS1C_PWM3_GPIO47;          // pwm引脚位gpio47
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM3_GPIO47;          // pwm引脚位gpio47
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
}


void group2(int b_sta, int d_sta, int g_sta) //X_sta 0为go 1为return
{
    float Duty_steering = 0;
	pwm_info_t pwm_info;
	/*这是b的翻转动作*/
	pin_set_purpose(06, PIN_PURPOSE_GPIO); //解除 GPIO06 的PWM功能
	pin_set_purpose(04, PIN_PURPOSE_OTHER); //设置 GPIO04 的复用功能
	pin_set_remap(LS1C_PWM0_GPIO04, PIN_REMAP_THIRD);//设置 GPIO04 第三功能 PWM0
    if(b_sta)
	{
		pwm_info.gpio= LS1C_PWM0_GPIO04;          // pwm引脚位gpio04
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM0_GPIO04;          // pwm引脚位gpio06
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	/*这是d的翻转动作*/
	pin_set_purpose(92, PIN_PURPOSE_GPIO);  //解除 GPIO92 的PWM功能
	pin_set_purpose(05, PIN_PURPOSE_OTHER); //设置 GPIO05 的复用功能
	pin_set_remap(LS1C_PWM1_GPIO05, PIN_REMAP_THIRD);//设置 GPIO04 第三功能 PWM1
    if(d_sta)
	{
		pwm_info.gpio= LS1C_PWM1_GPIO05;          // pwm引脚位gpio05
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM1_GPIO05;          // pwm引脚位gpio05
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	
	/*这是g的翻转动作*/
    pin_set_remap(LS1C_PWM3_GPIO47, PIN_REMAP_DEFAULT);
    gpio_init(47, gpio_mode_output);
    gpio_set(47, gpio_level_high); 
	pin_set_purpose(53, PIN_PURPOSE_OTHER); //设置 GPIO53 的复用功能
	pin_set_remap(LS1C_PWM3_GPIO53, PIN_REMAP_FOURTH);//设置 GPIO53 第四功能 PWM3
    //pin_set_remap(LS1C_PWM3_GPIO47, PIN_REMAP_FIFTH);//解除 GPIO47 的PWM功能
    if(g_sta)
	{
		pwm_info.gpio= LS1C_PWM3_GPIO53;          // pwm引脚位gpio53
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.075;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
	else
	{
		pwm_info.gpio= LS1C_PWM3_GPIO53;          // pwm引脚位gpio53
		pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
		pwm_info.duty = 0.125;                           // pwm占空比
		pwm_info.period_ns = 20*1000*1000; 
		// pwm初始化，初始化后立即产生pwm波形
		pwm_init(&pwm_info);
	}
}

void seg_group(int a_sta, int b_sta, int c_sta, int d_sta, int e_sta, int f_sta, int g_sta)
{
    group1(a_sta, c_sta, e_sta, f_sta);
    rt_thread_delay(RT_TICK_PER_SECOND * 0.2);
    group2(b_sta, d_sta, g_sta);
}

void seg_dis(int argc, char** argv)
{
    unsigned int a,b,c,d,e,f,g;
	a = strtoul(argv[1], NULL, 0);
	b = strtoul(argv[2], NULL, 0);
	c = strtoul(argv[3], NULL, 0);
	d = strtoul(argv[4], NULL, 0);
	e = strtoul(argv[5], NULL, 0);
	f = strtoul(argv[6], NULL, 0);
	g = strtoul(argv[7], NULL, 0);
	seg_group(a,b,c,d,e,f,g);
}


FINSH_FUNCTION_EXPORT(group1, group1 e.g.group1(1,1,0,0));
FINSH_FUNCTION_EXPORT(group2, group2 e.g.group2(1,0,1));
FINSH_FUNCTION_EXPORT(seg_group, seg_group e.g.seg_group(1,0,1,1,0,1,1));

MSH_CMD_EXPORT(seg_dis, seg_dis);

