#ifndef __TOUCHPAD_H
#define __TOUCHPAD_H

#include <rtthread.h>
#include <rtdevice.h>

// STEP_3(optional): Specify your special config info if needed
#define GTP_MAX_HEIGHT 480
#define GTP_MAX_WIDTH 854
#define GTP_INT_TRIGGER 0

//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION "V2.2<2014/01/14>"
#define GTP_I2C_NAME "Goodix-TS"
#define GT91XX_CONFIG_PROC_FILE "gt9xx_config"
#define GTP_POLL_TIME 10
#define GTP_ADDR_LENGTH 2
#define GTP_CONFIG_MIN_LENGTH 186
#define GTP_CONFIG_MAX_LENGTH 256
#define FAIL 0
#define SUCCESS 1
#define SWITCH_OFF 0
#define SWITCH_ON 1

//******************** For GT9XXF Start **********************//
#define GTP_REG_BAK_REF 0x99EC
#define GTP_REG_MAIN_CLK 0x8020
#define GTP_REG_CHIP_TYPE 0x8000
#define GTP_REG_HAVE_KEY 0x8057
#define GTP_REG_MATRIX_DRVNUM 0x8069
#define GTP_REG_MATRIX_SENNUM 0x806A
#define GTP_REG_COMMAND 0x8040

#define GTP_COMMAND_READSTATUS 0
#define GTP_COMMAND_DIFFERENCE 1
#define GTP_COMMAND_SOFTRESET 2
#define GTP_COMMAND_UPDATE 3
#define GTP_COMMAND_CALCULATE 4
#define GTP_COMMAND_TURNOFF 5

#define GTP_FL_FW_BURN 0x00
#define GTP_FL_ESD_RECOVERY 0x01
#define GTP_FL_READ_REPAIR 0x02

#define GTP_BAK_REF_SEND 0
#define GTP_BAK_REF_STORE 1
#define CFG_LOC_DRVA_NUM 29
#define CFG_LOC_DRVB_NUM 30
#define CFG_LOC_SENS_NUM 31

#define GTP_CHK_FW_MAX 40
#define GTP_CHK_FS_MNT_MAX 300
#define GTP_BAK_REF_PATH "/data/gtp_ref.bin"
#define GTP_MAIN_CLK_PATH "/data/gtp_clk.bin"
#define GTP_RQST_CONFIG 0x01
#define GTP_RQST_BAK_REF 0x02
#define GTP_RQST_RESET 0x03
#define GTP_RQST_MAIN_CLOCK 0x04
#define GTP_RQST_RESPONDED 0x00
#define GTP_RQST_IDLE 0xFF

//******************** For GT9XXF End **********************//
// Registers define
#define GTP_READ_COOR_ADDR 0x814E
#define GTP_REG_SLEEP 0x8040
#define GTP_REG_SENSOR_ID 0x814A
#define GTP_REG_CONFIG_DATA 0x8050
#define GTP_REG_VERSION 0x8140

#define RESOLUTION_LOC 3
#define TRIGGER_LOC 8
#define X2Y_LOC (1 << 3)

#endif
