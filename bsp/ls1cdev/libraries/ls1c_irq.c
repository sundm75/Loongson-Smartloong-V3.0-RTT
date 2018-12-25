/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-09-06     勤为本       first version
 */

/*************************************************************************
 *
 * 中断相关的函数
 * 龙芯1c中中断是一种类型的异常，
 * 可以理解为cpu中有几种异常，而中断是其中一种异常下面的一个子类型
 * 龙芯1c的异常分为四级
 * 第一级: 各种情况下异常向量的总入口
 * 第二级: 各个异常的入口，其中ExcCode=0的异常为外设中断的总入口
 * 第三级: 外设中断的每组的总入口(龙芯1c将所有外设分为五组)
 * 第四级: 每个外设中断的入口
 *
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ls1c_clock.h"
#include "ls1c_gpio.h"



// 每个异常有0x80字节的空间
#define EXCEPTION_HANDER_MAX_SIZE               (0x80)

// 正常运行时，异常的入口基地址
// 正常运行时，STATUS寄存器的BEV=0，cpu采用RAM空间的异常入口
// 0x80000000地址处不经TLB映射，但缓存
#define EXCEPTION_RAM_EBASE                     (0x80000000)

// 异常的最大个数
#define EXCEPTION_MAX_NUM                       (32)

#define LS1C_INTREG_BASE 0xbfd01040


struct ls1c_intc_regs
{
	volatile unsigned int int_isr;
	volatile unsigned int int_en;
	volatile unsigned int int_set;
	volatile unsigned int int_clr;		/* offset 0x10*/
	volatile unsigned int int_pol;
   	volatile unsigned int int_edge;		/* offset 0 */
}; 


unsigned long exception_handlers[EXCEPTION_MAX_NUM];



// 汇编实现的函数
extern void irq_disable(void);
extern void irq_enable(void);
extern void general_exception(void);
extern void handle_int(void);
extern void handle_reserved(void);


// 设置整个异常向量的处理函数
// @offset 异常向量总入口的偏移
// @src_addr 异常向量总入口处理函数的首地址
void irq_set_exception_vector_handler(unsigned long offset, void *src_addr, unsigned long size)
{
    unsigned long dst_addr;   // 异常入口

    dst_addr = EXCEPTION_RAM_EBASE+offset;
    memcpy((void *)dst_addr, src_addr, size);

    // 先回写dcache，再作废icache
    // memcpy之后，现在异常向量总入口的指令位于dcache中，需要回写到内存，
    // 并作废相应icache，作废后当有中断发生时，才会从内存重新加载新代码到icache，这样新代码就生效了
    //dcache_writeback_invalidate_range(dst_addr, dst_addr + size);
    //icache_invalidate_range(dst_addr, dst_addr + size);

    return ;
}



// 设置一个异常的处理函数
// @n 协处理器0的cause寄存器的[2,6]位，即ExcCode
// @addr 异常处理函数的首地址
void irq_set_one_exception_handler(int n, void *addr)
{
    unsigned long handler = (unsigned long)addr;
    exception_handlers[n] = handler;

    return ;
}


// 初始化中断
void irq_init(void)
{
    int i;
    
    // 禁止中断
    irq_disable();

    // 初始化高速缓存
    //cache_init();
    
    // 设置整个异常向量的处理函数
    irq_set_exception_vector_handler(0x180, &general_exception, EXCEPTION_HANDER_MAX_SIZE);
    irq_set_exception_vector_handler(0x200, &general_exception, EXCEPTION_HANDER_MAX_SIZE);

    // 设置各个异常的处理函数
    for (i=0; i<EXCEPTION_MAX_NUM; i++)
    {
        irq_set_one_exception_handler(i, handle_reserved);
    }
    irq_set_one_exception_handler(0, handle_int);

    // 先回写整个dcache，再作废整个icache
    //dcache_writeback_invalidate_all();
    //icache_invalidate_all();

    // 使能中断
    irq_enable();

    return ;
}


// 中断分发函数
void irq_dispatch(void)
{
/*
    unsigned int pending;

    pending = read_c0_cause() & read_c0_status() & ST0_IM;

    if (pending & CAUSEF_IP7)
    {
        sys_tick_handler();
    }
    else if (pending & CAUSEF_IP2)
    {
    }
    else if (pending & CAUSEF_IP3)
    {
    }
    else if (pending & CAUSEF_IP4)
    {
    }
    else if (pending & CAUSEF_IP5)
    {
    }
    else if (pending & CAUSEF_IP6)
    {
    }
    else
    {
        // 其它情况不处理
    }
*/

    gpio_set(32, gpio_level_high);

    return ;
}



