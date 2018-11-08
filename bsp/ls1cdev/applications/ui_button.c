/*
 * File      : ui_button.c
 
1.ui_button()  触摸屏按钮 及简易计算器
2. test_guidemo() 运行gui的demo
3. test_cali()   运行标定屏幕程序
*
*/
#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>



static struct rtgui_label *label = RT_NULL;

static void onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_kprintf("hello, button!\n");
    if (label != RT_NULL)
    {
        rtgui_label_set_text(label, "Hello from button");
        rtgui_widget_update(RTGUI_WIDGET(label));
    }

    return;
}


static void onbutton_caculator(struct rtgui_object *object, struct rtgui_event *event)
{
        extern int calculator(void);
    rt_kprintf("Oh! Run caculator !\n");
        
        /* 简易计算器 */
        calculator();

    return;
}

static void ui_thread_entry(void* parameter)
{
    struct rtgui_app* app;
    struct rtgui_win *win;
    struct rtgui_button *button;
    struct rtgui_button *button_caculator;
        struct rtgui_box *box;
        struct rtgui_box *box_caculator;

    /* create GUI application */
    app = rtgui_app_create("UiApp");
    RT_ASSERT(app != RT_NULL);

    /* create main window */
    win = rtgui_mainwin_create(RT_NULL, "UiWindow", RTGUI_WIN_STYLE_DEFAULT);

    /* we use layout engine to place sub-widgets */
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);
    
    /* we use layout engine to place sub-widgets */
    box_caculator = rtgui_box_create(RTGUI_VERTICAL, 30);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box_caculator);

    /* create the 'hello world' label */
    label = rtgui_label_create("Hello World!!!");
    rtgui_widget_set_minwidth(RTGUI_WIDGET(label), 150);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

    /* create the button */
    button = rtgui_button_create("  O K  ");
    rtgui_button_set_onbutton(button, onbutton);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button));
        
    /* create the cacultor button */
    button_caculator = rtgui_button_create(" Run Simple Cacultor ");
    rtgui_button_set_onbutton(button_caculator, onbutton_caculator);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button_caculator));

        rtgui_container_layout(RTGUI_CONTAINER(win));

    rtgui_win_show(win, RT_FALSE);
    rtgui_app_run(app);

    rtgui_win_destroy(win);
    rtgui_app_destroy(app);
}

int ui_button(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("UiApp", ui_thread_entry, RT_NULL, 
        2048, 20, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
extern int calibration(void);
extern void application_init(void);

int test_cali(void)
{
    calibration();
}

int test_guidemo(void)
{
    application_init(); 
}


 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(ui_button, ui_button  e.g.ui_button());
FINSH_FUNCTION_EXPORT(test_cali, test_cali  e.g.test_cali());
FINSH_FUNCTION_EXPORT(test_guidemo, test_guidemo  e.g.test_guidemo());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(ui_button, ui_button);
MSH_CMD_EXPORT(test_cali, test_cali);
MSH_CMD_EXPORT(test_guidemo, test_guidemo);
