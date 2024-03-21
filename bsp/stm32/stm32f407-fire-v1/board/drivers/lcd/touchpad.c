#include "touchpad.h"

#define TOUCH_DEV_NAME "GT5XX"
#define I2C_BUS_NAME "i2c1"
#define TOUCHPAD_ADDRESS 0xBA

struct drv_touchpad_device
{
	struct rt_device parent;
};

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;

const uint8_t CTP_CFG_GT5688[] = {
	0x96, 0xE0, 0x01, 0x56, 0x03, 0x05, 0x35, 0x00, 0x01, 0x00,
	0x00, 0x05, 0x50, 0x3C, 0x53, 0x11, 0x00, 0x00, 0x22, 0x22,
	0x14, 0x18, 0x1A, 0x1D, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x53, 0x00, 0x14, 0x00, 0x00, 0x84, 0x00, 0x00,
	0x3C, 0x19, 0x19, 0x64, 0x1E, 0x28, 0x88, 0x29, 0x0A, 0x2D,
	0x2F, 0x29, 0x0C, 0x20, 0x33, 0x60, 0x13, 0x02, 0x24, 0x00,
	0x00, 0x20, 0x3C, 0xC0, 0x14, 0x02, 0x00, 0x00, 0x54, 0xAC,
	0x24, 0x9C, 0x29, 0x8C, 0x2D, 0x80, 0x32, 0x77, 0x37, 0x6E,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x50, 0x3C,
	0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x02, 0x14, 0x14, 0x03,
	0x04, 0x00, 0x21, 0x64, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x32, 0x20, 0x50, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0D, 0x06, 0x0C, 0x05, 0x0B, 0x04, 0x0A, 0x03, 0x09, 0x02,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
	0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x05, 0x1E, 0x00, 0x02,
	0x2A, 0x1E, 0x19, 0x14, 0x02, 0x00, 0x03, 0x0A, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x86,
	0x22, 0x03, 0x00, 0x00, 0x33, 0x00, 0x0F, 0x00, 0x00, 0x00,
	0x50, 0x3C, 0x50, 0x00, 0x00, 0x00, 0x1A, 0x64, 0x01};

int drv_gt5xx_init(void)
{
	rt_kprintf("i2c1 bus scl = %d, sda = %d\n", rt_pin_get("PD.7"), rt_pin_get("PD.3"));
	i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_BUS_NAME);
	if (i2c_bus == RT_NULL)
	{
		rt_kprintf("can't find %s device!\n", I2C_BUS_NAME);
		return RT_EFULL;
	}

	rt_uint8_t reg_addr[2] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};

	struct rt_i2c_msg msgs = {
		.addr = TOUCHPAD_ADDRESS,
		.flags = RT_I2C_WR,
		.buf = reg_addr,
		.len = sizeof(reg_addr),
	};

	if (rt_i2c_transfer(i2c_bus, &msgs, 1) == 1)
	{
		return RT_EOK;
	}
    else
    {
        return -RT_ERROR;
    }
}

INIT_DEVICE_EXPORT(drv_gt5xx_init);
