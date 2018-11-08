/*
file: test_rtc.c
测试rtc驱动， 在finsh 中运行
1. test_rtc(0)   配置时间为2018.1.1 01:01:01
2. test_rtc(1)   显示当前时间
 */

#include <rtthread.h>
#include <stdlib.h>  
#include "ls1c_regs.h"
#include "ls1c_rtc.h"

void test_rtt_rtc(int flag)  
{
	rt_device_t rtc_device;//rtc设备

	time_t timestamp;
	struct tm *p_tm;
	/* 设置时间为2018.1.1 01:01:01*/
	struct tm tm_new ;
	tm_new.tm_year = 118;
	tm_new.tm_mon = 1 - 1;
	tm_new.tm_mday= 1;
	tm_new.tm_hour= 1;
	tm_new.tm_min= 1;
	tm_new.tm_sec= 1;
	timestamp = mktime(&tm_new); 
	
  	rtc_device = rt_device_find("rtc");
  
	if (rtc_device != RT_NULL)    
	{
	rt_device_init(rtc_device);	
	if(!flag)
		{

			rt_device_control(rtc_device,RT_DEVICE_CTRL_RTC_SET_TIME,  &timestamp);
		}
	else
		{
			rt_device_control(rtc_device,RT_DEVICE_CTRL_RTC_GET_TIME, &timestamp);
			p_tm = localtime(&timestamp);
				rt_kprintf("\r\nrtc time is %d.%d.%d - %d:%d:%d\r\n",p_tm->tm_year+1900, p_tm->tm_mon+1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);		
		}
	}
}  

void test_rtt_rtcmsh(int argc, char** argv)
{
    unsigned int num;
	num = strtoul(argv[1], NULL, 0);
	test_rtt_rtc(num);
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_rtt_rtc , test_rtt_rtc  e.g.test_rtt_rtc(1));
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_rtcmsh, test_rtt_rtcmsh 0);

