

/************************************************************************************************
** Includes
*************************************************************************************************/


#include <mach/board.h>
#include <mach/vreg.h>

#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <mach/gpio.h>
#include <asm/ioctls.h>
#include <linux/uaccess.h>
#include "snd_presto_audience_a2020.h"
#include <linux/gpio.h>

#include <linux/regulator/pmic8058-regulator.h>
#include <linux/pmic8058-othc.h>

#include "snd_presto_sub_fab2200.h"
#include <linux/i2c-gpio.h>

static struct i2c_client *audience_a2020_i2c_client = NULL;

#define POLLING_LOOP_COUNT 3

#define CONFIG_AUDIENCE_A2020_0627    // jmlee        

//#define CONFIG_AUDIENCE_DEBUG_PRINTK // jmlee 20110823

#ifndef byte // jmlee
#define byte u8
#endif

// A2020_CMD_BOOTLOAD_INITIATE is needed when reinitialize the system without resetting.
/* === not used === */
//SW Reset
#define A2020_CMD_SW_RESET 0x8002
#define A2020_CMD_SW_RESET_IMMEDIATE 0x0000
#define A2020_CMD_SW_RESET_DELAYED 0x0001
/* === not used === */
//Bootload Initiate
#define A2020_CMD_BOOTLOAD_INITIATE 0x8003
#define A2020_CMD_BOOTLOAD_INITIATE_ACK 0x80030000

#define A2020_CMD_BOOT 0x0001
#define A2020_CMD_BOOT_ACK 0x0101
#define A2020_CMD_BOOT_ACK_A2020 0x01

//Sync Control
#define A2020_CMD_SYNC 0x8000
#define A2020_CMD_SYNC_ACK 0x80000000L
#define A2020_CMD_SYNC_ONLY_ACK 0x80000005L
#define A2020_CMD_SYNC_MODE_POLLING 0x0000
#define A2020_CMD_SYNC_MODE_ACTIVE_LOW 0x0001
#define A2020_CMD_SYNC_MODE_ACTIVE_HIGH 0x0002
#define A2020_CMD_SYNC_MODE_FALLING_EDGE 0x0003
#define A2020_CMD_SYNC_MODE_RISING_EDGE 0x0004

//Set Power State
#define A2020_CMD_SLEEP 0x8010
#define A2020_CMD_SLEEP_VALUE 0x0001
#define A2020_CMD_SLEEP_ACK 0x80100001L

//Get/Set Device Parameter
#define A2020_CMD_GET_DEVICE_PARAMID 0x800B
#define A2020_CMD_DEVICE_PARAMID 0x800C
#define A2020_CMD_DEVICE_PARAM 0x800D

//Get/Set Algorithm Parameter
#define A2020_CMD_GET_ALGO_PARAMID 0x8016
#define A2020_CMD_ALGO_PARAMID 0x8017
#define A2020_CMD_ALGO_PARAM 0x8018

//Get/Set Digital Input Gain
#define A2020_CMD_SET_DIGI_INPUT_GAIN 0x801B
#define A2020_CMD_GET_DIGI_INPUT_GAIN 0x801A

//Get/Set Digital Output Gain
#define A2020_CMD_SET_DIGI_OUTPUT_GAIN 0x8015
#define A2020_CMD_GET_DIGI_OUTPUT_GAIN 0x801D

//Select Audio Route ID
#define A2020_CMD_SET_AUDIO_ROUTING 0x8026
#define A2020_CMD_GET_AUDIO_ROUTING 0x8027

//Enable/Disable Voice Processing
#define A2020_CMD_VOICE_PROCESSING 0x801C  // A100_msg_Bypass

#define ABS(a) (((a)>=(0))?(a):-(a))

#if 1
static const u8 a2020_firmware_image1[] =
{
#include "Presto_Audience_bin_1st.txt"
//#include "Presto_Audience_bin_1st_0811.txt"
};

static const u8 a2020_firmware_image2[] =
{
#include "Presto_Audience_bin_2nd.txt"
//#include "Presto_Audience_bin_2nd_0811.txt"
};
#else
static const u8 a2020_firmware_image[] =
{
#include "Presto_Audience_bin.txt"  //20110408 jhsong
};

#endif
int bBootSequence = 0;
int bVPonoff = 0;
a2020_talk_mode curr_talk_mode = MAX_TALK;
a2020_talk_mode prev_talk_mode = MAX_TALK;
a2020_mic_mode curr_mic_mode = BOTH_MIC_ON;
	
static a2020_status_type a2020_mode = A2020_MAX_MODE;
static a2020_sleep_status sleep_status = A2020_SLEEP_NONE;

typedef struct 
{
    u16 cmd;
    u16 value;
} a2020_reset_param;

//Software Reset
static a2020_reset_param sw_reset_param[] = 
{
    { A2020_CMD_SW_RESET, A2020_CMD_SW_RESET_DELAYED }, //Software Delayed Reset
    { 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a2020_device_param;

//Device Param
static a2020_device_param device_param[] = 
{
    //Set Device Parameter
    { A2020_CMD_DEVICE_PARAMID, 0x1001, A2020_CMD_DEVICE_PARAM, 0x0002 }, //ADC0 : 6.02dB
    { A2020_CMD_DEVICE_PARAMID, 0x1101, A2020_CMD_DEVICE_PARAM, 0x0002 }, //ADC1 : 6.02dB
    { A2020_CMD_DEVICE_PARAMID, 0x0a02, A2020_CMD_DEVICE_PARAM, 0x0001 }, 
    { A2020_CMD_DEVICE_PARAMID, 0x0a03, A2020_CMD_DEVICE_PARAM, 0x0001 },     
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd1;
    u16 id;
    u16 cmd2;
    u16 value;
} a2020_algo_param;

//DV
static a2020_algo_param algo_dv_param[] = 
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0006 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x0005 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x0000 },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0xFFF7 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0006 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xfffc },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },

    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

};

//DV VP Off
static a2020_algo_param algo_dv_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x000a },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x000a },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0xFFF7 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xffec },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT
static a2020_algo_param algo_ct_param[] = 
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0006 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFEC },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0006 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xfff7 },
    
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },

    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

};

//CT 1MIC
static a2020_algo_param algo_ct_1MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 },
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0005 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT VP Off
static a2020_algo_param algo_ct_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFEC },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xfff7 },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off close talk
// 20111004 jmlee 1mic only receiver path
static a2020_algo_param algo_ct_1MIC_VPOFF_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFEC },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xfff7 },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//FT
static a2020_algo_param algo_ft_param[] = 
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0006 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x0005 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x0000 },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0xFFF7 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0006 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xfffc },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },

    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },

};

//FT VP Off
static a2020_algo_param algo_ft_VPOFF_param[] = 
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFE6 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x000a },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x000a },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0xFFF7 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0040, A2020_CMD_ALGO_PARAM, 0xffec },

    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0030, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0031, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Voice LoopBack Primary/Secondary MIC VP Off
static a2020_algo_param algo_ct_vl_MIC_param[] =  
{
    //Set Algorithm Parameter    
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004B, A2020_CMD_ALGO_PARAM, 0x0006 },
    { A2020_CMD_ALGO_PARAMID, 0x0015, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0016, A2020_CMD_ALGO_PARAM, 0xFFEE },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    	
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0x0000 },    
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x004C, A2020_CMD_ALGO_PARAM, 0x0005 },

    { A2020_CMD_ALGO_PARAMID, 0x0010, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0011, A2020_CMD_ALGO_PARAM, 0x0019 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0000 },

    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { A2020_CMD_ALGO_PARAMID, 0x004F, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0050, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0051, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0052, A2020_CMD_ALGO_PARAM, 0x0000 },

    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

//Call Primary/Secondary MIC Test VP On
static a2020_algo_param algo_ct_call_MIC_param[] =  
{
    //Set Algorithm Parameter
    { A2020_CMD_ALGO_PARAMID, 0x0002, A2020_CMD_ALGO_PARAM, 0x0003 },
    { A2020_CMD_ALGO_PARAMID, 0x0000, A2020_CMD_ALGO_PARAM, 0x0005 },
    { A2020_CMD_ALGO_PARAMID, 0x0003, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0012, A2020_CMD_ALGO_PARAM, 0xFFD3 },
    { A2020_CMD_ALGO_PARAMID, 0x0023, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x0034, A2020_CMD_ALGO_PARAM, 0xFFFB },
    { A2020_CMD_ALGO_PARAMID, 0x0009, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x0025, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x003D, A2020_CMD_ALGO_PARAM, 0x000B },
    { A2020_CMD_ALGO_PARAMID, 0x000E, A2020_CMD_ALGO_PARAM, 0x0001 },

    { A2020_CMD_ALGO_PARAMID, 0x000F, A2020_CMD_ALGO_PARAM, 0x000F },
    { A2020_CMD_ALGO_PARAMID, 0x0004, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0028, A2020_CMD_ALGO_PARAM, 0x0000 },
    { A2020_CMD_ALGO_PARAMID, 0x0020, A2020_CMD_ALGO_PARAM, 0x0002 },
    { A2020_CMD_ALGO_PARAMID, 0x001F, A2020_CMD_ALGO_PARAM, 0x0002 },

    { A2020_CMD_ALGO_PARAMID, 0x001A, A2020_CMD_ALGO_PARAM, 0x0001 },
    { A2020_CMD_ALGO_PARAMID, 0x001B, A2020_CMD_ALGO_PARAM, 0xFFAC },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

typedef struct 
{
    u16 cmd;
    u16 value;
} a2020_path_param;

//Voice Processing On
static a2020_path_param path_vp_on_param[] = 
{
    { A2020_CMD_VOICE_PROCESSING, 0x0001 }, //Voice Processing On
    { 0xFFFF, 0xFFFF },
};

//Voice Processing Off
static a2020_path_param path_vp_off_param[] = 
{
    { A2020_CMD_VOICE_PROCESSING, 0x0001 }, //Voice Processing Off
    { 0xFFFF, 0xFFFF },
};

//Audio Routing
// jmlee 2020 CT init data
static a2020_path_param path_routing_param[] = 
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0001 },
    { 0xFFFF, 0xFFFF },
};


//Audio Routing Parameter 0x000A Setting
// jmlee 2020 FT init data
static a2020_path_param path_ct_A_routing_param[] = 
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0002 },
    { 0xFFFF, 0xFFFF },
};

//Voice LoopBack Primary MIC Audio Routing
static a2020_path_param path_vl_pri_MIC_routing_param[] =
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0007 },
    { 0xFFFF, 0xFFFF },
};

//Voice LoopBack Secondary MIC Audio Routing
static a2020_path_param path_vl_sec_MIC_routing_param[] =
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0049 },
    { 0xFFFF, 0xFFFF },
};

//Call Primary MIC Audio Routing
static a2020_path_param path_call_pri_MIC_routing_param[] =
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x000A },
    { 0xFFFF, 0xFFFF },
};

//Call Secondary MIC Audio Routing
static a2020_path_param path_call_sec_MIC_routing_param[] =
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0008 },
    { 0xFFFF, 0xFFFF },
};

// jmlee 2020 DV/FT OFF init data
static a2020_path_param path_call_sec_DV_MIC_routing_param[] =
{
    { A2020_CMD_SET_AUDIO_ROUTING, 0x0002 },
    { 0xFFFF, 0xFFFF },
};

//KeyTone Digital Gain
static a2020_path_param path_keytone_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0203 },
    { 0xFFFF, 0xFFFF },
};


//CT Digital Gain
static a2020_path_param path_ct_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0006 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0106 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0300 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x02F9 }, 
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x00F7 },
    //{ A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x01fe }, 
    { 0xFFFF, 0xFFFF },
};

//CT 1MIC Digital Gain
static a2020_path_param path_ct_1MIC_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT VP Off Digital Gain
static a2020_path_param path_ct_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0006 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0106 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x02F9 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0300 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x00F7 }, //Rx
    { 0xFFFF, 0xFFFF },
};

//CT 1MIC VP Off Digital Gain
// 20111004 jmlee 1mic only receiver path
static a2020_path_param path_ct_1MIC_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0006 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0xffa6},
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x02F9 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0300 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x00F7 },
    { 0xFFFF, 0xFFFF },
};


//FT Digital Gain
static a2020_path_param path_ft_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0007},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0107 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0203},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x03FC },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 },
    { 0xFFFF, 0xFFFF },
};

//FT VP Off Digital Gain
static a2020_path_param path_ft_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0007},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0107},
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0205 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x03FC },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 },
    { 0xFFFF, 0xFFFF },
};

//DV Digital Gain
static a2020_path_param path_dv_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0007},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0107 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0203},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x03FC },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 },
    { 0xFFFF, 0xFFFF },
};

//DV VP Off Digital Gain // jmlee
static a2020_path_param path_dv_VPOFF_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0007},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0107},
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0205 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x03FC },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 },
    { 0xFFFF, 0xFFFF },
};


//Voice LoopBack Primary/Secondary MIC Digital Gain
static a2020_path_param path_vl_MIC_gain_param[] = 
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0008},
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0108 },
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0300 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0000 },
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0200 },
    { 0xFFFF, 0xFFFF },
};

//InCall Primary/Secondary MIC Digital Gain
static a2020_path_param path_call_MIC_gain_param[] =
{
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0004 }, //Primary MIC
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0104 }, //Secondary MIC
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0004 }, //Tx
    { A2020_CMD_SET_DIGI_INPUT_GAIN, 0x0202 }, //Far End(Rx)
    { A2020_CMD_SET_DIGI_OUTPUT_GAIN, 0x0100 }, //Rx
    { 0xFFFF, 0xFFFF },
};

void A2020_msgCB_Chip_AckDataFlush(void)
{
    byte read_buf[4];
    struct i2c_msg msg;
    static int ret_value = 0;
    int check_count = 0;
	
    u32 get_value = 0;
    u32 swap_value = 0;

	while(1)
	{
	    if(!audience_a2020_i2c_client){
	        break;
	    }

	    memset((void*)read_buf, 0x00, 4);
		
	    msg.addr = audience_a2020_i2c_client->addr;
	    msg.flags = I2C_M_RD;
	    msg.len = 4;
	    msg.buf = read_buf;
	    
	    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);

	    if (ret_value < 0) 
	    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	    
	       printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
#endif
	    }

	    memcpy((byte*)&get_value,read_buf,4);
	    swap_value = SWAP32(get_value);
		
	    //printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush 0x%x !!!\n",swap_value);
		
	    if(swap_value == 0x0000)
	    {
	        break;
	    }
		
	    if(swap_value == 0xffff)
	    {
	        check_count++;
	        if(check_count > 4)
	        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                   printk( "[Snd_audience_a2020] A2020_msgCB_Chip_AckDataFlush swap_value IS 0xffff 4 times 0x%x !!!\n",swap_value);
#endif
		     break;
	        }
	    }
		
	}

}

int aud_a2020_is_active_mode(void)
{
    if(a2020_mode == A2020_ACTIVE_MODE)
        return 1;
    else
        return 0;
}

int aud_a2020_is_sleep_mode(void)
{
    if(a2020_mode == A2020_SLEEP_MODE)
        return 1;
    else
        return 0;
}

// length : byte num for sending (cmd + value)
// lengh limit 4 
// error num #define	ENXIO		 6	/* No such device or address */
int A2020_msg(u16 cmd, u16 value, u16 length)
{
    static int ret_value = 0;
    u16 len = 0;
    u16 swap_value = 0;

    unsigned char buf[4];	
    struct i2c_msg msg;

    if(len > 4) return 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
	
    len = length;
	
    swap_value = SWAP16(cmd);
    memcpy((byte*)&buf[0],(byte*)&swap_value, 2);
    swap_value = SWAP16(value);
    memcpy((byte*)&buf[2],(byte*)&swap_value, 2);
	

    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)buf;
    
    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    //printk( "[Snd_audience_a2020] A2020_msg cmd : 0x%x  value : 0x%x   !!!\n",cmd, value);

    if (ret_value < 0) 
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
       printk( "[Snd_audience_a2020] A2020_msg I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);    
#endif
        return 0;
    }

    return 1;	
}

int A2020_msgCB(u16 read_byte, byte * read_buf)
{
    static int ret_value = 0;
    struct i2c_msg msg;
	
    u32 get_value = 0;
    u32 swap_value = 0;

    if(!audience_a2020_i2c_client){
        return 0;
    }

    memset((void*)read_buf, 0x00, read_byte);
	
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = I2C_M_RD;
    msg.len = read_byte;
    msg.buf = read_buf;
    
    ret_value = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);

    if (ret_value < 0) 
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
       printk( "[Snd_audience_a2020] A2020_msgCB I2C ERROR OCCURE~~   I2C Error Num%d   !!!\n",ret_value);
#endif
       return 0;
    }

    memcpy((byte*)&get_value,read_buf,4);
    swap_value = SWAP32(get_value);
	
    //printk( "[Snd_audience_a2020] A2020_msgCB 0x%x !!!\n",swap_value);
	
    if(swap_value == 0x00)
    {
        //A2020 not ready
        return 0;
    }

    return 1;
}

int aud_boot_cmd_to_2020(void)
{
    int ret_val = 0;
    byte count = 0;

    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_BOOT, 0, 2); //send 2 bytes to the A2020

        mdelay(1); //sleep needed for firmware to process

        if(ret_val == 1)
        {
            byte read_buf[1];
            ret_val = A2020_msgCB(1, read_buf); //read the 1 byte ACK back

            if(ret_val == 1)
            {
                if(read_buf[0] != A2020_CMD_BOOT_ACK_A2020)					
                {
                    count++;
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                        printk( "[Snd_audience_a2020] A2020 Boot CMD ACK NOK !!!\n");
#endif
                        break;
                    }
                }
                else
                {
//                    printk( "[Snd_audience_a2020] A2020 Boot CMD ACK OK !!!\n");
                    break;
                }            
            }
            else
            {
                count++;
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                   printk( "[Snd_audience_a2020] A2020 Boot CMD I2C ACK NOK !!!\n");
#endif
                   break;
                }            
            }
        }
        else
        {
            count++;            
            mdelay(1);
            
            if(count > (POLLING_LOOP_COUNT * 6))
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Boot CMD I2C Write I2C NOK !!!\n");				
#endif
                break;
            }
        }
    }

    return ret_val;	
}

#if 1
int aud_bootimage_download_1st(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image1);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image1;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0){
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] aud_bootimage_download_1st NOK !!!\n");   
#endif
        return 0;
    }
    
    //printk( "[Snd_audience_a2020] aud_bootimage_download_1st OK !!!\n");
    return 1;
}


int aud_bootimage_download_2st(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image2);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image2;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0){
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] aud_bootimage_download_2st NOK !!!\n");   
#endif
        return 0;
    }
    
    //printk( "[Snd_audience_a2020] aud_bootimage_download_2st OK !!!\n");
    return 1;
}
#else
int aud_bootimage_download(void)
{

    struct i2c_msg msg;
    u16 len = 0;
    int ret_val = 0;
	
    if(!audience_a2020_i2c_client){
        return 0;
    }
   
    len = sizeof(a2020_firmware_image);
    
    msg.addr = audience_a2020_i2c_client->addr;
    msg.flags = 0;
    msg.len = len;
    msg.buf = (char *)a2020_firmware_image;
    
    ret_val = i2c_transfer(audience_a2020_i2c_client->adapter, &msg, 1);
	
    if (ret_val < 0) 
    {
        printk( "[Snd_audience_a2020] A2020 Boot DL I2C NOK !!!\n");   
        return 0;
    }
    
    printk( "[Snd_audience_a2020] A2020 Boot DL OK !!!\n");
    return 1;
}
#endif

int aud_bootload_initiate(void)
{
    int ret_val = 0;

#if 1
    ret_val = aud_bootimage_download_1st();
	if (ret_val < 0){
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk( "[Snd_audience_a2020] aud_bootimage_download_1st NOK !!!\n");	 
#endif
		return 0;
	}
	
	mdelay(1);
	
    ret_val = aud_bootimage_download_2st();
#else
    ret_val = aud_bootimage_download();	
#endif

    return ret_val;
}

int aud_sync_cmd_to_2020(void)
{
    int ret_val = 0;
    u32 get_value = 0;
    u32 swap_value = 0;
    byte count = 0;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
    printk( "[Snd_audience_a2020] aud_sync_cmd_to_2020 !!!\n");
#endif	
    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_SYNC, A2020_CMD_SYNC_MODE_POLLING, 4);

        if(ret_val == 1)
        {
            byte read_buf[4];
            if(bBootSequence)
           {
                mdelay(20); //need to wait 20ms for firmware to respond
      	     }
            else
           {
                mdelay(20);  // 20110824 jmlee firmware to respond
                //mdelay(1); //wait 1ms to try another read attempt
      	     }
			
            ret_val = A2020_msgCB(4,read_buf);
    
            if(ret_val == 1)
            {
                memcpy((byte*)&get_value, read_buf, 4);
                swap_value = SWAP32(get_value);
                    		
                if(swap_value != A2020_CMD_SYNC_ACK)
                {
                    count++;
                    mdelay(20);  // 20110824 jmlee firmware to respond
                    //mdelay(1);
					
                    if(count > POLLING_LOOP_COUNT)
                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                        printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ACK NOK 1 @@@@ !!!\n");				
#endif
                        break;
                    }
                }
                else
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ACK OK !!!\n");  // -2147483648				
#endif
                    break;
                }                
            }
            else
            {
                count++;
                mdelay(20);  // 20110824 jmlee firmware to respond				
                //mdelay(1);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK				
                printk( "[Snd_audience_a2020] A2020 Boot Sync POLLING_LOOP_COUNT %d !!!\n",count);					
#endif
                if(count > POLLING_LOOP_COUNT)
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Boot Sync POLLING_LOOP_COUNT %d !!!\n",count);					
#endif
                    break;
                }
            }			
        }
        else
        {
            count++;   
            mdelay(20);  // 20110824 jmlee firmware to respond
            //mdelay(1);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Boot Sync CMD I2C Write NOK 2 !!!POLLING_LOOP_COUNT : %d !!!\n",count);		
#endif
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Boot Sync CMD I2C Write NOK 3 ~~ POLLING_LOOP_COUNT : %d !!!\n",count);		
#endif
                break;
            }
        }
    }
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] A2020 Boot Sync CMD ret_val : %d !!!\n",ret_val);	
#endif
    return ret_val;
}

int aud_sleep_cmd_to_2020(void)
{
    int ret_val = 0;
#ifndef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020	
    u32 get_value = 0;
    u32 swap_value = 0;
#endif
    byte count = 0;
    
    if(aud_a2020_is_sleep_mode())
        return 1;
	
        A2020_msgCB_Chip_AckDataFlush();
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] ######### aud_sync_cmd_to_2020() Test IN  aud_sleep_cmd_to_2020() !!!\n");
#endif
    while(1)
    {
        ret_val = A2020_msg(A2020_CMD_SLEEP, A2020_CMD_SLEEP_VALUE, 4);

// if sleep command with eS310, it will enter idle and lowpower state
// so, i2c will be fail '-6'
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
        if(ret_val == 1)
        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK        
            printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK OK !!!\n");
#endif
            break;
        }
        else
        {
            count++;                
            mdelay(1);

            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C ACK NOK !!!\n");	
#endif
                break;
            }
         }			
 #else
        if(ret_val == 1)
        {
            byte read_buf[4];
            mdelay(20); //After sending the sleep command
			
            ret_val = A2020_msgCB(4,read_buf);

            if(ret_val == 1)
            {
                memcpy((byte*)&get_value,read_buf,4);
                swap_value = SWAP32(get_value);
                		
                if(swap_value != A2020_CMD_SLEEP_ACK)
                {
                    count++;                    
                    mdelay(1);	

                    if(count > POLLING_LOOP_COUNT)
                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                        printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK NOK !!!\n");			
#endif
                        break;
                    }
                }
                else
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Sleep CMD ACK OK !!!\n");
#endif
                    break;
                }
            }
            else
            {
                count++;                
                mdelay(1);

                if(count > POLLING_LOOP_COUNT)
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C ACK NOK !!!\n");	
#endif
                    break;
                }
            }			
        }
        else
        {
            count++;            
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Sleep CMD I2C Write NOK !!!\n");	
#endif
                break;
            }
        }
#endif	

    }

    return ret_val;
}

void aud_a2020_clk_on(void)
{
    gpio_request(GPIO_XO_OUT_D0_EN, "GPIO_XO_OUT_D0_EN");
    gpio_direction_output(GPIO_XO_OUT_D0_EN,1);  //high

    //Clock Stablize
    //Wait 4 Clock Cycles
    mdelay(4);   
}

void aud_a2020_clk_off(void)
{
    if(a2020_mode == A2020_SLEEP_MODE)
    {

    }
    gpio_request(GPIO_XO_OUT_D0_EN, "GPIO_XO_OUT_D0_EN");
    gpio_direction_output(GPIO_XO_OUT_D0_EN,0);  //Low    
}

int aud_a2020_wakeup_sequence(void)
{
    int ret_val = 0;

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] A2020 Wake Up Start !!!\n");	
#endif
    aud_a2020_clk_on();

    //First toggle the wake up pin to start the sequence 

    config_GPIO_NR_WAKEUP_N(0);

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] A2020 Wake Up Start --> config_GPIO_NR_WAKEUP_N(0); !!!\n");	
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
#if 1 // 20110531 audience debugging with hong suk-ho 
    mdelay(90);  // 20110824 jmlee firmware to respond
    //mdelay(60); //need to wait 30ms for eS310
#else
    mdelay(30); //need to wait 30ms for eS310
#endif	
#else
    mdelay(10); //need to wait 10ms for warm boot to finish xxx --- jmlee : more than 10ms needed 
    //mdelay(10);  // jmlee add more time
    //mdelay(10);  // jmlee add more time    
#endif	

    ret_val = aud_sync_cmd_to_2020();

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] ===================== A2020 SYNC CHECK 5 ]]]]>>> !!!\n");	
#endif

    //Put the wake up pin in the default state for the next wake up sequence	
    config_GPIO_NR_WAKEUP_N(1);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] A2020 Wake Up Start --> config_GPIO_NR_WAKEUP_N(1); !!!\n");	
#endif
    if(ret_val == 1)
    {
        a2020_mode = A2020_WAKEUP_MODE;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Wake Up Sequence OK !!!\n");
#endif
    }
    else
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Wake Up Sequence NOK !!!\n");			
#endif
        a2020_mode = A2020_MAX_MODE;
    }
	
    return ret_val;
}


int aud_config_sw_reset_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(sw_reset_param);    
    size = size/sizeof(a2020_reset_param);
    
    while(1)
    {
        if(sw_reset_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_1MIC_TALK:
            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_TALK:
            case FAR_VPOFF_TALK:
            case DV_TALK:  
            case DV_VPOFF_TALK:  
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);	
                break;

           default:
                ret_val = A2020_msg(sw_reset_param[index].cmd, sw_reset_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 SW Reset Write OK !!!\n");		
#endif
            mdelay(120);
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 SW Reset Write NOK !!!\n");
#endif
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_device_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
	
    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(device_param);
    size = size/sizeof(a2020_device_param);
    
    while(1)
    {
        while(1)
        {
            if(device_param[index].cmd1 == 0xFFFF && device_param[index].cmd2 == 0xFFFF)
                break;

            mdelay(1);

            ret_val = A2020_msg(device_param[index].cmd1, device_param[index].id, 4);				

            if(ret_val == 1)
            {
//                printk( "[Snd_audience_a2020] A2020 Device Param Write OK_1 !!!\n");

                mdelay(1);
        
                if(device_param[index].cmd2 == 0xFFFF && device_param[index].value == 0xFFFF)
                    break;

                ret_val = A2020_msg(device_param[index].cmd2, device_param[index].value, 4);					
                
                if(ret_val == 1)
                {
//                    printk( "[Snd_audience_a2020] A2020 Device Param Write ACK OK_2 !!!\n");
                    break;                
                }
                else
                {
                    count++;
                    			
                    if(count > POLLING_LOOP_COUNT)
                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                        printk( "[Snd_audience_a2020] A2020 Device Param Write NOK_2 !!!\n");
#endif
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Device Param Write NOK_1 !!!\n");	
#endif
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
            break;
        else
            index++;
    
        if(index >= size)  
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_algo_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;
#ifndef CONFIG_AUDIENCE_A2020_0627            	
    u32 get_value = 0;
    u32 swap_value1 = 0;
    u32 swap_value2 = 0;
#endif	
    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    switch(talk_mode)
    {
        case CLOSE_TALK:
            size = sizeof(algo_ct_param);
            break;

        case CLOSE_VPOFF_TALK:
            size = sizeof(algo_ct_VPOFF_param);
            break;

        case CLOSE_1MIC_TALK:
            size = sizeof(algo_ct_1MIC_param);
            break;
			
        case CLOSE_1MIC_VPOFF_TALK:
            size = sizeof(algo_ct_1MIC_VPOFF_param);
            break;

        case FAR_TALK:
            size = sizeof(algo_ft_param);
            break;

        case FAR_VPOFF_TALK:
            size = sizeof(algo_ft_VPOFF_param);
            break;

        case DV_TALK: 
            size = sizeof(algo_dv_param);
            break;

        case DV_VPOFF_TALK: 
            size = sizeof(algo_dv_VPOFF_param);
            break;

        case VL_PRI_MIC_TALK:
        case VL_SEC_MIC_TALK:
            size = sizeof(algo_ct_vl_MIC_param);
            break;

        case CALL_PRI_MIC_TALK:
        case CALL_SEC_MIC_TALK:
            size = sizeof(algo_ct_call_MIC_param);
            break;

        default:
            size = sizeof(algo_ct_param);			
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Param Size Error !!!\n");
#endif
            break;
    }

    size = size/sizeof(a2020_algo_param);
    
    while(1)
    {
        while(1)
        {
            if(talk_mode == CLOSE_TALK)
            {
                if(algo_ct_param[index].cmd1 == 0xFFFF && algo_ct_param[index].cmd2 == 0xFFFF)
                    break;
            }
            else if(talk_mode == CLOSE_VPOFF_TALK)
            {
                if(algo_ct_VPOFF_param[index].cmd1 == 0xFFFF && algo_ct_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CLOSE_1MIC_TALK)
            {
                if(algo_ct_1MIC_param[index].cmd1 == 0xFFFF && algo_ct_1MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            {
                if(algo_ct_1MIC_VPOFF_param[index].cmd1 == 0xFFFF && algo_ct_1MIC_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == FAR_TALK)
            {
                if(algo_ft_param[index].cmd1 == 0xFFFF && algo_ft_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == FAR_VPOFF_TALK)
            {
                if(algo_ft_VPOFF_param[index].cmd1 == 0xFFFF && algo_ft_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == DV_TALK) 
            {
                if(algo_dv_param[index].cmd1 == 0xFFFF && algo_dv_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == DV_VPOFF_TALK) 
            {
                if(algo_dv_VPOFF_param[index].cmd1 == 0xFFFF && algo_dv_VPOFF_param[index].cmd2 == 0xFFFF)
                    break;            
            }			
            else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            {
                if(algo_ct_vl_MIC_param[index].cmd1 == 0xFFFF && algo_ct_vl_MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            {
                if(algo_ct_call_MIC_param[index].cmd1 == 0xFFFF && algo_ct_call_MIC_param[index].cmd2 == 0xFFFF)
                    break;            
            }
            else
            {
                if(algo_ct_param[index].cmd1 == 0xFFFF && algo_ct_param[index].cmd2 == 0xFFFF)
                    break;            
            }

            mdelay(1);

            if(talk_mode == CLOSE_TALK)
            {
                ret_val = A2020_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }
            else if(talk_mode == CLOSE_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_VPOFF_param[index].cmd1, algo_ct_VPOFF_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_param[index].cmd1, algo_ct_1MIC_param[index].id);
            }
            else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd1, algo_ct_1MIC_VPOFF_param[index].id);
            }
            else if(talk_mode == FAR_TALK)
            {
#ifndef CONFIG_AUDIENCE_A2020_0627            
                A2020_msgCB_Chip_AckDataFlush(); // for msg CB 
#endif
                ret_val = A2020_msg(algo_ft_param[index].cmd1, algo_ft_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ft_param[index].cmd1, algo_ft_param[index].id);
            }
            else if(talk_mode == FAR_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ft_VPOFF_param[index].cmd1, algo_ft_VPOFF_param[index].id);
            }
            else if(talk_mode == DV_TALK)
            {
                ret_val = A2020_msg(algo_dv_param[index].cmd1, algo_dv_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_dv_param[index].cmd1, algo_dv_param[index].id);
            }
            else if(talk_mode == DV_VPOFF_TALK)
            {
                ret_val = A2020_msg(algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_dv_VPOFF_param[index].cmd1, algo_dv_VPOFF_param[index].id);
            }			
            else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_vl_MIC_param[index].cmd1, algo_ct_vl_MIC_param[index].id);
            }
            else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            {
                ret_val = A2020_msg(algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id, 4);
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_call_MIC_param[index].cmd1, algo_ct_call_MIC_param[index].id);
            }
            else
            {
                ret_val = A2020_msg(algo_ct_param[index].cmd1, algo_ct_param[index].id, 4);				
                //printk( "[Snd_audience_a2020]CMD1 : %x, ID : %x!!!", algo_ct_param[index].cmd1, algo_ct_param[index].id);
            }

            if(ret_val == 1)
            {
//                printk( "[Snd_audience_a2020] A2020 Param Write OK_1 !!!\n");

                mdelay(1);
        
#ifndef CONFIG_AUDIENCE_A2020_0627            
                if(talk_mode == FAR_TALK)
                {
                    byte read_buf[4];
                    ret_val = A2020_msgCB(4,read_buf);
                    
                    if(ret_val == 1)
                    {
                        memcpy((byte*)&get_value, read_buf, 2);
                        swap_value1 = SWAP16(get_value);
                        	
                        memcpy((byte*)&get_value, &read_buf[2], 2);
                        swap_value2 = SWAP16(get_value);
        
                        if(swap_value1 != A2020_CMD_ALGO_PARAMID)
                        {
                            count++;
                            mdelay(20);
        
                            if(count > POLLING_LOOP_COUNT)
                            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                printk( "[Snd_audience_a2020] A2020 Algo CMD1 ACK NOK !!!\n");
#endif
                                break;
                            }
                        }
                        else
                        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                            printk( "[Snd_audience_a2020]Algo CMD1 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].id);
#endif
    
                            if(swap_value2 == algo_ft_param[index].id)
                            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                printk( "[Snd_audience_a2020] A2020 Algo Param ID ACK OK !!!\n");                        
#endif
                            }
                            else
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                    printk( "[Snd_audience_a2020] A2020 Algo Param ID ACK NOK !!!\n");
#endif
                                    break;
                                }                            
                            }
                        }
                    }
                    else
                    {
                        count++;
                        mdelay(20);
    
                        if(count > POLLING_LOOP_COUNT)
                        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                            printk( "[Snd_audience_a2020] A2020 Algo CMD1 Response ACK NOK !!!\n");
#endif
                            break;
                        }
                    }
                }
#endif

                if(talk_mode == CLOSE_TALK)
                {
                    if(algo_ct_param[index].cmd2 == 0xFFFF && algo_ct_param[index].value == 0xFFFF)
                        break;
                }					
                else if(talk_mode == CLOSE_VPOFF_TALK)
                {
                    if(algo_ct_VPOFF_param[index].cmd2 == 0xFFFF && algo_ct_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CLOSE_1MIC_TALK)
                {
                    if(algo_ct_1MIC_param[index].cmd2 == 0xFFFF && algo_ct_1MIC_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
                {
                    if(algo_ct_1MIC_VPOFF_param[index].cmd2 == 0xFFFF && algo_ct_1MIC_VPOFF_param[index].value == 0xFFFF)
                        break;
                }					
                else if(talk_mode == FAR_TALK)
                {
                    if(algo_ft_param[index].cmd2 == 0xFFFF && algo_ft_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == FAR_VPOFF_TALK)
                {
                    if(algo_ft_VPOFF_param[index].cmd2 == 0xFFFF && algo_ft_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == DV_TALK) 
                {
                    if(algo_dv_param[index].cmd2 == 0xFFFF && algo_dv_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == DV_VPOFF_TALK) 
                {
                    if(algo_dv_VPOFF_param[index].cmd2 == 0xFFFF && algo_dv_VPOFF_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
                {
                    if(algo_ct_vl_MIC_param[index].cmd2 == 0xFFFF && algo_ct_vl_MIC_param[index].value == 0xFFFF)
                        break;
                }
                else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
                {
                    if(algo_ct_call_MIC_param[index].cmd2 == 0xFFFF && algo_ct_call_MIC_param[index].value == 0xFFFF)
                        break;
                }
                else
                {
                    if(algo_ct_param[index].cmd2 == 0xFFFF && algo_ct_param[index].value == 0xFFFF)
                        break;
                }

                if(talk_mode == CLOSE_TALK)
                {
                    ret_val = A2020_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);					
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
                }
                else if(talk_mode == CLOSE_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_VPOFF_param[index].cmd2, algo_ct_VPOFF_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_param[index].cmd2, algo_ct_1MIC_param[index].value);
                }
                else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value, 4);					
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_1MIC_VPOFF_param[index].cmd2, algo_ct_1MIC_VPOFF_param[index].value);
                }
                else if(talk_mode == FAR_TALK)
                {
#ifndef CONFIG_AUDIENCE_A2020_0627                            
                A2020_msgCB_Chip_AckDataFlush(); // for msg CB 
#endif
                    ret_val = A2020_msg(algo_ft_param[index].cmd2, algo_ft_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ft_param[index].cmd2, algo_ft_param[index].value);
                }
                else if(talk_mode == FAR_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value, 4);
                   // printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ft_VPOFF_param[index].cmd2, algo_ft_VPOFF_param[index].value);
                }
                else if(talk_mode == DV_TALK)
                {
                    ret_val = A2020_msg(algo_dv_param[index].cmd2, algo_dv_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_dv_param[index].cmd2, algo_dv_param[index].value);
                }
                else if(talk_mode == DV_VPOFF_TALK)
                {
                    ret_val = A2020_msg(algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_dv_VPOFF_param[index].cmd2, algo_dv_VPOFF_param[index].value);
                }
                else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_vl_MIC_param[index].cmd2, algo_ct_vl_MIC_param[index].value);
                }
                else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
                {
                    ret_val = A2020_msg(algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value, 4);
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_call_MIC_param[index].cmd2, algo_ct_call_MIC_param[index].value);
                }
                else
                {
                    ret_val = A2020_msg(algo_ct_param[index].cmd2, algo_ct_param[index].value, 4);   
                    //printk( "[Snd_audience_a2020]CMD2 : %x, Value : %x!!!", algo_ct_param[index].cmd2, algo_ct_param[index].value);
                }

                if(ret_val == 1)
                {
//                    printk( "[Snd_audience_a2020] A2020 Param Write ACK OK_2 !!!\n");

#ifndef CONFIG_AUDIENCE_A2020_0627            
                    if(talk_mode == FAR_TALK)
                    {
                        byte read_buf[4];
                        ret_val = A2020_msgCB(4,read_buf);
                        
                        if(ret_val == 1)
                        {
                            memcpy((byte*)&get_value, read_buf, 2);
                            swap_value1 = SWAP16(get_value);
                            	
                            memcpy((byte*)&get_value, &read_buf[2], 2);
                            swap_value2 = SWAP16(get_value);
            
                            if(swap_value1 != A2020_CMD_ALGO_PARAM)
                            {
                                count++;
                                mdelay(20);
            
                                if(count > POLLING_LOOP_COUNT)
                                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                    printk( "[Snd_audience_a2020] A2020 Algo CMD2 ACK NOK !!!\n");
#endif
                                    break;
                                }
                            }
                            else
                            {
//                                printk( "[Snd_audience_a2020]Algo CMD2 ACK OK, Val2 = %x, FT ID = %x !!!", swap_value2, algo_ft_param[index].value);
        
                                if(swap_value2 == algo_ft_param[index].value)
                                {
//                                    printk( "[Snd_audience_a2020] A2020 Algo Value ACK OK !!!\n");
                                    break;
                                }
                                else
                                {
                                    count++;
                                    mdelay(20);
                
                                    if(count > POLLING_LOOP_COUNT)
                                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                        printk( "[Snd_audience_a2020] A2020 Algo Value ACK NOK !!!\n");
#endif
                                        break;
                                    }                            
                                }
                            }
                        }
                        else
                        {
                            count++;
                            mdelay(20);
        
                            if(count > POLLING_LOOP_COUNT)
                            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                                printk( "[Snd_audience_a2020] A2020 Algo CMD2 Response ACK NOK !!!\n");
#endif
                                break;
                            }
                        }
                    }
                    else
#endif
                    break;
                }
                else
                {
                    count++;
                    			
                    if(count > POLLING_LOOP_COUNT)
                    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                        printk( "[Snd_audience_a2020] A2020 Param Write NOK_2 !!!\n");
#endif
                        break;
                    }
                }
            }
            else
            {
                count++;
                				
                if(count > POLLING_LOOP_COUNT)
                {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                    printk( "[Snd_audience_a2020] A2020 Param Write NOK_1 !!!\n");	
#endif
                    break;
                }            
            }
        }
    
        if(ret_val == 0)
        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Algo Param Write Retry !!!\n");
#endif
            //break;
        }
        else
            index++;
    
        if(index >= size) 
        {
            break;
        }
    }

    return ret_val;
}

int aud_config_path_vp_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_vp_on_param);    
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_vp_on_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
            case CLOSE_1MIC_TALK:
            case FAR_TALK:
            case DV_TALK:
            case VL_PRI_MIC_TALK:
            case VL_SEC_MIC_TALK:
            case CALL_PRI_MIC_TALK:
            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);	
                break;

            case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK:
            case FAR_VPOFF_TALK:
            case DV_VPOFF_TALK: 
                ret_val = A2020_msg(path_vp_off_param[index].cmd, path_vp_off_param[index].value, 4);	
                break;

            default:
                ret_val = A2020_msg(path_vp_on_param[index].cmd, path_vp_on_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Path VP Param Write OK !!!\n");		
#endif
            mdelay(1);			
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Path VP Param Write NOK !!!\n");
#endif
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_keytone_gain_cmd_to_2020(void)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_keytone_gain_param);
	
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_keytone_gain_param[index].cmd == 0xFFFF)
            break;

        ret_val = A2020_msg(path_keytone_gain_param[index].cmd, path_keytone_gain_param[index].value, 4);

        if(ret_val == 1)
        {
//            printk( "[Snd_audience_a2020] A2020 Path Digital KeyTone Gain Param Write OK !!!\n");		
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Path Digital KeyTone Gain Param Write NOK !!!\n");
#endif
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_gain_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    switch(talk_mode)
    {
        case CLOSE_TALK:
            size = sizeof(path_ct_gain_param);
            break;

        case CLOSE_VPOFF_TALK:
            size = sizeof(path_ct_VPOFF_gain_param);
            break;

        case CLOSE_1MIC_TALK:
            size = sizeof(path_ct_1MIC_gain_param);
            break;

        case CLOSE_1MIC_VPOFF_TALK:
            size = sizeof(path_ct_1MIC_VPOFF_gain_param);
            break;

        case FAR_TALK:
            size = sizeof(path_ft_gain_param);
            break;
			
        case FAR_VPOFF_TALK:
            size = sizeof(path_ft_VPOFF_gain_param);
            break;

        case DV_TALK: 
            size = sizeof(path_dv_gain_param);
            break;
			
        case DV_VPOFF_TALK: 
            size = sizeof(path_dv_VPOFF_gain_param);
            break;


        case VL_PRI_MIC_TALK:
        case VL_SEC_MIC_TALK:
            size = sizeof(path_vl_MIC_gain_param);
            break;

        case CALL_PRI_MIC_TALK:
        case CALL_SEC_MIC_TALK:
            size = sizeof(path_call_MIC_gain_param);
            break;

        default:
            size = sizeof(path_ct_gain_param);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Size Error !!!\n");			
#endif
            break;		
    }
	
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(talk_mode == CLOSE_TALK)
        {
            if(path_ct_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_VPOFF_TALK)
        {
            if(path_ct_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_1MIC_TALK)
        {
            if(path_ct_1MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
        {
            if(path_ct_1MIC_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == FAR_TALK)
        {
            if(path_ft_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == FAR_VPOFF_TALK)
        {
            if(path_ft_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == DV_TALK) 
        {
            if(path_dv_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == DV_VPOFF_TALK) 
        {
            if(path_dv_VPOFF_gain_param[index].cmd == 0xFFFF)
                break;
        }		
        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
        {
            if(path_vl_MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }
        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
        {
            if(path_call_MIC_gain_param[index].cmd == 0xFFFF)
                break;
        }			
        else
        {
            if(path_ct_gain_param[index].cmd == 0xFFFF)
                break;
        }

        if(talk_mode == CLOSE_TALK)
            ret_val = A2020_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);
        else if(talk_mode == CLOSE_VPOFF_TALK)
            ret_val = A2020_msg(path_ct_VPOFF_gain_param[index].cmd, path_ct_VPOFF_gain_param[index].value, 4);
        else if(talk_mode == CLOSE_1MIC_TALK)
            ret_val = A2020_msg(path_ct_1MIC_gain_param[index].cmd, path_ct_1MIC_gain_param[index].value, 4);
        else if(talk_mode == CLOSE_1MIC_VPOFF_TALK)
            ret_val = A2020_msg(path_ct_1MIC_VPOFF_gain_param[index].cmd, path_ct_1MIC_VPOFF_gain_param[index].value, 4);
        else if(talk_mode == FAR_TALK)
            ret_val = A2020_msg(path_ft_gain_param[index].cmd, path_ft_gain_param[index].value, 4);
        else if(talk_mode == FAR_VPOFF_TALK)
            ret_val = A2020_msg(path_ft_VPOFF_gain_param[index].cmd, path_ft_VPOFF_gain_param[index].value, 4);
        else if(talk_mode == DV_TALK) 
            ret_val = A2020_msg(path_dv_gain_param[index].cmd, path_dv_gain_param[index].value, 4);
        else if(talk_mode == DV_VPOFF_TALK) 
            ret_val = A2020_msg(path_dv_VPOFF_gain_param[index].cmd, path_dv_VPOFF_gain_param[index].value, 4);		
        else if(talk_mode == VL_PRI_MIC_TALK || talk_mode == VL_SEC_MIC_TALK)
            ret_val = A2020_msg(path_vl_MIC_gain_param[index].cmd, path_vl_MIC_gain_param[index].value, 4);
        else if(talk_mode == CALL_PRI_MIC_TALK || talk_mode == CALL_SEC_MIC_TALK)
            ret_val = A2020_msg(path_call_MIC_gain_param[index].cmd, path_call_MIC_gain_param[index].value, 4);
        else
            ret_val = A2020_msg(path_ct_gain_param[index].cmd, path_ct_gain_param[index].value, 4);

//        printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param !!!\n");

        if(ret_val == 1)
        {
            //printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Write OK !!!\n");		
            mdelay(1);
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Write NOK !!!\n");
#endif
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_path_routing_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;
    byte count = 0;
    byte index = 0;
    u16 size = 0;

    if(a2020_mode != A2020_ACTIVE_MODE)
        return 0;

    size = sizeof(path_routing_param);    
    size = size/sizeof(a2020_path_param);
    
    while(1)
    {
        if(path_routing_param[index].cmd == 0xFFFF)
            break;

        switch(talk_mode)
        {
            case CLOSE_TALK:
	     case CLOSE_VPOFF_TALK:
            case CLOSE_1MIC_VPOFF_TALK: // 20111004 jmlee 1mic only receiver path
                ret_val = A2020_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);	
                break;

            case CLOSE_1MIC_TALK:
            //case CLOSE_1MIC_VPOFF_TALK: // 20111004 jmlee 1mic only receiver path
            case FAR_TALK:
                ret_val = A2020_msg(path_ct_A_routing_param[index].cmd, path_ct_A_routing_param[index].value, 4);	
                break;
				
            case DV_TALK: 
            case DV_VPOFF_TALK: 
	     case FAR_VPOFF_TALK:
                ret_val = A2020_msg(path_call_sec_DV_MIC_routing_param[index].cmd, path_call_sec_DV_MIC_routing_param[index].value, 4);	
                break;
            case VL_PRI_MIC_TALK:
                ret_val = A2020_msg(path_vl_pri_MIC_routing_param[index].cmd, path_vl_pri_MIC_routing_param[index].value, 4);	
                break;

            case VL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_vl_sec_MIC_routing_param[index].cmd, path_vl_sec_MIC_routing_param[index].value, 4);	
                break;

            case CALL_PRI_MIC_TALK:
                ret_val = A2020_msg(path_call_pri_MIC_routing_param[index].cmd, path_call_pri_MIC_routing_param[index].value, 4);	
                break;

            case CALL_SEC_MIC_TALK:
                ret_val = A2020_msg(path_call_sec_MIC_routing_param[index].cmd, path_call_sec_MIC_routing_param[index].value, 4);	
                break;

            default:
                ret_val = A2020_msg(path_routing_param[index].cmd, path_routing_param[index].value, 4);
                break;                
        }

        if(ret_val == 1)
        {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
            printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Write OK !!!\n");		
#endif
            mdelay(1);			
            index++;

            if(index >= size)
            {
                break;
            }	        
        }
        else
        {
            count++;
            				
            if(count > POLLING_LOOP_COUNT)
            {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
                printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Write NOK !!!\n");
#endif
                break;
            }        
        }
    }

    return ret_val;	
}

int aud_config_algo_path_cmd_to_2020(a2020_talk_mode talk_mode)
{
    int ret_val = 0;

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] prev_talk_mode = %d, curr_talk_mode = %d \n", prev_talk_mode, talk_mode);	
#endif

#ifndef CONFIG_AUDIENCE_A2020_0627            
    ret_val = aud_config_sw_reset_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Path SW Reset Param Configration NOK !!!\n");
#endif
    }
#endif

    ret_val = aud_config_path_vp_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Path VP Param Configration NOK !!!\n");
#endif
    }

#if 1 // jmlee  todo for presto
    ret_val = aud_config_device_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK    
        printk( "[Snd_audience_a2020] A2020 Set Device Gain Param Configration NOK !!!\n");
#endif
    }
#endif

    ret_val = aud_config_path_gain_cmd_to_2020(talk_mode);

    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK    
        printk( "[Snd_audience_a2020] A2020 Path Digital Gain Param Configration NOK !!!\n");
#endif
    }

    ret_val = aud_config_algo_cmd_to_2020(talk_mode);

    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK    
        printk( "[Snd_audience_a2020] A2020 Algo Param Configration NOK !!!\n");
#endif
    }

    ret_val = aud_config_path_routing_cmd_to_2020(talk_mode);
    if(ret_val == 0)
    {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK    
        printk( "[Snd_audience_a2020] A2020 Path Audio Routing Param Configration NOK !!!\n");
#endif
    }

    prev_talk_mode = talk_mode;

    return ret_val;
}

int aud_a2020_active_mode_sequence(a2020_talk_mode talk_mode)
{
    int ret_val = 0;

    if(bBootSequence == 0)
    {
        return ret_val;
    }

    if(a2020_mode == A2020_ACTIVE_MODE)
    {
        ret_val = aud_config_algo_path_cmd_to_2020(talk_mode);
        return ret_val;
    }
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
    printk( "[Snd_audience_a2020] A2020 Active Sequence Start !!!\n");	
#endif
    ret_val = aud_a2020_wakeup_sequence();

/*================= End of Wake up Sequence ==================*/

    if(ret_val == 1)
    {
        a2020_mode = A2020_ACTIVE_MODE;
		
        aud_config_algo_path_cmd_to_2020(talk_mode);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
        printk( "[Snd_audience_a2020] A2020 Active Sequence OK !!!\n");
#endif
    }
    else
    {
        a2020_mode = A2020_MAX_MODE;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
        printk( "[Snd_audience_a2020] A2020 Active Sequence NOK !!!\n");		
#endif
    }
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
    printk( "[Snd_audience_a2020] A2020 Active Sequence End !!!\n");	
#endif

    return ret_val;
}

int aud_a2020_sleep_sequence(void)
{
    int ret_val = 1;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Sleep Sequence !!!\n");
#endif		
	
    if((a2020_mode == A2020_SLEEP_MODE) || (bBootSequence == 0) )
    {
        return 1;
    }
    sleep_status = A2020_SLEEP_START;
    ret_val = aud_sleep_cmd_to_2020();		

    if(ret_val)
    {
        mdelay(120);

        prev_talk_mode = MAX_TALK;
        a2020_mode = A2020_SLEEP_MODE;
        sleep_status = A2020_SLEEP_END;
        aud_a2020_clk_off();
//        printk( "[Snd_audience_a2020] A2020 Sleep Sequence OK !!!\n");
    }
    else
    {
        aud_a2020_clk_off();
        a2020_mode = A2020_MAX_MODE;
        sleep_status = A2020_SLEEP_NONE;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
        printk( "[Snd_audience_a2020] A2020 Sleep Sequence NOK !!!\n");
#endif
    }

    return ret_val;
}


int aud_a2020_boot_sequence(void)
{
    int ret_val = 0;
    byte count = 0;

AGAIN_BOOT:

    a2020_mode = A2020_MAX_MODE;
    bBootSequence = 0;

    config_GPIO_NR_WAKEUP_N(1);

    config_GPIO_NR_RESET_N(0);

    //Power On (may be NULL function if power always on)
    
    //Clock On 
    aud_a2020_clk_on();
	    
    //Reset High      
    config_GPIO_NR_RESET_N(1);

    //Wait 50ms
    mdelay(50);
	
    ret_val = aud_boot_cmd_to_2020();

    if(ret_val == 1)
    {
        ret_val = aud_bootload_initiate();

        if(ret_val == 1)
        {
            mdelay(10); //need to wait 10ms before firmware is ready

            ret_val = aud_sync_cmd_to_2020();

           if(ret_val == 1)
           {
               a2020_mode = A2020_BOOTUP_MODE;
               bBootSequence = 1;

               ret_val = aud_a2020_sleep_sequence();
           }
           else
           {
               count++;
               if(count < (POLLING_LOOP_COUNT))
               {
                   goto AGAIN_BOOT;
               }
           }		   
       }
       else
       {
           count++;
           if(count < (POLLING_LOOP_COUNT))
           {
               goto AGAIN_BOOT;
           }
        }
    }
    else
    {
        count++;
        if(count < (POLLING_LOOP_COUNT))
        {
            goto  AGAIN_BOOT;
        }
    }
	
//    printk( "[Snd_audience_a2020] A2020 Boot Up OK !!!\n");

    if(ret_val == 0)
    {
        a2020_mode = A2020_MAX_MODE;
        aud_a2020_clk_off();
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
        printk( "[Snd_audience_a2020] A2020 Boot Up NOK !!!\n");
#endif
    }

    return ret_val;
}

///////////////////////////////// >>>>>
// refered by marimba snddev_ecodec_data  [snddev_data_marimba.c]
// called at [snddev_ecodec.c] ==> snddev_ecodec_open/close function.

/*==========================================================================
** snd_subsystem_pmic_vreg_l2_on
**=========================================================================*/
static struct regulator *snddev_reg_l9_audience;
#if (BOARD_REV > WS10)
static struct regulator *snddev_reg_s3_audience;
#else
static struct regulator *snddev_reg_l8_audience;
#endif

// presto 8058_l9 and 8058_l8 power enable
int audience_pmic_vreg_switch_on(void)
{
	int rc = 0;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
	printk("[Snd_audience_a2020] audience_pmic_vreg_switch_on ==> START  \n");
#endif
	
	snddev_reg_l9_audience = regulator_get(NULL, "8058_l9");
	if (IS_ERR(snddev_reg_l9_audience)) {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l9", rc);
#endif
	}

	rc = regulator_set_voltage(snddev_reg_l9_audience, 2700000, 2700000);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l9", rc);
#endif
	}

	rc = regulator_enable(snddev_reg_l9_audience);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n",	 "8058_l9", rc);
#endif
	}

////
#if (BOARD_REV > WS10)
	snddev_reg_s3_audience = regulator_get(NULL, "8058_s3");
	if (IS_ERR(snddev_reg_s3_audience)) {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_s3", rc);
#endif
	}

	rc = regulator_set_voltage(snddev_reg_s3_audience, 1800000, 1800000);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_s3", rc);
#endif
	}

	rc = regulator_enable(snddev_reg_s3_audience);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n",	 "8058_s3", rc);
#endif
	}

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
	printk("[Snd_audience_a2020] audience_pmic_vreg_switch_on <== END  \n");
#endif
#else
	snddev_reg_l8_audience = regulator_get(NULL, "8058_l8");
	if (IS_ERR(snddev_reg_l8_audience)) {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l8", rc);
#endif
	}

	rc = regulator_set_voltage(snddev_reg_l8_audience, 1800000, 1800000);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l8", rc);
#endif
	}
	rc = regulator_enable(snddev_reg_l8_audience);
	if (rc < 0)
	{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n",	 "8058_l8", rc);
#endif
	}

#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	
	printk("[Snd_audience_a2020] audience_pmic_vreg_switch_on <== END  \n");
#endif
#endif
	return rc;
}


/*==========================================================================
** snd_subsystem_pmic_vreg_l2_off
**=========================================================================*/
// presto 8058_l9 and 8058_l8 power disable
int audience_pmic_vreg_switch_off(void)
{
	int rc = 0;
	if (!snddev_reg_l9_audience)
	{	
		rc = regulator_disable(snddev_reg_l9_audience);
		if (rc < 0)
		{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l9", rc);
#endif
		}

		regulator_put(snddev_reg_l9_audience);

		snddev_reg_l9_audience = NULL;
	}
#if (BOARD_REV > WS10)
	if (!snddev_reg_s3_audience)
	{	
		rc = regulator_disable(snddev_reg_s3_audience);
		if (rc < 0)
		{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK			
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_s3", rc);
#endif
		}

		regulator_put(snddev_reg_s3_audience);

		snddev_reg_s3_audience = NULL;
	}
#else	
	if (!snddev_reg_l8_audience)
	{	
		rc = regulator_disable(snddev_reg_l8_audience);
		if (rc < 0)
		{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l8", rc);
#endif
		}
		regulator_put(snddev_reg_l8_audience);

		snddev_reg_l8_audience = NULL;
	}
#endif
	return rc;
}

static unsigned audience_config_gpio_on[] = {
	GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(112, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(113, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(114, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_CLK */
};

static unsigned audience_config_gpio_off[] = {
	GPIO_CFG(111, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(112, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(113, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(114, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_CLK */};

static int Audience_Config_GPIO_onoff(int on)
{
	int pin, rc;

	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(audience_config_gpio_on); pin++) {
			rc = gpio_tlmm_config(audience_config_gpio_on[pin], GPIO_CFG_ENABLE);
			if (rc) {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK				
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",	__func__, audience_config_gpio_on[pin], rc);
#endif
				return -EIO;
			}
		}
	} else {
		for (pin = 0; pin < ARRAY_SIZE(audience_config_gpio_off); pin++) {
			rc = gpio_tlmm_config(audience_config_gpio_off[pin], GPIO_CFG_ENABLE);
			if (rc) {
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK				
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",	__func__, audience_config_gpio_off[pin], rc);
#endif
				return -EIO;
			}
		}
	}
	return 0;
}

#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
static int jmlee_adience_forced_test = 0;
void Audiece_Test_forced_by_jmlee(void)
{
        if(!jmlee_adience_forced_test)
      	{
	      printk("[JMLEE LOG] Audiece_Test_forced_by_jmlee ~~~~~~~ start \n");		  
             snd_audience_a2020_api_Init();
	      printk("[JMLEE LOG] Audiece_Test_forced_by_jmlee ~~~~~~~ end\n");
	      jmlee_adience_forced_test = 1;
             mdelay(500);
       }
}		
#endif

#define AUDIENCE_RESETTING_GUARD 
#ifdef AUDIENCE_RESETTING_GUARD
static int audience_a2020_ct_on_guard = 0;
static int audience_a2020_ct_2mic_on_guard = 0;
static int audience_a2020_ct_vpoff_on_guard = 0;
static int audience_a2020_ct_vpoff_1mic_on_guard = 0; // 20111004 jmlee 1mic only receiver path
static int audience_a2020_ft_on_guard = 0;
static int audience_a2020_ft_vpoff_on_guard = 0;
static int audience_a2020_dv_on_guard = 0;
static int audience_a2020_dv_vpoff_on_guard = 0;
#endif

void audience_a2020_ct_on(void)
{
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC 1 \n");
#endif

#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ct_on_guard) return;
     audience_a2020_ct_on_guard = 1;
#endif

#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC 2 \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(CLOSE_TALK);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	 
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC 3 \n");
#endif
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK	 
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_TALK ]] BY ECODEC  4 \n");	 
#endif
}

void audience_a2020_ct_2mic_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ct_2mic_on_guard) return;
     audience_a2020_ct_2mic_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ 2mic only ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(VL_SEC_MIC_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

// not used
void audience_a2020_ct_vpoff_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ct_vpoff_on_guard) return;
     audience_a2020_ct_vpoff_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_VPOFF_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(CLOSE_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();	 
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

// 20111004 jmlee 1mic only receiver path
void audience_a2020_ct_vpoff_1mic_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ct_vpoff_1mic_on_guard) return;
     audience_a2020_ct_vpoff_1mic_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ CLOSE_1MIC_VPOFF_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(CLOSE_1MIC_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on();	 
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}
void audience_a2020_ft_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ft_on_guard) return;
     audience_a2020_ft_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ FAR_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)
     aud_a2020_active_mode_sequence(FAR_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_ft_poweron();  
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

// not used
void audience_a2020_ft_vpoff_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_ft_vpoff_on_guard) return;
     audience_a2020_ft_vpoff_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ FAR_VPOFF_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_active_mode_sequence(FAR_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();	 
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();	
     snd_subsystem_nr_off_ft_poweron();
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

void audience_a2020_dv_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_dv_on_guard) return;
     audience_a2020_dv_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ DV_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_active_mode_sequence(DV_TALK);
     msm_snddev_mic_sel_a2020();
     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);
     msm_snddev_nr_receiver_amp_off();
     // FAB2200 ON Insert !!
     snd_subsystem_nr_dv_poweron(); 
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

// not used
void audience_a2020_dv_vpoff_on(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     if(audience_a2020_dv_vpoff_on_guard) return;
     audience_a2020_dv_vpoff_on_guard = 1;
#endif
#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
     Audiece_Test_forced_by_jmlee();
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ DV_VPOFF_TALK ]] BY ECODEC  \n");
#endif
     audience_pmic_vreg_switch_on();   //20110301 jhsong : rf 2.2VReg Control
     Audience_Config_GPIO_onoff(0); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_active_mode_sequence(DV_VPOFF_TALK);
     msm_snddev_mic_sel_a2020();
     pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
}

void audience_a2020_off(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     audience_a2020_ct_on_guard = 0;
     audience_a2020_ct_2mic_on_guard = 0;
     audience_a2020_ct_vpoff_on_guard = 0;
     audience_a2020_ct_vpoff_1mic_on_guard = 0; // 20111004 jmlee 1mic only receiver path	 
     audience_a2020_ft_on_guard = 0;
     audience_a2020_ft_vpoff_on_guard = 0;
     audience_a2020_dv_on_guard = 0;
     audience_a2020_dv_vpoff_on_guard = 0;
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ AUDIENCE OFF ]] BY ECODEC  \n");
#endif
     snd_subsystem_sp_powerdown(); // temp setting
     pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     // FAB2200 ON Insert !!
     audience_pmic_vreg_switch_off();	   //20110301 jhsong : rf 2.2VReg Control
}

void audience_a2020_ct_off(void)
{
#ifdef AUDIENCE_RESETTING_GUARD
     audience_a2020_ct_on_guard = 0;
     audience_a2020_ct_2mic_on_guard = 0;
     audience_a2020_ct_vpoff_on_guard = 0;
     audience_a2020_ct_vpoff_1mic_on_guard = 0; // 20111004 jmlee 1mic only receiver path
     audience_a2020_ft_on_guard = 0;
     audience_a2020_ft_vpoff_on_guard = 0;
     audience_a2020_dv_on_guard = 0;
     audience_a2020_dv_vpoff_on_guard = 0;
#endif
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
     printk("[Snd_audience_a2020] #@#@#@#@#@#@#@#@#  [[ AUDIENCE NR CT OFF ]] BY ECODEC  \n");
#endif

     msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker();
     mdelay(1);

     pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
     Audience_Config_GPIO_onoff(1); // Aux Pcm(about PCM Block <--> BT)	 
     aud_a2020_sleep_sequence();
     msm_snddev_nr_receiver_amp_off();
     audience_pmic_vreg_switch_off();	  //20110301 jhsong : rf 2.2VReg Control
}
///////////////////////////////// <<<<<

 int config_GPIO_NR_WAKEUP_N(int enable)
{
	int rc;
	gpio_free(GPIO_NR_WAKEUP_N);

	if (enable) {
		rc = gpio_request(GPIO_NR_WAKEUP_N, "GPIO_NR_WAKEUP_N");
		if (rc) {
			pr_err("%s: GPIO_NR_WAKEUP_N %d request"
			"failed\n", __func__, GPIO_NR_WAKEUP_N);
			return rc;
		}
		gpio_direction_output(GPIO_NR_WAKEUP_N, 1);
	} else {

		rc = gpio_request(GPIO_NR_WAKEUP_N, "GPIO_NR_WAKEUP_N");
		if (rc) {
			pr_err("%s: GPIO_NR_WAKEUP_N %d request"
			"failed\n", __func__, GPIO_NR_WAKEUP_N);
			return rc;
		}
		gpio_direction_output(GPIO_NR_WAKEUP_N, 0);
	}
	return 0;
}

 int config_GPIO_NR_RESET_N(int enable)
{
	int rc;
	gpio_free(GPIO_NR_RESET_N);

	if (enable) {
		rc = gpio_request(GPIO_NR_RESET_N, "GPIO_NR_RESET_N");
		if (rc) {
			pr_err("%s: GPIO_NR_RESET_N %d request"
			"failed\n", __func__, GPIO_NR_RESET_N);
			return rc;
		}
		gpio_direction_output(GPIO_NR_RESET_N, 1);
	} else {
		rc = gpio_request(GPIO_NR_RESET_N, "GPIO_NR_RESET_N");
		if (rc) {
			pr_err("%s: GPIO_NR_RESET_N %d request"
			"failed\n", __func__, GPIO_NR_RESET_N);
			return rc;
		}
		gpio_direction_output(GPIO_NR_RESET_N, 0);
	}
	return 0;
}

 int config_GPIO_MIC_SEL_SEL(int enable)
{
	int rc;
	gpio_free(GPIO_MIC_SEL_SEL);

	if (enable) {
		rc = gpio_request(GPIO_MIC_SEL_SEL, "GPIO_MIC_SEL_SEL");
		if (rc) {
			pr_err("%s: GPIO_MIC_SEL_SEL %d request"
			"failed\n", __func__, GPIO_MIC_SEL_SEL);
			return rc;
		}
		gpio_direction_output(GPIO_MIC_SEL_SEL, 1);
	} else {
		rc = gpio_request(GPIO_MIC_SEL_SEL, "GPIO_MIC_SEL_SEL");
		if (rc) {
			pr_err("%s: GPIO_MIC_SEL_SEL %d request"
			"failed\n", __func__, GPIO_MIC_SEL_SEL);
			return rc;
		}
		gpio_direction_output(GPIO_MIC_SEL_SEL, 0);
	}
	return 0;
}

  int config_GPIO_NR_AMP_EN(int enable)
{
	int rc;
	gpio_free(GPIO_NR_AMP_EN);

	if (enable) {
		rc = gpio_request(GPIO_NR_AMP_EN, "GPIO_NR_AMP_EN");
		if (rc) {
			pr_err("%s: GPIO_NR_AMP_EN %d request"
			"failed\n", __func__, GPIO_NR_AMP_EN);
			return rc;
		}
		gpio_direction_output(GPIO_NR_AMP_EN, 1);
	} else {
		rc = gpio_request(GPIO_NR_AMP_EN, "GPIO_NR_AMP_EN");
		if (rc) {
			pr_err("%s: GPIO_NR_AMP_EN %d request"
			"failed\n", __func__, GPIO_NR_AMP_EN);
			return rc;
		}
		gpio_direction_output(GPIO_NR_AMP_EN, 0);
	}
	return 0;
}

 int config_GPIO_NR_OUT_SEL(int enable)
{
	int rc;
	gpio_free(GPIO_NR_OUT_SEL);

	if (enable) {
		rc = gpio_request(GPIO_NR_OUT_SEL, "GPIO_NR_OUT_SEL");
		if (rc) {
			pr_err("%s: GPIO_NR_OUT_SEL %d request"
			"failed\n", __func__, GPIO_NR_OUT_SEL);
			return rc;
		}
		gpio_direction_output(GPIO_NR_OUT_SEL, 1);
	} else {
		rc = gpio_request(GPIO_NR_OUT_SEL, "GPIO_NR_OUT_SEL");
		if (rc) {
			pr_err("%s: GPIO_NR_OUT_SEL %d request"
			"failed\n", __func__, GPIO_NR_OUT_SEL);
			return rc;
		}
		gpio_direction_output(GPIO_NR_OUT_SEL, 0);
	}
	return 0;
}

static int audience_a2020_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
 	int rc = 0;
	
       audience_pmic_vreg_switch_on(); // switch power up.
	   
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		audience_a2020_i2c_client = NULL;
		rc = -1;
	}
	else {
		audience_a2020_i2c_client = client;
		
#if 0  // jmlee for test code i2c device check for AUDIENCE DEBUG TOOL
    gpio_tlmm_config(GPIO_CFG(73, 0, GPIO_CFG_INPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);    
    gpio_tlmm_config(GPIO_CFG(72, 0, GPIO_CFG_INPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);    
#endif

		aud_a2020_boot_sequence(); 
	}

	audience_pmic_vreg_switch_off();    //20110301 jhsong : rf 2.2VReg Control
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
	printk("[Snd_audience_a2020] audience_a2020_probe %d \n",rc);
#endif
	return rc;
}

static int __exit audience_a2020_remove(struct i2c_client *client)
{
	int rc = 0;
	audience_a2020_i2c_client = NULL;
	return rc;
}

/*static*/ int audience_a2020_suspend(struct i2c_client *client, pm_message_t mesg)
{
//	printk("[Snd_audience_a2020] audience_a2020_suspend  \n");
	return 0;
}

static int audience_a2020_resume(struct i2c_client *client)
{
//	printk("[Snd_audience_a2020] audience_a2020_resume  \n");

	return 0;
}

static void audience_a2020_shutdown(struct i2c_client *client)
{
//	printk("[Snd_audience_a2020] audience_a2020_shutdown \n");

}

static const struct i2c_device_id audience_a2020_id[] = {
	{ "audience-a2020", 0},
};

static struct i2c_driver audience_a2020_driver = {
	.id_table = audience_a2020_id,
	.probe  = audience_a2020_probe,
	.remove = __exit_p(audience_a2020_remove),
	.suspend = audience_a2020_suspend,
	.resume = audience_a2020_resume,
	.shutdown = audience_a2020_shutdown,
	.driver = {
		.name = "audience-a2020",
	},
};

void snd_audience_a2020_api_Init(void)
{
	int result = 0;
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK
	printk( "[Snd_audience_a2020] snd_audience_a2020_api_Init()\n");
#endif
	result = i2c_add_driver(&audience_a2020_driver);
	if(result){
#ifdef CONFIG_AUDIENCE_DEBUG_PRINTK		
	printk(KERN_ERR "[Snd_audience_a2020] snd_audience_a2020_api_Init  Fail\n");
#endif
	}
}

void snd_audience_a2020_api_DeInit(void)
{
	//misc_deregister(&miscdev);
	i2c_del_driver(&audience_a2020_driver);
}




