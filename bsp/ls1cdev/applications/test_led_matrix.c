/*
file: test_led_matrix.c
    测试数码管驱动， 在msh 中运行 test_led_matrix m n
    m为数据 n为亮度(1~15)
	MAX7219 硬件接线： CS-P90 CLK-P89 DIN-P91
*/
#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"

#define MAX7219_CS 90
#define MAX7219_CLK 89
#define MAX7219_DIN 91

#define MAX_7219_CSHigh   gpio_set(MAX7219_CS, gpio_level_high)
#define MAX_7219_CSLow    gpio_set(MAX7219_CS, gpio_level_low)
#define MAX_7219_CLKHigh   gpio_set(MAX7219_CLK, gpio_level_high)
#define MAX_7219_CLKLow    gpio_set(MAX7219_CLK, gpio_level_low)

unsigned char  disp[11][8] = {
	{ 0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C },//0
	{ 0x08,0x18,0x28,0x08,0x08,0x08,0x08,0x08 },//1
	{ 0x7E,0x2,0x2,0x7E,0x40,0x40,0x40,0x7E },//2
	{ 0x3E,0x2,0x2,0x3E,0x2,0x2,0x3E,0x0 },//3
	{ 0x8,0x18,0x28,0x48,0xFE,0x8,0x8,0x8 },//4
	{ 0x3C,0x20,0x20,0x3C,0x4,0x4,0x3C,0x0 },//5
	{ 0x3C,0x20,0x20,0x3C,0x24,0x24,0x3C,0x0 },//6
	{ 0x3E,0x22,0x4,0x8,0x8,0x8,0x8,0x8 },//7
	{ 0x0,0x3E,0x22,0x22,0x3E,0x22,0x22,0x3E },//8
	{ 0x3E,0x22,0x22,0x3E,0x2,0x2,0x2,0x3E },//9
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}//消隐
};
/*
 *Max7219的CS CLK DIN 初始化
*/
void Max7219_Gpio_init(void)
{
	int i;
	rt_kprintf("Init  gpio! \n");
	gpio_init(MAX7219_CS, gpio_mode_output); 
	gpio_set(MAX7219_CS, gpio_level_high);  /*低电平时芯片被选中，初始化为高电平*/
	gpio_init(MAX7219_CLK, gpio_mode_output);
	gpio_set(MAX7219_CLK, gpio_level_high); 
	gpio_init(MAX7219_DIN, gpio_mode_output);
	gpio_set(MAX7219_DIN, gpio_level_high);

    return ;
}

/*
 *Max7219字节写入
 *输入参数：8位数据 unsigned char Data
 *输出参数： 无
*/
void Max7219_Byte_Write(unsigned char data)
{
	unsigned char i;
	MAX_7219_CSLow;
	for (i = 0; i < 8; i++)
	{
		MAX_7219_CLKLow;
		gpio_set(MAX7219_DIN, data & 0x80); /* 将数据以MSB形式写入 */
		data=data << 1;
		MAX_7219_CLKHigh;
	}
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
void Max7219_Write_2(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	MAX_7219_CSHigh;
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
void Max7219_Write_3(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	MAX_7219_CSHigh;
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
void Max7219_Write_4(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	MAX_7219_CSHigh;
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
void Max7219_Write(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	MAX_7219_CSHigh;
	return ;
}

/*
 * Max7219初始化程序
 * 输入参数：无
 * 输出参数：无
*/
void Max7219_init(unsigned char brightnum)
{
	
	Max7219_Write(0x09, 0x00);
	Max7219_Write(0x0a, brightnum); /* 亮度设置 */
	Max7219_Write(0x0b, 0x07); 
	Max7219_Write(0x0c, 0x01); /*正常模式 */
	Max7219_Write(0x0f, 0x00); /* 正常显示模式 */
	
	Max7219_Write_2(0x09, 0x00); 
	Max7219_Write_2(0x0a, brightnum); /* 亮度设置 */
	Max7219_Write_2(0x0b, 0x07);
	Max7219_Write_2(0x0c, 0x01); /*正常模式 */
	Max7219_Write_2(0x0f, 0x00); /* 正常显示模式 */
	
	Max7219_Write_3(0x09, 0x00); 
	Max7219_Write_3(0x0a, brightnum); /* 亮度设置 */
	Max7219_Write_3(0x0b, 0x07); 
	Max7219_Write_3(0x0c, 0x01); /*正常模式 */
	Max7219_Write_3(0x0f, 0x00); /* 正常显示模式 */
	
	Max7219_Write_4(0x09, 0x00); 
	Max7219_Write_4(0x0a, brightnum); /* 亮度设置 */
	Max7219_Write_4(0x0b, 0x07); 
	Max7219_Write_4(0x0c, 0x01); /*正常模式 */
	Max7219_Write_4(0x0f, 0x00); /* 正常显示模式 */

	return ;
}

void led_num(int index, int num)
{
	for (int i = 1; i < 9; i++) //8行
	{
		switch (index) 
		{
			case 1:
			 Max7219_Write(i,   disp[num][i-1]);
			break;
			case 2:
			 Max7219_Write_2(i,   disp[num][i-1]);
			break;
			case 3:
			 Max7219_Write_3(i,   disp[num][i-1]);
			 break;
			case 4:
			 Max7219_Write_4(i,   disp[num][i-1]);
			break;
			default:
			break;
		}
	}
}

void led_disp_num(int date)
{	
	unsigned char num[4];
	int temp,i;

	for(i=0;i<8;i++)
	{
		led_num(1,10);
		led_num(2,10);
		led_num(3,10);
		led_num(4,10);
	}

	temp = 1;
	for(i=0;i<4;i++)
	{
		num[3-i] = (date/temp) % 10;
		if(num[3-i]==0 && date<temp)
		   num[3-i] = 10;
		temp = temp*10;
	}
	for(i=0;i<4;i++)
	{
		led_num(i+1,num[i]);
	}
}

void test_led_matrix(int argc, char** argv)
{
	unsigned int num1, num2;
	if(argc != 3)
		return;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	
	unsigned char i;
	Max7219_Gpio_init();
	Max7219_init(num2);//数据 是亮度  1~15

	led_disp_num(num1);
	
  return  ;	
}

MSH_CMD_EXPORT(test_led_matrix, test_led_matrix 3 2);
