/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date                 Author         Notes
 * 2010-06-25     Bernard        first version
 * 2011-08-08     lgnq             modified for Loongson LS1B
 * 2015-07-06     chinesebear  modified for Loongson LS1C
 */

#include <rthw.h>
#include <rtthread.h>



/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* disable interrupt first */
    rt_hw_interrupt_disable();

    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_show_version();


    /* init scheduler system */
    rt_system_scheduler_init();

    /* initialize timer */
    rt_system_timer_init();

    /* initialize timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* init application */
    rt_application_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return;
}

/*@}*/
