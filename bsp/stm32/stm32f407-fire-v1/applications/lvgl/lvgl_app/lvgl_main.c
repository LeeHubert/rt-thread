#include <rtthread.h>
#include <lvgl.h>

#define LV_THREAD_STACK_SIZE 4096
#define LV_THREAD_PRIO (RT_THREAD_PRIORITY_MAX * 2 / 3)

static void lvgl_thread(void *parameter)
{
    lv_obj_t *button = lv_led_create(lv_scr_act());
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);

    while (1)
    {
        lv_task_handler();
        rt_thread_mdelay(5);
    }
}

static int lvgl_app(void)
{
    extern void lv_port_disp_init(void);

    lv_init();
    lv_port_disp_init();

    rt_thread_t tid = rt_thread_create("LVGL", lvgl_thread, RT_NULL,
                                          LV_THREAD_STACK_SIZE,
                                          LV_THREAD_PRIO,
                                          0);
    if(tid == RT_NULL)
    {
        rt_kprintf("Fail to create 'LVGL' thread");
    }

    rt_thread_startup(tid);
    return 0;
}
INIT_APP_EXPORT(lvgl_app);
