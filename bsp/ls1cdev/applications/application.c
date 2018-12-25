/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date                Author         Notes
 * 2010-06-25          Bernard        first version
 * 2011-08-08          lgnq           modified for Loongson LS1B
 * 2015-07-06          chinesebear    modified for Loongson LS1C
 */

#include <rtthread.h>
#include "net/synopGMAC.h"
#include <lwip/api.h>


void rt_init_thread_entry(void *parameter)
{
    #ifdef XPT2046_USING_TOUCH
        /*触摸屏使用SPI总线SPI1 CS0  初始化*/
            rtgui_touch_hw_init("spi10");
    #endif
        rt_kprintf("Loongson系统初始化完成！\n");
    #if (defined RT_USING_RTGUI) && (defined RTGUI_USING_DEMO)
    #ifdef USING_DEMO
    extern int test_guidemo(void);
        test_guidemo();
    #endif
    #ifdef USING_UI_BUTTON
        ui_button();
    #endif
    #endif	
}

int rt_application_init(void)
{
    rt_thread_t tid;

    /* create initialization thread */
    tid = rt_thread_create("init",
                            rt_init_thread_entry, RT_NULL,
                            4096, RT_THREAD_PRIORITY_MAX/3, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
