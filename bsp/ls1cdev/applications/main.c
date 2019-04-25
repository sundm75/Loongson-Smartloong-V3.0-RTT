/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 */

#include <rtthread.h>

int main(int argc, char** argv)
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
   
    rt_thread_delay(15000);

    return 0;
}
