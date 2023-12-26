/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     misonyo   first version
 * 2023-12-03     Meco Man     support nano version
 */

#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#include <drv_usart.h>
#endif /* RT_USING_NANO */

#include "at24c02.h"

rt_uint8_t buffer[24] = {0};
int main(void)
{
    if (dev_at24c02_init() != RT_EOK)
    {
        rt_kprintf("at24c02 init err\n");
    }

    rt_device_t dev_at24c02 = rt_device_find("at24c02");
    if (dev_at24c02 == RT_NULL)
    {
        rt_kprintf("can't find at24c02\n");
    }

    if (rt_device_open(dev_at24c02, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("at24c02 init err\n");
    }

    if (rt_device_write(dev_at24c02, 0, "hello2024", 9) != RT_EOK)
    {
        rt_kprintf("at24c02 write err\n");
    }

    if (rt_device_read(dev_at24c02, 0, buffer, 9) != RT_EOK)
    {
        rt_kprintf("at24c02 write err\n");
    }

    rt_kprintf("[at24c02 read buffer]: %s\n", buffer);

    while (1)
    {
    }

    return RT_EOK;
}
