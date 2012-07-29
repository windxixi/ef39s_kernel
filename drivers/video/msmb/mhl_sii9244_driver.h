/* Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved. 
 *          
 *  No part of this work may be reproduced, modified, distributed, transmitted,    
 * transcribed, or translated into any language or computer format, in any form   
 * or by any means without written permission of: Silicon Image, Inc.,            
 * 1060 East Arques Avenue, Sunnyvale, California 94085      
 */

#ifndef __MHL_sii9244_H__
#define __MHL_sii9244_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <linux/types.h>
#include <linux/i2c.h>

/*===========================================================================*/
#define _ZERO		     0x00
#define BIT_0                   0x01
#define BIT_1                   0x02
#define BIT_2                   0x04
#define BIT_3                   0x08
#define BIT_4                   0x10
#define BIT_5                   0x20
#define BIT_6                   0x40
#define BIT_7                   0x80

//
// CBUS register defintions
//
#define REG_CBUS_INTR_STATUS            0x08
#define BIT_DDC_ABORT                          (BIT_2)    /* Responder aborted DDC command at translation layer */
#define BIT_MSC_MSG_RCV                      (BIT_3)    /* Responder sent a VS_MSG packet (response data or command.) */
#define BIT_MSC_XFR_DONE                     (BIT_4)    /* Responder sent ACK packet (not VS_MSG) */
#define BIT_MSC_XFR_ABORT                   (BIT_5)    /* Command send aborted on TX side */
#define BIT_MSC_ABORT                           (BIT_6)    /* Responder aborted MSC command at translation layer */

#define REG_CBUS_INTR_ENABLE            0x09

#define REG_DDC_ABORT_REASON        	0x0C
#define REG_CBUS_BUS_STATUS             0x0A
#define BIT_BUS_CONNECTED                   0x01
#define BIT_LA_VAL_CHG                      0x02

#define REG_PRI_XFR_ABORT_REASON     0x0D

#define REG_CBUS_PRI_FWR_ABORT_REASON     0x0E
#define CBUSABORT_BIT_REQ_MAXFAIL			(0x01 << 0)
#define CBUSABORT_BIT_PROTOCOL_ERROR		(0x01 << 1)
#define CBUSABORT_BIT_REQ_TIMEOUT		(0x01 << 2)
#define CBUSABORT_BIT_UNDEFINED_OPCODE	(0x01 << 3)
#define CBUSSTATUS_BIT_CONNECTED			(0x01 << 6)
#define CBUSABORT_BIT_PEER_ABORTED		(0x01 << 7)

#define REG_CBUS_PRI_START                             0x12
#define BIT_TRANSFER_PVT_CMD                         0x01
#define BIT_SEND_MSC_MSG                                0x02
#define MSC_START_BIT_MSC_CMD		               (0x01 << 0)
#define MSC_START_BIT_VS_CMD		               (0x01 << 1)
#define MSC_START_BIT_READ_REG		        (0x01 << 2)
#define MSC_START_BIT_WRITE_REG		        (0x01 << 3)
#define MSC_START_BIT_WRITE_BURST	        (0x01 << 4)

#define REG_CBUS_PRI_ADDR_CMD           0x13
#define REG_CBUS_PRI_WR_DATA_1ST        0x14
#define REG_CBUS_PRI_WR_DATA_2ND        0x15
#define REG_CBUS_PRI_RD_DATA_1ST        0x16
#define REG_CBUS_PRI_RD_DATA_2ND        0x17

#define REG_CBUS_PRI_VS_CMD             0x18
#define REG_CBUS_PRI_VS_DATA            0x19

#define	REG_MSC_WRITE_BURST_LEN         0x20       // only for WRITE_BURST
#define MSC_REQUESTOR_DONE_NACK         	(0x01 << 6)      

#define REG_CBUS_MSC_RETRY_INTERVAL			0x1A		// default is 16
#define REG_CBUS_DDC_FAIL_LIMIT				0x1C		// default is 5
#define REG_CBUS_MSC_FAIL_LIMIT				0x1D		// default is 5
#define	REG_CBUS_MSC_INT2_STATUS        	0x1E
#define REG_CBUS_MSC_INT2_ENABLE             	0x1F
#define MSC_INT2_REQ_WRITE_MSC                  (0x01 << 0)	// Write REG data written.
#define MSC_INT2_HEARTBEAT_MAXFAIL          (0x01 << 1)	// Retry threshold exceeded for sending the Heartbeat

#define REG_MSC_WRITE_BURST_LEN         0x20       // only for WRITE_BURST

#define REG_MSC_HEARTBEAT_CONTROL                 0x21       // Periodic heart beat. TX sends GET_REV_ID MSC command
#define MSC_HEARTBEAT_PERIOD_MASK		    0x0F	// bits 3..0
#define MSC_HEARTBEAT_FAIL_LIMIT_MASK	    0x70	// bits 6..4
#define MSC_HEARTBEAT_ENABLE			           0x80	// bit 7

#define REG_MSC_TIMEOUT_LIMIT           0x22
#define MSC_TIMEOUT_LIMIT_MSB_MASK	        (0x0F)	        // default is 1
#define MSC_LEGACY_BIT					        (0x01 << 7)	    // This should be cleared.

#define	REG_CBUS_LINK_CONTROL_1				0x30	// 
#define	REG_CBUS_LINK_CONTROL_2				0x31	// 
#define	REG_CBUS_LINK_CONTROL_3				0x32	// 
#define	REG_CBUS_LINK_CONTROL_4				0x33	// 
#define	REG_CBUS_LINK_CONTROL_5				0x34	// 
#define	REG_CBUS_LINK_CONTROL_6				0x35	// 
#define	REG_CBUS_LINK_CONTROL_7				0x36	// 
#define REG_CBUS_LINK_STATUS_1          	0x37
#define REG_CBUS_LINK_STATUS_2          	0x38
#define	REG_CBUS_LINK_CONTROL_8				0x39	// 
#define	REG_CBUS_LINK_CONTROL_9				0x3A	// 
#define	REG_CBUS_LINK_CONTROL_10				0x3B	// 
#define	REG_CBUS_LINK_CONTROL_11				0x3C	// 
#define	REG_CBUS_LINK_CONTROL_12				0x3D	// 

#define REG_CBUS_LINK_CTRL9_0           0x3A
#define REG_CBUS_LINK_CTRL9_1           0xBA

#define	REG_CBUS_DRV_STRENGTH_0				0x40	// 
#define	REG_CBUS_DRV_STRENGTH_1				0x41	// 
#define	REG_CBUS_ACK_CONTROL				0x42	// 
#define	REG_CBUS_CAL_CONTROL				0x43	// Calibration

#define REG_CBUS_SCRATCHPAD_0           0xC0
#define REG_CBUS_DEVICE_CAP_0           0x80
#define REG_CBUS_DEVICE_CAP_1           0x81
#define REG_CBUS_DEVICE_CAP_2           0x82
#define REG_CBUS_DEVICE_CAP_3           0x83
#define REG_CBUS_DEVICE_CAP_4           0x84
#define REG_CBUS_DEVICE_CAP_5           0x85
#define REG_CBUS_DEVICE_CAP_6           0x86
#define REG_CBUS_DEVICE_CAP_7           0x87
#define REG_CBUS_DEVICE_CAP_8           0x88
#define REG_CBUS_DEVICE_CAP_9           0x89
#define REG_CBUS_DEVICE_CAP_A           0x8A
#define REG_CBUS_DEVICE_CAP_B           0x8B
#define REG_CBUS_DEVICE_CAP_C           0x8C
#define REG_CBUS_DEVICE_CAP_D           0x8D
#define REG_CBUS_DEVICE_CAP_E           0x8E
#define REG_CBUS_DEVICE_CAP_F           0x8F
#define REG_CBUS_SET_INT_0				0xA0
#define REG_CBUS_SET_INT_1				0xA1
#define REG_CBUS_SET_INT_2				0xA2
#define REG_CBUS_SET_INT_3				0xA3
#define REG_CBUS_WRITE_STAT_0        	0xB0
#define REG_CBUS_WRITE_STAT_1        	0xB1
#define REG_CBUS_WRITE_STAT_2        	0xB2
#define REG_CBUS_WRITE_STAT_3        	0xB3


// DEVCAP we will initialize to
#define MHL_LOGICAL_DEVICE_MAP_9244		(MHL_DEV_LD_AUDIO | MHL_DEV_LD_VIDEO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_GUI )


//
// This file contains MHL Specs related definitions.
//

// Device Power State
#define MHL_DEV_UNPOWERED		0x00
#define MHL_DEV_INACTIVE		0x01
#define MHL_DEV_QUIET			0x03
#define MHL_DEV_ACTIVE			0x04

// Version that this chip supports
#define MHL_VER_MAJOR		(0x01 << 4)	// bits 4..7
#define MHL_VER_MINOR		0x00		// bits 0..3
#define MHL_VERSION			(MHL_VER_MAJOR | MHL_VER_MINOR)

//Device Category
#define MHL_DEV_CATEGORY_OFFSET				0x02
#define MHL_DEV_CATEGORY_POW_BIT			       (BIT_4)

#define MHL_DEV_CAT_SOURCE					0x00
#define MHL_DEV_CAT_SINGLE_INPUT_SINK		0x01
#define MHL_DEV_CAT_MULTIPLE_INPUT_SINK		0x02
#define MHL_DEV_CAT_UNPOWERED_DONGLE		0x03
#define MHL_DEV_CAT_SELF_POWERED_DONGLE	0x04
#define MHL_DEV_CAT_HDCP_REPEATER			0x05
#define MHL_DEV_CAT_NON_DISPLAY_SINK		       0x06
#define MHL_DEV_CAT_POWER_NEUTRAL_SINK		0x07
#define MHL_DEV_CAT_OTHER					       0x80

//Video Link Mode
#define MHL_DEV_VID_LINK_SUPPRGB444			0x01
#define MHL_DEV_VID_LINK_SUPPYCBCR444		0x02
#define MHL_DEV_VID_LINK_SUPPYCBCR422		0x04
#define MHL_DEV_VID_LINK_PPIXEL				0x08
#define MHL_DEV_VID_LINK_SUPP_ISLANDS		0x10

//Audio Link Mode Support
#define MHL_DEV_AUD_LINK_2CH				       0x01
#define MHL_DEV_AUD_LINK_8CH				       0x02

//Feature Flag in the devcap
#define MHL_DEV_FEATURE_FLAG_OFFSET			0x0A
#define MHL_FEATURE_RCP_SUPPORT				BIT_0	// Dongles have freedom to not support RCP
#define MHL_FEATURE_RAP_SUPPORT				BIT_1	// Dongles have freedom to not support RAP
#define MHL_FEATURE_SP_SUPPORT				BIT_2	// Dongles have freedom to not support SCRATCHPAD

/*
#define MHL_POWER_SUPPLY_CAPACITY		16		// 160 mA current
#define MHL_POWER_SUPPLY_PROVIDED		16		// 160mA 0r 0 for Wolverine.
#define MHL_HDCP_STATUS					0		// Bits set dynamically
*/

// VIDEO TYPES
#define MHL_VT_GRAPHICS				0x00		
#define MHL_VT_PHOTO					0x02		
#define MHL_VT_CINEMA					0x04		
#define MHL_VT_GAMES					0x08		
#define MHL_SUPP_VT						0x80		

//Logical Dev Map
#define MHL_DEV_LD_DISPLAY					(0x01 << 0)
#define MHL_DEV_LD_VIDEO					(0x01 << 1)
#define MHL_DEV_LD_AUDIO					(0x01 << 2)
#define MHL_DEV_LD_MEDIA					(0x01 << 3)
#define MHL_DEV_LD_TUNER					(0x01 << 4)
#define MHL_DEV_LD_RECORD					(0x01 << 5)
#define MHL_DEV_LD_SPEAKER				(0x01 << 6)
#define MHL_DEV_LD_GUI						(0x01 << 7)

//Bandwidth
#define MHL_BANDWIDTH_LIMIT					22		// 225 MHz


#define MHL_STATUS_DCAP_RDY				BIT_0
#define MHL_INT_DCAP_CHG					BIT_0

// On INTR_1 the EDID_CHG is located at BIT 0
#define MHL_INT_EDID_CHG					BIT_1

#define MHL_INT_AND_STATUS_SIZE			0x303		// This contains one nibble each - max offset
#define MHL_SCRATCHPAD_SIZE				16
#define MHL_MAX_BUFFER_SIZE				MHL_SCRATCHPAD_SIZE	// manually define highest number

enum
{
    MHL_MSC_MSG_RCP             = 0x10,     // RCP sub-command
    MHL_MSC_MSG_RCPK            = 0x11,     // RCP Acknowledge sub-command
    MHL_MSC_MSG_RCPE            = 0x12,     // RCP Error sub-command
    MHL_MSC_MSG_RAP             = 0x20,     // Mode Change Warning sub-command
    MHL_MSC_MSG_RAPK            = 0x21,     // MCW Acknowledge sub-command
};

#define RCPE_NO_ERROR				0x00
#define RCPE_INEEFECTIVE_KEY_CODE	0x01
#define RCPE_BUSY					0x02
//
// MHL spec related defines
//
enum
{
	MHL_ACK						= 0x33,	// Command or Data byte acknowledge
	MHL_NACK					       = 0x34,	// Command or Data byte not acknowledge
	MHL_ABORT					       = 0x35,	// Transaction abort
	MHL_WRITE_STAT				= 0x60 | 0x80,	// Write one status register strip top bit
	MHL_SET_INT					= 0x60,	// Write one interrupt register
	MHL_READ_DEVCAP				= 0x61,	// Read one register
	MHL_GET_STATE				       = 0x62,	// Read CBUS revision level from follower
	MHL_GET_VENDOR_ID			= 0x63,	// Read vendor ID value from follower.
	MHL_SET_HPD					= 0x64,	// Set Hot Plug Detect in follower
	MHL_CLR_HPD					= 0x65,	// Clear Hot Plug Detect in follower
	MHL_SET_CAP_ID				= 0x66,	// Set Capture ID for downstream device.
	MHL_GET_CAP_ID				= 0x67,	// Get Capture ID from downstream device.
	MHL_MSC_MSG					= 0x68,	// VS command to send RCP sub-commands
	MHL_GET_SC1_ERRORCODE		= 0x69,	// Get Vendor-Specific command error code.
	MHL_GET_DDC_ERRORCODE		= 0x6A,	// Get DDC channel command error code.
	MHL_GET_MSC_ERRORCODE		= 0x6B,	// Get MSC command error code.
	MHL_WRITE_BURST				= 0x6C,	// Write 1-16 bytes to responder’s scratchpad.
	MHL_GET_SC3_ERRORCODE		= 0x6D,	// Get channel 3 command error code.
};

#define MHL_RAP_CONTENT_ON		0x10	// Turn content streaming ON.
#define MHL_RAP_CONTENT_OFF		0x11	// Turn content streaming OFF.

///////////////////////////////////////////////////////////////////////////////
//
// MHL Timings applicable to this driver.
//
//
#define T_SRC_VBUS_CBUS_TO_STABLE	(200)	// 100 - 1000 milliseconds. Per MHL 1.0 Specs
#define T_SRC_WAKE_PULSE_WIDTH_1	(18)// kkcho_888 (20)	// 20 milliseconds. Per MHL 1.0 Specs
#define T_SRC_WAKE_PULSE_WIDTH_2	(60)	// 60 milliseconds. Per MHL 1.0 Specs

#define T_SRC_WAKE_TO_DISCOVER	(500)	// 100 - 1000 milliseconds. Per MHL 1.0 Specs

#define T_SRC_VBUS_CBUS_T0_STABLE	(500)

// Allow RSEN to stay low this much before reacting.
// Per specs between 100 to 200 ms
#define T_SRC_RSEN_DEGLITCH		(150)

// Wait this much after connection before reacting to RSEN (300-500ms)
// Per specs between 300 to 500 ms
#define T_SRC_RXSENSE_CHK			(400)


#define PAGE_0_0X72			0x72
#define PAGE_1_0x7A			0x7A
#define PAGE_2_0x92			0x92
#define PAGE_CBUS_0XC8		0xC8

enum
{
  MHD_RCP_CMD_SELECT                = 0x00,
  MHD_RCP_CMD_UP                        = 0x01,
  MHD_RCP_CMD_DOWN                  = 0x02,
  MHD_RCP_CMD_LEFT                    = 0x03,
  MHD_RCP_CMD_RIGHT                  = 0x04,
  MHD_RCP_CMD_RIGHT_UP            = 0x05,
  MHD_RCP_CMD_RIGHT_DOWN      = 0x06,
  MHD_RCP_CMD_LEFT_UP              = 0x07,
  MHD_RCP_CMD_LEFT_DOWN        = 0x08,
  MHD_RCP_CMD_ROOT_MENU           = 0x09,
  MHD_RCP_CMD_SETUP_MENU          = 0x0A,
  MHD_RCP_CMD_CONTENTS_MENU   = 0x0B,
  MHD_RCP_CMD_FAVORITE_MENU   = 0x0C,
  MHD_RCP_CMD_EXIT                       = 0x0D,
	
	//0x0E - 0x1F are reserved

  MHD_RCP_CMD_NUM_0           = 0x20,
  MHD_RCP_CMD_NUM_1           = 0x21,
  MHD_RCP_CMD_NUM_2           = 0x22,
  MHD_RCP_CMD_NUM_3           = 0x23,
  MHD_RCP_CMD_NUM_4           = 0x24,
  MHD_RCP_CMD_NUM_5           = 0x25,
  MHD_RCP_CMD_NUM_6           = 0x26,
  MHD_RCP_CMD_NUM_7           = 0x27,
  MHD_RCP_CMD_NUM_8           = 0x28,
  MHD_RCP_CMD_NUM_9           = 0x29,

  MHD_RCP_CMD_DOT             = 0x2A,
  MHD_RCP_CMD_ENTER           = 0x2B,
  MHD_RCP_CMD_CLEAR           = 0x2C,

	//0x2D - 0x2F are reserved

  MHD_RCP_CMD_CH_UP           = 0x30,
  MHD_RCP_CMD_CH_DOWN         = 0x31,
  MHD_RCP_CMD_PRE_CH          = 0x32,
  MHD_RCP_CMD_SOUND_SELECT    = 0x33,
  MHD_RCP_CMD_INPUT_SELECT    = 0x34,
  MHD_RCP_CMD_SHOW_INFO       = 0x35,
  MHD_RCP_CMD_HELP            = 0x36,
  MHD_RCP_CMD_PAGE_UP         = 0x37,
  MHD_RCP_CMD_PAGE_DOWN       = 0x38,

	//0x39 - 0x40 are reserved

  MHD_RCP_CMD_VOL_UP	        = 0x41,
  MHD_RCP_CMD_VOL_DOWN        = 0x42,
  MHD_RCP_CMD_MUTE            = 0x43,
  MHD_RCP_CMD_PLAY            = 0x44,
  MHD_RCP_CMD_STOP            = 0x45,
  MHD_RCP_CMD_PAUSE           = 0x46,
  MHD_RCP_CMD_RECORD          = 0x47,
  MHD_RCP_CMD_REWIND          = 0x48,
  MHD_RCP_CMD_FAST_FWD        = 0x49,
  MHD_RCP_CMD_EJECT           = 0x4A,
  MHD_RCP_CMD_FWD             = 0x4B,
  MHD_RCP_CMD_BKWD            = 0x4C,

	//0x4D - 0x4F are reserved

  MHD_RCP_CMD_ANGLE            = 0x50,
  MHD_RCP_CMD_SUBPICTURE       = 0x51,

//0x52 - 0x5F are reserved

  MHD_RCP_CMD_PLAY_FUNC       = 0x60,
  MHD_RCP_CMD_PAUSE_PLAY_FUNC = 0x61,
  MHD_RCP_CMD_RECORD_FUNC     = 0x62,
  MHD_RCP_CMD_PAUSE_REC_FUNC  = 0x63,
  MHD_RCP_CMD_STOP_FUNC       = 0x64,
  MHD_RCP_CMD_MUTE_FUNC       = 0x65,
  MHD_RCP_CMD_UN_MUTE_FUNC    = 0x66,
  MHD_RCP_CMD_TUNE_FUNC       = 0x67,
  MHD_RCP_CMD_MEDIA_FUNC      = 0x68,

//0x69 - 0x70 are reserved

  MHD_RCP_CMD_F1              = 0x71,
  MHD_RCP_CMD_F2              = 0x72,
  MHD_RCP_CMD_F3              = 0x73,
  MHD_RCP_CMD_F4              = 0x74,
  MHD_RCP_CMD_F5              = 0x75,

//0x76 - 0x7D are reserved

  MHD_RCP_CMD_VS              = 0x7E,
  MHD_RCP_CMD_RSVD            = 0x7F

};

#define FALSE	0
#define TRUE	1

/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/
struct i2c_client* get_sii9244_client(u8 device_id);

typedef struct 
{
  u8 reqStatus;                      // CBUS_IDLE, CBUS_PENDING
  u8 command;                        // VS_CMD or RCP opcode
  u8 offsetData;                     // Offset of register on CBUS or RCP data
  u8 length;                         // Only applicable to write burst. ignored otherwise.
  u8 msgData[16];   // Pointer to message data area.
  unsigned char	*pdatabytes;			// pointer for write burst or read many bytes
} cbus_req_t;


typedef struct 
{
  bool	interruptDriven;	// Remember what app told us about interrupt availability.
  u8		pollIntervalMs;		// Remember what app set the polling frequency as.

  u8		mscState;			// Internal MSC engine's states.

  u8		status_0;			// Received status from peer is stored here
  u8		status_1;			// Received status from peer is stored here

  bool	mhlConnectionEvent;
  u8		mhlConnected;

  // mscMsgArrived == true when a MSC MSG arrives, false when it has been picked up
  bool	mscMsgArrived;
  u8		mscMsgSubCommand;
  u8		mscMsgData;

  // Remember FEATURE FLAG of the peer to reject app commands if unsupported
  u8		mscFeatureFlag;

  // Remember last command, offset that was sent. Mostly for READ_DEVCAP command
  u8		mscLastCommand;
  u8		mscLastOffset;

  // Remember last MSC_MSG command (RCPE particularly)
  u8		mscMsgLastCommand;
  u8		mscMsgLastData;
  u8		mscSaveRcpKeyCode;

  //  uint8_t 	mscData[ 16 ]; 		// What we got back as message data

} mhlTx_config_t;

enum
{
	MSC_STATE_IDLE		= 0x00,		// Until MHL is connected, do nothing
	MSC_STATE_BEGIN		= 0x01,		// Start of MSC engine to read devcap of the peer
	MSC_STATE_POW_DONE	= 0x02,		// Just read devcap[2]. Next do the feature flags
	MSC_STATE_RCP_READY = 0x03		// Feature flags done. Ready to take in RCP.
};


void sii9244_interrupt_event(void);
void sii9244_driver_init(void);
void HDMI_Control_On(bool on);
void sii9244_remote_control_init(void);
void MHL_En_Control(bool on);
void MHL_On(bool on);
int MHL_Cable_On(int on);
void MHL_Set_Cable_State(bool connect);
uint32_t MHL_Get_Cable_State(void);
u8 sii9244_i2c_read(struct i2c_client *client, u8 reg);
int sii9244_i2c_write(struct i2c_client *client, u8 reg, u8 data);

#endif /*__MHL_sii9244_H__*/
