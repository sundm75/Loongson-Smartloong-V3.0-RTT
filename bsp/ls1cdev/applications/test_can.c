/*
 * File      : test_can.c
测试硬件can驱动， 在finsh 中运行 
1.test_cansend() 初始化CAN口（100K）后，以速率100K, 发送标准数据帧（1-8 共8个数据）和扩展数据帧（1-8 共8个数据)
2. candump() 打印出当前CAN寄存器值
3. 当收到数据，打印出来。
 */

 
 #include <rtthread.h>
#include "ls1c.h"

#include "ls1c_public.h"
#include "ls1c_regs.h"
#include "ls1c_clock.h"
#include "ls1c_can.h"
#include "ls1c_pin.h"

CanRxMsg RxMessage;
CanTxMsg TxMessage;

void candump(void)
{
    CAN_TypeDef* CANx;
    unsigned char temp;
    CANx =  CAN0;
    int i;

    temp =  CANx->MOD;
    rt_kprintf("\r\ncan0->MOD= 0x%02x  \r\n",temp);   

    temp =  CANx->CMR;
    rt_kprintf("\ncan0->CMR = 0x%02x  \r\n",temp);  

    temp =  CANx->SR;
    rt_kprintf("\ncan0->SR = 0x%02x  \r\n",temp); 

    temp =  CANx->IR;
    rt_kprintf("\ncan0->IR = 0x%02x  \r\n",temp); 

    temp =  CANx->IER;
    rt_kprintf("\ncan0->IER = 0x%02x  \r\n",temp); 

    temp =  CANx->BTR0;
    rt_kprintf("\ncan0->BTR0 = 0x%02x  \r\n",temp); 

    temp =  CANx->BTR1;
    rt_kprintf("\ncan0->BTR1 = 0x%02x  \r\n",temp); 

    temp =  CANx->CDR;
    rt_kprintf("\ncan0->CDR = 0x%02x  \r\n",temp); 

    temp =  CANx->RMCR;
    rt_kprintf("\ncan0->RMCR = 0x%02x  \r\n",temp); 

    rt_kprintf("\r\n验收代码:");
    temp =  CANx->IDE_RTR_DLC;
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->ID[0];
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->ID[1];
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->ID[2];
    rt_kprintf("  0x%02x ",temp); 

    rt_kprintf("\r\n验收屏蔽:");
    temp =  CANx->ID[3];
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->BUF[0];
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->BUF[1];
    rt_kprintf("  0x%02x ",temp); 
    temp =  CANx->BUF[2];
    rt_kprintf("  0x%02x ",temp); 

}

void ls1c_can0_irqhandler(int irq, void *param)  
{  
    CAN_TypeDef* CANx;
    int i;
    unsigned char status;
    CANx =  CAN0;
    /*读寄存器清除中断*/
   status = CANx->IR;
   rt_kprintf("\r\ncan0 int happened!\r\n");
    /*接收中断*/
    if (( status & CAN_IR_RI) == CAN_IR_RI) 
    {
        /*清除RI 中断*/
       CAN_Receive(CANx, &RxMessage);
       CANx->CMR |= CAN_CMR_RRB; 
       CANx->CMR |= CAN_CMR_CDO; 
       rt_kprintf("\r\ncan0 receive:\r\n");
       rt_kprintf(" IDE=%d   RTR = %d DLC=%d  ",RxMessage.IDE, RxMessage.RTR ,RxMessage.DLC);
       if(RxMessage.IDE == CAN_Id_Standard)
       {
         rt_kprintf("\r\n Standard ID= %02X  ",RxMessage.StdId);
       }
       else if(RxMessage.IDE == CAN_Id_Extended)
       {
         rt_kprintf("\r\n Extended ID= %02X  ",RxMessage.ExtId);
       }

       if(RxMessage.RTR== CAN_RTR_DATA)
       {
         rt_kprintf("\r\ndata= ");
         for(i=0;i<RxMessage.DLC;i++)
         {
           rt_kprintf("0x%02X  ",RxMessage.Data[i]);
         }
       }
       else if(RxMessage.IDE == CAN_RTR_Remote)
       {
         rt_kprintf("\r\nCAN_RTR_Remote  no data!");
       }
       rt_kprintf("\r\n");
    }
    /*发送中断*/
    else if (( status  & CAN_IR_TI) == CAN_IR_TI) 
    {
       rt_kprintf("\r\ncan0 send success! \r\n");
    }
}  

void ls1c_can1_irqhandler(int irq, void *param)  
{  
    CAN_TypeDef* CANx;
    int i;
    unsigned char status;
    CANx =  (CAN_TypeDef* )LS1C_REG_BASE_CAN1;
    /*读寄存器清除中断*/
   status = CANx->IR;
   rt_kprintf("\r\ncan1 int happened!\r\n");
    /*接收中断*/
    if (( status & CAN_IR_RI) == CAN_IR_RI) 
    {
        /*清除RI 中断*/
       CAN_Receive(CANx, &RxMessage);
       CANx->CMR |= CAN_CMR_RRB; 
       CANx->CMR |= CAN_CMR_CDO; 
       rt_kprintf("\r\ncan1 receive:\r\n");
       rt_kprintf(" IDE=%d   RTR = %d DLC=%d  ",RxMessage.IDE, RxMessage.RTR ,RxMessage.DLC);
       if(RxMessage.IDE == CAN_Id_Standard)
       {
         rt_kprintf("\r\n Standard ID= %02X  ",RxMessage.StdId);
       }
       else if(RxMessage.IDE == CAN_Id_Extended)
       {
         rt_kprintf("\r\n Extended ID= %02X  ",RxMessage.ExtId);
       }

       if(RxMessage.RTR== CAN_RTR_DATA)
       {
         rt_kprintf("\r\ndata= ");
         for(i=0;i<RxMessage.DLC;i++)
         {
           rt_kprintf("0x%02X  ",RxMessage.Data[i]);
         }
       }
       else if(RxMessage.IDE == CAN_RTR_Remote)
       {
         rt_kprintf("\r\nCAN_RTR_Remote  no data!");
       }
       rt_kprintf("\r\n");
    }
    /*发送中断*/
    else if (( status  & CAN_IR_TI) == CAN_IR_TI) 
    {
       rt_kprintf("\r\ncan1 send success! \r\n");
    }
}  

void Can_Config(CAN_TypeDef* CANx, Ls1c_CanBPS_t bps)
{
    unsigned char initresult;

    pin_set_purpose(54, PIN_PURPOSE_OTHER);
    pin_set_purpose(55, PIN_PURPOSE_OTHER);
    pin_set_remap(54, PIN_REMAP_THIRD);
    pin_set_remap(55, PIN_REMAP_THIRD);
    
    CAN_InitTypeDef        CAN_InitStructure;
  
    CAN_InitStructure.CAN_Mode = CAN_Mode_STM;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

    /* BaudRate= f(APB)/((1+BS1+BS2)(SJW*2*Prescaler))=126000000/[(1+7+2)*1*2*63]=100000=100K*/
    /* BPS     PRE   BS1   BS2   最低40K
       1M      9       4       2   
       800K    8       7       2   
       500K    9       11      2   
       250K    36      4       2   
       125K    36      11      2   
       100K    63      7       2   
       50K     63      16      3`  
       40K     63      16      8   
    */
    switch (bps)
    {
        case CAN1MBaud: 
            CAN_InitStructure.CAN_Prescaler = 9;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN800kBaud: 
            CAN_InitStructure.CAN_Prescaler = 8;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN500kBaud: 
            CAN_InitStructure.CAN_Prescaler = 9;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN250kBaud: 
            CAN_InitStructure.CAN_Prescaler = 36;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN125kBaud: 
            CAN_InitStructure.CAN_Prescaler = 36;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN100kBaud: 
            CAN_InitStructure.CAN_Prescaler = 63;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
        case CAN50kBaud: 
            CAN_InitStructure.CAN_Prescaler = 63;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_16tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
        break;
        case CAN40kBaud: 
            CAN_InitStructure.CAN_Prescaler = 63;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_16tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
        break;
        default: //100K
            CAN_InitStructure.CAN_Prescaler = 63;
            CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
        break;
    }
    initresult = CAN_Init(CANx, &CAN_InitStructure);// 配置成100K
	
    /*无验收屏蔽*/
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    CAN_FilterInitStructure.CAN_FilterId0  = 0x00;
    CAN_FilterInitStructure.CAN_FilterMaskId0 = 0xFF;
    CAN_FilterInitStructure.CAN_FilterId1  = 0x00;
    CAN_FilterInitStructure.CAN_FilterMaskId1 = 0xFF;
    CAN_FilterInitStructure.CAN_FilterId2  = 0x00;
    CAN_FilterInitStructure.CAN_FilterMaskId2 = 0xFF;
    CAN_FilterInitStructure.CAN_FilterId3  = 0x00;
    CAN_FilterInitStructure.CAN_FilterMaskId3 = 0xFF;
    CAN_FilterInit(CANx, &CAN_FilterInitStructure);	
	
    if( (CAN_TypeDef* )LS1C_REG_BASE_CAN0 == CANx)
    {
       /* 初始化CAN0接收中断*/
      rt_hw_interrupt_install(LS1C_CAN0_IRQ, ls1c_can0_irqhandler, RT_NULL, "can0");  
      rt_hw_interrupt_umask(LS1C_CAN0_IRQ); 
     }
    else if( (CAN_TypeDef* )LS1C_REG_BASE_CAN1 == CANx)
    {
       /* 初始化CAN1接收中断*/
      rt_hw_interrupt_install(LS1C_CAN1_IRQ, ls1c_can1_irqhandler, RT_NULL, "can1");  
      rt_hw_interrupt_umask(LS1C_CAN1_IRQ); 
    }
}

void test_cansend(void)
{
    unsigned char temp;
    unsigned char initresult;

    Can_Config(CAN0, CAN100kBaud);

    int i;
    TxMessage.StdId = 1;
    TxMessage.ExtId = 1;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.DLC = 8;
    for( i=0; i<8;i++)
    {
      TxMessage.Data[i] = i+1;
      rt_kprintf("%02x ",  TxMessage.Data[i]);
    }
    CAN_Transmit(CAN0, &TxMessage);
    
    TxMessage.StdId = 1;
    TxMessage.ExtId = 2;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_Id_Extended;
    TxMessage.DLC = 8;
    for( i=0; i<8;i++)
    {
      TxMessage.Data[i] = i+1;
      rt_kprintf("%02x ",  TxMessage.Data[i]);
    }
    CAN_Transmit(CAN0, &TxMessage);

  	
}

 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_cansend, test_cansend  e.g.test_cansend());
FINSH_FUNCTION_EXPORT(candump, candump  e.g.candump());


