/*
file: test_rtt_wdt.c
测试看门狗 rtt 驱动， 在finsh 中运行
1. wdt_test (10, 0)   配置看门狗重启时间为 10 秒 模式 0（表示轮询方式）
                      模式 1 为中断方式，目前不能使用
    wdt_test_msh 10 0 
 */

#include <rtdef.h>
#include <rtthread.h>
#include <rtdevice.h>

#ifdef  RT_USING_WDT

#include <drivers/watchdog.h>
#include "drv_wdt.h"

void wdt_test(int timeout, int mode)
{
    rt_device_t wdt_dev;

    wdt_dev = rt_device_find("wdt");
    if (!wdt_dev)
    {
        rt_kprintf("cann't find the wdt dev\n");
    }

    rt_device_open(wdt_dev, 0);

    rt_device_control(wdt_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    rt_device_control(wdt_dev, RT_DEVICE_CTRL_WDT_START, &mode);
    rt_kprintf("system shall reboot in %d seconds.\n", timeout);

    for (; timeout > 0; timeout--)
    {
        rt_kprintf("kicked %d\n", timeout);
        rt_thread_delay(RT_TICK_PER_SECOND);
        /*以下语句喂狗， 删除后，则在10秒后立即重启；
        如果保留，则在10内一直喂狗，10定时时间后再过10秒重启*/
        rt_device_control(wdt_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    }
}

void wdt_test_msh(int argc, char** argv)
{
    int num1,num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);
	wdt_test(num1, num2);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(wdt_test,  enable wdt e.g.wdt_test (10, 0));
MSH_CMD_EXPORT(wdt_test_msh, wdt_test_msh 10 0);
#endif

#endif
