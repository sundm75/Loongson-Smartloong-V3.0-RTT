#include <rtthread.h>

/* 定时器的控制块 */
static rt_timer_t timer1;
static rt_uint8_t count;

/* 定时器超时函数 */
static void timeout1(void* parameter)
{
    rt_kprintf("periodic timer is timeout\n");

    count ++;
    /* 当超过8次时，更改定时器的超时长度 */
    if (count >= 8)
    {
        int timeout_value = 500;
        /* 控制定时器更改定时器超时时间长度 */
        rt_timer_control(timer1, RT_TIMER_CTRL_SET_TIME, (void*)&timeout_value);
        count = 0;
    }
}

void test_timer_03(void)
{

	/* 创建定时器1 */
	timer1 = rt_timer_create("timer1",  /* 定时器名字是 timer1 */
							timeout1, /* 超时时回调的处理函数 */
							RT_NULL, /* 超时函数的入口参数 */
							100, /* 定时长度，以OS Tick为单位，即10个OS Tick */
							RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */
	/* 启动定时器 */
	if (timer1 != RT_NULL)
		rt_timer_start(timer1);
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_timer_03, timer test);