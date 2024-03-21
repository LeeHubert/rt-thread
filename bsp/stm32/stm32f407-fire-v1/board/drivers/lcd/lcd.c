#include "lcd.h"
#include "board.h"
#include <drv_gpio.h>

#define ILI9806G_LESS_PIXEL 479 // 液晶屏较短方向像素宽度
#define ILI9806G_MORE_PIXEL 853 // 液晶屏较长方向像素宽度

/* USER CODE BEGIN 0 */
#define FSMC_ADDR_ILI9806G_CMD ((uint32_t)0x68000000)
#define FSMC_ADDR_ILI9806G_DATA ((uint32_t)0x68000002)

#define CMD_SetCoordinateX 0x2A // 设置X坐标
#define CMD_SetCoordinateY 0x2B // 设置Y坐标
#define CMD_SetPixel 0x2C		// 填充像素

#define BL_Pin GPIO_PIN_9
#define BL_GPIO_Port GPIOF
#define RST_Pin GPIO_PIN_11
#define RST_GPIO_Port GPIOF

#define LCD_RST_PIN GET_PIN(F, 11)
#define LCD_BL_PIN GET_PIN(F, 9)

#define LCD_DEV_NAME "lcd"

static uint16_t lcd_x_length = ILI9806G_MORE_PIXEL;
static uint16_t lcd_y_length = ILI9806G_LESS_PIXEL;
static SRAM_HandleTypeDef hsram3;

struct drv_lcd_device
{
	struct rt_device parent;
	struct rt_device_graphic_info lcd_info;
};

static struct drv_lcd_device _lcd;

/**
 * @brief  向ILI9806G写入命令
 * @param  usCmd :要写入的命令
 * @retval 无
 */
static void ili9806g_write_cmd(uint16_t cmd)
{
	*(__IO uint16_t *)(FSMC_ADDR_ILI9806G_CMD) = cmd;
}

/**
 * @brief  向ILI9806G写入数据
 * @param  usCmd :要写入的数据
 * @retval 无
 */
static void ili9806G_write_data(uint16_t data)
{
	*(__IO uint16_t *)(FSMC_ADDR_ILI9806G_DATA) = data;
}

/**
 * @brief  从ILI9806G读取数据
 * @param  无
 * @retval 读取到的数据
 */
static uint16_t ili9806G_read_data(void)
{
	return (*(__IO uint16_t *)(FSMC_ADDR_ILI9806G_DATA));
}

static void _drv_lcd_rst(void)
{
	rt_pin_write(LCD_RST_PIN, PIN_LOW);
	rt_thread_mdelay(10);

	rt_pin_write(LCD_RST_PIN, PIN_HIGH);
	rt_thread_mdelay(10);
}

/**
 * @brief  ??ILI9806G?GRAM?????
 * @param  ucOption :??GRAM?????
 *     @arg 0-7 :??????0-7?????
 *
 *	!!!??0?3?5?6 ????????????,
 *				?????????????	??????????????
 *
 *	??0?2?4?6 ???X?????480,Y?????854
 *	??1?3?5?7 ???X?????854,Y?????480
 *
 *	?? 6 ?????????????????
 *	?? 3 ?????????????
 *	?? 0 ???BMP???????????
 *
 * @retval ?
 * @note  ????:A????,V????,<????,>????
					X??X?,Y??Y?

------------------------------------------------------------
??0:				.		??1:		.	??2:			.	??3:
					A		.					A		.		A					.		A
					|		.					|		.		|					.		|
					Y		.					X		.		Y					.		X
					0		.					1		.		2					.		3
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->
------------------------------------------------------------
??4:				.	??5:			.	??6:			.	??7:
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->
					4		.					5		.		6					.		7
					Y		.					X		.		Y					.		X
					|		.					|		.		|					.		|
					V		.					V		.		V					.		V
---------------------------------------------------------
											 LCD???
								|-----------------|
								|			??Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								????(?480,?854)

 *******************************************************/
static void _drv_lcd_display_dir(uint8_t dir)
{
	if (dir > 7)
		return;

	if (dir % 2 == 0)
	{
		lcd_x_length = ILI9806G_LESS_PIXEL;
		lcd_y_length = ILI9806G_MORE_PIXEL;
	}
	else
	{
		lcd_x_length = ILI9806G_MORE_PIXEL;
		lcd_y_length = ILI9806G_LESS_PIXEL;
	}

	ili9806g_write_cmd(0x36);
	ili9806G_write_data(0x00 | (dir << 5)); //??ucOption????LCD??,?0-7???
	ili9806g_write_cmd(CMD_SetCoordinateX);
	ili9806G_write_data(0x00);							   /* x ?????8? */
	ili9806G_write_data(0x00);							   /* x ?????8? */
	ili9806G_write_data(((lcd_x_length - 1) >> 8) & 0xFF); /* x ?????8? */
	ili9806G_write_data((lcd_x_length - 1) & 0xFF);		   /* x ?????8? */

	ili9806g_write_cmd(CMD_SetCoordinateY);
	ili9806G_write_data(0x00);							   /* y ?????8? */
	ili9806G_write_data(0x00);							   /* y ?????8? */
	ili9806G_write_data(((lcd_y_length - 1) >> 8) & 0xFF); /* y ?????8? */
	ili9806G_write_data((lcd_y_length - 1) & 0xFF);		   /* y ?????8? */

	/* write gram start */
	ili9806g_write_cmd(CMD_SetPixel);
}

void _drv_lcd_open_window ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{	
	ili9806g_write_cmd ( CMD_SetCoordinateX );
	ili9806G_write_data ( usX >> 8  );
	ili9806G_write_data ( usX & 0xff  );
	ili9806G_write_data ( ( usX + usWidth - 1 ) >> 8  );
	ili9806G_write_data ( ( usX + usWidth - 1 ) & 0xff  );

	ili9806g_write_cmd ( CMD_SetCoordinateY );
	ili9806G_write_data ( usY >> 8  );
	ili9806G_write_data ( usY & 0xff  );
	ili9806G_write_data ( ( usY + usHeight - 1 ) >> 8 );
	ili9806G_write_data ( ( usY + usHeight - 1) & 0xff );
	
}

static void _drv_lcd_fill_color(uint32_t ulAmout_Point, uint16_t *usColor)
{
	uint32_t i = 0;

	/* memory write */
	ili9806g_write_cmd(CMD_SetPixel);

	for (i = 0; i < ulAmout_Point; i++,usColor++)
		ili9806G_write_data(*usColor);
}

void drv_lcd_clear(uint16_t usX, uint16_t usY, uint16_t ueX, uint16_t ueY, uint16_t *color)
{
	_drv_lcd_open_window(usX, usY, (ueX + 1 - usX), (ueY + 1 - usY));
	_drv_lcd_fill_color((ueX + 1 - usX) * (ueY + 1 - usY), color);
}

void ILI9806G_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t color)	
{	
	_drv_lcd_open_window ( usX, usY, 1, 1 );
	_drv_lcd_fill_color ( 1, &color);
}

/* FSMC initialization function */
rt_err_t drv_lcd_init(rt_device_t dev)
{
	/* USER CODE BEGIN FSMC_Init 0 */
	rt_pin_mode(LCD_BL_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(LCD_RST_PIN, PIN_MODE_OUTPUT);
	/* USER CODE END FSMC_Init 0 */

	FSMC_NORSRAM_TimingTypeDef Timing = {0};

	/* USER CODE BEGIN FSMC_Init 1 */

	/* USER CODE END FSMC_Init 1 */

	/** Perform the SRAM3 memory initialization sequence
	 */
	hsram3.Instance = FSMC_NORSRAM_DEVICE;
	hsram3.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
	/* hsram3.Init */
	hsram3.Init.NSBank = FSMC_NORSRAM_BANK3;
	hsram3.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
	hsram3.Init.MemoryType = FSMC_MEMORY_TYPE_NOR;
	hsram3.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
	hsram3.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
	hsram3.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
	hsram3.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
	hsram3.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
	hsram3.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
	hsram3.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
	hsram3.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
	hsram3.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
	hsram3.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
	hsram3.Init.PageSize = FSMC_PAGE_SIZE_NONE;
	/* Timing */
	Timing.AddressSetupTime = 4;
	Timing.AddressHoldTime = 0;
	Timing.DataSetupTime = 4;
	Timing.BusTurnAroundDuration = 0;
	Timing.CLKDivision = 0;
	Timing.DataLatency = 0;
	Timing.AccessMode = FSMC_ACCESS_MODE_B;
	/* ExtTiming */

	if (HAL_SRAM_Init(&hsram3, &Timing, NULL) != HAL_OK)
	{
		return RT_ERROR;
	}

	_drv_lcd_rst();
	uint16_t dat1, dat2;
	HAL_Delay(5);
	ili9806g_write_cmd(0xFF);
	ili9806G_write_data(0x98);
	dat1 = ili9806G_read_data();
	ili9806G_write_data(0x06);
	dat2 = ili9806G_read_data();

	ili9806G_write_data(dat1);
	ili9806G_write_data(dat2);

	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xFF);
	ili9806G_write_data(0xFF);
	ili9806G_write_data(0x98);
	ili9806G_write_data(0x06);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xBA);
	ili9806G_write_data(0x60);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xBC);
	ili9806G_write_data(0x03);
	ili9806G_write_data(0x0E);
	ili9806G_write_data(0x61);
	ili9806G_write_data(0xFF);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x1B);
	ili9806G_write_data(0x10);
	ili9806G_write_data(0x73);
	ili9806G_write_data(0x63);
	ili9806G_write_data(0xFF);
	ili9806G_write_data(0xFF);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0xD5);
	ili9806G_write_data(0xD0);
	ili9806G_write_data(0x01);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x40);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xBD);
	ili9806G_write_data(0x01);
	ili9806G_write_data(0x23);
	ili9806G_write_data(0x45);
	ili9806G_write_data(0x67);
	ili9806G_write_data(0x01);
	ili9806G_write_data(0x23);
	ili9806G_write_data(0x45);
	ili9806G_write_data(0x67);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xBE);
	ili9806G_write_data(0x01);
	ili9806G_write_data(0x2D);
	ili9806G_write_data(0xCB);
	ili9806G_write_data(0xA2);
	ili9806G_write_data(0x62);
	ili9806G_write_data(0xF2);
	ili9806G_write_data(0xE2);
	ili9806G_write_data(0x22);
	ili9806G_write_data(0x22);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xC7);
	ili9806G_write_data(0x63);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xED);
	ili9806G_write_data(0x7F);
	ili9806G_write_data(0x0F);
	ili9806G_write_data(0x00);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xC0);
	ili9806G_write_data(0x03);
	ili9806G_write_data(0x0B);
	ili9806G_write_data(0x00);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xFC);
	ili9806G_write_data(0x08);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xDF);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x20);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xF3);
	ili9806G_write_data(0x74);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xF9);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0xFD);
	ili9806G_write_data(0x80);
	ili9806G_write_data(0x80);
	ili9806G_write_data(0xC0);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xB4);
	ili9806G_write_data(0x02);
	ili9806G_write_data(0x02);
	ili9806G_write_data(0x02);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xF7);
	ili9806G_write_data(0x81);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xB1);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x13);
	ili9806G_write_data(0x13);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xF2);
	ili9806G_write_data(0xC0);
	ili9806G_write_data(0x02);
	ili9806G_write_data(0x40);
	ili9806G_write_data(0x28);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xC1);
	ili9806G_write_data(0x17);
	ili9806G_write_data(0x75);
	ili9806G_write_data(0x75);
	ili9806G_write_data(0x20);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xE0);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x08);
	ili9806G_write_data(0x0C);
	ili9806G_write_data(0x0F);
	ili9806G_write_data(0x15);
	ili9806G_write_data(0x09);
	ili9806G_write_data(0x07);
	ili9806G_write_data(0x01);
	ili9806G_write_data(0x06);
	ili9806G_write_data(0x09);
	ili9806G_write_data(0x16);
	ili9806G_write_data(0x14);
	ili9806G_write_data(0x3E);
	ili9806G_write_data(0x3E);
	ili9806G_write_data(0x00);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0xE1);
	ili9806G_write_data(0x00);
	ili9806G_write_data(0x09);
	ili9806G_write_data(0x12);
	ili9806G_write_data(0x12);
	ili9806G_write_data(0x13);
	ili9806G_write_data(0x1c);
	ili9806G_write_data(0x07);
	ili9806G_write_data(0x08);
	ili9806G_write_data(0x05);
	ili9806G_write_data(0x08);
	ili9806G_write_data(0x03);
	ili9806G_write_data(0x02);
	ili9806G_write_data(0x04);
	ili9806G_write_data(0x1E);
	ili9806G_write_data(0x1B);
	ili9806G_write_data(0x00);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0x3A);
	ili9806G_write_data(0x55);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0x35);
	ili9806G_write_data(0x00);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0x11);
	rt_thread_mdelay(5);
	ili9806g_write_cmd(0x29);
	rt_thread_mdelay(5);

	_drv_lcd_display_dir(6);

	rt_pin_write(LCD_BL_PIN, PIN_HIGH);

	// uint16_t color = 0x7edf;
	// drv_lcd_clear(0, 0, ILI9806G_LESS_PIXEL, ILI9806G_MORE_PIXEL, &color);
	return RT_EOK;
}

int drv_lcd_hw_init(void)
{
	rt_err_t result = RT_EOK;
	struct rt_device *device = &_lcd.parent;
	/* memset _lcd to zero */
	memset(&_lcd, 0x00, sizeof(_lcd));

	_lcd.lcd_info.bits_per_pixel = 16;
	_lcd.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565;

	device->type = RT_Device_Class_Graphic;
	device->init = drv_lcd_init;
	/* register lcd device */
	rt_device_register(device, LCD_DEV_NAME, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

	return result;
}
INIT_DEVICE_EXPORT(drv_lcd_hw_init);

/**
 * @
 */
