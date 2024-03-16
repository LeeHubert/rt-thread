
#include "at24c02.h"
#include "at24cxx.h"

#define AT24C02_ADDR    0x00

static at24cxx_device_t at24cxx_dev;

static rt_err_t _init(rt_device_t dev)
{
    at24cxx_dev = at24cxx_init("i2c1", AT24C02_ADDR);
    if (at24cxx_dev == RT_NULL)
    {
        return RT_EEMPTY;
    }

    return RT_EOK;
}

rt_err_t  _open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_ssize_t _read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    at24cxx_read(at24cxx_dev, (rt_uint32_t)pos, (rt_uint8_t *)buffer, (rt_uint16_t)size);
    return RT_EOK;
}

static rt_ssize_t _write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    at24cxx_write(at24cxx_dev, (rt_uint32_t)pos, (rt_uint8_t *)buffer, (rt_uint16_t)size);
    return RT_EOK;
}

rt_err_t dev_at24c02_init()
{
    static struct rt_device rt_dev_at24cxx;

    rt_dev_at24cxx.type = RT_Device_Class_I2CBUS;
    rt_dev_at24cxx.init = _init;
    rt_dev_at24cxx.open = _open;
    rt_dev_at24cxx.read = _read;
    rt_dev_at24cxx.write = _write;
    return rt_device_register(&rt_dev_at24cxx, "at24c02", RT_DEVICE_FLAG_RDWR);
}
