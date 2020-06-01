/*
 * File      :gpio_func.c
配置端口复用 
*/
#include <rtthread.h>
#include <stdlib.h> 
#include "ls1c_public.h"
#include "ls1c_gpio.h"
#include "ls1c_delay.h"
#include "ls1c_regs.h"
#include "ls1c_pin.h"

int  gpio_func(int argc, char** argv)
{
    int i;
    unsigned int num1, num2;
	num1 = strtoul(argv[1], NULL, 0);
	num2 = strtoul(argv[2], NULL, 0);

    pin_set_purpose(num1, PIN_PURPOSE_OTHER);
    if( num2 == 0)
    {
        pin_set_purpose(num1, PIN_PURPOSE_GPIO);
        pin_set_remap(num1, PIN_REMAP_DEFAULT);
        
    }
    else
    {
        pin_set_remap(num1, num2-1);
    }
    return 0;
}

int  bit_get(int argc, char *argv[])
{
    int i;


    
    return 0;
}

int  bit_set(int argc, char *argv[])
{
    int i;


    
    return 0;
}

int  bit_clr(int argc, char *argv[])
{
    int i;


    
    return 0;
}
#include  <finsh.h> 
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(gpio_func, gpio_func );
MSH_CMD_EXPORT(bit_get, bit_get );
MSH_CMD_EXPORT(bit_set, bit_set );
MSH_CMD_EXPORT(bit_clr, bit_clr );
