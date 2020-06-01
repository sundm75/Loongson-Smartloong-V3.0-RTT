 /**************************************************************************
修改自：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#include <rtthread.h>
#define DBG_LEVEL DBG_LOG
#include <rtdbg.h>

#include <board.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define UART_PORT USART4
#define UART_PORT_REV Usart4_Receive

/*外部用到的变量  标志位*/
uint8_t Remoter_Ch1;  //航模遥控1
uint8_t Remoter_Ch2;  //航模遥控2
uint8_t Flag_Qian;
uint8_t Flag_Hou;
uint8_t Flag_Left;
uint8_t Flag_Right;
uint8_t Show_Data_Mb; //串口接收的字节数
uint8_t PID_Send;
uint8_t Flash_Send;
uint8_t Flag_sudu; //2为低速挡（默认值）1为高速档
uint8_t Flag_Hover;
uint8_t Bi_zhang;
/*外部用到的变量  接收参数 PID参数*/
uint32_t Data;
float Balance_Kp;
float Balance_Kd;
float Velocity_Kp;
float Velocity_Ki;

/*定义邮箱存放收到的数据*/
/* 邮箱控制块 */
struct rt_mailbox mb;
/* 用于放邮件的内存池 */
char mb_pool[128];

uint8_t UART_PORT_REV; //串口接收上次暂存值
/**************************************************************************
函数功能：串口接收处理
入口参数：无
返回  值：无
**************************************************************************/
void USARTREV_Handler(uint8_t uart_receive)//从串口接收到一个字符
{	
  static uint8_t Flag_PID,i,j,Receive[50],Flag_CSB;
  static float Data;

  UART_PORT_REV=uart_receive;

  if(uart_receive==0x59)  Flag_sudu=2;  //低速挡（默认值）
  if(uart_receive==0x58)  Flag_sudu=1;  //高速档 
        
  if(uart_receive==0x62)Flag_Hover=!Flag_Hover;
  if(uart_receive==0x61)Flag_CSB=!Flag_CSB;
  if(Bi_zhang!=0)
  {		
         Bi_zhang=Flag_CSB+1;
  }
        
  if(uart_receive>10)  //默认使用app
  {			
    /* 发送数据到邮箱中 */
    if( ( uart_receive>='A' && uart_receive<='H')|| uart_receive=='Z')
      rt_mb_send(&mb, (rt_uint32_t)uart_receive);
    Remoter_Ch1=0,Remoter_Ch2=0; //APP接收到信号之后 屏蔽航模遥控
    if(uart_receive==0x5A)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
    else if(uart_receive==0x41)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////前
    else if(uart_receive==0x45)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////后
    else if(uart_receive==0x42||uart_receive==0x43||uart_receive==0x44)	
                                                                                            Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //左
    else if(uart_receive==0x46||uart_receive==0x47||uart_receive==0x48)	    //右
                                                                                            Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
    else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
  }
  if(uart_receive<10)     //备用app为：MiniBalanceV1.0  因为MiniBalanceV1.0的遥控指令为A~H 其HEX都小于10
  {		
    Remoter_Ch1=0,Remoter_Ch2=0; //APP接收到信号之后 屏蔽航模遥控			
    if(uart_receive==0x00)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
    else if(uart_receive==0x01)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////前
    else if(uart_receive==0x05)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////后
    else if(uart_receive==0x02||uart_receive==0x03||uart_receive==0x04)	
                                                                                            Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //左
    else if(uart_receive==0x06||uart_receive==0x07||uart_receive==0x08)	    //右
                                                                                            Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
    else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
  }	

  if(UART_PORT_REV==0x7B) Flag_PID=1;  //指令起始位
  if(UART_PORT_REV==0x7D) Flag_PID=2,rt_kprintf((char const*)&UART_PORT_REV),rt_kprintf("\r\n"); ;  //指令停止位
  if(Flag_PID==1)                      //记录参数
  {
    Receive[i]=UART_PORT_REV;
    rt_kprintf((char const*)&UART_PORT_REV); 
    i++;
  }
  if(Flag_PID==2)   //执行
  {
    Show_Data_Mb=i;
      if(Receive[3]==0x50) 	       PID_Send=1;     //获取设备参数
      else  if(Receive[3]==0x57) 	 Flash_Send=1;   //掉电保存参数
      else  if(Receive[1]!=0x23)                    //更新PID参数
      {								
        for(j=i;j>=4;j--)
        {
          Data+=(Receive[j-1]-48)*pow(10,i-j);
        }
        switch(Receive[1])
         {
           case 0x30:  Balance_Kp=Data/100;break;
           case 0x31:  Balance_Kd=Data/100;break;
           case 0x32:  Velocity_Kp=Data/100;break;
           case 0x33:  Velocity_Ki=Data/100;break;
           case 0x34:  break;
           case 0x35:  break;
           case 0x36:  break;
           case 0x37:  break;
           case 0x38:  break;
         }
      }	
      Flag_PID=0;   //相关标志位清零
      i=0;
      j=0;
      Data=0;
      memset(Receive, 0, sizeof(uint8_t)*50);
  } 
} 



#define SAMPLE_UART_NAME       "uart4"

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    char ch;

    while (1)
    {
        /* 从串口读取1个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, 0, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        /* 读取到的数据通过串口输出 */
        USARTREV_Handler(ch);
    }
}

int test_rtt_uart4(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "hello RT-Thread!\r\n";

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        LOG_I("find %s failed!", uart_name);
        return RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 20, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    
    /* 初始化一个mailbox */
    rt_mb_init(&mb,
        "mbt",             /* 名称是mbt */
        &mb_pool[0],       /* 邮箱用到的内存池是mb_pool */
        sizeof(mb_pool)/4, /* 大小是mb_pool/4，因为每封邮件的大小是4字节 */
        RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */
    
    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_uart4, uart device sample of sem);



void motor_control_entry(void* parameter)
{
  int speed = 500;
  rt_uint32_t data;
  while(1)
  {
    if(Flag_sudu==1)     speed = 600;  //高速档 
    else if(Flag_sudu==2) speed = 300; //低速挡（默认值）
    
    /* 从邮箱中收取邮件 */
    if (rt_mb_recv(&mb, (rt_ubase_t*)&data, 1000)
            == RT_EOK)
    {
        /* 显示邮箱内容 */
        rt_kprintf((char const*)&data);
        /*控制车辆*/
        // switch (data)
        // {
        // case 'A':function_up(speed);
        //   break;
        // case 'B':function_right_up(speed); 
        //   break;
        // case 'C':function_right(speed); 
        //   break;
        // case 'D':function_right_down(speed);
        //   break;
        // case 'E':function_down(speed); 
        //   break;
        // case 'F':function_left_down(speed);
        //   break;
        // case 'G': function_left(speed);
        //   break;
        // case 'H':function_left_up(speed);   
        //   break;
        // case 'Z':function_stop1();   
        //   break;
        // default:
        //   break;
        // }
    }
    
  }
   
}
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5
static rt_thread_t tid1 = RT_NULL;

int test_hc05(void)
{
 
    test_rtt_uart4();    
    /*串口接收数据控制线程*/
    tid1 = rt_thread_create("thread1",
            motor_control_entry, 
            RT_NULL,
            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE); 
    if (tid1 != RT_NULL)
            rt_thread_startup(tid1);        
    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_hc05, uart device sample of sem);