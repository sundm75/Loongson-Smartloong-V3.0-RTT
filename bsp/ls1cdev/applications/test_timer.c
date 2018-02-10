/*
 * File      : test_timer.c
 测试硬件定时器 在finsh 中运行 
1.test_timer_poll_time_out()  测试硬件定时器的定时功能(读取中断状态位的方式判断是否超时)
2.test_timer_get_time ()
* 用硬件定时器计时
 * 用硬件定时器测量红外接收头HX1838产生的波形中高低电平的时长，
 * 来识别红外遥控板上是那个按键被按下，选择NEC编码方式的红外遥控器
 */


#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"


// 一条红外命令的长度
#define IR_MAX_BYTE_NUM                 (4)                             // 一条红外命令包括4字节数据
#define IR_MAX_BIT_NUM                  (1 + IR_MAX_BYTE_NUM * 8)       // 1bit的起始信号和4个字节

// 红外命令中一个bit数据的最大时长
#define IR_ONE_BIT_MAX_TIME             (20*1000*1000)


// 红外命令中一个bit的信息
typedef struct
{
    unsigned long low_time_ns;          // 低电平时长，单位ns
    unsigned long high_time_ns;         // 高电平时长，单位ns
    unsigned int bit;                   // 比特值，0或1
}ir_one_bit_t;


// 测试硬件定时器的定时功能(读取中断状态位的方式判断是否超时)
void test_timer_poll_time_out(void)
{
    unsigned int gpio = 6;
    timer_info_t timer_info = {0};

    gpio_init(gpio, gpio_mode_output);
    gpio_set(gpio,  gpio_level_high);

    while (1)
    {
        // 定时10us
        timer_info.timer    = TIMER_PWM0;
        timer_info.time_ns  = 10*1000;
        timer_init(&timer_info);
        while (! timer_is_time_out(&timer_info))    // 等待，直到超时
            ;
//        timer_print_regs(&timer_info);            // 调试时，打印寄存器信息
//        timer_print_regs(&timer_info);
//        timer_print_regs(&timer_info);
        gpio_set(gpio, gpio_level_low);

        // 定时10ms
        timer_info.time_ns  = 10*1000*1000;
        timer_init(&timer_info);
        while (! timer_is_time_out(&timer_info))
            ;
        gpio_set(gpio, gpio_level_high);
    }
}

/*
 * 用硬件定时器计时
 * 用硬件定时器测量红外接收头HX1838产生的波形中高低电平的时长，
 * 来识别红外遥控板上是那个按键被按下，选择NEC编码方式的红外遥控器
 */
void test_timer_get_time(void)
{
    unsigned int ir_gpio = 47;          // 红外接收头的数据引脚
    timer_info_t timer_info = {0};      // 硬件定时器信息
    ir_one_bit_t ir_bits[IR_MAX_BIT_NUM];
    unsigned char ir_bytes[IR_MAX_BYTE_NUM];
    int tmp, i, j;
    unsigned long time_ns = 0;
    gpio_init(ir_gpio, gpio_mode_input);
    
    timer_info.timer = TIMER_PWM0;              // PWM0用作硬件定时器
    timer_info.time_ns = IR_ONE_BIT_MAX_TIME;   // 20ms

    // 等待，直到低电平(起始信号)出现
    while (gpio_get(ir_gpio))
        ;

    // 接收一条完整的红外命令
    for (tmp=0; tmp<IR_MAX_BIT_NUM; tmp++)
    {
        // 开始计时
        timer_init(&timer_info);                    // 开始计时

        // 等待，直到高电平或定时器超时为止
        while ((!gpio_get(ir_gpio)) && (!timer_is_time_out(&timer_info)))
            ;

        // 获取时间，并重新开始计时
        time_ns = timer_get_time_ns(&timer_info);
        timer_init(&timer_info);
        if (IR_ONE_BIT_MAX_TIME == time_ns)     // 判断是否超时，超时意味着一条红外命令接收完成
            break;
        ir_bits[tmp].low_time_ns = time_ns;

        // 等待，直到低电平或定时器超时为止
        while ((gpio_get(ir_gpio)) && (!timer_is_time_out(&timer_info)))
            ;

        // 获取时间，并重新计时
        time_ns = timer_get_time_ns(&timer_info);
        timer_init(&timer_info);
        if (IR_ONE_BIT_MAX_TIME == time_ns)     // 判断是否超时
            break;
        ir_bits[tmp].high_time_ns = time_ns;
    }

    // 将高低电平信息转换为bit值
    for (tmp=0; tmp<IR_MAX_BIT_NUM; tmp++)
    {
        // 根据每个bit的高电平时长，判断该bit是0，还是1
        if (1*1000*1000 < ir_bits[tmp].high_time_ns)        // >1ms为1
        {
            ir_bits[tmp].bit = 1;
        }
        else                                                // <1ms为0
        {
            ir_bits[tmp].bit = 0;
        }
    }

    // 打印详细的红外命令信息
    myprintf("\n----------------one ir cmd---------------\n");
    for (tmp=0; tmp<IR_MAX_BIT_NUM; tmp++)
    {
        myprintf("low_time_ns=%d, high_time_ns=%d, bit=%d\n",
                  ir_bits[tmp].low_time_ns,
                  ir_bits[tmp].high_time_ns,
                  ir_bits[tmp].bit);
    }

    // 将bit值组合成红外命令
    for (i=0; i<IR_MAX_BYTE_NUM; i++)
    {
        tmp = 0;
        for (j=0; j<8; j++)
        {
            tmp = tmp + (ir_bits[1+i*8+j].bit << j);
        }
        ir_bytes[i] = tmp;
    }

    // 检查命令是否正确，正确则打印红外命令
    if ((ir_bytes[0] == (unsigned char)(~ir_bytes[1])) 
        && (ir_bytes[2] == (unsigned char)(~ir_bytes[3])))
    {
        myprintf("[%s] ir cmd=0x%02x%02x\n", __FUNCTION__, ir_bytes[0], ir_bytes[2]);
    }
    else
    {
        myprintf("[%s] ir cmd err. cmd=0x%x, 0x%x, 0x%x, 0x%x\n",
                  __FUNCTION__, ir_bytes[0], ir_bytes[1], ir_bytes[2], ir_bytes[3]);
    }

    return ;
}
