//----------------------------------------------------------------------------
//   FileName    : TC317X_Common.h
//    Description : 
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_COMMON_H__
#define __TC317X_COMMON_H__

#include "../tdmb_comdef.h"

#include "IO/TC317X_IO_UserDefine.h"

#if defined(USE_ANDROID)
#include <linux/types.h>
#elif defined(USE_WINCE)
#elif defined(USE_NEUCLEUS)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum
{
    DEVICE_BB_NONE = 0,
    DEVICE_BB_TC3X,
    DEVICE_BB_OTHER
} TCCBB_DEVICE;

typedef enum
{
    DEVICE_BB_NORMAL_BOARD = 0
} TCCBB_BoardDefine;

//---------------------------------------------------------------------------------------
//
//  TC3X Interface Form
//
typedef enum
{
    TC3X_IO_FAIL = -1,
    TC3X_IO_SUCCESS,
    TC3X_IO_ILLEGAL_INTF                                   //illegal interface
} TC3X_IO_Err;

//---------------------------------------------------------------------------------------
//
//      Interface Structure
//
typedef enum
{
    TC3X_INT_LEVEL_FALLING = 0,
    TC3X_INT_LEVEL_RISING,
    TC3X_INT_EDGE_FALLING,
    TC3X_INT_EDGE_RISING
} TC3X_INT_TRIG_Set;

// -----------------------------------------------------------------
// IO Series Enum
//
typedef enum
{
    TC3X_IF_SRAMLIKE,
    TC3X_IF_I2C,
    TC3X_IF_CSPI,
    TC3X_IF_SDIO1BIT,
    TC3X_IF_SDIO4BIT
} TC3X_MainIOSeries;

typedef enum
{
    TC3X_STREAM_IO_MAINIO,
    TC3X_STREAM_IO_PTS,                                    // DVB-T, DVBH : EDGE Int
    TC3X_STREAM_IO_STS,                                    // DVB-T, DVBH : EDGE Int
    TC3X_STREAM_IO_SPIMS,                                  // DVB-T, DVBH : EDGE Int
    TC3X_STREAM_IO_SPISLV,                                 // DVB-T, DVBH : LEVEL Int
    TC3X_STREAM_IO_HPI_HEADERON,                           // DVB-T : LEVEL Int
    // Header Format
    // Header[31:20] : 0xa80
    // ID [19:18] : 0x01:BufferA , 0x02:BufferB, 0x03:BufferC
    // Private[17:16]
    // Length[15:0] : WordSize
    TC3X_STREAM_IO_HPI_HEADEROFF,                          // DVB-T : LEVEL Int
    TC3X_STREAM_IO_MAX
} TC3X_StreamIOSeries;

//---------------------------------------------------------------------------------------
//
//  TC3X Standards Enum
//
typedef enum
{
    TC3X_STD_DVBT = 0,
    TC3X_STD_DMB,
    TC3X_STD_MAX
} STD_TC3X;

//---------------------------------------------------------------------------------------
//
//  TC3X Commands Enum
//
enum
{
    CMD_BB_INT_INIT = 0,
    CMD_BB_INT_ENABLE,
    CMD_BB_INT_DISABLE,
    CMD_BB_INT_PAUSE,
    CMD_BB_INT_RESUME
};

enum
{
    CMD_SDIO_INT_INIT = 0,
    CMD_SDIO_INT_ENABLE,
    CMD_SDIO_INT_DISABLE,
    CMD_SDIO_INT_PAUSE,
    CMD_SDIO_INT_RESUME
};

enum
{
    CMD_SPI_ALLOC_BUFF = 0,
    CMD_SPI_INIT,
    CMD_SPI_START,
    CMD_SPI_STOP
};

enum
{
    CMD_BB_COLDBOOT_NORMAL = 0,
    CMD_BB_COLDBOOT_BROADCAST,
    CMD_BB_COLDBOOT_NON_WRITE
};

// -----------------------------------------------------------------
//
// User Function Msg
//
typedef enum
{
    // BB
    CMD_BB_NONE = 0,
    CMD_BB_PW_RESET,
    CMD_BB_PW_DN,
    CMD_BB_PW_ON,
    CMD_BB_PW_TSPD,
    CMD_BB_PW_TSPON,
    CMD_BB_WARMBOOT,
    CMD_BB_SET_FIFO_SIZE,

    CMD_BB_REG_R,
    CMD_BB_REG_W,

    CMD_BB_MAILBOX_R,
    CMD_BB_MAILBOX_W,

    CMD_BB_STREAMOUT_PAUSE,
    CMD_BB_STREAMOUT_RESTART,

    CMD_BB_CHECK_OP,

    CMD_BB_GET_CHIPID,

    CMD_BB_WRITE_MEM,
    CMD_BB_READ_MEM,

    CMD_BB_PLL_SET,

    CMD_BB_GET_BOOTCODE_VERSION,

    CMD_BB_TDMB_REG,
    CMD_BB_TDMB_UNREG,

    CMD_GET_TDMB_LOCK,
    CMD_GET_TDMB_SNR_SET,
    CMD_GET_TDMB_PCBER_SET,
    CMD_GET_TDMB_VITERBIBER_SET,
    CMD_GET_TDMB_TSPER_SET,
    CMD_GET_TDMB_RSSI_SET,
    CMD_PUSH_TDMB_STAT_DATA,
    CMD_REQUEST_PARSE_TDMB_DATA,
    CMD_REQUEST_TDMB_RESET_MONITOR_VALUE,

    // RF
    CMD_RF_NONE,
    CMD_RF_SET_RF_ADDR,
    CMD_RF_READ_REG_SINGLE,
    CMD_RF_WRITE_REG_SINGLE,
    CMD_RF_GET_PLL_LOCKSTAT,
    MSG_SET_FN_RF_READ,
    MSG_SET_FN_RF_WRITE,
    MSG_SET_FN_RF_WRITEEX
} ENUM_CMD_TO_BBRF;

// -----------------------------------------------------------------
//
// Frequency Setting Option
//
typedef struct
{
    uint32_t Hierarchy;
    uint32_t EnablePIDFiltering;
} tFreqSet_Option;

// -----------------------------------------------------------------
//
// Services Define
//

//---------------------------------------------------------------------------------------
//  TDMB Struct for Stream Receive
typedef struct
{
    int32_t       ServType;   // if DMB : 0x04, other : other value (0x00)
    int32_t       PType;      // UEP : 0x00, EEP : 0x01
    int32_t       SubChID;    // Sub channel ID
    int32_t       CUSize;     // CU Size
    int32_t       StartCU;
    int32_t       ReConfig;   // 0x02
    int32_t       RS_ON;      // If DMB 0x01 else 0x00
    int32_t       PLevel;
    int32_t       BitRate;
} TDMBService_t;

typedef enum
{
    DATAMODE_DAB = 0,
    DATAMODE_DMB,
    DATAMODE_EPM,
    DATAMODE_DABPLUS
} TDMBDataMode;


// -----------------------------------------------------------------
// TDMB Service Types
//
#define SRVTYPE_NONE                0x00
#define SRVTYPE_DAB                 0x01
#define SRVTYPE_DABPLUS             0x02
#define SRVTYPE_DATA                0x03
#define SRVTYPE_DMB                 0x04
#define SRVTYPE_FIDC                0x05
#define SRVTYPE_RAWDATA             0x06
#define SRVTYPE_FIC                 0x07
#define SRVTYPE_FIC_WITH_ERR        0x08
#define EWS_ANNOUNCE_FLAG                   0x09
#define RECONFIGURATION_FLAG                0x0a
#define EWS_ANNOUNCE_RECONFIGURATION_FLAG   0x0b
#define SRVTYPE_STATUS                 0x0c

//------------------------------------------------------------------
// Monitoring Value
//

typedef struct
{
    uint8_t AGC;
    uint8_t DC;
    uint8_t CTO;
    uint8_t CFO;
    uint8_t FFO;
    uint8_t FTO;
    uint8_t SYNC_BYTE;
    uint8_t OFDM_DETECT;
} t_TDMBLock;

// -----------------------------------------------------------------
// Stream Type & Event Function Type
//
typedef enum
{
    CMD_STREAM_NONE = 0,
    CMD_STREAM_TDMB,
    CMD_STREAM_STOPPED,

    // Error Handler
    TC3X_ERROR_UNKNOWN,
    TC3X_ERROR_CRITICAL_MAILBOX,

    EVENT_QUIET_DATALINE = 0xDA
} ENUM_CMD_STREAM;

// -----------------------------------------------------------------
// MailboxLog
//
typedef enum
{
    MAILBOX_LOG_NONE = 0,
    MAILBOX_LOG_FREQSET_PREPARE,
    MAILBOX_LOG_FREQSET_START,
    MAILBOX_LOG_FREQSET_START_STEP1,
    MAILBOX_LOG_FREQSET_START_STEP2,
    MAILBOX_LOG_FREQSET_STOP,
    MAILBOX_LOG_SERVICE
} ENUM_MAILBOX_LOG;

typedef struct
{
    uint32_t Frequency;
    int32_t       SpurTap0_A;
    int32_t       SpurTap0_B;
    int32_t       SpurTap1_A;
    int32_t       SpurTap1_B;
    int32_t       SpurTap2_A;
    int32_t       SpurTap2_B;
} tTunningForOSC;

#define TCDBG(msg, arg...) PRINTF_LV_0("[%s:%d] "msg, __func__, __LINE__, ##arg)
#define TCASSERT(__arg) \
do { \
  if((__arg)) PRINTF_LV_0("!!ASSERT!! [%s:%d]\n", __func__, __LINE__);\
}while(0)
extern int32_t tc317x_err;


#define SWAP16(x) \
    ((u16)( \
    (((u16)(x) & (u16)0x00ffU) << 8) | \
    (((u16)(x) & (u16)0xff00U) >> 8) ))

#define SWAP32(x) \
    ((u32)( \
    (((u32)(x) & (u32)0x000000ffUL) << 24) | \
    (((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
    (((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
    (((u32)(x) & (u32)0xff000000UL) >> 24) ))

#endif
