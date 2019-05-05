/*
file: test_watchdog.c
测试看门狗驱动， 在finsh 中运行
最大定时时间：
0xffffffff / 126000000 = 34 S 
1. test_wdog(10)   配置看门狗重启时间为 10 秒
   test_wdog_msh 10 
 */

#include <rtthread.h>
#include <stdlib.h> 
#include "ls1c_public.h"  
#include "ls1c_wdog.h"
#include "ls1c.h"
#include "ls1c_regs.h"
#include "ls1c_clock.h"
#include "ls1c_delay.h"

void test_wdog(unsigned int second)
{
	unsigned int cnt;
    int i = 1;
    cnt = clk_get_apb_rate() * second;
    Wdog_Init();
    Wdog_Enable();
    Wdog_LoadValue(cnt);
    printf("\r\nWDT_SET = %d S  cnt=0x%x \r\n",second, cnt);
    Wdog_Set();
    while(1)
    {
        /*喂狗*/
        Wdog_LoadValue(cnt);
        Wdog_Set();
        /*喂狗结束*/
        printf("\r\n i= %d  ", i++);
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}  

void test_wdog_msh(int argc, char** argv)
{
    int num1;
	num1 = strtoul(argv[1], NULL, 0);
	test_wdog(num1);
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_wdog , test_wdog  e.g.test_wdog(10));
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_wdog_msh , e.g.test_wdog_msh 10);


