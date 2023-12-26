#include "drv_jhm1400.h"

#define JHM1400_I2C_BUS_NAME "i2c0" /* 传感器连接的I2C总线设备名称 */
#define DEV_JHM1400B_ADDRESS 0x70 >> 1

#define JHM1400B_SYS_CFG_CMD 0xA8
#define JHM1400B_VDIFF_CMD 0xA1

#define JHM1400_POWER_PIN 27
#define JHM1400B_VDIFF_CMD 0xA1
#define JHM1400B_VOFFSET 4
#define JHM1400B_SAMPLE_NUM 10
#define JHM1400B_CONVERTION_MAX_DELAY 100

/* JHM1400B 增益 */
typedef enum
{
    JHM1400B_THIRD_GAIN_1_1,
    JHM1400B_THIRD_GAIN_1_2,
    JHM1400B_THIRD_GAIN_1_3,
    JHM1400B_THIRD_GAIN_1_4,
    JHM1400B_THIRD_GAIN_1_5,
    JHM1400B_THIRD_GAIN_1_6,
    JHM1400B_THIRD_GAIN_1_7,
    JHM1400B_THIRD_GAIN_1_8,
} jhm1400b_first_gain_t;

typedef enum
{
    JHM1400B_SECOND_GAIN_2,
    JHM1400B_SECOND_GAIN_3,
    JHM1400B_SECOND_GAIN_5,
    JHM1400B_SECOND_GAIN_7,
} jhm1400b_second_gain_t;

typedef enum
{
    JHM1400B_FIRST_GAIN_2,
    JHM1400B_FIRST_GAIN_13_3,
    JHM1400B_FIRST_GAIN_5,
    JHM1400B_FIRST_GAIN_40,
} jhm1400b_third_gain_t;

static rt_err_t _write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t *data, rt_uint32_t len)
{
    if (rt_i2c_master_send(bus, DEV_JHM1400B_ADDRESS, RT_I2C_WR, data, len))
    {
    }

    return RT_EOK;
}

static rt_err_t _read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t *data, rt_uint32_t len)
{
    if (rt_i2c_master_recv(bus, DEV_JHM1400B_ADDRESS, RT_I2C_RD, data, len))
    {
    }

    return RT_EOK;
}

rt_err_t _jhm1400_init(rt_device_t dev)
{
    return RT_EOK;
}

rt_err_t _jhm1400_open(rt_device_t dev, rt_uint16_t oflag)
{
    RT_ASSERT(dev != RT_NULL);

    rt_base_t jhm1400_power = ((struct jhm1400_user_dat *)(dev->user_data))->power_pin;
    rt_base_t sensor_pwr = ((struct jhm1400_user_dat *)(dev->user_data))->sensor_pwr;
    rt_pin_mode(jhm1400_power, PIN_MODE_OUTPUT);
    rt_pin_write(jhm1400_power, PIN_LOW);
    rt_pin_mode(sensor_pwr, PIN_MODE_OUTPUT);
    rt_pin_write(sensor_pwr, PIN_HIGH);

    return RT_EOK;
}

rt_err_t _jhm1400_close(rt_device_t dev)
{
    RT_ASSERT(dev != RT_NULL);

    rt_base_t jhm1400_power = ((struct jhm1400_user_dat *)(dev->user_data))->power_pin;
    rt_pin_write(jhm1400_power, PIN_HIGH);
    rt_pin_mode(jhm1400_power, PIN_MODE_INPUT);

    return RT_EOK;
}

rt_ssize_t _jhm1400_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(buffer != RT_NULL);

    if (size != sizeof(rt_int32_t))
    {
        return RT_EINVAL;
    }

    struct rt_i2c_bus_device *bus = ((struct jhm1400_user_dat *)(dev->user_data))->i2c_bus;
    RT_ASSERT(bus != RT_NULL);

    rt_uint8_t cmd_buf[3];
    cmd_buf[0] = JHM1400B_VDIFF_CMD;
    cmd_buf[1] = JHM1400B_VOFFSET << 1;
    cmd_buf[2] = ((JHM1400B_VOFFSET & 0x01) << 7) | (JHM1400B_THIRD_GAIN_1_1 << 4) | (JHM1400B_SECOND_GAIN_2 << 2) | (JHM1400B_FIRST_GAIN_5);

    rt_int32_t ret = _write_reg(bus, cmd_buf, sizeof(cmd_buf));
    if (ret != RT_EOK)
    {
        return ret;
    }

    rt_uint8_t data_buf[4];
    rt_memset(data_buf, 0xff, sizeof(data_buf));
    for (rt_uint32_t idx = 0; idx < JHM1400B_CONVERTION_MAX_DELAY; idx++)
    {
        if (_read_reg(bus, data_buf, sizeof(data_buf)) == RT_EOK)
        {
            if ((data_buf[0] & 0x80) == 0)
            {
                break; // 数据转换完成
            }

            if (idx == JHM1400B_CONVERTION_MAX_DELAY - 1)
            {
                return RT_EIO;
            }
        }

        rt_thread_mdelay(1);
    }

    // 读取原始AD值数据
    ret = _read_reg(bus, data_buf, sizeof(data_buf));
    if (ret != RT_EOK)
    {
        rt_kprintf("[drv_jhm1400b_ioctl] read ad_dts err %d", ret);
    }
    else
    {
        if (data_buf[1] & 0x80)
        {
            *(rt_int32_t *)buffer = ((data_buf[1] << 16) | (data_buf[2] << 8) | (data_buf[3])) - 0x1000000;
        }
        else
        {
            *(rt_int32_t *)buffer = (data_buf[1] << 16) | (data_buf[2] << 8) | (data_buf[3]);
        }
    }
    return ret;
}

static rt_ssize_t _jhm1400_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return RT_EOK;
}

static rt_err_t _jhm1400_control(rt_device_t dev, int cmd, void *args)
{
    return RT_EOK;
}

static struct rt_device dev_jhm1400;
rt_err_t drv_jhm1400_init(struct jhm1400_user_dat *user_data)
{
    if (user_data == RT_NULL)
    {
        return RT_EINVAL;
    }

    dev_jhm1400.type = RT_Device_Class_Miscellaneous;
    dev_jhm1400.rx_indicate = RT_NULL;
    dev_jhm1400.tx_complete = RT_NULL;
    dev_jhm1400.init = _jhm1400_init;
    dev_jhm1400.open = _jhm1400_open;
    dev_jhm1400.close = _jhm1400_close;
    dev_jhm1400.read = _jhm1400_read;
    dev_jhm1400.write = RT_NULL;
    dev_jhm1400.control = RT_NULL;
    dev_jhm1400.user_data = user_data;

    rt_kprintf("dev_jhm1400.parent = %d\n", &(dev_jhm1400));
    /* register a character device */
    return rt_device_register(&dev_jhm1400, "jhm1400", RT_DEVICE_FLAG_RDWR);
}
