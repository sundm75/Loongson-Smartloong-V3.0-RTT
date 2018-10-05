/*
file: test_user_i2c.c
测试祼机i2c2驱动程序ls1c_i2c.c 在finsh 中运行
1. test_at24(1,17) //使用i2c1 , 写入at24c32(地址0~31)数据(17*i(i=0~31)),再读出并打印
   test_at2402(1,17) //使用i2c1 , 写入at24c02(地址0~7)数据(17*i(i=0~7)),再读出并打印
2. ds3231_getdata(1) //使用i2c1,读出ds3231的日期
3. ds3231_gettime(1)  //使用i2c1,读出ds3231的时间
4. ds3231_setdata(1,180101)  //使用i2c1,写入ds3231的日期(2018.1.1)
5. ds3231_settime(1,140000)  //使用i2c1,写入ds3231的时间(14：00：00)
 */
#include <rtthread.h>  
#include <stdlib.h>  
#include "ls1c.h"  
#include "ls1c_public.h"  
#include "ls1c_i2c.h"  
#include "ls1c_pin.h"  
  
// I2C1
#define LS1C_I2C_SDA1            (2)  
#define LS1C_I2C_SCL1            (3)  
  
// I2C2 
#define LS1C_I2C_SDA2          (54)  
#define LS1C_I2C_SCL2          (55)  
  
// 测试at24C32  页面长度32 字节 ，地址2 字节
void test_at24(rt_int8_t ic_no, rt_int8_t num )    
{  
    int i;
    ls1c_i2c_info_t i2c_info;  
    int slave_addr = 0xA0 >> 1;    
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
      
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
    {
      case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
    }
    i2c_init(&i2c_info);  
      
    send_buff[0] = 0x00;
    send_buff[1] = 0x00;            
    for(i=0;i<32;i++)
    {
        send_buff[i+2] = i*num;
    }
    
    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 34);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  

    rt_thread_delay(2);  
    // 发送读指令  的地址
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 2);  
    i2c_send_stop(&i2c_info);  

    // 读取一页寄存器数据  
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_READ);  
    i2c_receive_ack(&i2c_info); 
    i2c_receive_data(&i2c_info, recv_buff, 32);  
    i2c_send_stop(&i2c_info);  
 
    for(i=0;i<32;i++)
    {
        rt_kprintf("  0x%02x  ",  recv_buff[i] );
    }
    rt_kprintf("\n\r");
}

// 测试at24C02   I2C总线E2PROM  2KBit  每页8字节  一共256页 ，写的时候注意地址范围是0-255
void test_at2402(rt_int8_t ic_no, rt_int8_t num )    
{  
    int i;
    ls1c_i2c_info_t i2c_info;  
    int slave_addr = 0x50;    
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
      
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
    {
      case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
    }
    i2c_init(&i2c_info);  
      
    send_buff[0] = 0x00;
    for(i=0;i<8;i++)
    {
        send_buff[i+1] = i*num;
    }
    
    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 9);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  
	
    rt_thread_delay(2);  

    // 发送读指令  的地址
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 1);  
    i2c_send_stop(&i2c_info);  

    // 读取一页寄存器数据  
    i2c_send_start_and_addr(&i2c_info, slave_addr, LS1C_I2C_DIRECTION_READ);  
    i2c_receive_ack(&i2c_info); 
    i2c_receive_data(&i2c_info, recv_buff, 8);  
    i2c_send_stop(&i2c_info);  
 
    for(i=0;i<8;i++)
    {
        rt_kprintf("  0x%02x  ",  recv_buff[i] );
    }
    rt_kprintf("\n\r");
}

/* 测试ds3231   */
#define ds3231_slave_addr 0x68
#define bcd(x)  ( ((x/10)<<4) | (x%10))
#define frombcd(x)   ((x >> 4) * 10 + (x & 0x0f))

void ds3231_getdata(rt_int8_t ic_no )    
{  
    ls1c_i2c_info_t i2c_info;  
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
      
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
        {
        case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
}
    i2c_init(&i2c_info);  
      
    send_buff[0] = 0x04;    
    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 1);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  

    // 读取数据
    rt_thread_delay(2);  
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_READ);  
    i2c_receive_ack(&i2c_info); 
    i2c_receive_data(&i2c_info, recv_buff, 3);  
    i2c_send_stop(&i2c_info);  

 
    rt_kprintf("\ndata : 20%02d-%02d-%02d\n ",frombcd(recv_buff[2]),frombcd(recv_buff[1]),frombcd(recv_buff[0]));
    rt_kprintf("\n\r");
}

void ds3231_gettime(rt_int8_t ic_no )    
{  
    ls1c_i2c_info_t i2c_info;  
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
      
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
        {
        case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
}
    i2c_init(&i2c_info);  
      
    send_buff[0] = 0x00;    
    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 1);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  

    // 读取数据
    rt_thread_delay(2);  
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_READ);  
    i2c_receive_ack(&i2c_info); 
    i2c_receive_data(&i2c_info, recv_buff, 3);  
    i2c_send_stop(&i2c_info);  

 
    rt_kprintf("\ntime : %02d:%02d:%02d\n",frombcd(recv_buff[2]),frombcd(recv_buff[1]),frombcd(recv_buff[0]));
    rt_kprintf("\n\r");
}

/*设置日期格式为yymmdd。如2018-01-01写为180101*/
void ds3231_setdata(rt_int8_t ic_no , rt_uint32_t data)    
{  
    ls1c_i2c_info_t i2c_info;  
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
    rt_uint8_t yy,mm,dd;
    
    yy = data / 10000;
    mm = data % 10000  / 100;
    dd= data % 100;

    //设置寄存器地址4    
    send_buff[0]=4;//寄存器地址4
    send_buff[1]=bcd(dd);//日
    send_buff[2]=bcd(mm);//月
    send_buff[3]=bcd(yy);//年
    
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
        {
        case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
}
    i2c_init(&i2c_info);  

    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 4);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  
}

/*设置到ds3231。 格式为hhmmss。如14:00:00写为140000*/
void ds3231_settime(rt_int8_t ic_no , rt_uint32_t time)    
{  
    ls1c_i2c_info_t i2c_info;  
    unsigned char send_buff[64] = {0};  
    unsigned char recv_buff[64] = {0};  
    rt_uint8_t hh,mm,ss;
    
    hh = time / 10000;
    mm = time % 10000  / 100;
    ss = time % 100;

    //设置寄存器地址0    
    send_buff[0]=0;//寄存器地址0
    send_buff[1]=bcd(ss);//秒
    send_buff[2]=bcd(mm);//分
    send_buff[3]=bcd(hh);//时
    
    i2c_info.clock = 50*1000;       // 50kb/s  
    switch( ic_no)
        {
        case 0:
        i2c_info.I2Cx = LS1C_I2C_0;
      break;
      case 1:
          i2c_info.I2Cx = LS1C_I2C_1;
      break;
      case 2:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
      default:
          i2c_info.I2Cx = LS1C_I2C_2;
      break;
}
    i2c_init(&i2c_info);  
      
    
    // 发送器件地址和要写入的地址、数据
    i2c_send_start_and_addr(&i2c_info, ds3231_slave_addr, LS1C_I2C_DIRECTION_WRITE);  
    i2c_receive_ack(&i2c_info);  
    i2c_send_data(&i2c_info, send_buff, 4);  
    rt_thread_delay(1);  
    i2c_send_stop(&i2c_info);  
}

void test_at24_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	test_at24(num1, num2);
}
void test_at2402_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	test_at2402(num1, num2);
}
void ds3231_getdata_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	ds3231_getdata(num1);
}
void ds3231_gettime_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	ds3231_gettime(num1);
}
void ds3231_setdata_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	ds3231_setdata(num1, num2);
}
void ds3231_settime_msh(int argc, char** argv)
{
    unsigned int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	ds3231_settime(num1, num2);
}
 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_at24 , test_at24  e.g.test_at24(1,17));
FINSH_FUNCTION_EXPORT(test_at2402 , test_at2402  e.g.test_at2402(1,17));
FINSH_FUNCTION_EXPORT(ds3231_getdata , ds3231_getdata  e.g.ds3231_getdata(1));
FINSH_FUNCTION_EXPORT(ds3231_gettime , ds3231_gettime  e.g.ds3231_gettime(1));
FINSH_FUNCTION_EXPORT(ds3231_setdata , ds3231_setdata  e.g.ds3231_setdata(1,180101));
FINSH_FUNCTION_EXPORT(ds3231_settime , ds3231_settime  e.g.ds3231_settime(1,140000));
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_at24_msh, test_at24_msh 1 17);
MSH_CMD_EXPORT(test_at2402_msh, test_at2402_msh 1 17);
MSH_CMD_EXPORT(ds3231_getdata_msh, ds3231_getdata_msh 1);
MSH_CMD_EXPORT(ds3231_gettime_msh, ds3231_gettime_msh 1);
MSH_CMD_EXPORT(ds3231_setdata_msh, ds3231_setdata_msh 1 180101);
MSH_CMD_EXPORT(ds3231_settime_msh, ds3231_settime_msh 1 140000);

