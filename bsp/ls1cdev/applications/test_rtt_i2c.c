/*
 * File      : test_rtt_i2c.c
测试rtt 硬件i2c2驱动， 在finsh 中运行 test_at24c32()
1. 每运行一次,读出地址1的数据,打印 AT24c32_I2C_BUS_NAME定义所使用的i2c总线
2. 该数据+1 后，再写入地址1的寄存器
 */

#include <rtthread.h>
#include <stdlib.h>  
#include <drivers/i2c.h>
#include "../drivers/drv_i2c.h"

#define AT24c32_I2C_BUS_NAME                ("i2c2")        // 注意与i2c bus初始化函数中的bus name保持一致
struct rt_i2c_bus_device *at24c32_i2c_bus = RT_NULL;
int at24c32_addr = 0xA0 >> 1;               // 地址前7位

/*
 * 从指定地址读出一个字节
 * @read_addr 地址
 */
unsigned char at24c32_read_byte(unsigned char read_addr)
{
    struct rt_i2c_msg msgs[2];
    unsigned char data;
    unsigned char reg_addr[2];
    
    reg_addr[0] = 0;
    reg_addr[1] = read_addr;
    
    msgs[0].addr    = at24c32_addr;
    msgs[0].flags   = RT_I2C_WR;
    msgs[0].buf     = reg_addr;
    msgs[0].len     = 2;

    msgs[1].addr    = at24c32_addr;
    msgs[1].flags   = RT_I2C_RD;
    msgs[1].buf     = &data;
    msgs[1].len     = 1;
    rt_i2c_transfer(at24c32_i2c_bus, msgs, 2);

    return data;
}
/*
 * 在指定地址写入一个字节的数据
 * @write_addr 地址
 * @data 待写入的数据
 */
void at24c32_write_byte(unsigned char write_addr, unsigned char data)
{
    struct rt_i2c_msg msg[1] = {0};
    unsigned char buf[3] = {0};
    unsigned char reg_addr[2];
    
    buf[0] = 0;
    buf[1] = write_addr;
    buf[2] = data;
    
    msg[0].addr    = at24c32_addr;
    msg[0].flags   = RT_I2C_WR;
    msg[0].buf     = buf;
    msg[0].len     = 3;
    rt_i2c_transfer(at24c32_i2c_bus, msg, 1);

    return ;
}
// 测试用的线程的入口  
void test_at24c32(void )  
{
    unsigned char read_addr = 1;    // 地址
    unsigned char count = 0;        // 用于计数的变量

    // find设备
    at24c32_i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(AT24c32_I2C_BUS_NAME);
    if (RT_NULL == at24c32_i2c_bus)
    {
        rt_kprintf("[%s] no i2c device -- at24c32!\n", __FUNCTION__);
        return ;
    }

    // 读
    count = at24c32_read_byte(read_addr);
    rt_kprintf("[%s] last's count=%u\n", __FUNCTION__, count);

    // 加一，然后写
    count++;
    at24c32_write_byte(read_addr, count);
    rt_thread_delay(6);     // 一定要延时5ms以上

    // 读
    count = at24c32_read_byte(read_addr);
    rt_kprintf("[%s] current count=%d\n", __FUNCTION__, count);
}  

 #include  <finsh.h>
 FINSH_FUNCTION_EXPORT(test_at24c32, test_at24c32  e.g.test_at24c32());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_at24c32, test_at24c32);
