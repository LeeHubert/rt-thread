#ifndef __DRV_JHM1400_H__
#define __DRV_JHM1400_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "drivers/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

struct jhm1400_user_dat
{
    rt_base_t power_pin;
    rt_base_t sensor_pwr;
    struct rt_i2c_bus_device *i2c_bus;
};

rt_err_t drv_jhm1400_init(struct jhm1400_user_dat *user_data);

#ifdef __cplusplus
}
#endif

#endif
