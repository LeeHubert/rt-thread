#ifndef __DRV_LCD_H
#define __DRV_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include <rtdevice.h>
#include <drv_common.h>

#define MY_DISP_HOR_RES    480
#define MY_DISP_VER_RES    854

void drv_lcd_clear(uint16_t usX, uint16_t usY, uint16_t ueX, uint16_t ueY, uint16_t *color);

#ifdef __cplusplus
}
#endif

#endif
