/*
 * File      : test_ad7705.c 
 * 在 msh 中运行
 * 1. init_AD7705 初始化 AD 模块
 * 2. test_AD7705 进行一次AD转换
 */
#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 
#include <drivers/spi.h>
#include <stdint.h>
#include <string.h>
 
#include "ls1c_spi.h"
#include "drv_spi.h"
#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_regs.h"

#if  1
 
/*
*********************************************************************************************************
*	所有的寄存器（包括通信寄存器本身和输出数据寄存器）进行读操作之前，
*	必须先写通信寄存器，然后才能读选定的寄存器。 
*********************************************************************************************************
*/
enum   
{  
    AD7705_REG_COMM         = (0 << 4),  		//8bits
    AD7705_REG_SETUP        = (1 << 4),  		//8bits
    AD7705_REG_CLOCK        = (2 << 4), 		//8bits
    AD7705_REG_DATA         = (3 << 4), 		//16bits
    AD7705_REG_TEST         = (4 << 4), 		//8bits
    AD7705_REG_OFFSET       = (6 << 4),  		//24bits
    AD7705_REG_GAIN         = (7 << 4),  		//24bits
      
    AD7705_WRITE            = (0 << 3),
    AD7705_READ             = (1 << 3), 
  
    AD7705_CH_1             = 0,   		 		// AIN1+  AIN1-  
    AD7705_CH_2             = 1,    			// AIN2+  AIN2-  
    AD7705_CH_3             = 2,    			// AIN1-  AIN1-  
    AD7705_CH_4             = 3     			// AIN1-  AIN2-  
};  
  
enum  
{  
    AD7705_MD_NORMAL        = (0 << 6),   		//正常模式
    AD7705_MD_CAL_SELF      = (1 << 6),   		//自校准
    AD7705_MD_CAL_ZERO      = (2 << 6),   		//零标度系统校准
    AD7705_MD_CAL_FULL      = (3 << 6),   		//满标度系统校准
  
    AD7705_GAIN_1           = (0 << 3),  
    AD7705_GAIN_2           = (1 << 3),   
    AD7705_GAIN_4           = (2 << 3),   
    AD7705_GAIN_8           = (3 << 3),   
    AD7705_GAIN_16          = (4 << 3),     
    AD7705_GAIN_32          = (5 << 3),  
    AD7705_GAIN_64          = (6 << 3),    
    AD7705_GAIN_128         = (7 << 3),  
  
    AD7705_BIPOLAR          = (0 << 2),    		//双极性工作
    AD7705_UNIPOLAR         = (1 << 2),   		//单极性工作
  
    AD7705_BUF_NO           = (0 << 1),   		//缓冲器控制
    AD7705_BUF_EN           = (1 << 1),   
  
    AD7705_FSYNC_0          = 0,     			//滤波器同步控制
    AD7705_FSYNC_1          = 1    
};  
  
enum  
{  
    AD7705_CLKDIS_0         = (0 << 4),      	//主时钟禁止位
    AD7705_CLKDIS_1         = (1 << 4),      
  
    AD7705_CLKDIV_0         = (0 << 3),     	//2.4576MHz	（CLKDIV=0 ）
    AD7705_CLKDIV_1         = (1 << 3),      	//4.9152MHz （CLKDIV=1 ）
  
    AD7705_CLK_0            = (0 << 2),    		//时钟位，主时钟频率为2.4576MHz（CLKDIV=0）或为4.9152MHz（CLKDIV=1），CLK应置“0”，主时钟频率为1MHz（CLKDIV=0）或2MHz（CLKDIV=1），CLK应置“1”
    AD7705_CLK_1            = (1 << 2),    
  
    AD7705_UPDATE_20        = (0),  			//输出更新速度，不同于AD采样速率
    AD7705_UPDATE_25        = (1),  			
    AD7705_UPDATE_100       = (2),  
    AD7705_UPDATE_200       = (3),  
    
    AD7705_UPDATE_50        = (0),  
    AD7705_UPDATE_60        = (1),  
    AD7705_UPDATE_250       = (2),  
    AD7705_UPDATE_500       = (3)  
};  
   
#define SPIDEV_MAJOR			153						// 主设备号
#define N_SPI_MINORS			123						// 次设备号
  
#define AD7705_CHANNEL_NUM      (2)     
 
#define AD7705_DRDY_PIN         (88)  	 //BUF_GPS_TXD AD7705的引脚DRDY   ，配置为输入引脚
#define AD7705_RESET_PIN        (87)    //BUF_GPS_RXD AD7705的引脚RESET  ,配置为输出引脚
#define AD7705_MISO_PIN			(EXYNOS4_GPC1(3))	
#define AD7705_MOSI_PIN			(EXYNOS4_GPC1(4))
#define AD7705_SCLK_PIN			(EXYNOS4_GPC1(1)) 
#define AD7705_NSS_PIN			(EXYNOS4_GPC1(2)) 
	
#define AD7705_BUFFER 			2048  

#define AD7705_SPIDEV           "spi03"


rt_device_t spi;
struct rt_spi_device * spi_device;
/*
*********************************************************************************************************
*	函 数 名: AD7705_reset
*	功能说明: 复位 AD7705芯片
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
static void AD7705_reset(void)  
{  
	printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    gpio_set(AD7705_RESET_PIN, gpio_level_high);
    rt_thread_mdelay(1);  
    gpio_set(AD7705_RESET_PIN, gpio_level_low);
    rt_thread_mdelay(5);  
    gpio_set(AD7705_RESET_PIN, gpio_level_high);
    rt_thread_mdelay(1);  
  
    return ;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_sync_spi
*	功能说明: 同步AD7705芯片SPI接口时序
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD7705_sync_spi(struct rt_spi_device * spi_device)  
{  
    uint8_t tx_buf[4] = {0xFF,0xFF,0xFF,0xFF};      
	printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
	// 至少32个串行时钟内向AD7705的DIN线写入逻辑"1" 
    rt_spi_send(spi_device, tx_buf,  sizeof(tx_buf));
}  
  
/*
*********************************************************************************************************
*	函 数 名: AD7705_wait_DRDY
*	功能说明: 等待内部操作完成。 自校准时间较长，需要等待。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/   
static int AD7705_wait_DRDY(void)  
{  
    int i = 0;  
    uint32_t time_cnt = 500;  		//超时1s	
  
	// printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    for (i=0; i<time_cnt; i++)  
    {  
        if (0 == gpio_get(AD7705_DRDY_PIN))  
        {  
            break;  
        }  
        rt_thread_mdelay(10);  
    }  
  
    if (i >= time_cnt)  		
    {  
		printf("[%s][%d]: AD7705_wait_DRDY Time Out ...\r\n", __FUNCTION__, __LINE__);
        return -1;  
    }  
  
    return 0;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_calibZero_self
*	功能说明: 启动自校准. 内部自动短接AIN+ AIN-校准0位，内部短接到Vref 校准满位。此函数执行过程较长 
*			  实测约 180ms
*	形    参:  channel : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD7705_systemcalib_self(struct rt_spi_device *spi_dev, uint8_t channel)  
{   
    uint8_t tx_buf[2] = {0};   
    uint8_t rx_buf[1] = {0};
  
	printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    tx_buf[0] = AD7705_REG_SETUP | AD7705_WRITE | channel;  	// 写设置寄存器 
    tx_buf[1] = AD7705_MD_CAL_SELF | AD7705_GAIN_1 | AD7705_UNIPOLAR | AD7705_BUF_EN | AD7705_FSYNC_0;  
	//配置通道channel(0x46)：激活自校准，增益为1，单极性工作，允许输入缓冲，滤波器同步设置为0
	printf("[%s][%d]: Write SETUP Register: 0x%x\r\n", __FUNCTION__, __LINE__, tx_buf[1]); 
    rt_spi_send(spi_dev, tx_buf, sizeof(tx_buf));   
	
    rt_thread_mdelay(250); 		/* 等待内部操作完成 --- 时间较长，约180ms*/  
	 
/**********************************读寄存器写入的数据，用作调试****************************************/
	AD7705_wait_DRDY(); 
	memset(rx_buf, 0, sizeof(rx_buf));
    tx_buf[0] = AD7705_REG_SETUP | AD7705_READ | channel;  	//(0x46) 读设置寄存器 , 用作调试  
	rx_buf[0] = rt_spi_sendrecv8(spi_dev, tx_buf[0]); 
	if(rx_buf[0] < 0)
	{
		printf("[%s][%d]: Read SETUP Register faild.\r\n", __FUNCTION__, __LINE__);
	}else{
		printf("[%s][%d]: Read SETUP Register: 0x%x\r\n", __FUNCTION__, __LINE__, rx_buf[0]); 
	}  
	
	AD7705_wait_DRDY();	 
	memset(rx_buf, 0, sizeof(rx_buf));
    tx_buf[0] = AD7705_REG_CLOCK | AD7705_READ | channel;  	//(0x8) 读时钟寄存器 , 用作调试  
	rx_buf[0] = rt_spi_sendrecv8(spi_dev, tx_buf[0]); 
	if(rx_buf[0] < 0)
	{
		printf("[%s][%d]: Read CLOCK Register faild.\r\n", __FUNCTION__, __LINE__);
	}else{
		printf("[%s][%d]: Read CLOCK Register: 0x%x\r\n", __FUNCTION__, __LINE__, rx_buf[0]); 
	}	 
	
/***********************************************************************************************************/	 
    return ;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_calibZero_self
*	功能说明: 启动系统校准零位. 请将AIN+ AIN-短接后，执行该函数。校准应该由主程序控制并保存校准参数。
*			 执行完毕后。可以通过 AD7705_ReadReg(REG_ZERO_CH1) 和  AD7705_ReadReg(REG_ZERO_CH2) 读取校准参数。
*	形    参: channel : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD7705_calibZero_self(struct rt_spi_device *spi_dev, uint8_t channel) 
{
    uint8_t tx_buf[2] = {0};  
  
	printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    tx_buf[0] = AD7705_REG_SETUP | AD7705_WRITE | channel;  	// 写设置寄存器 
    tx_buf[1] = AD7705_MD_CAL_ZERO | AD7705_GAIN_1 | AD7705_UNIPOLAR | AD7705_BUF_EN | AD7705_FSYNC_0;  	//配置通道channel：激活零标度系统校准，增益为1，单极性工作，允许输入缓冲，滤波器同步设置为0
    rt_spi_send(spi_dev, tx_buf, sizeof(tx_buf));  
  
    rt_thread_mdelay(200);   
      
    return ;  
}
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_calibFull_self
*	功能说明: 启动系统校准满位. 请将AIN+ AIN-接最大输入电压源，执行该函数。校准应该由主程序控制并保存校准参数。
*			 执行完毕后。可以通过 AD7705_ReadReg(REG_FULL_CH1) 和  AD7705_ReadReg(REG_FULL_CH2) 读取校准参数。
*	形    参:  channel : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD7705_calibFull_self(struct rt_spi_device *spi_dev, uint8_t channel)  
{  
    uint8_t tx_buf[2] = {0};  
  
	printf("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    tx_buf[0] = AD7705_REG_SETUP | AD7705_WRITE | channel;  	// 写设置寄存器 
    tx_buf[1] = AD7705_MD_CAL_FULL | AD7705_GAIN_1 | AD7705_UNIPOLAR  | AD7705_BUF_EN | AD7705_FSYNC_0;  //配置通道channel(0x46)：激活满标度系统校准，增益为1，单极性工作，允许输入缓冲，滤波器同步设置为0
    rt_spi_send(spi_dev, tx_buf, sizeof(tx_buf));  
   
    rt_thread_mdelay(200);  
      
    return ;  
} 
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_config_channel
*	功能说明: 配置AD7705的指定通道  
*	形    参: channel : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/  
static void AD7705_config_channel(struct rt_spi_device *spi_dev, uint8_t channel)  
{  
    uint8_t tx_buf[2] = {0}; 
  
	// printk("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    tx_buf[0] = AD7705_REG_CLOCK | AD7705_WRITE | channel;  	// 写时钟寄存器 
    tx_buf[1] = AD7705_CLKDIS_0 | AD7705_CLKDIV_1 | AD7705_CLK_0 | AD7705_UPDATE_50;  	 
	//配置通道channel(0x8)：允许主时钟输出，时钟分频，CLK为4.9152MHz，输出更新速率为50Hz 
	printf("[%s][%d]: Write CLOCK Register: 0x%x\r\n", __FUNCTION__, __LINE__, tx_buf[1]); 
    rt_spi_send(spi_dev, tx_buf, sizeof(tx_buf));  
  
    AD7705_systemcalib_self(spi_dev, channel);  		//内部自校准通道channel
  
    return ;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_reset_and_reconfig
*	功能说明: 初始化AD7705, 复位AD7705并重新配置  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/  
static void AD7705_reset_and_reconfig(struct rt_spi_device *spi_dev)  
{  
	// printk("[%s][%d]\r\n", __FUNCTION__, __LINE__);
	 
    AD7705_reset();  		//复位AD7705 
    rt_thread_mdelay(1);   
    AD7705_sync_spi(spi_dev);  	//同步SPI接口时序, AD7705串行接口失步后将其复位。复位后要延时500us再访问 
    rt_thread_mdelay(1);   
    AD7705_config_channel(spi_dev, AD7705_CH_1);   
    
    return ;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_read_channel
*	功能说明: 读AD7705  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/  
static int AD7705_read_channel(struct rt_spi_device *spi_dev, uint8_t channel, uint16_t *ad)  
{    
    int ret = -1;   
    uint16_t value = 0xffff;  
    uint8_t tx_buf[1] = {0};   
    uint8_t rx_buf[2] = {0};  

	// printk("[%s][%d]\r\n", __FUNCTION__, __LINE__); 
    AD7705_wait_DRDY();  									// 等待转换完成  
	 
    tx_buf[0] = AD7705_REG_DATA | AD7705_READ | channel;  	//(0x38)下一步对数据寄存器进行读操作 
    rt_spi_send(spi_dev, tx_buf, sizeof(tx_buf)); 
    ret = rt_spi_recv(spi_dev, rx_buf, sizeof(rx_buf));  		//每次读的都是上一次转换结果
    value = (rx_buf[0]<<8) | rx_buf[1];  
     printf("rx_buf[0]=0x%02x rx_buf[1]=0x^%02x \n", rx_buf[0], rx_buf[1]);  
	//value = value << 1;    								//读出数据会比真实数据向右偏移一位，比如实际数据0xc,可能读出0x8006,且最高位总为1,正在查找原因 !!!!!!!!!!!!!!!
    if (ret < 0)    
    {  
        printf("[%s][%d]: AD7705_read_byte() faild. ret=%d \n", __FUNCTION__, __LINE__, ret);  
        goto fail;  
    }  
	 
	if (0xffff == value)  									//接口迷失, // AD7705上电一段时间后，可能会出现读到的值一直是0xfff的情况  
	{      
        printf(" Error: [%s][%d]: value = 0xffff \n", __FUNCTION__, __LINE__);  
		ret = -1; 
		goto fail;
	} 
       
    *ad = value;   
  
fail:   
    return ret;  
}  
 
/*
*********************************************************************************************************
*	函 数 名: AD7705_get_value
*	功能说明: 获取AD7705的读数值 
*	形    参: channel : ADC通道，1或2;  val: AD7705转换结果，传入参数地址
*	返 回 值: 无
*********************************************************************************************************
*/ 
static ssize_t AD7705_get_value(struct rt_spi_device *spi_dev, uint8_t channel, uint16_t *val)  
{    
    int ret = 0;   
	int i=0;
	// printk("[%s][%d]\r\n", __FUNCTION__, __LINE__);
 /* 
 * 为了避免通道切换造成读数失效，读2次 
 * 实际上每次读到的是上一次采集的结果(可以两个通道交替采集就能看到效果) 
 */ 
	for(i=0; i<2; i++)
	{
		ret = AD7705_read_channel(spi_dev, channel, val);  
		if (ret < 0)  
		{  
			/*失败，重启AD7705并重新配置*/
			//AD7705_reset_and_reconfig(spi_dev);   
			printf("  Error: [%s][%d]: AD7705_reset_and_reconfig...\n", __FUNCTION__, __LINE__);  
			return ret;  
		}  
		rt_thread_mdelay(5);	//防止读数频率过快 
	}		
	
    return ret;  
} 
 
static int  init_AD7705(void)  
{ 
    spi = rt_device_find(AD7705_SPIDEV);
    spi_device = (struct rt_spi_device *)spi;
   /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 ; /* SPI Compatible Modes 0 */
        cfg.max_hz = 200 * 1000; /* 500K */
        rt_spi_configure(spi_device, &cfg);
    }

    rt_err_t err = rt_device_open(spi, RT_DEVICE_OFLAG_RDWR);
    if(err != RT_EOK)
    {
        printf("Open %S failed %08X, exit thread....\n",  AD7705_SPIDEV,  err);
        return err;
    }

    AD7705_reset_and_reconfig(spi_device);
    printf("[%s][%d]: AD7705 module init success!\n", __FUNCTION__, __LINE__);
	
    return 0;
}  
  


void  test_AD7705(void)  
{ 
    uint16_t data;
 
    AD7705_systemcalib_self(spi_device, AD7705_CH_1);   
    rt_thread_mdelay(100);	//防止读数频率过快 
    printf("[%s][%d]: AD7705 test exit success!\n", __FUNCTION__, __LINE__);
    AD7705_get_value(spi_device, 1, &data);
    printf("AD[1] = %d \n",  data);
    AD7705_get_value(spi_device, 0, &data);
    printf("AD[0] = %d \n",  data);

}  
#include  <finsh.h>    
MSH_CMD_EXPORT(test_AD7705, test_AD7705  );
MSH_CMD_EXPORT(init_AD7705, init_AD7705  );

#endif 
 