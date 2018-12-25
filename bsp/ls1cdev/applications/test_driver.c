
#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#define led_gpio 52
#define key_gpio 85

struct led_device
{
    int led_num;
};


struct led_device led1 =
{
    led_gpio,
};

struct rt_device led1_device;


/* RT-Thread Device Interface */
static rt_err_t rt_led_init (rt_device_t dev)
{
    struct led_device* led_dev = (struct led_device*) dev->user_data;
    // 初始化
    rt_kprintf("Init gpio! gpio_num = %d \n", led_dev->led_num);
    gpio_init(led_dev->led_num, gpio_mode_output);	
    return RT_EOK;
}

static rt_err_t rt_led_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_led_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t rt_led_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{

    return RT_EOK;
}

static rt_size_t rt_led_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{

    return RT_EOK;
}

static rt_err_t rt_led_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct led_device* led_dev = (struct led_device*) dev->user_data;
    switch (cmd)
    {
    case 1:
        gpio_set(led_dev->led_num, gpio_level_low);
        break;
    case 0:
        gpio_set(led_dev->led_num, gpio_level_high);
        break;
    }
    return RT_EOK;
}


rt_err_t rt_hw_led_register(rt_device_t device, const char* name, rt_uint32_t flag, void *user_data)
{
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
    device->init        = rt_led_init;
    device->open        = rt_led_open;
    device->close       = rt_led_close;
    device->read        = rt_led_read;
    device->write       = rt_led_write;
    device->control     = (void *)rt_led_control;
    device->user_data   = user_data;
    
    /* register a character device */
    return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);
}



/*
 * 测试库中gpio作为输出时的相关接口
 * led闪烁10次
 */
void test_driver(void)
{
    int i;
    static rt_device_t led_device;//led设备
    
  rt_hw_led_register(&led1_device, "led1",  RT_DEVICE_FLAG_RDWR , &led1);
    
  led_device = rt_device_find("led1");
  
  if (led_device != RT_NULL)    
  {
        rt_device_init(led_device);
        
        for (i=0; i<10; i++)
        {

            rt_device_control(led_device, 0,RT_NULL);
            rt_thread_delay(100);
            rt_device_control(led_device, 1,RT_NULL);
            rt_thread_delay(100);
        }
  }   
    return ;
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_driver, test_driver  e.g.test_driver());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_driver, test_driver);
