/*
调用方法，在msh中运行以下命令
1. baidu_indentify onblue  //将存放在SD卡根目录上的 onblue.pcm 发送到百度语音api并识别，返回识别的词语
*/

#include <string.h>
#include <rtthread.h>
#include "webclient.h"
#include <dfs_posix.h> 

#include <rtdbg.h>

/* 语音合成 */
#define FILE_NAME_MAX                 32
#define MP3_FILE "/%s.pcm"

/* 语音识别 */
#define POST_FILE_URL    "http://vop.baidu.com/server_api?dev_pid=1536&cuid=asr&token=24.890e327245c548078b9c453d6d483dc3.2592000.1544565032.282335-14676469" //将***更换成自己的token,共70个
/* Content-Type*/
#define C_TYPE   "audio/pcm;rate=16000"

#define DBG_ENABLE
#define DBG_SECTION_NAME               "web.baidu"
#define DBG_LEVEL                      DBG_LOG
#define DBG_COLOR

#define blue_led 			52
#define yellow_led 			53

/* LED ON or OFF */
#define ON    0
#define OFF   1

char *cmd1 = "打开蓝灯";
char *cmd2 = "关闭蓝灯";
char *cmd3 = "打开黄灯";
char *cmd4 = "关闭黄灯";

/* initialize led */
void led_init(void)
{
    //设置管脚为输出模式
    rt_pin_mode(blue_led, PIN_MODE_OUTPUT); 
    //设置管脚为输出模式
    rt_pin_mode(yellow_led, PIN_MODE_OUTPUT); 
    
    rt_pin_write(blue_led, PIN_HIGH);
    rt_pin_write(yellow_led, PIN_HIGH);
}

/* control the led status */
void led_ctl_stat(char led, char ctrl_stat)
{
    /* initialize led */
    led_init();
    
    if((led == blue_led) && (ctrl_stat == ON))
    {
        rt_kprintf("Blue LED ON\r\n");
        //输出低电平，blue_led 亮   
        rt_pin_write(blue_led, PIN_LOW); 
    }
    else if((led == blue_led) && (ctrl_stat == OFF))
    {
        rt_kprintf("Blue LED OFF\r\n");
         //输出高电平，blue_led 灭
        rt_pin_write(blue_led, PIN_HIGH);        
    }
    else if((led == yellow_led) && (ctrl_stat == ON))
    {
        rt_kprintf("Yellow LED ON\r\n");
        //输出低电平，yellow_led 亮
        rt_pin_write(yellow_led, PIN_LOW);
        
    }
    else if((led == yellow_led) && (ctrl_stat == OFF))
    {
        rt_kprintf("Yellow LED OFF\r\n");
        //输出高电平，yellow_led 灭
        rt_pin_write(yellow_led, PIN_HIGH);
    }              
}

int webclient_post_file_baidu(const char* URI, const char* filename, const char* form_data)
{
    size_t length;
    int fd = -1, rc = WEBCLIENT_OK;
    unsigned char *buffer = RT_NULL;
    struct webclient_session* session = RT_NULL;
    int bytes_read = 0, index = 0;
    int ret = 0;
    int send_count = 0;

    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        LOG_D("post file failed, open file(%s) error.", filename);
        rc = -WEBCLIENT_FILE_ERROR;
        goto __exit;
    }

    /* get the size of file */
    length = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    buffer = (unsigned char *) web_malloc(WEBCLIENT_RESPONSE_BUFSZ);
    if (buffer == RT_NULL)
    {
        LOG_D("post file failed, no memory for response buffer.");
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        rc = -WEBCLIENT_NOMEM;
        rt_kprintf("create session error!\n");
        goto __exit;
    }

    /* build header for upload */
    webclient_header_fields_add(session, "Content-Length: %d\r\n", length);
    webclient_header_fields_add(session, "Content-Type: %s\r\n", form_data);

    rc = webclient_post(session, URI, NULL);
    if (rc < 0)
    {
        rt_kprintf("webclient post data error!\n");
        goto __exit;
    }
    else if (rc == 0)
    {
        rt_kprintf("webclient connected and send header msg!\n");
    }
    else
    {
        rt_kprintf("rc code: %d!\n", rc);
    }

    /* send file data */
    while (1)
    {
        rt_memset(buffer, 0x00, WEBCLIENT_RESPONSE_BUFSZ);
        length = read(fd, buffer, WEBCLIENT_RESPONSE_BUFSZ);
        if (length <= 0)
        {
            break;
        }
        rt_kprintf("read file data length %d; webclient write %d cnt!\n", length, ++send_count);
        ret = webclient_write(session, buffer, length);
        if (ret < 0)
        {
            rt_kprintf("webclient write error!\n");
            break;
        }

        rt_thread_mdelay(100);
    }
    rt_kprintf("send file completed!\n");

    if( webclient_resp_status_get(session) != 200)
    {
        LOG_E("get handle resposne error!");
        //goto __exit;
    }

#if 1
    int content_pos = 0;
    /* 获取接收的响应数据长度 */
    int content_length = webclient_content_length_get(session);
    rt_thread_delay(400);
    /* 循环接收响应数据直到数据接收完毕 */
    do
    {
        /* 总是返回超时错误 */
        bytes_read = webclient_read(session, buffer, WEBCLIENT_HEADER_BUFSZ);
        if (bytes_read <= 0)
        {
            break;
        }
	    rt_kprintf("\r\n收到回应 %d 字节:\r\n",bytes_read);

        /* 打印响应数据 */
        for (index = 0; index < bytes_read; index++)
        {
            rt_kprintf("%c", buffer[index]);
        }

        content_pos += bytes_read;
    } while (content_pos < content_length);
#endif
	/**************************** process the cmd ***********************************/
	rt_kprintf("\r\nProcess the voice command...\r\n");
	if(strstr(buffer, cmd1) != NULL) /* blue_led On */
	{
		led_ctl_stat(blue_led, ON);
	}
	else if(strstr(buffer, cmd2) != NULL) /* blue_led Off */
	{
		led_ctl_stat(blue_led, OFF);
	}
	else if(strstr(buffer, cmd3) != NULL) /* yellow_led On */
	{
		led_ctl_stat(yellow_led, ON);
	}
	else if(strstr(buffer, cmd4) != NULL) /* yellow_led Off */
	{
		led_ctl_stat(yellow_led, OFF);
	}
	else
	{
		int i = 0;
		rt_kprintf("其他语音，则yellow_led 闪烁五次\r\n");
		i = 5;
		while(i--)
		{
			led_ctl_stat(yellow_led, ON);
			rt_thread_delay(rt_tick_from_millisecond(1000));
			led_ctl_stat(yellow_led, OFF);
			rt_thread_delay(rt_tick_from_millisecond(1000));
		}    
	}
/**************************** process the end ***********************************/

__exit:
    if (fd >= 0)
    {
        close(fd);
    }

    if ((session != RT_NULL))
    {
        webclient_close(session);
    }

    if (buffer != RT_NULL)
    {
        web_free(buffer);
    }

    return 0;
}

/* 使用Raw 方式上传 */
int webclient_post_voice_file_to_identify(int argc, char **argv)
{
    char *URI = RT_NULL;
    int ret = 0;
    char *file_name = RT_NULL;

    URI = web_strdup(POST_FILE_URL);
    if (URI == RT_NULL)
    {
        rt_kprintf("no memory for create URI buffer.\n");
        return -1;
    }

	file_name = rt_calloc(1, FILE_NAME_MAX);
	if (file_name == RT_NULL)
	{
		rt_kprintf("No memory for file name!\n");
		return -1;
	}
	
    if (argc == 1)
    {
        /* 拼接 GET 网址 */
        rt_snprintf(file_name, FILE_NAME_MAX, MP3_FILE, "on1");
		rt_kprintf("webclient_post_test [file]  - %s.\n", file_name);
    }
    else  if (argc == 2)
    {
	    rt_snprintf(file_name, FILE_NAME_MAX, MP3_FILE, argv[1]);
        rt_kprintf("webclient_post_test [file]  - %s.\n", file_name);
    }
	else
	{
		if (file_name)
		{
			web_free(file_name);
		}
	}


    if ((webclient_post_file_baidu(URI, file_name, C_TYPE)))
    {
        ret = -RT_ERROR;
        goto __exit;
    }

    rt_kprintf("\n");
    __exit:

    if (URI)
    {
        web_free(URI);
    }

    if (file_name)
    {
        web_free(file_name);
    }

    return ret;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(webclient_post_voice_file_to_identify, baidu_identify,
        webclient_post_test [URI] - webclient POST request test.);
#endif /* FINSH_USING_MSH */

