/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-29     supperthomas first version
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "ports/drv_jhm1400.h"
#define DK_BOARD_LED_1  18

static struct rt_thread thread1;
static char thread1_stack[1024];

static void thread1_entry(void *param)
{
    rt_int32_t value;
    rt_device_t dev = rt_device_find("jhm1400");    
    if (dev == RT_NULL)
    {
        rt_kprintf("can't find dev\n");
    }
    rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
    while (1)
    {
        rt_thread_mdelay(1000);
        rt_device_read(dev, 0, &value, sizeof(rt_int32_t));
        rt_kprintf("[jhm1400_read]: ad = %d\n", value);
    }
}

int main(void)
{
    rt_thread_init(&thread1,"thread1",thread1_entry,RT_NULL,&thread1_stack[0],sizeof(thread1_stack),20,10);
    rt_thread_startup(&thread1);

    while (1)
    {
        rt_thread_mdelay(2000);
    }
    

    return RT_EOK;
}