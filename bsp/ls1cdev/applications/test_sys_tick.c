// 测试滴答定时器的源文件

#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_delay.h"


/*
 * 测试滴答定时器
 */
void test_sys_tick(void)
{
    int i;
    
    // 滴答定时器初始化
    // 每次滴答定时器中断就会将tick加一，这里只需要读取tick值，并打印出来即可
    rt_kprintf("sys tick init ok! tick = %u\r\n", RT_TICK_PER_SECOND);
    for (i=0; i<5; i++)
    {
        rt_kprintf("[%s] tick=%d\r\n", __FUNCTION__, rt_tick_get());
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}
#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_sys_tick, test_sys_tick  e.g.test_sys_tick());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_sys_tick, test_sys_tick);
