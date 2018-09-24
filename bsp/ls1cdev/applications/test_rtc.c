/*
file: test_rtc.c
测试rtc驱动， 在finsh 中运行
1. test_rtc_set()   配置时间为2018.1.1 01:01:01
2. test_rtc_get()   显示当前时间
 */

#include <rtthread.h>
#include <stdlib.h>  
#include "ls1c_regs.h"
#include "ls1c_rtc.h"

RTC_TypeDef *RTC_Handler = RTC; 

void test_rtc_set(void)  
{
	RTC_TimeTypeDef rtcDate; 
	rtcDate.Date = 1;
	rtcDate.Hours = 1;
	rtcDate.Minutes = 1;
	rtcDate.Month = 1;
	rtcDate.Seconds = 1;
	rtcDate.Year = 18;
	
	RTC_SetTime(RTC_Handler, &rtcDate);
}  

void test_rtc_get(void)  
{
	RTC_TimeTypeDef rtcDate; 
	RTC_GetTime(RTC_Handler, &rtcDate); 
	rt_kprintf("\r\nrtc time is %d.%d.%d - %d:%d:%d\r\n",rtcDate.Year, rtcDate.Month, rtcDate.Date,rtcDate.Hours, rtcDate.Minutes, rtcDate.Seconds);		
} 

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_rtc_set , test_rtc_set  e.g.test_rtc_set());
FINSH_FUNCTION_EXPORT(test_rtc_get , test_rtc_get  e.g.test_rtc_get());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtc_set, set time );
MSH_CMD_EXPORT(test_rtc_get, get time);


