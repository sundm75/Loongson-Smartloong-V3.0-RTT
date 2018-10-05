/*
 * File      : test_spi.c
测试祼机SPI驱动程序ls1c_spi.c 在finsh 中运行
1. test_spi01()   SPI0 CS1 发送16个字节
2. test_spi10()   SPI1 CS0 发送16个字节

 */
 #include <rtthread.h>
#include <drivers/spi.h>
#include <stdlib.h>  
#include "ls1c_spi.h"

#include "ls1c_public.h"
#include "ls1c_pin.h"
#include "ls1c_spi.h"
#include "ls1c_gpio.h"
#include "ls1c_delay.h"

void test_spi01(void)
{    
    int i;
    rt_uint8_t buf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    unsigned char SPIx = 0;
    void *spi_base = NULL;
    unsigned char cpol = 0;
    unsigned char cpha = 0;
    unsigned char val = 0;
    
    spi_base = ls1c_spi_get_base(SPIx);
    ls1c_spi_set_cs(spi_base, 1, 0); //SPI0 CS1
    {
        // 使能SPI控制器，master模式，关闭中断
        reg_write_8(0x53, spi_base + LS1C_SPI_SPCR_OFFSET);

        // 清空状态寄存器
        reg_write_8(0xc0, spi_base + LS1C_SPI_SPSR_OFFSET);

        // 1字节产生中断，采样(读)与发送(写)时机同时
        reg_write_8(0x03, spi_base + LS1C_SPI_SPER_OFFSET);

        // 关闭SPI flash
        val = reg_read_8(spi_base + LS1C_SPI_SFC_PARAM_OFFSET);
        val &= 0xfe;
        reg_write_8(val, spi_base + LS1C_SPI_SFC_PARAM_OFFSET);

        // spi flash时序控制寄存器
        reg_write_8(0x05, spi_base + LS1C_SPI_SFC_TIMING_OFFSET);
    }
    
    // baudrate
    ls1c_spi_set_clock(spi_base, 100*1000);
    // 设置通信模式(时钟极性和相位)
    ls1c_spi_set_mode(spi_base, SPI_CPOL_1, SPI_CPHA_1);
    
    delay_us(1);
    for( i=0; i<16; i++)
    {
        ls1c_spi_txrx_byte(spi_base, buf[i]);
    }    
    ls1c_spi_set_cs(spi_base, 1, 1); //SPI0 CS1
}

void test_spi10(void)
{    
    int i;
    rt_uint8_t buf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    unsigned char SPIx = 1;
    void *spi_base = NULL;
    unsigned char cpol = 0;
    unsigned char cpha = 0;
    unsigned char val = 0;
    
    spi_base = ls1c_spi_get_base(SPIx);
    ls1c_spi_set_cs(spi_base, 0, 0); //SPI1 CS0
    {
        // 使能SPI控制器，master模式，关闭中断
        reg_write_8(0x53, spi_base + LS1C_SPI_SPCR_OFFSET);

        // 清空状态寄存器
        reg_write_8(0xc0, spi_base + LS1C_SPI_SPSR_OFFSET);

        // 1字节产生中断，采样(读)与发送(写)时机同时
        reg_write_8(0x03, spi_base + LS1C_SPI_SPER_OFFSET);

        // 关闭SPI flash
        val = reg_read_8(spi_base + LS1C_SPI_SFC_PARAM_OFFSET);
        val &= 0xfe;
        reg_write_8(val, spi_base + LS1C_SPI_SFC_PARAM_OFFSET);

        // spi flash时序控制寄存器
        reg_write_8(0x05, spi_base + LS1C_SPI_SFC_TIMING_OFFSET);
    }
    
    // baudrate
    ls1c_spi_set_clock(spi_base, 100*1000);
    // 设置通信模式(时钟极性和相位)
    ls1c_spi_set_mode(spi_base, SPI_CPOL_1, SPI_CPHA_1);
    
    delay_us(1);
    for( i=0; i<16; i++)
    {
        ls1c_spi_txrx_byte(spi_base, buf[i]);
    }    
    ls1c_spi_set_cs(spi_base, 0, 1); //SPI1 CS0
}

  #include  <finsh.h>
FINSH_FUNCTION_EXPORT(test_spi01, test_spi01 e.g.test_spi01());
FINSH_FUNCTION_EXPORT(test_spi10, test_spi10  e.g.test_spi10());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_spi01, test_spi01);
MSH_CMD_EXPORT(test_spi10, test_spi10);
