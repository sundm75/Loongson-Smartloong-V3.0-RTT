/*
 * File      : test_modbus.c
测试free_modbus运行
1. mb_poll 启动轮询
2. mb_master 启动主机
3. mb_slave 启动从机

 */
#include <board.h>
#include <rtthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <rthw.h>	
#include <rtthread.h>

#ifdef  RT_USING_MODBUS
#include "mb.h"
#include "mb_m.h"
#include "mbrtu.h"
#include "mbframe.h"
#include "user_mb_app.h"

#define MB_PORT 1   //定义modbus串口号
#define ElecMeter_ModbusID  0x1 //定义 主机/从机 ID
#define Volt_Addr    0x00       // 主机/从机 寄存器地址
#define IntReglen 2  // 主机/从机 寄存器长度
#define BPS   4800// 通信波特率

#define LOG_D rt_kprintf
extern USHORT usMRegHoldBuf[1][6];
//协议栈运行
void xMBPollTask(void *pvParameters)
{
    // Initialize protocol stack in RTU mode for a Master
    eMBMasterInit( MB_RTU, MB_PORT,BPS, MB_PAR_NONE );
    // Enable the Modbus Protocol Stack.
    eMBMasterEnable(  );

    for(;;)
    {
        eMBMasterPoll();
    }
}
//创建xMBPollTask进程
void  mb_poll(void)
{
    /* user app entry */
    rt_thread_t tid;
    tid = rt_thread_create("mbpoll",
                           xMBPollTask, RT_NULL,
                           1024, 10 , 500);
    if (tid != RT_NULL) 
        rt_thread_startup(tid);
    LOG_D("mb_poll running");
}

//master 请求线程
void mbMasterThreadEntry(void * para)
{
    rt_uint16_t i,j;
    uint16_t errorCount = 0;
    eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
    while (1)
    {
        errorCode = eMBMasterReqWriteHoldingRegister(ElecMeter_ModbusID, Volt_Addr,errorCount,RT_WAITING_FOREVER);
		LOG_D("errorCode = %d \r\n",errorCode); 
        rt_thread_delay(100);
        errorCode == eMBMasterReqReadHoldingRegister(ElecMeter_ModbusID, Volt_Addr, IntReglen, RT_WAITING_FOREVER);
        if(MB_MRE_NO_ERR == errorCode)
        {
            LOG_D("Receive success\n");
            for(i=0;i<2;i++) 
            {
                for(j=0;j<IntReglen*2;j++) 
                    LOG_D("%02x ",usMRegHoldBuf[i][j]); 
            } 
        }
        else
        {
            LOG_D(" poll failed\n");
            LOG_D("errorCode = %d \r\n",errorCode);
        }
    	//记录出错次数
        if (errorCode != MB_MRE_NO_ERR) 
        {
            errorCount++;
        }
        rt_thread_delay(1000);
    }

}
void mb_master(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("mb_master",
                            mbMasterThreadEntry,RT_NULL,
                            1024, 10 , 5);
    if(tid != RT_NULL)
        rt_thread_startup(tid);
    LOG_D("mb_master running");
}

/**
* \brief Modbus从机轮询入口
*/
void thread_entry_ModbusSlavePoll (void* parameter)
{
    // 端口选择Port1 即Uart1
    eMBInit(MB_RTU, ElecMeter_ModbusID, MB_PORT, BPS,  MB_PAR_NONE);
    eMBEnable();
    while (1) {
        eMBPoll();
    }
}

void mb_slave(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("mb_slave",
                            thread_entry_ModbusSlavePoll,RT_NULL,
                            1024, 10 , 5);
    if(tid != RT_NULL)
        rt_thread_startup(tid);
    LOG_D("mb_slave running");
}
#include  <finsh.h>
MSH_CMD_EXPORT(mb_poll, modbus poll );
MSH_CMD_EXPORT(mb_master, modbus master );
MSH_CMD_EXPORT(mb_slave, modbus slave );

#endif 