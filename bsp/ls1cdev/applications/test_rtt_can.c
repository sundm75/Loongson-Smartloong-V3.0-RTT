/*
 * File      : test_rtt_can.c
测试硬件can0驱动， 在finsh 中运行 
1. test_rtt_canrev  测试RTT 的CAN0接收，先初始化CAN,后开启接收线程，每接收一个帧就打印出来。
2. test_rtt_csmsh 0 1  从CAN0测试发送数据,一共8个 ； test_rtt_csmsh 1 1   从CAN1测试发送数据,
 */

#include <rtthread.h>

#ifdef RT_USING_CAN
#include <ipc/completion.h>
#include <drivers/can.h>
#include <stdlib.h>  
#include "ls1c.h"

#include "ls1c_public.h"
#include "ls1c_regs.h"
#include "ls1c_clock.h"
#include "ls1c_can.h"
#include "ls1c_pin.h"

static CanRxMsg RxMessage;
static CanTxMsg TxMessage;

static void can_config(int no)
{
    rt_uint8_t buf[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
    rt_int8_t i;
    struct rt_can_device *can;
    rt_device_t can_dev ;
    
    if(0 == no )
    {
      rt_kprintf("Can0 test thread start...\n");
      can_dev = rt_device_find("bxcan0");
    }
    else if(1 == no)
    {
      rt_kprintf("Can1 test thread start...\n");
      can_dev = rt_device_find("bxcan1");
    }
    else
    {
      return;  
    }

      can = (struct rt_can_device *)can_dev;

      can->hdr == RT_NULL;
      
      /*配置发送和接收缓冲邮箱和通道个数*/
      can->config.sndboxnumber = 1;
      can->config.msgboxsz = 1;
      can->config.maxhdr = 1;
      rt_device_open(can_dev, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX));

      /*配置模式和速率*/
      struct can_configure config;
      config.baud_rate = 250000;
      config.mode = RT_CAN_MODE_LOOPBACK;
      config.maxhdr = 1;
      config.privmode = RT_CAN_MODE_NOPRIV;
      rt_device_control(can_dev, RT_DEVICE_CTRL_CONFIG, &config);

      //rt_device_control(can_dev, RT_CAN_CMD_SET_BAUD, (void*)250000);

      /*配置滤波器*/
      struct  rt_can_filter_config filter_config;
      struct  rt_can_filter_item filter_item;
      filter_item.ide = 1;
      filter_item.rtr = 0;
      filter_item.id = 1;
      filter_item.mask = 0xFFFFFFFF;
      filter_item.mode = 1;
      filter_item.hdr = 1;
    
      filter_config.count = 1;
      filter_config.actived = 1;
      filter_config.items = &filter_item;
      can->hdr[0].connected = 1;
      rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &filter_config);

      /*发送数据*/
      struct rt_can_msg pmsg[1];
      pmsg[0].id = 0x01;
      pmsg[0].ide = 1;
      pmsg[0].rtr = 0;
      pmsg[0].len = 8;
      pmsg[0].priv= 0;
      pmsg[0].hdr = 1;
      pmsg[0].hdr = 1;
      for(i=0;i<8;i++)
      	{
                pmsg[0].data[i] = i+3;
      	}
      rt_device_write(can_dev, 0,&pmsg[0], 1*sizeof(struct rt_can_msg));

     // rt_device_close(can_dev);

}

/*测试发送数据*/
void test_rtt_cansnd(int no, int num)
{
    rt_device_t can_dev ;
    int i;
    struct rt_can_msg pmsg[1];
	  

    if(0 == no )
    {
      can_dev = rt_device_find("bxcan0");
    }
    else if(1 == no)
    {
      can_dev = rt_device_find("bxcan1");
    }
    else
    {
      return;  
    }
    
	  
      pmsg[0].id = 0x01;
      pmsg[0].ide = 1;
      pmsg[0].rtr = 0;
      pmsg[0].len = 8;
      pmsg[0].priv= 0;
      pmsg[0].hdr = 1;
      pmsg[0].hdr = 1;
      for(i=0;i<8;i++)
      {
                pmsg[0].data[i] = i+num;
      }
      rt_device_write(can_dev, 0,&pmsg[0], 1*sizeof(struct rt_can_msg));
}

/*测试接收数据线程*/
void test_rev_thread(void *parameter)
{
    rt_device_t can_dev = rt_device_find("bxcan0");
      int i;
    can_config(0);
    can_config(1);

      struct rt_can_msg pmsg[1];
      rt_size_t size;
      pmsg[0].id = 0x01;
      pmsg[0].ide = 1;
      pmsg[0].rtr = 0;
      pmsg[0].len = 8;
      pmsg[0].priv= 0;
      pmsg[0].hdr =0;

      
      for(i=0;i<8;i++)
      {
                pmsg[0].data[i] = i;
      }
      while(1)
      {
         size = rt_device_read(can_dev, 0,&pmsg[0], 16);

         if(size>0)
         {
           rt_kprintf("\r\n Size rev = %d .\r\n",size);
           rt_kprintf(" IDE=%d   RTR = %d DLC=%d  ",pmsg[0].ide, pmsg[0].rtr , pmsg[0].len);
           if(pmsg[0].ide  == CAN_Id_Standard)
           {
             rt_kprintf("\r\n Standard ID= %02X  ",pmsg[0].id);
           }
           else if(pmsg[0].ide == CAN_Id_Extended)
           {
             rt_kprintf("\r\n Extended ID= %02X  ",pmsg[0].id);
           }
           if(pmsg[0].rtr== CAN_RTR_DATA)
           {
                rt_kprintf("\r\ndata= ");
                for(i=0;i<8;i++)
                {
                rt_kprintf("0x%02X  ",pmsg[0].data[i]);
                }
           }
           else if(pmsg[0].rtr== CAN_RTR_Remote)
           {
                rt_kprintf("\r\nCAN_RTR_Remote  no data!");
           }
           rt_kprintf("\r\n");
           rt_memset (&pmsg[0], 0x00, sizeof( struct rt_can_msg));
           size = 0;
         }
         rt_thread_delay(RT_TICK_PER_SECOND/3);
     }
}

void test_rtt_canrev(void)
{
    rt_thread_t tid;

    /* create initialization thread */
    tid = rt_thread_create("can_rev",
                            test_rev_thread, RT_NULL,
                            4096, RT_THREAD_PRIORITY_MAX/3, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    rt_kprintf("\r\nTest CAN  Receive Thread Start========================================\r\n");
}

void test_rtt_csmsh(int argc, char** argv)
{
  unsigned int no,num;
	no = strtoul(argv[1], NULL, 0);
	num = strtoul(argv[2], NULL, 0);
	test_rtt_cansnd(no, num);
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_rtt_cansnd, test_rtt_cansnd  e.g.test_rtt_cansnd(0,1));
FINSH_FUNCTION_EXPORT(test_rtt_canrev, test_rtt_canrev  e.g.test_rtt_canrev);
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_csmsh, test_rtt_csmsh 0 1);
MSH_CMD_EXPORT(test_rtt_canrev, can rev test );
#endif
