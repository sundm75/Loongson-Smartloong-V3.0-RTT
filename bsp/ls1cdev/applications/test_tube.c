/*
file: test_tube.c
    测试数码管驱动， 在msh 中运行 test_tube m n
    m为数据 n为亮度(1~15)
    MAX7219 硬件接线： CS-P92 CLK-P73 DIN-P92
	MAX7219是一种高集成化的串行输入/输出共阴极显示驱动器，
	可实现微处理器与7段码的接口，可以显示8位或64位单一LED。
	芯片上包括BCD码译码器、多位扫描电路、段驱动器、位驱动器、
	内含8&TImes;8位静态RAM，用于存放显示数据。
	只需外接一个电阻就可为所有的LED提供段电流。
	字符-、E、H、L、P和消隐分别对应的16进制码为×A～×F

*/
#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"

#define MAX7219_CS 6
#define MAX7219_CLK 73
#define MAX7219_DIN 92

#define MAX_7219_CSHigh   gpio_set(MAX7219_CS, gpio_level_high)
#define MAX_7219_CSLow    gpio_set(MAX7219_CS, gpio_level_low)
#define MAX_7219_CLKHigh   gpio_set(MAX7219_CLK, gpio_level_high)
#define MAX_7219_CLKLow    gpio_set(MAX7219_CLK, gpio_level_low)

/*
 *Max7219的CS CLK DIN 初始化
*/
static void Max7219_Gpio_init(void)
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
static void Max7219_Byte_Write(unsigned char data)
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
static void Max7219_Write_2(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	MAX_7219_CSHigh;
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
static void Max7219_Write_3(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	MAX_7219_CSHigh;
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	return ;
}

/*
*Max7219字节写入
*输入参数：8位数据 unsigned char Data
*输出参数： 无
*/
static void Max7219_Write_4(unsigned char add, unsigned char data)
{
	MAX_7219_CSLow;
	Max7219_Byte_Write(add);   /* 写入寄存器地址 */
	Max7219_Byte_Write(data);  /* 写入数据 */
	MAX_7219_CSHigh;
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	Max7219_Byte_Write(0x00);
	return ;
}

/*
*Max7219字节写入
*输入参数：add 要写入的寄存器的地址；data:8位数据 unsigned char 
*输出参数： 无
*/
static void Max7219_Write(unsigned char add, unsigned char data)
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
static void Max7219_init(unsigned char brightnum)
{
	
	Max7219_Write(0x09, 0xff); /* 译码方式:BCD译码 */
	Max7219_Write(0x0a, brightnum); /* 亮度设置 0~F*/
	Max7219_Write(0x0b, 0x07); /* 八个数码管显示 */
	Max7219_Write(0x0c, 0x01); /*正常工作状态*/
	Max7219_Write(0x0f, 0x00); /* 正常显示模式 */
	return ;
}

void tube_disp_num(int date)
{	
	unsigned char num;
	int temp,i;
	/*消隐处理*/
    for(i=1;i<9;i++)
	{
		Max7219_Write(i,0x0F);
	}

	temp = 1;
	for(i=0;i<8;i++)
	{
		num = (date/temp) % 10;
		if(num==0 && date<temp)
		   break;

		if(i<4)
		{
			if(i==1)
			 Max7219_Write(i+1+4,num+0xF0); //第一位加上小数点
			else
			{
				Max7219_Write(i+1+4,num);
			}			
		}
		else
		{
			Max7219_Write(i+1-4,num);
		}
		temp = temp*10;
	}
}

void test_tube(int argc, char** argv)
{
	unsigned int num1, num2;
	if(argc != 3)
		return;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);

	unsigned char i, j;
	Max7219_Gpio_init();
	Max7219_init(num2); //数据 是亮度  1~15
	tube_disp_num(num1);

  	return;	
}

MSH_CMD_EXPORT(test_tube, test_tube 12345678 8);