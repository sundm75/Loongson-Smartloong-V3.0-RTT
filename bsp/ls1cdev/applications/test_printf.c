/*
 * File      : test_printf.c
测试浮点运行
1. test_printf   
分别计算浮点+ - * / sin sqrt 自定义函数打印。
 */

#include <rtthread.h>
#include <math.h>

#define MY_PRINTF(value) \
if(value>=0)\
rt_kprintf (" %d.%04d", (int)value,(int)((value- (int)value)*10000));\
if(value<0)\
rt_kprintf (" -%d.%04d", (-1)*(int)value,(-1)*(int)((value- (int)value)*10000));

void test_printf(void)
{
	static float float_value = 0;  
	static float float_angle = 0; 
	int angle; 
	float float_sin; 
	char float_str[80];  

	static float sqrt_val = 1;

	unsigned int *result_p = (unsigned int *)&float_sin;

	//sprintf(float_str, "\nFloat value is %f", float_value);      
	float_value = float_value + 3.1415;     
	rt_kprintf("\n add 3.1415: ");
	MY_PRINTF(float_value);
	float_value = float_value -1.23456;     
	rt_kprintf("\n sub 1.23456: ");
	MY_PRINTF(float_value);
	float_value = float_value * 1.2;     
	rt_kprintf("\n mul 1.2: ");
	MY_PRINTF(float_value);
	float_value = float_value / 1.5;     
	rt_kprintf("\n div 1.5: ");
	MY_PRINTF(float_value);

	float_angle = float_angle + 0.1;
	float_sin = sin(float_angle);
	angle = (int) (180/M_PI * float_angle);
	rt_kprintf("\nfloat_angle = ");
	MY_PRINTF(float_angle);
	rt_kprintf("   float_sin (%d)= ", angle);
	MY_PRINTF(float_sin);

	sqrt_val = sqrt_val * 15;
	rt_kprintf("\n sqrt(");
	MY_PRINTF(sqrt_val);
	sqrt_val = sqrt(sqrt_val);
	rt_kprintf(") =  ");
	MY_PRINTF(sqrt_val);	

	rt_kprintf("\n");
}

#include  <finsh.h> 
MSH_CMD_EXPORT(test_printf, test_printf );
