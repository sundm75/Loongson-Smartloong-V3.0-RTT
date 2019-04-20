/*
 * File      : test_timer.c
 测试硬件定时器 在msh 中运行 
1.test_timer_poll  测试硬件定时器的定时功能(读取中断状态位的方式判断是否超时)
2.test_timer_get 将47引脚接到 HX1838 接收引脚，可读出引脚高电平的时间
3. test_pwmint_msh 1 100000 50000 测试产生定时周期100000us 高电平宽度5000us 定时，在中断中操作LED1
* 用硬件定时器计时
 * 用硬件定时器测量红外接收头HX1838产生的波形中高低电平的时长，
 * 来识别红外遥控板上是那个按键被按下，选择NEC编码方式的红外遥控器
 */

#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_timer.h"
#include "../libraries/ls1c_regs.h"
#include "../libraries/ls1c_pwm.h"

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
void test_timer_poll(void)
{
    unsigned int gpio = 53;
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
void test_timer_get(void)
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
    rt_kprintf("\n----------------one ir cmd---------------\n");
    for (tmp=0; tmp<IR_MAX_BIT_NUM; tmp++)
    {
        rt_kprintf("low_time_ns=%d, high_time_ns=%d, bit=%d\n",
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
        rt_kprintf("[%s] ir cmd=0x%02x%02x\n", __FUNCTION__, ir_bytes[0], ir_bytes[2]);
    }
    else
    {
        rt_kprintf("[%s] ir cmd err. cmd=0x%x, 0x%x, 0x%x, 0x%x\n",
                  __FUNCTION__, ir_bytes[0], ir_bytes[1], ir_bytes[2], ir_bytes[3]);
    }

    return ;
}

// 测试pwm定时器中断
#define led_gpio 52
#define test_pin 6

void ls1c_test_pwm_timer_irqhandler(int irq, void *param)  
{  
    timer_info_t timer_info;
    static unsigned long cnt = 0;
    unsigned int timer_reg_base = 0;

    timer_info.timer = irq - LS1C_PWM0_IRQ;  
    timer_reg_base = LS1C_REG_BASE_PWM0 + 0x10 * timer_info.timer ;
    cnt = reg_read_32((volatile unsigned int *)(timer_reg_base + LS1C_PWM_CNTR));
    timer_int_clr(&timer_info);
    /*进入了低中断 */
    if (cnt>= reg_read_32((volatile unsigned int *)(timer_reg_base + LS1C_PWM_LRC)))
    {    
        timer_cnt_clr(&timer_info);
        timer_int_clr(&timer_info);
        gpio_set(led_gpio, gpio_level_low ); //点亮
        gpio_set(test_pin, gpio_level_low ); //点亮
    }
    else /*进入了高中断*/
    {
        gpio_set(led_gpio, gpio_level_high); //熄灭
        gpio_set(test_pin, gpio_level_high); //熄灭
    }
}  

//测试硬件pwm产生timer中断，产生周期中断，LRC为低中断，HRC为高中断
// pwm_n 为使用的pwm号
// period为定时周期（LRC），time_width为定时宽度（HRC） 单位us,最大133152
void test_pwm_int(int pwm_n, int period, int time_width)
{
    int i;
    pwm_info_t pwm_info;
    timer_info_t timer_info;
    rt_uint32_t ram_addr;
    int pwm_timer_irq = LS1C_PWM0_IRQ + pwm_n; 

    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_low );
    gpio_init(test_pin, gpio_mode_output);
    gpio_set(test_pin, gpio_level_low );
    rt_thread_delay(10);

    timer_info.timer = pwm_n;  
    timer_info.time_ns = period * 1000;       //133152 us 周期最大 = 133ms
    timer_info.timer = pwm_n;  
    timer_info.time_h_ns = time_width * 1000;       
    timer_int_init(&timer_info, 1, 1); //

    rt_hw_interrupt_install(pwm_timer_irq, ls1c_test_pwm_timer_irqhandler, RT_NULL, "pwm_timer"); 
    rt_hw_interrupt_umask(pwm_timer_irq); 
    ram_addr = LS1C_REG_BASE_PWM0 +  0x10 * timer_info.timer + LS1C_PWM_LRC;
    rt_kprintf("\r\n LRC 0x%x = 0x%x  \r\n", ram_addr, __REG32(ram_addr));
    ram_addr = LS1C_REG_BASE_PWM0 +  0x10 * timer_info.timer + LS1C_PWM_HRC;
    rt_kprintf("\r\n HRC 0x%x = 0x%x  \r\n", ram_addr, __REG32(ram_addr));
}

void test_pwmint_msh(int argc, char** argv)
{
    rt_uint32_t pwm_n;
    int period;
    int time_width;
	pwm_n = strtoul(argv[1], NULL, 0);
    period = strtoul(argv[2], NULL, 0);
    time_width = strtoul(argv[3], NULL, 0);
    test_pwm_int(pwm_n, period, time_width);   
}

/*
 * 用硬件定时器计时
 * 用硬件定时器测量红外接收头HS0038产生的波形中高低电平的时长，
 * 来识别红外遥控板上是那个按键被按下
 * 编码规则： 同步码头 9ms的低电平+个4.5ms的高电平
 * “0”、“1”均以0.56ms的高电平开始，低电平的宽度“0”为0.56ms,“1”为1.685ms
 * 
 */
void test_HS0038(void)
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
    rt_kprintf("\n----------------one ir cmd---------------\n");
    for (tmp=0; tmp<IR_MAX_BIT_NUM; tmp++)
    {
        rt_kprintf("low_time_ns=%d, high_time_ns=%d, bit=%d\n",
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
        rt_kprintf("[%s] ir cmd=0x%02x%02x\n", __FUNCTION__, ir_bytes[0], ir_bytes[2]);
    }
    else
    {
        rt_kprintf("[%s] ir cmd err. cmd=0x%x, 0x%x, 0x%x, 0x%x\n",
                  __FUNCTION__, ir_bytes[0], ir_bytes[1], ir_bytes[2], ir_bytes[3]);
    }

    return ;
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_pwm_int, test_pwm_int e.g.test_pwm_int(1,100000,50000));
FINSH_FUNCTION_EXPORT(test_timer_poll, test_timer_poll e.g.test_timer_poll());
FINSH_FUNCTION_EXPORT(test_timer_get, test_timer_get e.g.test_timer_get());

MSH_CMD_EXPORT(test_pwmint_msh, test_pwmint_msh 1 100000 50000);// PWM号： 0~3， 周期：10~133000 us  高中断时间： 大于10小于周期
MSH_CMD_EXPORT(test_timer_poll, test_timer_poll time out );
MSH_CMD_EXPORT(test_timer_get, test_timer_get time  );
