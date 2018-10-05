/*
 * File      : test_rtt_spi.c
 测试SPI发送， 在finsh中运行
1. test_spi01()   SPI0 CS1 发送16个字节
2. test_spi10()   SPI1 CS0 发送16个字节
 */
#include <rtthread.h>
#include <drivers/spi.h>
#include <stdlib.h>  
#include "ls1c_spi.h"
#include "drv_spi.h"
void test_rtt_spi01(void)
{    
    rt_uint32_t count = 0;
    rt_uint8_t buf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    rt_int8_t i;

    rt_kprintf("spi0 thread start...\n");
    
    rt_device_t spi = rt_device_find("spi01");
    if(spi ==  RT_NULL)
    {
        rt_kprintf("Did not find spi01, exit thread....\n");
        return;
    }
    struct rt_spi_device * spi_device;
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
        rt_kprintf("Open spi0 failed %08X, exit thread....\n", err);
        return;
    }

    for(i=0;i<16;i++)
    {
        if(spi != RT_NULL)
        {
            rt_device_write(spi, 0, buf, 16);
            
        }
        rt_thread_delay(1);
    }
    rt_device_close(spi);
}

void test_rtt_spi10(void)
{    
    rt_uint32_t count = 0;
    rt_uint8_t buf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    rt_int8_t i;
       rt_uint8_t send_buffer[1];
       rt_uint8_t recv_buffer[2];
            
    rt_kprintf("spi1 thread start...\n");
    rt_device_t spi = rt_device_find("spi10");
    if(spi ==  RT_NULL)
    {
        rt_kprintf("Did not find spi1, exit thread....\n");
        return;
    }
    struct rt_spi_device * spi_device;
    spi_device = (struct rt_spi_device *)spi;
     rt_err_t err = rt_device_open(spi, RT_DEVICE_OFLAG_RDWR);
    if(err != RT_EOK)
    {
        rt_kprintf("Open spi1 failed %08X, exit thread....\n", err);
        return;
    }
   /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = SPI_CPOL_0 | SPI_CPHA_0; 
        cfg.max_hz = 200 * 1000;
        rt_spi_configure(spi_device, &cfg);
    }

    for(i=0;i<16;i++)
    {
        if(spi != RT_NULL)
        {
            rt_device_write(spi, 0, buf, 16);
        }
        rt_thread_delay(1);
    }
      
      struct ls1c_spi *ls1c_spi = NULL;
      struct ls1c_spi_cs *ls1c_spi_cs = NULL;
      
      ls1c_spi = spi_device->bus->parent.user_data;
      ls1c_spi_cs = spi_device->parent.user_data;
      
      ls1c_spi_set_cs( (void*)  ls1c_spi_get_base(ls1c_spi->SPIx), ls1c_spi_cs->cs, 1);                
    
    rt_device_close(spi);
}

#include  <finsh.h>
FINSH_FUNCTION_EXPORT(test_rtt_spi01, test_spi01 e.g.test_rtt_spi01());
FINSH_FUNCTION_EXPORT(test_rtt_spi10, test_spi10  e.g.test_rtt_spi10());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_spi01, test_rtt_spi01);
MSH_CMD_EXPORT(test_rtt_spi10, test_rtt_spi10);
