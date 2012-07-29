/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/delay.h>
#include <linux/interrupt.h>

//#include <linux/irq.h>
#include <asm/irq.h>

#include <linux/i2c.h>
//#include <linux/gpio.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/syscalls.h> 
#include <linux/fcntl.h> 
#include <asm/uaccess.h> 

#include <mach/gpio.h>
#include "mhl_sii9244_driver.h"
#include "msm_fb.h"
#include <linux/input.h>

#define MHL_TX_EVENT_NONE			       0x00	/* No event worth reporting.  */
#define MHL_TX_EVENT_DISCONNECTION	0x01	/* MHL connection has been lost */
#define MHL_TX_EVENT_CONNECTION		0x02	/* MHL connection has been established */
#define MHL_TX_EVENT_RCP_READY		0x03	/* MHL connection is ready for RCP */
#define MHL_TX_EVENT_RCP_RECEIVED	       0x04	/* Received an RCP. Key Code in "eventParameter" */
#define MHL_TX_EVENT_RCPK_RECEIVED	0x05	/* Received an RCPK message */
#define MHL_TX_EVENT_RCPE_RECEIVED	0x06	/* Received an RCPE message .*/

#define TX_HW_RESET_PERIOD                     200
#define SiI_DEVICE_ID                                  0xB0
#define DDC_XLTN_TIMEOUT_MAX_VAL	       0x30
#define INDEXED_PAGE_0		                     0x01
#define INDEXED_PAGE_1		                     0x02
#define INDEXED_PAGE_2		                     0x03

#define ASR_VALUE 0x04

#define TX_POWER_STATE_MASK			(BIT_1 | BIT_0)
#define TX_POWER_STATE_D0				(_ZERO)
#define TX_POWER_STATE_D1				(BIT_0)
#define TX_POWER_STATE_D2				(BIT_1)
#define TX_POWER_STATE_D3				(BIT_1 | BIT_0)

#define TX_POWER_STATE_D0_NO_MHL		TX_POWER_STATE_D2
#define TX_POWER_STATE_D0_MHL		TX_POWER_STATE_D0
#define TX_POWER_STATE_FIRST_INIT		0xFF

#define MHL_INIT_POWER_OFF                     0x00
#define MHL_POWER_ON                                0x01
#define MHL_1K_IMPEDANCE_VERIFIED        0x02
#define MHL_RSEN_VERIFIED                         0x04
#define MHL_TV_OFF_CABLE_CONNECT         0x08

#define MHL_DEVICE_CATEGORY                    0x02 //(MHL_DEV_CAT_SOURCE)
#define MHL_LOGICAL_DEVICE_MAP	        (MHL_DEV_LD_AUDIO | MHL_DEV_LD_VIDEO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_GUI )

//#define READ   1
//#define WRITE  0

#define LAST_BYTE      1
#define NOT_LAST_BYTE  0

#define TPI_INDEXED_PAGE_REG		        0xBC
#define TPI_INDEXED_OFFSET_REG	        0xBD
#define TPI_INDEXED_VALUE_REG		        0xBE

#define TX_DEBUG_PRINT(x) printk x
   
#define F_MHL_USB_SW_CONTROL_BUG_FIX // 20110811, kkcho, HPD로 인해 부팅시 스위치 되는 case 방지 
#define F_MHL_CABLE_DETECT_BUG_PIECEOUT // 20110819, kkcho, 0xFA에 대한 return반복하면 phone-reset

/*==============================================================================*/

byte fwPowerState = TX_POWER_STATE_FIRST_INIT;
//static	bool	gotFifoUnderRunOverRun = FALSE;
//static	bool	deglitchingRsenNow = FALSE;
static	bool		mscCmdInProgress;	// FALSE when it is okay to send a new command
static	byte	dsHpdStatus = 0;
byte mhl_cable_status =MHL_INIT_POWER_OFF;			//NAGSM_Android_SEL_Kernel_Aakash_20101214

#define MHL_REMOCON_CONTROL  // 20110526, kkcho, MHL 인증을 위해 추가. (RCP_FUNC)
#define F_MHL_AUTO_CONNECT  // 20110627, kkcho, MHL cable detect event (with Msm_charger.c, pmic8059_charger.c, qwerth.kl)
#ifdef MHL_REMOCON_CONTROL 
static struct kobject *mhl_remocon_control_kobj;
static struct input_dev *ipdev;
#endif

#ifdef F_MHL_AUTO_CONNECT
unsigned int mhl_cable_state = 0;
#define MHL_DEVICE_ON			1
#define MHL_DEVICE_OFF			0
#define MHL_CABLE_CONNCET			1
#define MHL_CABLE_DISCONNCET	       0
#endif

#define	MHL_MAX_RCP_KEY_CODE	(0x7F + 1)	// inclusive
byte rcpSupportTable [MHL_MAX_RCP_KEY_CODE] = {
	(MHL_DEV_LD_GUI),		// 0x00 = Select
	(MHL_DEV_LD_GUI),		// 0x01 = Up
	(MHL_DEV_LD_GUI),		// 0x02 = Down
	(MHL_DEV_LD_GUI),		// 0x03 = Left
	(MHL_DEV_LD_GUI),		// 0x04 = Right
	0, 0, 0, 0,				// 05-08 Reserved
	(MHL_DEV_LD_GUI),		// 0x09 = Root Menu
	0, 0, 0,				// 0A-0C Reserved
	(MHL_DEV_LD_GUI),		// 0x0D = Select
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 0E-1F Reserved
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),	// Numeric keys 0x20-0x29
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),
	0,						// 0x2A = Dot
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),	// Enter key = 0x2B
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),	// Clear key = 0x2C
	0, 0, 0,				// 2D-2F Reserved
	(MHL_DEV_LD_TUNER),		// 0x30 = Channel Up
	(MHL_DEV_LD_TUNER),		// 0x31 = Channel Dn
	(MHL_DEV_LD_TUNER),		// 0x32 = Previous Channel
	(MHL_DEV_LD_AUDIO),		// 0x33 = Sound Select
	0,						// 0x34 = Input Select
	0,						// 0x35 = Show Information
	0,						// 0x36 = Help
	0,						// 0x37 = Page Up
	0,						// 0x38 = Page Down
	0, 0, 0, 0, 0, 0, 0,	// 0x39-0x3F Reserved
	0,						// 0x40 = Undefined

	(MHL_DEV_LD_SPEAKER),	// 0x41 = Volume Up
	(MHL_DEV_LD_SPEAKER),	// 0x42 = Volume Down
	(MHL_DEV_LD_SPEAKER),	// 0x43 = Mute
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	// 0x44 = Play
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),	// 0x45 = Stop
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),	// 0x46 = Pause
	(MHL_DEV_LD_RECORD),	// 0x47 = Record
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	// 0x48 = Rewind
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	// 0x49 = Fast Forward
	(MHL_DEV_LD_MEDIA),		// 0x4A = Eject
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),	// 0x4B = Forward
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),	// 0x4C = Backward
	0, 0, 0,				// 4D-4F Reserved
	0,						// 0x50 = Angle
	0,						// 0x51 = Subpicture
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 52-5F Reserved
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	// 0x60 = Play Function
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	// 0x61 = Pause the Play Function
	(MHL_DEV_LD_RECORD),	// 0x62 = Record Function
	(MHL_DEV_LD_RECORD),	// 0x63 = Pause the Record Function
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),	// 0x64 = Stop Function

	(MHL_DEV_LD_SPEAKER),	// 0x65 = Mute Function
	(MHL_DEV_LD_SPEAKER),	// 0x66 = Restore Mute Function
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	// Undefined or reserved
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 		// Undefined or reserved
};

static	mhlTx_config_t	mhlTxConfig;


/*==============================================================================*/
void I2C_WriteByte(byte deviceID, byte offset, byte value);

byte I2C_ReadByte(byte deviceID, byte offset);

void WriteByteTPI(byte RegOffset, byte Data);

byte ReadByteTPI(byte RegOffset);
byte ReadByteCBUS (byte Offset);
void WriteByteCBUS(byte Offset, byte Data);

void ReadModifyWriteTPI(byte Offset, byte Mask, byte Value);

void SiiMhlTxGetEvents(byte *event, byte *eventparameter);

void sii9244_driver_init(void);
void sii9244_remote_control_init(void);
void SiiMhlTxInitialize(void);
static void MhlTxResetStates( void );
bool SiiMhlTxChipInitialize ( void );
void	CbusReset(void);
void	SwitchToD3( void );
static void ReleaseUsbIdSwitchOpen ( void );
void SiiMhlTxGetEvents(byte *event, byte *eventparameter);
void WriteInitialRegisterValues ( void );
static void InitCBusRegs( void );
void MhlTxDrvProcessConnection ( void );
void	SwitchToD0( void );
void CbusWakeUpPulseGenerator(void);
void	ProcessRgnd( void );
static void Int4Isr( void );
void	SiiMhlTxDrvNotifyEdidChange ( void );
void	SiiMhlTxGotMhlIntr( byte intr_0, byte intr_1 );

void	SiiMhlTxGotMhlStatus( byte status_0, byte status_1 );
static byte CBusProcessErrors( byte intStatus );

void	SiiMhlTxMscCommandDone( byte data1 );
void SiiMhlTxGotMhlMscMsg( byte subCommand, byte cmdData );
void	SiiMhlTxNotifyConnection( bool mhlConnected );
void MhlTxDrvProcessDisconnection ( void );
static void ForceUsbIdSwitchOpen ( void );
static void ApplyDdcAbortSafety(void);
static void MhlCbusIsr( void );
void	SiiMhlTxDrvTmdsControl( bool enable );
void	SiiMhlTxNotifyDsHpdChange( byte dsHpdStatus );
void	Int1RsenIsr( void );
void SiiMhlTxDeviceIsr( void );
bool SiiMhlTxReadDevcap( byte offset );
static void MhlTxDriveStates( void );
static bool SiiMhlTxRapkSend( void );
bool SiiMhlTxDrvSendCbusCommand ( cbus_req_t *pReq  );
static bool MhlTxSendMscMsg ( byte command, byte cmdData );
bool SiiMhlTxRcpeSend( byte rcpeErrorCode );
void SiiMhlTxGetEvents(byte *event, byte *eventparameter);

bool SiiMhlTxRcpkSend( byte rcpKeyCode );
void	AppRcpDemo( byte event, byte eventParameter);
void sii9244_interrupt_event(void);

void	AppVbusControl( bool powerOn );

int MHL_Cable_On(int on);
void HDMI_Control_On(bool on);

#ifdef MHL_REMOCON_CONTROL 
void rcp_cbus_uevent(u8);
#endif 

#define I2C_READ_MODIFY_WRITE(saddr,offset,mask)  I2C_WriteByte(saddr, offset, I2C_ReadByte(saddr, offset) | (mask))
#define SET_BIT(saddr,offset,bitnumber)		            I2C_READ_MODIFY_WRITE(saddr,offset, (1<<bitnumber))
#define CLR_BIT(saddr,offset,bitnumber)		            I2C_WriteByte(saddr, offset, I2C_ReadByte(saddr, offset) & ~(1<<bitnumber))

#define DISABLE_DISCOVERY				CLR_BIT(PAGE_0_0X72, 0x90, 0);
#define ENABLE_DISCOVERY				SET_BIT(PAGE_0_0X72, 0x90, 0);

#define INTR_4_DESIRED_MASK			( BIT_2 | BIT_3 | BIT_4 | BIT_6) 
#define UNMASK_INTR_4_INTERRUPTS		I2C_WriteByte(PAGE_0_0X72, 0x78, 0x00) 
#define MASK_INTR_4_INTERRUPTS	       I2C_WriteByte(PAGE_0_0X72, 0x78, INTR_4_DESIRED_MASK)	

#define INTR_1_DESIRED_MASK			(BIT_5|BIT_6) 
#define UNMASK_INTR_1_INTERRUPTS		I2C_WriteByte(PAGE_0_0X72, 0x75, 0x00)
#define MASK_INTR_1_INTERRUPTS		I2C_WriteByte(PAGE_0_0X72, 0x75, INTR_1_DESIRED_MASK)

#define INTR_CBUS1_DESIRED_MASK		(BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6)
#define UNMASK_CBUS1_INTERRUPTS		I2C_WriteByte(PAGE_CBUS_0XC8, 0x09, 0x00)	
#define MASK_CBUS1_INTERRUPTS			I2C_WriteByte(PAGE_CBUS_0XC8, 0x09, INTR_CBUS1_DESIRED_MASK)

#define INTR_CBUS2_DESIRED_MASK		(BIT_2 | BIT_3)
#define UNMASK_CBUS2_INTERRUPTS		 I2C_WriteByte(PAGE_CBUS_0XC8, 0x1F, 0x00)	
#define MASK_CBUS2_INTERRUPTS			 I2C_WriteByte(PAGE_CBUS_0XC8, 0x1F, INTR_CBUS2_DESIRED_MASK)

/*==============================================================================*/

void sii9244_driver_init(void)
{
       printk(KERN_ERR "[SKY_MHL]+%s 4th i2c-reg \n", __FUNCTION__);
       SiiMhlTxInitialize(); 
}
EXPORT_SYMBOL(sii9244_driver_init);

#ifdef MHL_REMOCON_CONTROL 
void Remote_control_init(void)
{
	int ret;
	
	ipdev =  input_allocate_device();
	ipdev->name = "MHL_REMOCON";

	input_set_capability(ipdev, EV_KEY, KEY_0);
	input_set_capability(ipdev, EV_KEY, KEY_1);
	input_set_capability(ipdev, EV_KEY, KEY_2);
	input_set_capability(ipdev, EV_KEY, KEY_3);
	input_set_capability(ipdev, EV_KEY, KEY_4);
	input_set_capability(ipdev, EV_KEY, KEY_5);
	input_set_capability(ipdev, EV_KEY, KEY_6);
	input_set_capability(ipdev, EV_KEY, KEY_7);
	input_set_capability(ipdev, EV_KEY, KEY_8);
	input_set_capability(ipdev, EV_KEY, KEY_9);
	input_set_capability(ipdev, EV_KEY, KEY_LEFT);
	input_set_capability(ipdev, EV_KEY, KEY_RIGHT);
	input_set_capability(ipdev, EV_KEY, KEY_UP);
	input_set_capability(ipdev, EV_KEY, KEY_DOWN);
	input_set_capability(ipdev, EV_KEY, KEY_REPLY/*KEY_ENTER*/);
	input_set_capability(ipdev, EV_KEY, KEY_F1);
	input_set_capability(ipdev, EV_KEY, KEY_F2);
	input_set_capability(ipdev, EV_KEY, KEY_END);
	input_set_capability(ipdev, EV_KEY, KEY_F3);
	input_set_capability(ipdev, EV_KEY, KEY_F4);

	input_set_capability(ipdev, EV_KEY, KEY_OK);
	input_set_capability(ipdev, EV_KEY, KEY_SEND);	
	
	input_set_capability(ipdev, EV_KEY, KEY_BACK);
	input_set_capability(ipdev, EV_KEY, KEY_PLAY);	
	input_set_capability(ipdev, EV_KEY, KEY_STOP);
	input_set_capability(ipdev, EV_KEY, KEY_PAUSE);
	input_set_capability(ipdev, EV_KEY, KEY_VIDEO_NEXT);
	input_set_capability(ipdev, EV_KEY, KEY_VIDEO_PREV);	
	input_set_capability(ipdev, EV_KEY, KEY_PLAYCD);
	input_set_capability(ipdev, EV_KEY, KEY_STOPCD);
	input_set_capability(ipdev, EV_KEY, KEY_PAUSECD);
	input_set_capability(ipdev, EV_KEY, KEY_NEXTSONG);
	input_set_capability(ipdev, EV_KEY, KEY_PREVIOUSSONG);	
	input_set_capability(ipdev, EV_KEY, KEY_VOLUMEUP);
	input_set_capability(ipdev, EV_KEY, KEY_VOLUMEDOWN);
	input_set_capability(ipdev, EV_KEY, KEY_PLAYPAUSE);
	input_set_capability(ipdev, EV_KEY, KEY_REWIND);
	input_set_capability(ipdev, EV_KEY, KEY_FASTFORWARD);
	input_set_capability(ipdev, EV_KEY, KEY_FORWARD);
	input_set_capability(ipdev, EV_KEY, KEY_MEDIA);
	input_set_capability(ipdev, EV_KEY, KEY_NEXT);
	input_set_capability(ipdev, EV_KEY, KEY_PREVIOUS);
	input_set_capability(ipdev, EV_KEY, KEY_MEDIA_REPEAT);
	input_set_capability(ipdev, EV_KEY, KEY_FORWARD);
	input_set_capability(ipdev, EV_KEY, KEY_CHANNELDOWN);
	input_set_capability(ipdev, EV_KEY, KEY_CHANNELUP);

#ifdef F_MHL_AUTO_CONNECT
	input_set_capability(ipdev, EV_KEY, KEY_F21);  // hdmi_contorl_on
	input_set_capability(ipdev, EV_KEY, KEY_F22);  // hdmi_control_off
	input_set_capability(ipdev, EV_KEY, KEY_F23);  // mhl_connect_on
	input_set_capability(ipdev, EV_KEY, KEY_F24);  // mhl_connect_off
#endif
	
	ret = input_register_device(ipdev);	
	if (ret<0)
	{
		printk("MHL_REMOCON: input_device_init_fail\n");
	}	
}

void sii9244_remote_control_init(void)
{
       printk(KERN_ERR "[SKY_MHL]+%s  \n", __FUNCTION__);
	Remote_control_init();
}
EXPORT_SYMBOL(sii9244_remote_control_init);
#endif

void SiiMhlTxInitialize( void )
{

	printk("MhlTx: SiiMhlTxInitialize\n");
	
	MhlTxResetStates();

	SiiMhlTxChipInitialize();
	mhl_cable_status =MHL_POWER_ON;			//NAGSM_Android_SEL_Kernel_Aakash_20101214
}

static void	MhlTxResetStates( void )
{
	mhlTxConfig.mhlConnectionEvent	= FALSE;
	mhlTxConfig.mhlConnected		= MHL_TX_EVENT_DISCONNECTION;
	mhlTxConfig.mscMsgArrived		= FALSE;
	mhlTxConfig.mscState			= MSC_STATE_IDLE;
}

void WriteInitialRegisterValues ( void )
{
      TX_DEBUG_PRINT(("Drv: WriteInitialRegisterValues\n"));
	// Power Up
	I2C_WriteByte(PAGE_1_0x7A, 0x3D, 0x3F);	// Power up CVCC 1.2V core
	I2C_WriteByte(PAGE_2_0x92, 0x11, 0x01);	// Enable TxPLL Clock
	I2C_WriteByte(PAGE_2_0x92, 0x12, 0x15);	// Enable Tx Clock Path & Equalizer
	I2C_WriteByte(PAGE_0_0X72, 0x08, 0x35);	// Power Up TMDS Tx Core


	// Analog PLL Control
	I2C_WriteByte(PAGE_2_0x92, 0x10, 0xC1);	// bits 5:4 = 2b00 as per characterization team.
	I2C_WriteByte(PAGE_2_0x92, 0x17, 0x03);	// PLL Calrefsel
	I2C_WriteByte(PAGE_2_0x92, 0x1A, 0x20);	// VCO Cal
	I2C_WriteByte(PAGE_2_0x92, 0x22, 0x8A);	// Auto EQ
	I2C_WriteByte(PAGE_2_0x92, 0x23, 0x6A);	// Auto EQ
	I2C_WriteByte(PAGE_2_0x92, 0x24, 0xAA);	// Auto EQ
	I2C_WriteByte(PAGE_2_0x92, 0x25, 0xCA);	// Auto EQ
	I2C_WriteByte(PAGE_2_0x92, 0x26, 0xEA);	// Auto EQ
	I2C_WriteByte(PAGE_2_0x92, 0x4C, 0xA0);	// Manual zone control
	I2C_WriteByte(PAGE_2_0x92, 0x4D, 0x00);	// PLL Mode Value


	I2C_WriteByte(PAGE_0_0X72, 0x80, 0x34);	// Enable Rx PLL Clock Value
	I2C_WriteByte(PAGE_2_0x92, 0x45, 0x44);	// Rx PLL BW value from I2C
	I2C_WriteByte(PAGE_2_0x92, 0x31, 0x0A);	// Rx PLL BW ~ 4MHz
	I2C_WriteByte(PAGE_0_0X72, 0xA0, 0xD0);
	I2C_WriteByte(PAGE_0_0X72, 0xA1, 0xFC);	// Disable internal MHL driver
	I2C_WriteByte(PAGE_0_0X72, 0xA3, 0xEB);
	I2C_WriteByte(PAGE_0_0X72, 0xA6, 0x0C);
	I2C_WriteByte(PAGE_0_0X72, 0x2B, 0x01);	// Enable HDCP Compliance safety

	//
	// CBUS & Discovery
	// CBUS discovery cycle time for each drive and float = 150us
	//

	ReadModifyWriteTPI(0x90, BIT_3 | BIT_2, BIT_3);
	I2C_WriteByte(PAGE_0_0X72, 0x91, 0xA5);		// Clear bit 6 (reg_skip_rgnd)

	// Changed from 66 to 65 for 94[1:0] = 01 = 5k reg_cbusmhl_pup_sel
	//I2C_WriteByte(PAGE_0_0X72, 0x94, 0x65);			// 1.8V CBUS VTH & GND threshold
       I2C_WriteByte(PAGE_0_0X72, 0x94, 0x75);			// 1.8V CBUS VTH & GND threshold

	//set bit 2 and 3, which is Initiator Timeout
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x31, I2C_ReadByte(PAGE_CBUS_0XC8, 0x31) | 0x0c);

	//I2C_WriteByte(PAGE_0_0X72, 0xA5, 0xAC);			// RGND Hysterisis.
       I2C_WriteByte(PAGE_0_0X72, 0xA5, 0xA0);			

	// RGND & single discovery attempt (RGND blocking)
	I2C_WriteByte(PAGE_0_0X72, 0x95, 0x31);

	// use 1K and 2K setting
	//I2C_WriteByte(PAGE_0_0X72, 0x96, 0x22);

	// Use VBUS path of discovery state machine
	I2C_WriteByte(PAGE_0_0X72, 0x97, 0x00);
	ReadModifyWriteTPI(0x95, BIT_6, BIT_6);		// Force USB ID switch to open

	// For MHL compliance we need the following settings for register 93 and 94
	// Bug 20686
	//
	// To allow RGND engine to operate correctly.
	//
	// When moving the chip from D2 to D0 (power up, init regs) the values should be
	// 94[1:0] = 01  reg_cbusmhl_pup_sel[1:0] should be set for 5k
	// 93[7:6] = 10  reg_cbusdisc_pup_sel[1:0] should be set for 10k (default)
	// 93[5:4] = 00  reg_cbusidle_pup_sel[1:0] = open (default)
	//

	WriteByteTPI(0x92, 0x86);				//

	// change from CC to 8C to match 5K
	WriteByteTPI(0x93, 0x8C);				// Disable CBUS pull-up during RGND measurement

 	ReadModifyWriteTPI(0x79, BIT_5 | BIT_4, BIT_4);	// Force upstream HPD to 0 when not in MHL mode.
 	//ReadModifyWriteTPI(0x79, BIT_1 | BIT_2, 0); //Set interrupt active high

	msleep(25);

	ReadModifyWriteTPI(0x95, BIT_6, 0x00);		// Release USB ID switch
	I2C_WriteByte(PAGE_0_0X72, 0x90, 0x27);			// Enable CBUS discovery

       CbusReset();
       InitCBusRegs();

	// Enable Auto soft reset on SCDT = 0
	I2C_WriteByte(PAGE_0_0X72, 0x05, 0x04);

	// HDMI Transcode mode enable
	I2C_WriteByte(PAGE_0_0X72, 0x0D, 0x1C);

}

void	CbusReset(void)
{
	SET_BIT(PAGE_0_0X72, 0x05, 3);
	msleep(2);
	CLR_BIT(PAGE_0_0X72, 0x05, 3);
	mscCmdInProgress = FALSE;
	UNMASK_CBUS1_INTERRUPTS;
	UNMASK_CBUS2_INTERRUPTS;
}

static void InitCBusRegs( void )
{
	byte		regval;
	// Increase DDC translation layer timer

	TX_DEBUG_PRINT(("Drv: InitCBusRegs\n"));

       // kkcho_888	
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x07, 0x32);//for default DDC byte mode
	//I2C_WriteByte(PAGE_CBUS_0XC8, 0x07, 0x36);
	
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x40, 0x03); 			// CBUS Drive Strength
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x42, 0x06); 			// CBUS DDC interface ignore segment pointer
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x36, 0x0C);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x3D, 0xFD);	
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x1C, 0x00);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x44, 0x02);

	// Setup our devcap
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x80, MHL_DEV_ACTIVE);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x81, MHL_VERSION);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x82, MHL_DEVICE_CATEGORY);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x83, 0);  						
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x84, 0);						
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x85, (MHL_DEV_VID_LINK_SUPPRGB444|MHL_DEV_VID_LINK_SUPPYCBCR444));
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x86, MHL_DEV_AUD_LINK_2CH);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x87, 0);										// not for source
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x88, MHL_LOGICAL_DEVICE_MAP);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x89, 0);										// not for source
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8A, MHL_FEATURE_RCP_SUPPORT | MHL_FEATURE_RAP_SUPPORT|MHL_FEATURE_SP_SUPPORT);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8B, 0);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8C, 0);										// reserved
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8D, MHL_SCRATCHPAD_SIZE);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8E, 0x44 ); //MHL_INT_AND_STATUS_SIZE);
	I2C_WriteByte(PAGE_CBUS_0XC8, 0x8F, 0);										//reserved

	// Make bits 2,3 (initiator timeout) to 1,1 for register CBUS_LINK_CONTROL_2
	regval = I2C_ReadByte(PAGE_CBUS_0XC8, REG_CBUS_LINK_CONTROL_2 );
	regval = (regval | 0x0C);
	I2C_WriteByte(PAGE_CBUS_0XC8,REG_CBUS_LINK_CONTROL_2, regval);
       // Clear legacy bit on Wolverine TX.
       regval = I2C_ReadByte(PAGE_CBUS_0XC8, REG_MSC_TIMEOUT_LIMIT);
       I2C_WriteByte(PAGE_CBUS_0XC8, REG_MSC_TIMEOUT_LIMIT, (regval & MSC_TIMEOUT_LIMIT_MSB_MASK));

	// Set NMax to 1
	I2C_WriteByte(PAGE_CBUS_0XC8, REG_CBUS_LINK_CONTROL_1, 0x01);

}

bool SiiMhlTxChipInitialize ( void )
{
	// setup device registers. Ensure RGND interrupt would happen.

       TX_DEBUG_PRINT( ("Drv: SiiMhlTxChipInitialize: %02X44\n", (int)I2C_ReadByte(PAGE_0_0X72, 0x03)) );      
	WriteInitialRegisterValues();
	// Setup interrupt masks for all those we are interested.
	MASK_INTR_4_INTERRUPTS;
	MASK_INTR_1_INTERRUPTS;
	//MASK_CBUS1_INTERRUPTS; 
	//MASK_CBUS2_INTERRUPTS;

	SwitchToD3();

	return TRUE;
}

static void ReleaseUsbIdSwitchOpen ( void )
{
	mdelay(50);

	// Release USB ID switch
	ReadModifyWriteTPI(0x95, BIT_6, 0x00);
	ENABLE_DISCOVERY;
}

void SwitchToD3( void )
{
	//
	// To allow RGND engine to operate correctly.
	// So when moving the chip from D0 MHL connected to D3 the values should be
	// 94[1:0] = 00  reg_cbusmhl_pup_sel[1:0] should be set for open
	// 93[7:6] = 00  reg_cbusdisc_pup_sel[1:0] should be set for open
	// 93[5:4] = 00  reg_cbusidle_pup_sel[1:0] = open (default)
	//
	// Disable CBUS pull-up during RGND measurement

	//I2C_WriteByte(PAGE_0_0X72, 0x93, 0x04);
       ReadModifyWriteTPI(0x93, BIT_7 | BIT_6 | BIT_5 | BIT_4, 0);
       ReadModifyWriteTPI(0x94, BIT_1 | BIT_0, 0);


	// 1.8V CBUS VTH & GND threshold
	//I2C_WriteByte(PAGE_0_0X72, 0x94, 0x64);

       ReleaseUsbIdSwitchOpen();
       printk("TX_POWER_STATE_D3\n");

	// Change TMDS termination to high impedance on disconnection
	// Bits 1:0 set to 11
	I2C_WriteByte(PAGE_2_0x92, 0x01, 0x03);

	//
	// Change state to D3 by clearing bit 0 of 3D (SW_TPI, Page 1) register
	// ReadModifyWriteIndexedRegister(INDEXED_PAGE_1, 0x3D, BIT_0, 0x00);
	//
	CLR_BIT(PAGE_1_0x7A, 0x3D, 0);

	fwPowerState = TX_POWER_STATE_D3;
}

void MhlTxDrvProcessConnection ( void )
{
	bool	mhlConnected = TRUE;

	TX_DEBUG_PRINT (("Drv: MHL Cable Connected. CBUS:0x0A = %02X\n", (int) ReadByteCBUS(0x0a)));
   
	if( TX_POWER_STATE_D0_MHL == fwPowerState )
	{
		TX_DEBUG_PRINT(("POWER_STATE_D0_MHL == fwPowerState\n"));
		return;
	}

	TX_DEBUG_PRINT(("$$$$$$$\n"));

	//
	// Discovery over-ride: reg_disc_ovride	
	//

	I2C_WriteByte(PAGE_0_0X72, 0xA0, 0x10);

	fwPowerState = TX_POWER_STATE_D0_MHL;

	// Increase DDC translation layer timer (byte mode)
	// Setting DDC Byte Mode
	//

	WriteByteCBUS(0x07, 0x32);

	// Enable segment pointer safety
	SET_BIT(PAGE_CBUS_0XC8, 0x44, 1);
	// Un-force HPD (it was kept low, now propagate to source
	CLR_BIT(PAGE_0_0X72, 0x79, 4);

	// Enable TMDS
	SiiMhlTxDrvTmdsControl( TRUE );

	// Keep the discovery enabled. Need RGND interrupt
	// SET_BIT(PAGE_0_0X72, 0x90, 0);
	ENABLE_DISCOVERY;

	// Notify upper layer of cable connection
	SiiMhlTxNotifyConnection(mhlConnected = TRUE);
}

void	SwitchToD0( void )
{
	//
	// WriteInitialRegisterValues switches the chip to full power mode.
	//
	WriteInitialRegisterValues();

	// Setup interrupt masks for all those we are interested.
	//MASK_INTR_4_INTERRUPTS;
	MASK_INTR_1_INTERRUPTS;
	//MASK_CBUS1_INTERRUPTS;
	//MASK_CBUS2_INTERRUPTS;

	// Force Power State to ON

	I2C_WriteByte(PAGE_0_0X72, 0x90, 0x25);

	fwPowerState = TX_POWER_STATE_D0_NO_MHL;
       mhl_cable_status =MHL_POWER_ON;
}

#if (1) // kkcho_888
/* To use hrtimer*/
#define	MS_TO_NS(x)	(x * 1000000)

DECLARE_WAIT_QUEUE_HEAD(wake_wq);

static struct hrtimer hr_wake_timer;

static bool wakeup_time_expired;

static bool hrtimer_initialized;
static bool first_timer;

enum hrtimer_restart hrtimer_wakeup_callback(struct hrtimer *timer)
{
	wake_up(&wake_wq);
	wakeup_time_expired = true;
//	hrtimer_cancel(&hr_wake_timer);
	return HRTIMER_NORESTART;
}


void start_hrtimer_ms(unsigned long delay_in_ms)
{
	ktime_t ktime;
	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));

	wakeup_time_expired = false;
//	hrtimer_init(&hr_wake_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	if (first_timer)
		first_timer = false;
	else
		hrtimer_cancel(&hr_wake_timer);

//	hr_wake_timer.function = &hrtimer_wakeup_callback;
	hrtimer_start(&hr_wake_timer, ktime, 0x1);
}
#endif

void CbusWakeUpPulseGenerator(void)
{	
	TX_DEBUG_PRINT(("Drv: CbusWakeUpPulseGenerator\n"));

#if (1) // kkcho_888
	if (!hrtimer_initialized) {
		hrtimer_init(&hr_wake_timer, 1, 0x1);
		hr_wake_timer.function = &hrtimer_wakeup_callback;
		hrtimer_initialized = true;
		first_timer = true;
	}
		
	//
	// I2C method
	//
	//I2C_WriteByte(SA_TX_Page0_Primary, 0x92, (I2C_ReadByte(SA_TX_Page0_Primary, 0x92) | 0x10));

	// Start the pulse
	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
	start_hrtimer_ms(60);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));

	start_hrtimer_ms(T_SRC_WAKE_TO_DISCOVER);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

#else
	// Start the pulse
	I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_2);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
       mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) | 0xC0));
       mdelay(20);
       I2C_WriteByte(PAGE_0_0X72, 0x96, (I2C_ReadByte(PAGE_0_0X72, 0x96) & 0x3F));
       mdelay(T_SRC_WAKE_TO_DISCOVER);
#endif
       //
       // Toggle MHL discovery bit
       // 
       //I2C_WriteByte(PAGE_0_0X72, 0x92, (I2C_ReadByte(PAGE_0_0X72, 0x92) & 0xEF));
   
       //DISABLE_DISCOVERY;
       //ENABLE_DISCOVERY;
}

///////////////////////////////////////////////////////////////////////////
// ProcessRgnd
//
// H/W has detected impedance change and interrupted.
// We look for appropriate impedance range to call it MHL and enable the
// hardware MHL discovery logic. If not, disable MHL discovery to allow
// USB to work appropriately.
//
// In current chip a firmware driven slow wake up pulses are sent to the
// sink to wake that and setup ourselves for full D0 operation.
///////////////////////////////////////////////////////////////////////////
void	ProcessRgnd( void )
{
	byte		reg99RGNDRange;
	//
	// Impedance detection has completed - process interrupt
	//
	reg99RGNDRange = I2C_ReadByte(PAGE_0_0X72, 0x99) & 0x03;

	TX_DEBUG_PRINT(("Drv: RGND Reg 99 = %02X : ", (int)reg99RGNDRange));
	//
	// Reg 0x99
	// 00 or 11 means USB.
	// 10 means 1K impedance (MHL)
	// 01 means 2K impedance (MHL)
	//
	if (reg99RGNDRange == 0x00 || reg99RGNDRange == 0x03)
	{
             TX_DEBUG_PRINT((" : USB impedance. Disable MHL discovery.\n"));
             CLR_BIT(PAGE_0_0X72, 0x95, 5);
             mhl_cable_status =MHL_INIT_POWER_OFF;
             MHL_On(0);  
	}
	else
	{
		mhl_cable_status |= MHL_1K_IMPEDANCE_VERIFIED;
		if(0x01==reg99RGNDRange)
		{
                    TX_DEBUG_PRINT(("MHL 2K\n"));
                    mhl_cable_status =MHL_TV_OFF_CABLE_CONNECT;
                    printk(KERN_ERR "MHL Connection Fail Power off ###\n");
		      MHL_On(0);		
                    return ;
		}
		else if(0x02==reg99RGNDRange)
		{
                    TX_DEBUG_PRINT(("MHL 1K\n"));
                    msleep(T_SRC_VBUS_CBUS_TO_STABLE);

                    // Discovery enabled
                    I2C_WriteByte(PAGE_0_0X72, 0x90, 0x25);
					
                    //   
                    // Send slow wake up pulse using GPIO or I2C    
                    //
                    CbusWakeUpPulseGenerator();
		}
	}
}

////////////////////////////////////////////////////////////////////
// Int4Isr
//
//
//	Look for interrupts on INTR4 (Register 0x74)
//		7 = RSVD		(reserved)
//		6 = RGND Rdy	(interested)
//		5 = VBUS Low	(ignore)	
//		4 = CBUS LKOUT	(interested)
//		3 = USB EST		(interested)
//		2 = MHL EST		(interested)
//		1 = RPWR5V Change	(ignore)
//		0 = SCDT Change	(interested during D0)
////////////////////////////////////////////////////////////////////

#ifdef F_MHL_CABLE_DETECT_BUG_PIECEOUT 
boolean delay_first_init = FALSE;
boolean cable_detect_problem_check = TRUE;

struct mhl_cable_check_delayed_work  {
	struct delayed_work		work;
};

struct mhl_cable_check_delayed_work mhl_cable_check_control;
static void mhl_cable_check(struct work_struct *work)
{
       printk("[SKY_MHL] MHL_ON_Start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	cable_detect_problem_check = FALSE;
}
#endif

static void Int4Isr( void )
{
	byte reg74;

	reg74 = I2C_ReadByte(PAGE_0_0X72, (0x74));	// read status
	printk(KERN_ERR "[SKY_MHL] Int4Isr : REG74 : %x\n",(int)reg74);

	// When I2C is inoperational (say in D3) and a previous interrupt brought us here, do nothing.
	if(0xFF == reg74)
	{
		
              printk("RETURN (0xFF == reg74)\n");
		return;
	}

#ifdef F_MHL_CABLE_DETECT_BUG_PIECEOUT 
	if (!delay_first_init)
	{
		INIT_DELAYED_WORK_DEFERRABLE(&mhl_cable_check_control.work, mhl_cable_check);
		delay_first_init = TRUE;
	}
	
	if(0xFA == reg74 && cable_detect_problem_check && gpio_get_value(169/*MHL_WAKE_UP*/))
	{
	       schedule_delayed_work(&mhl_cable_check_control.work, 100);  // 1초...
              printk("RETURN (0xFA == reg74)\n");
		//I2C_WriteByte(PAGE_0_0X72, (0x74), reg74);	// clear all interrupts
		return;
	}
	cancel_delayed_work(&mhl_cable_check_control.work);	
	cable_detect_problem_check = TRUE;
#endif	


	if(reg74 & BIT_2) // MHL_EST_INT
	{
		if((I2C_ReadByte(PAGE_0_0X72, 0x09) & BIT_2) == 0x00) {
			printk(KERN_ERR "[MHL] RSEN is low - status incorrect\n");
			MHL_On(0);
			return;
		}	
              MASK_CBUS1_INTERRUPTS; 
              MASK_CBUS2_INTERRUPTS;
		//fwPowerState = TX_POWER_STATE_D0_MHL;
		//EnableFSA9480Interrupts(); //daniel 
		MhlTxDrvProcessConnection(); 
	}

	// process USB_EST interrupt
	else if(reg74 & BIT_3) // MHL_DISC_FAIL_INT
	{
            if(mhl_cable_status == (MHL_1K_IMPEDANCE_VERIFIED|MHL_POWER_ON))//|MHL_RSEN_VERIFIED))
            {
                  mhl_cable_status =MHL_TV_OFF_CABLE_CONNECT;
                  printk(KERN_ERR "MHL Connection Fail Power off ###\n");
		    MHL_On(0);		  
            }
            else
            {
                  MhlTxDrvProcessDisconnection();
            }
            return;
	}

	if((TX_POWER_STATE_D3 == fwPowerState) && (reg74 & BIT_6))
	{	
		SwitchToD0();
		ProcessRgnd();
	}

	// CBUS Lockout interrupt?
	if (reg74 & BIT_4)
	{
		ForceUsbIdSwitchOpen();
		ReleaseUsbIdSwitchOpen();
	}
	I2C_WriteByte(PAGE_0_0X72, (0x74), reg74);	// clear all interrupts
}

void	SiiMhlTxDrvNotifyEdidChange ( void )
{
       TX_DEBUG_PRINT(("Drv: SiiMhlTxDrvNotifyEdidChange\n"));
	//
	// Prepare to toggle HPD to upstream
	//
	// set reg_hpd_out_ovr_en to first control the hpd
	SET_BIT(PAGE_0_0X72, 0x79, 4);

	// reg_hpd_out_ovr_val = LOW to force the HPD low
	CLR_BIT(PAGE_0_0X72, 0x79, 5);

	// wait a bit
	mdelay(110);

	// release HPD back to high by reg_hpd_out_ovr_val = HIGH
	SET_BIT(PAGE_0_0X72, 0x79, 5);
}


void	SiiMhlTxGotMhlIntr( byte intr_0, byte intr_1 )
{
	TX_DEBUG_PRINT( ("MhlTx: INTERRUPT Arrived. %02X, %02X\n", (int) intr_0, (int) intr_1) );
	//
	// Handle DCAP_CHG INTR here
	//

	if(MHL_INT_DCAP_CHG & intr_0)
	{
		SiiMhlTxReadDevcap( MHL_DEV_CATEGORY_OFFSET );
       // MhlTxDriveStates( );
	}
	else if(MHL_INT_EDID_CHG & intr_1)
	{
		// force upstream source to read the EDID again.
		// Most likely by appropriate togggling of HDMI HPD
		SiiMhlTxDrvNotifyEdidChange ( );
	}

}

void	SiiMhlTxGotMhlStatus( byte status_0, byte status_1 )
{
	TX_DEBUG_PRINT( ("MhlTx: STATUS Arrived. %02X, %02X\n", (int) status_0, (int) status_1) );
	//
	// Handle DCAP_RDY STATUS here itself
	//

	if(MHL_STATUS_DCAP_RDY & status_0)
	{
           // MhlTxDriveStates( );
           // SiiMhlTxReadDevcap( MHL_DEV_CATEGORY_OFFSET );
           mhlTxConfig.mscState	 = MSC_STATE_BEGIN;
	}

	// status_1 has the PATH_EN etc. Not yet implemented
	// Remeber the event.
	mhlTxConfig.status_0 = status_0;
	mhlTxConfig.status_1 = status_1;
}

static byte CBusProcessErrors( byte intStatus )
{
  byte result          = 0;
  byte mscAbortReason  = 0;
  byte ddcAbortReason  = 0;

  /* At this point, we only need to look at the abort interrupts. */
    intStatus &=  (BIT_MSC_ABORT | BIT_MSC_XFR_ABORT);

    if ( intStatus )
    {
            //      result = ERROR_CBUS_ABORT;		// No Retry will help
              /* If transfer abort or MSC abort, clear the abort reason register. */

		if( intStatus & BIT_DDC_ABORT )
		{
			result = ddcAbortReason = ReadByteCBUS( REG_DDC_ABORT_REASON );
			TX_DEBUG_PRINT( ("CBUS DDC ABORT happened, reason:: %02X\n", (int)(ddcAbortReason)));
		}

    if ( intStatus & BIT_MSC_XFR_ABORT )
    {
      result = mscAbortReason = ReadByteCBUS( REG_PRI_XFR_ABORT_REASON );
      TX_DEBUG_PRINT( ("CBUS:: MSC Transfer ABORTED. Clearing 0x0D\n"));
      WriteByteCBUS( REG_PRI_XFR_ABORT_REASON, 0xFF );
    }

    if ( intStatus & BIT_MSC_ABORT )
    {
      TX_DEBUG_PRINT( ("CBUS:: MSC Peer sent an ABORT. Clearing 0x0E\n"));
       WriteByteCBUS( REG_CBUS_PRI_FWR_ABORT_REASON, 0xFF );
    }

    // Now display the abort reason.
    if ( mscAbortReason != 0 )
    {
      TX_DEBUG_PRINT( ("CBUS:: Reason for ABORT is ....0x%02X = ", (int)mscAbortReason ));
      if ( mscAbortReason & CBUSABORT_BIT_REQ_MAXFAIL)
      {
        TX_DEBUG_PRINT( ("Requestor MAXFAIL - retry threshold exceeded\n"));
      }

      if ( mscAbortReason & CBUSABORT_BIT_PROTOCOL_ERROR)
      {
        TX_DEBUG_PRINT( ("Protocol Error\n"));
      }

      if ( mscAbortReason & CBUSABORT_BIT_REQ_TIMEOUT)
      {
        TX_DEBUG_PRINT( ("Requestor translation layer timeout\n"));
      }

      if ( mscAbortReason & CBUSABORT_BIT_PEER_ABORTED)
      {
        TX_DEBUG_PRINT( ("Peer sent an abort\n"));
      }

      if ( mscAbortReason & CBUSABORT_BIT_UNDEFINED_OPCODE)
      {
        TX_DEBUG_PRINT( ("Undefined opcode\n"));
      }
    }
  }
    return( result );
}


void	SiiMhlTxMscCommandDone( byte data1 )
{
	TX_DEBUG_PRINT( ("MhlTx: SiiMhlTxMscCommandDone. data1 = %02X\n", (int) data1) );
	if(( MHL_READ_DEVCAP == mhlTxConfig.mscLastCommand ) && 
		(MHL_DEV_CATEGORY_OFFSET == mhlTxConfig.mscLastOffset))
	{
		// We are done reading POW. Next we read Feature Flag
		mhlTxConfig.mscState	= MSC_STATE_POW_DONE;
		//AppVbusControl( (bool) ( data1 & MHL_DEV_CATEGORY_POW_BIT) );
	}
	else if((MHL_READ_DEVCAP == mhlTxConfig.mscLastCommand) &&
				(MHL_DEV_FEATURE_FLAG_OFFSET == mhlTxConfig.mscLastOffset))
	{
		// We are done reading Feature Flag. Let app know we are done.
		mhlTxConfig.mscState	= MSC_STATE_RCP_READY;
		// Remember features of the peer
		mhlTxConfig.mscFeatureFlag	= data1;

		// Now we can entertain App commands for RCP
		// Let app know this state

		mhlTxConfig.mhlConnectionEvent = TRUE;
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_RCP_READY;

		// These variables are used to remember if we issued a READ_DEVCAP
		// Since we are done, reset them.

		mhlTxConfig.mscLastCommand = 0;
		mhlTxConfig.mscLastOffset  = 0;

		TX_DEBUG_PRINT( ("MhlTx: Peer's Feature Flag = %02X\n\n", (int) data1) );

	}
	else if(MHL_MSC_MSG_RCPE == mhlTxConfig.mscMsgLastCommand)
	{
		//
		// RCPE is always followed by an RCPK with original key code that came.
		//
		if( SiiMhlTxRcpkSend( mhlTxConfig.mscSaveRcpKeyCode ) )
		{
			// Once the command has been sent out successfully, forget this case.
			mhlTxConfig.mscMsgLastCommand = 0;
			mhlTxConfig.mscMsgLastData    = 0;
		}
	}
}

void SiiMhlTxGotMhlMscMsg( byte subCommand, byte cmdData )
{
	// Remeber the event.
	mhlTxConfig.mscMsgArrived		= TRUE;
	mhlTxConfig.mscMsgSubCommand	= subCommand;
	mhlTxConfig.mscMsgData			= cmdData;
}

void	SiiMhlTxNotifyConnection( bool mhlConnected )
{

	mhlTxConfig.mhlConnectionEvent = TRUE;
       mhlTxConfig.mscState	 = MSC_STATE_IDLE;

	if(mhlConnected)
	{
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_CONNECTION;
	}
	else
	{
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_DISCONNECTION;
	}

}


void MhlTxDrvProcessDisconnection ( void )
{
	bool	mhlConnected = FALSE;
	TX_DEBUG_PRINT (("Drv: MhlTxDrvProcessDisconnection\n"));

	// clear all interrupts
	I2C_WriteByte(PAGE_0_0X72, (0x74), I2C_ReadByte(PAGE_0_0X72, (0x74)));
	I2C_WriteByte(PAGE_0_0X72, 0xA0, 0xD0);

	//
	// Reset CBus to clear register contents
	// This may need some key reinitializations
	//

	CbusReset();

	// Disable TMDS
	SiiMhlTxDrvTmdsControl( FALSE );

	if( TX_POWER_STATE_D0_MHL == fwPowerState )
	{
		// Notify upper layer of cable connection
		SiiMhlTxNotifyConnection(mhlConnected = FALSE);
	}

	// Now put chip in sleep mode
	SwitchToD3();

}


static void ForceUsbIdSwitchOpen ( void )
{

	I2C_WriteByte(PAGE_0_0X72, 0x90, 0x26);		// Disable CBUS discovery
	ReadModifyWriteTPI(0x95, BIT_6, BIT_6);	// Force USB ID switch to open
	WriteByteTPI(0x92, 0x86);
	// Force HPD to 0 when not in MHL mode.
	ReadModifyWriteTPI(0x79, BIT_5 | BIT_4, BIT_4);
}


static	void	ApplyDdcAbortSafety()
{
	byte		bTemp, bPost;

/*	TX_DEBUG_PRINT(("[%d] Drv: Do we need DDC Abort Safety\n",
								(int) (HalTimerElapsed( ELAPSED_TIMER ) * MONITORING_PERIOD)));*/

	WriteByteCBUS(0x29, 0xFF);
	bTemp = ReadByteCBUS(0x29);
	mdelay(3);
	bPost = ReadByteCBUS(0x29);

	if ((bPost > (bTemp + 50)))
	{
		TX_DEBUG_PRINT(("Drv: Applying DDC Abort Safety(SWWA 18958)\n"));

		SET_BIT(PAGE_0_0X72, 0x05, 3);
		CLR_BIT(PAGE_0_0X72, 0x05, 3);

		InitCBusRegs();

		// Why do we do these?
		ForceUsbIdSwitchOpen();
		ReleaseUsbIdSwitchOpen();

		MhlTxDrvProcessDisconnection();
	}
}


static void MhlCbusIsr( void )
{
	byte cbusInt;
	byte gotData[4];	// Max four status and int registers.
	byte i;

	//
	// Main CBUS interrupts on CBUS_INTR_STATUS
	//

	cbusInt = ReadByteCBUS(0x08);

	// When I2C is inoperational (say in D3) and a previous interrupt brought us here, do nothing.
	if(cbusInt == 0xFF)
	{
		return;
	}

	// Look for DDC_ABORT

	if (cbusInt & BIT_2)
	{
		ApplyDdcAbortSafety();
	}

	// MSC_MSG (RCP/RAP)
	if((cbusInt & BIT_3))
	{
		SiiMhlTxGotMhlMscMsg( ReadByteCBUS( 0x18 ), ReadByteCBUS( 0x19 ) );
	}

#if (0) // kkcho_888
	if(cbusInt & BIT_4)
	{
		mscCmdInProgress = FALSE;
		SiiMhlTxMscCommandDone( ReadByteCBUS( 0x16 ) );
	}
#endif

	if((cbusInt & BIT_5) || (cbusInt & BIT_6))	// MSC_REQ_ABORT or MSC_RESP_ABORT
	{
		gotData[0] = CBusProcessErrors(cbusInt);
	}

	if(cbusInt)
	{
		WriteByteCBUS(0x08, cbusInt);
	}


#if (1) // kkcho_888
  // MSC_REQ_DONE received.
	if(cbusInt & BIT_4)
	{
		TX_DEBUG_PRINT(("Drv: MSC_REQ_DONE: %02X\n", (int) cbusInt));

		mscCmdInProgress = FALSE;

		SiiMhlTxMscCommandDone( ReadByteCBUS( 0x16 ) );
	}
#endif



	cbusInt = ReadByteCBUS(0x1E);
	if( cbusInt )
	{
          TX_DEBUG_PRINT(("Drv: CBUS INTR_2: %x\n", (int) cbusInt));
	}

	if(cbusInt & BIT_2)
	{
	       TX_DEBUG_PRINT(("Drv: INT Received: %x\n", (int) cbusInt));
		// We are interested only in first two bytes.
		SiiMhlTxGotMhlIntr( ReadByteCBUS( 0xA0 ), ReadByteCBUS( 0xA1) );

 		for(i = 0; i < 4; i++)
		{
			// Clear all
			WriteByteCBUS( (0xA0 + i), ReadByteCBUS( 0xA0 + i ));
		}
	}

	if(cbusInt & BIT_3)
	{

		// We are interested only in first two bytes.
		SiiMhlTxGotMhlStatus( ReadByteCBUS( 0xB0 ), ReadByteCBUS( 0xB1) );
		for(i = 0; i < 4; i++)
		{
			WriteByteCBUS( (0xB0 + i), ReadByteCBUS( 0xB0 + i ));
		}
	}

	if(cbusInt)
	{
		WriteByteCBUS(0x1E, cbusInt);
	}

  #if 0

	//

	// Check if a SET_HPD came from the downstream device.

	//

	cbusInt = ReadByteCBUS(0x0D);



	// CBUS_HPD status bit

	if((BIT_6 & cbusInt) != dsHpdStatus)

	{

			// Inform upper layer of change in Downstream HPD

			SiiMhlTxNotifyDsHpdChange( cbusInt );

		    //TX_DEBUG_PRINT(("Drv: Downstream HPD changed to: %02X\n", (int) cbusInt));



			// Remember

			dsHpdStatus = (BIT_6 & cbusInt);

	}

  #endif

}

void	SiiMhlTxDrvTmdsControl( bool enable )
{
	if( enable )
	{
		SET_BIT(PAGE_0_0X72, 0x80, 4);
		TX_DEBUG_PRINT(("Drv: TMDS Output Enabled\n"));
	}
	else
	{
		CLR_BIT(PAGE_0_0X72, 0x80, 4);   
		TX_DEBUG_PRINT(("Drv: TMDS Ouput Disabled\n"));
	}
}

void	SiiMhlTxNotifyDsHpdChange( byte dsHpdStatus )
{
	if( 0 == dsHpdStatus )
	{
	       TX_DEBUG_PRINT(("MhlTx: Disable TMDS\n"));
		SiiMhlTxDrvTmdsControl( FALSE );
	}
	else
	{
	       TX_DEBUG_PRINT(("MhlTx: Enable TMDS\n"));
		SiiMhlTxDrvTmdsControl( TRUE );
	}
}

void	Int1RsenIsr( void )
{
	byte reg71 = I2C_ReadByte(PAGE_0_0X72, 0x71);
	byte rsen  = I2C_ReadByte(PAGE_0_0X72, 0x09) & BIT_2;

	// Look at RSEN interrupt
	if(reg71 & BIT_5)
	{
		TX_DEBUG_PRINT (("Drv: Got2 INTR_1: reg71 = %02X, rsen = %02X\n", (int) reg71, (int) rsen));
		//pinCBusToggle = 1;	// for debug measurements. RSEN intr
		//
		// RSEN becomes LOW in SYS_STAT register 0x72:0x09[2]
		// SYS_STAT	==> bit 7 = VLOW, 6:4 = MSEL, 3 = TSEL, 2 = RSEN, 1 = HPD, 0 = TCLK STABLE
		//
		// Start countdown timer for deglitch
		// Allow RSEN to stay low this much before reacting
		//

		if(rsen == 0x00)
		{
                      if(TX_POWER_STATE_D0_MHL != fwPowerState)
                      {
                             TX_DEBUG_PRINT (("Drv: Got1 INTR_1: reg71 = %02X, rsen = %02X\n", (int) reg71, (int) rsen));
                              I2C_WriteByte(PAGE_0_0X72, 0x71, reg71);
                              return;
                      }                      
                      if(mhl_cable_status & MHL_1K_IMPEDANCE_VERIFIED)
                      {
                            TX_DEBUG_PRINT((KERN_ERR "RSEN Low and 1K impedance\n"));
                            msleep(120);

                            if((I2C_ReadByte(PAGE_0_0X72, 0x09) & BIT_2) == 0x00)
                            {
                                  TX_DEBUG_PRINT((KERN_ERR "Really RSEN Low\n"));
                                  //mhl_cable_status =MHL_INIT_POWER_OFF;

                                  mhl_cable_status = MHL_TV_OFF_CABLE_CONNECT;
				      MHL_On(0); 			  
                            }
                            else
                            {
                                  TX_DEBUG_PRINT((KERN_ERR "RSEN Stable\n"));
                            }
                      }
                      else
                      {
                            printk(KERN_ERR "%s: MHL Cable disconnect### 2\n", __func__);
                            mhl_cable_status =MHL_INIT_POWER_OFF;
				MHL_On(0);			
                      }
                	  return;
		}
		else if(rsen == 0x04)
		{
  		       mhl_cable_status |= MHL_RSEN_VERIFIED;
			printk("MHL Cable Connection ###\n");
		}
	}

      if(reg71 & BIT_6)
      {
            byte cbusInt;
            printk("HPD \n");
          
          	cbusInt = ReadByteCBUS(0x0D);

          	// CBUS_HPD status bit
          	if((BIT_6 & cbusInt) != dsHpdStatus)
          	{
                      // Inform upper layer of change in Downstream HPD
     			SiiMhlTxNotifyDsHpdChange( cbusInt );
			TX_DEBUG_PRINT(("Drv: Downstream HPD changed to: %02X\n", (int) cbusInt));		  
    	  
     			// Remember
      			dsHpdStatus = (BIT_6 & cbusInt);
          	}
			MHL_Cable_On(1);
			//HDMI_Control_On(1);
      }

      I2C_WriteByte(PAGE_0_0X72, 0x71, reg71);

}

void SiiMhlTxDeviceIsr( void )
{
	byte tmp;		//NAGSM_Android_SEL_Kernel_Aakash_20101214
	
	if( TX_POWER_STATE_D0_MHL != fwPowerState )
	{  
		//
		// Check important RGND, MHL_EST, CBUS_LOCKOUT and SCDT interrupts
		// During D3 we only get RGND but same ISR can work for both states
		//

		Int4Isr();

		if(mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT)
		{
			return ;
		}

		tmp = ReadByteCBUS(0x08);
		WriteByteCBUS(0x08, tmp);
		tmp = ReadByteCBUS(0x1E);
		WriteByteCBUS(0x1E, tmp);  
	}
	else if( TX_POWER_STATE_D0_MHL == fwPowerState ) //NAGSM_Android_SEL_Kernel_Aakash_20101214
	{
		MhlCbusIsr();
	}

	Int1RsenIsr();		//NAGSM_Android_SEL_Kernel_Aakash_20101214

}

bool SiiMhlTxReadDevcap( byte offset )
{
	cbus_req_t	req;
	//
	// Send MHL_READ_DEVCAP command
	//

	req.command     = mhlTxConfig.mscLastCommand = MHL_READ_DEVCAP;
	req.offsetData  = mhlTxConfig.mscLastOffset  = offset;

	return(SiiMhlTxDrvSendCbusCommand( &req  ));
}

static void MhlTxDriveStates( void )
{
	switch( mhlTxConfig.mscState )
	{

		case MSC_STATE_BEGIN:
                     printk("MSC_STATE_BEGIN \n");
			SiiMhlTxReadDevcap( MHL_DEV_CATEGORY_OFFSET );
			break;

		case MSC_STATE_POW_DONE:

			printk("MSC_STATE_POW_DONE \n");
			SiiMhlTxReadDevcap( MHL_DEV_FEATURE_FLAG_OFFSET );

			break;

		case MSC_STATE_IDLE:

		case MSC_STATE_RCP_READY:

			break;

		default:

			break;
	}

}

static bool SiiMhlTxRapkSend( void )
{
	return	( MhlTxSendMscMsg ( MHL_MSC_MSG_RAPK, 0 ) );
}


bool SiiMhlTxDrvSendCbusCommand ( cbus_req_t *pReq  )
{
       bool  success = TRUE;
       byte i, startbit;

	//
	// If not connected, return with error
	//
	//if( (TX_POWER_STATE_D0_MHL != fwPowerState ) || (0 == ReadByteCBUS(0x0a) || mscCmdInProgress))
       if( (TX_POWER_STATE_D0_MHL != fwPowerState ) || (mscCmdInProgress))
	{
   		return FALSE;
	}

	// Now we are getting busy
	mscCmdInProgress	= TRUE;

       /****************************************************************************************/
       /* Setup for the command - write appropriate registers and determine the correct        */
       /*                         start bit.                                                   */
       /****************************************************************************************/

	// Set the offset and outgoing data u8 right away
	WriteByteCBUS( 0x13, pReq->offsetData); 	// set offset
	WriteByteCBUS( 0x14, pReq->msgData[0] );

       startbit = 0x00;
  
       switch ( pReq->command )
       { 
     		case MHL_SET_INT:	// Set one interrupt register = 0x60
     			WriteByteCBUS( (REG_CBUS_PRI_ADDR_CMD & 0xFF), pReq->offsetData + 0x20 ); 	// set offset
     			startbit = MSC_START_BIT_WRITE_REG;
     			break;
           
             case MHL_WRITE_STAT:	// Write one status register = 0x60 | 0x80
                     WriteByteCBUS( (REG_CBUS_PRI_ADDR_CMD & 0xFF), pReq->offsetData + 0x30 ); 	// set offset
                     startbit = MSC_START_BIT_WRITE_REG;
               break;
           
             case MHL_READ_DEVCAP:	// Read one device capability register = 0x61
                     startbit = MSC_START_BIT_READ_REG;
               break;
 
             case MHL_GET_STATE:			// 0x62 -
             case MHL_GET_VENDOR_ID:		// 0x63 - for vendor id	
             case MHL_SET_HPD:			// 0x64	- Set Hot Plug Detect in follower
             case MHL_CLR_HPD:			// 0x65	- Clear Hot Plug Detect in follower
             case MHL_GET_SC1_ERRORCODE:		// 0x69	- Get channel 1 command error code
             case MHL_GET_DDC_ERRORCODE:		// 0x6A	- Get DDC channel command error code.
             case MHL_GET_MSC_ERRORCODE:		// 0x6B	- Get MSC command error code.
             case MHL_GET_SC3_ERRORCODE:		// 0x6D	- Get channel 3 command error code.
                      WriteByteCBUS( (REG_CBUS_PRI_ADDR_CMD & 0xFF), pReq->command );
                      startbit = MSC_START_BIT_MSC_CMD;
               break;
 
             case MHL_MSC_MSG:
         		WriteByteCBUS( (REG_CBUS_PRI_WR_DATA_2ND & 0xFF), pReq->msgData[1] );
         		WriteByteCBUS( (REG_CBUS_PRI_ADDR_CMD & 0xFF), pReq->command );
                     startbit = MSC_START_BIT_VS_CMD;
               break;
 
             case MHL_WRITE_BURST:
                     WriteByteCBUS( (REG_CBUS_PRI_ADDR_CMD & 0xFF), pReq->offsetData + 0x40 );
                     WriteByteCBUS( (REG_MSC_WRITE_BURST_LEN & 0xFF), pReq->length -1 );
     
                     // Now copy all bytes from array to local scratchpad
                     for ( i = 0; i < pReq->length; i++ )
                     {
                       WriteByteCBUS( (REG_CBUS_SCRATCHPAD_0 & 0xFF) + i, pReq->msgData[i] );
                     }
               
                     startbit = MSC_START_BIT_WRITE_BURST;
               break;
           
             default:
                     success = FALSE;
               break;
 
      }

      /****************************************************************************************/
  
      /* Trigger the CBUS command transfer using the determined start bit.                    */
  
      /****************************************************************************************/

     if ( success )
     {
         WriteByteCBUS( REG_CBUS_PRI_START & 0xFF, startbit );
     }

 return( success );

}

static bool MhlTxSendMscMsg ( byte command, byte cmdData )
{
	cbus_req_t	req;
	byte		ccode;

	//
	// Send MSC_MSG command
	//

	req.command     = MHL_MSC_MSG;
	req.msgData[0]  = mhlTxConfig.mscMsgLastCommand = command;
	req.msgData[1]  = mhlTxConfig.mscMsgLastData    = cmdData;

	ccode = SiiMhlTxDrvSendCbusCommand( &req  );

	return( (bool) ccode );
}

bool SiiMhlTxRcpeSend( byte rcpeErrorCode )
{
	return( MhlTxSendMscMsg ( MHL_MSC_MSG_RCPE, rcpeErrorCode ) );
}

#ifdef MHL_REMOCON_CONTROL 
void rcp_cbus_uevent(u8 rcpCode)	
{
	char env_buf[120];
	char *envp[2];
	int env_offset = 0;
	int key = 0;

	memset(env_buf, 0, sizeof(env_buf));
	printk("[SKY_MHL]%s : RCP Message Recvd , rcpCode = 0x%x\n",__func__,rcpCode);
#if (1) // kkcho_temp
       switch(rcpCode)
   	{

		case 0x01:  key = KEY_UP; break;// UP-key	
		case 0x02:  key = KEY_DOWN; break;// DOWN-key;
		case 0x03:  key = KEY_LEFT; break;// LEFT-key
		case 0x04:  key = KEY_RIGHT; break;// RIGHT-key
		case 0x00:  key = KEY_REPLY/*KEY_ENTER*/; break;// select-key //KEY_OK  //KEY_SEND	
		case 0x20 : key = KEY_0; break;
		case 0x21 : key = KEY_1; break;
		case 0x22 : key = KEY_2; break;
		case 0x23 : key = KEY_3; break;
		case 0x24 : key = KEY_4; break;
		case 0x25 : key = KEY_5; break;
		case 0x26 : key = KEY_6; break;
		case 0x27 : key = KEY_7; break;
		case 0x28 : key = KEY_8; break;
		case 0x29 : key = KEY_9; break;
		//case 0x71 : key = KEY_F1; break;
		//case 0x72 : key = KEY_F2; break;
		//case 0x74 : key = KEY_F4; break;	
		//case 0x73 : key = KEY_F3; break;
		case 0x0D : key = KEY_BACK; break;	  // KEY_END는 종료키다.
/*
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
  */
  		case 0x41 : key = KEY_VOLUMEUP; break;//SYS_KEY_MMI_VOL_UP; break;//Vol Up Key
		case 0x42 : key = KEY_VOLUMEDOWN; break; //SYS_KEY_MMI_VOL_DOWN; break;// Vol Down Key	
		case 0x44:  case 0x60: key = KEY_PLAY; break;// 207, java->MEDIA_PLAY_PAUSE
		case 0x45:  case 0x64: key = KEY_STOP; break;// STOP-key  //KEY_STOPCD
		case 0x46:  case 0x61: key = KEY_PAUSE; break;// 119, java->MEDIA_PLAY_PAUSE
		case 0x48:  key = KEY_VIDEO_PREV ; break;// <<  FORWARD로 동작 되어야 함..
		case 0x49:  key = KEY_VIDEO_NEXT ; break;// >>	  REWIND로 동작 되어야 함. 	
		case 0x4B:  key = KEY_FASTFORWARD ; break;// <<ㅇ
		case 0x4C:  key = KEY_REWIND ; break;// >>
		
		default:
			// Any freak value here would continue with no event to app
			break;	
   	}

#if (0)
       if(key == KEY_REWIND || key == KEY_FASTFORWARD)
	{
		input_report_key(ipdev, key, 1);
		input_sync(ipdev);
		msleep(10);
	}
#endif

	input_report_key(ipdev, key, 1);
	input_sync(ipdev);
	input_report_key(ipdev, key, 0);
	input_sync(ipdev);
	return;
#endif
	sprintf(env_buf, "MHL_RCP=%x", rcpCode);	
	envp[env_offset++] = env_buf;
	envp[env_offset] = NULL;
	kobject_uevent_env(mhl_remocon_control_kobj, KOBJ_CHANGE, envp);
	return;
}
#endif

#ifdef F_MHL_AUTO_CONNECT
void MHL_Set_Cable_State(bool connect)
{
	printk("[SKY_MHL] MHL_Set_Cable_State connect : %d\n", connect);

       if (connect)
   	{
		mhl_cable_state =MHL_CABLE_CONNCET;  //(connect)
   	}
	else
   	{
		mhl_cable_state =MHL_CABLE_DISCONNCET;  //(disconnect)   	
   	}	   	
}

void MHL_En_Control(bool on)
{
	printk("[SKY_MHL] MHL_En_Control  : %d\n", on);

       if (on)
   	{
#if (0) //   	
	 /* txs0102 control : HDMI-siganl level shift*/
        /* MHL_SHDW : active_low */
        gpio_set_value(23, 1);
        msleep(5);
        gpio_set_value(23, 0);
        msleep(5);
        gpio_set_value(23, 1);  
#endif

#ifdef F_MHL_USB_SW_CONTROL_BUG_FIX
   	       gpio_set_value(169/*MHL_WAKE_UP*/, 1);  //20110811, kkcho,  HPD pin이 제대로 동작 안되는 상황이므로.. 보완을 위해...	
#endif    	 
	
		gpio_set_value(16/*MHL_EN*/, 1); // switch-MHL
   	}
	else
   	{
		gpio_set_value(16/*MHL_EN*/, 0); // switch-USB	  	

#ifdef F_MHL_USB_SW_CONTROL_BUG_FIX   	
   		gpio_set_value(169/*MHL_WAKE_UP*/, 0);  //20110811, kkcho,  HPD pin이 제대로 동작 안되는 상황이므로.. 보완을 위해...	
#endif 	

#if (0)
        /* txs0102 control : HDMI-siganl level shift*/
        gpio_set_value(23, 0);	
#endif
		
   	}	   	
}

uint32_t MHL_Get_Cable_State(void)
{
	uint32_t cabel_state = 0;

	printk("[SKY_MHL] MHL_Get_Cable_State : %d\n", mhl_cable_state);

	if (mhl_cable_state == MHL_CABLE_CONNCET)
		return 1;

       return cabel_state;	   	
}

int MHL_Cable_On(int on)
{
	int key = 0;
	static int prev_on;	

	printk("[SKY_MHL] MHL_Cable_On path change : %d\n", on);

	if (on == prev_on)
		return 0;

       if (on)
   	{
		/* txs0102 control : HDMI-siganl level shift*/
		/* MHL_SHDW : active_low */
		gpio_set_value(23, 1);
		msleep(5);
		gpio_set_value(23, 0);
		msleep(5);
		gpio_set_value(23, 1);  

		   //MHL_On(1);      
		key = KEY_F23;  //KEYCODE_BUTTON_THUMBL  <= ON (connect)
   	}
	else
   	{	
		/* txs0102 control : HDMI-siganl level shift*/
		gpio_set_value(23, 0);		
		key = KEY_F24;  //KEYCODE_BUTTON_THUMBR  <= OFF (disconnect) 
   	}	   	
	
	input_report_key(ipdev, key, 1);
	input_sync(ipdev);
	input_report_key(ipdev, key, 0);
	input_sync(ipdev);

	prev_on = on;
	return 0;
}

void HDMI_Control_On(bool on)
{
	int key = 0;

	printk("[SKY_MHL] HDMI_On path change : %d\n", on);

       if (on)
   	{
		key = KEY_F21;  //KEYCODE_BUTTON_THUMBL  <= ON (connect)
   	}
	else
   	{
		key = KEY_F22;  //KEYCODE_BUTTON_THUMBR  <= OFF (disconnect)   	
   	}	   	
	
	input_report_key(ipdev, key, 1);
	input_sync(ipdev);
	input_report_key(ipdev, key, 0);
	input_sync(ipdev);
}

EXPORT_SYMBOL(MHL_Set_Cable_State);
EXPORT_SYMBOL(MHL_Get_Cable_State);
EXPORT_SYMBOL(MHL_Cable_On);
EXPORT_SYMBOL(MHL_En_Control);
EXPORT_SYMBOL(HDMI_Control_On);
#endif

void SiiMhlTxGetEvents(byte *event, byte *eventParameter)
{

       SiiMhlTxDeviceIsr();

	if(mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT)
	{
		return ;
	}

	MhlTxDriveStates( );

	*event = MHL_TX_EVENT_NONE;
	*eventParameter = 0;

	if( mhlTxConfig.mhlConnectionEvent )
	{
		// Consume the message
		mhlTxConfig.mhlConnectionEvent = FALSE;

		//
		// Let app know the connection went away.
		//
		*event          = mhlTxConfig.mhlConnected;
		*eventParameter	= mhlTxConfig.mscFeatureFlag;


		// If connection has been lost, reset all state flags.
		if(MHL_TX_EVENT_DISCONNECTION == mhlTxConfig.mhlConnected)
		{
			MhlTxResetStates( );
		}
	}
	else if( mhlTxConfig.mscMsgArrived )
	{
		// Consume the message
		mhlTxConfig.mscMsgArrived = FALSE;

		//
		// Map sub-command to an event id
		//

		switch(mhlTxConfig.mscMsgSubCommand)
		{

			case MHL_MSC_MSG_RAP:
				// RAP is fully handled here.
				//
				// Handle RAP sub-commands here itself
				//
				if( MHL_RAP_CONTENT_ON == mhlTxConfig.mscMsgData)
				{
					SiiMhlTxDrvTmdsControl( TRUE );
				}
				else if( MHL_RAP_CONTENT_OFF == mhlTxConfig.mscMsgData)
				{
					SiiMhlTxDrvTmdsControl( FALSE );
				}

				// Always RAPK to the peer
				SiiMhlTxRapkSend( );
				break;

			case MHL_MSC_MSG_RCP:
				// If we get a RCP key that we do NOT support, send back RCPE
				// Do not notify app layer.
				if(MHL_LOGICAL_DEVICE_MAP & rcpSupportTable [mhlTxConfig.mscMsgData] )
				{
					*event          = MHL_TX_EVENT_RCP_RECEIVED;
					*eventParameter = mhlTxConfig.mscMsgData; // key code
#ifdef MHL_REMOCON_CONTROL 
                                   rcp_cbus_uevent(*eventParameter);	
#endif
                                   printk("Key Code:%x \n",(int)mhlTxConfig.mscMsgData);
				}
				else
				{
  				       printk("Key Code Error:%x \n",(int)mhlTxConfig.mscMsgData);
				 #if (1) // kkcho_888
			             mhlTxConfig.mscSaveRcpKeyCode = mhlTxConfig.mscMsgData;
				 #endif
					SiiMhlTxRcpeSend( 0x01 );
				}
				break;

			case MHL_MSC_MSG_RCPK:
				*event = MHL_TX_EVENT_RCPK_RECEIVED;
                            *eventParameter = mhlTxConfig.mscMsgData; // key code
				break;

			case MHL_MSC_MSG_RCPE:
				*event = MHL_TX_EVENT_RCPE_RECEIVED;
                            *eventParameter = mhlTxConfig.mscMsgData; // status code
				break;

			case MHL_MSC_MSG_RAPK:
				// Do nothing if RAPK comes
				break;

			default:
				// Any freak value here would continue with no event to app
				break;
		}
	}
}

bool SiiMhlTxRcpkSend( byte rcpKeyCode )
{
	return	( MhlTxSendMscMsg ( MHL_MSC_MSG_RCPK, rcpKeyCode ) );
}
void	AppRcpDemo( byte event, byte eventParameter)
{
	byte	rcpKeyCode;

	switch( event )
      {
    		case MHL_TX_EVENT_DISCONNECTION:
    			printk("App: Got event = MHL_TX_EVENT_DISCONNECTION\n");
    			break;

    		case MHL_TX_EVENT_CONNECTION:
    			printk("App: Got event = MHL_TX_EVENT_CONNECTION\n");
    			break;

    		case MHL_TX_EVENT_RCP_READY:

    			// Demo RCP key code PLAY
    			rcpKeyCode = 0x44;
    			printk("App: Got event = MHL_TX_EVENT_RCP_READY...Sending RCP (%02X)\n", (int) rcpKeyCode);

          #if 0
    			if( (0 == (MHL_FEATURE_RCP_SUPPORT & eventParameter)) )
    			{
    				printk( "App: Peer does NOT support RCP\n" );
    			}
    			if( (0 == (MHL_FEATURE_RAP_SUPPORT & eventParameter)) )
    			{
    				printk( "App: Peer does NOT support RAP\n" );
    			}
    			if( (0 == (MHL_FEATURE_SP_SUPPORT & eventParameter)) )
    			{
    				printk( "App: Peer does NOT support WRITE_BURST\n" );
    			}
    			//
    			// If RCP engine is ready, send one code
    			//
    			if( SiiMhlTxRcpSend( rcpKeyCode ))
    			{
    				printk("App: SiiMhlTxRcpSend (%02X)\n", (int) rcpKeyCode);
    			}
    			else
    			{
    				printk("App: SiiMhlTxRcpSend (%02X) Returned Failure.\n", (int) rcpKeyCode);
    			}

          #endif

    			break;

    		case MHL_TX_EVENT_RCP_RECEIVED:
    			//
    			// Check if we got an RCP. Application can perform the operation here
    			// and send RCPK or RCPE. For now, we send the RCPK
    			//
    			printk("App: Received an RCP key code = %02X\n", eventParameter );      
			switch(eventParameter)
			{
				case MHD_RCP_CMD_SELECT:
					TX_DEBUG_PRINT(( "\nSelect received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_UP:
					TX_DEBUG_PRINT(( "\nUp received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_DOWN:
					TX_DEBUG_PRINT(( "\nDown received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_LEFT:
					TX_DEBUG_PRINT(( "\nLeft received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_RIGHT:
					TX_DEBUG_PRINT(( "\nRight received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_RIGHT_UP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_RIGHT_UP = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_RIGHT_DOWN:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_RIGHT_DOWN = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_LEFT_UP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_LEFT_UP = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_LEFT_DOWN:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_LEFT_DOWN = %02x\n\n", (int)eventParameter ));
					break;      

				case MHD_RCP_CMD_ROOT_MENU:
					TX_DEBUG_PRINT(( "\nRoot Menu received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_SETUP_MENU:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_SETUP_MENU = %02x\n\n", (int)eventParameter ));
					break;      

				case MHD_RCP_CMD_CONTENTS_MENU:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_CONTENTS_MENU = %02x\n\n", (int)eventParameter ));
					break;      

				case MHD_RCP_CMD_FAVORITE_MENU:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_FAVORITE_MENU = %02x\n\n", (int)eventParameter ));
					break;            

				case MHD_RCP_CMD_EXIT:
					TX_DEBUG_PRINT(( "\nExit received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_0:
					TX_DEBUG_PRINT(( "\nNumber 0 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_1:
					TX_DEBUG_PRINT(( "\nNumber 1 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_2:
					TX_DEBUG_PRINT(( "\nNumber 2 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_3:
					TX_DEBUG_PRINT(( "\nNumber 3 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_4:
					TX_DEBUG_PRINT(( "\nNumber 4 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_5:
					TX_DEBUG_PRINT(( "\nNumber 5 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_6:
					TX_DEBUG_PRINT(( "\nNumber 6 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_7:
					TX_DEBUG_PRINT(( "\nNumber 7 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_8:
					TX_DEBUG_PRINT(( "\nNumber 8 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_NUM_9:
					TX_DEBUG_PRINT(( "\nNumber 9 received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_DOT:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_DOT = %02x\n\n", (int)eventParameter ));
					break;          

				case MHD_RCP_CMD_ENTER:
					TX_DEBUG_PRINT(( "\nEnter received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_CLEAR:
					TX_DEBUG_PRINT(( "\nClear received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_CH_UP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_CH_UP = %02x\n\n", (int)eventParameter ));
					break; 

				case MHD_RCP_CMD_CH_DOWN:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_CH_DOWN = %02x\n\n", (int)eventParameter ));
					break;       

				case MHD_RCP_CMD_PRE_CH:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_PRE_CH = %02x\n\n", (int)eventParameter ));
					break;           

				case MHD_RCP_CMD_SOUND_SELECT:
					TX_DEBUG_PRINT(( "\nSound Select received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_INPUT_SELECT:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_INPUT_SELECT = %02x\n\n", (int)eventParameter ));
					break;    

				case MHD_RCP_CMD_SHOW_INFO:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_SHOW_INFO = %02x\n\n", (int)eventParameter ));
					break;     

				case MHD_RCP_CMD_HELP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_HELP = %02x\n\n", (int)eventParameter ));
					break;   

				case MHD_RCP_CMD_PAGE_UP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_PAGE_UP = %02x\n\n", (int)eventParameter ));
					break;  

				case MHD_RCP_CMD_PAGE_DOWN:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_PAGE_DOWN = %02x\n\n", (int)eventParameter ));
					break;             

				case MHD_RCP_CMD_VOL_UP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_VOL_UP = %02x\n\n", (int)eventParameter ));
					break;             

				case MHD_RCP_CMD_VOL_DOWN:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_VOL_DOWN = %02x\n\n", (int)eventParameter ));
					break;             

				case MHD_RCP_CMD_MUTE:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_MUTE = %02x\n\n", (int)eventParameter ));
					break;             

				case MHD_RCP_CMD_PLAY:
					TX_DEBUG_PRINT(( "\nPlay received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_STOP:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_STOP = %02x\n\n", (int)eventParameter ));
					break;   

				case MHD_RCP_CMD_PAUSE:
					TX_DEBUG_PRINT(( "\nPause received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_RECORD:
					TX_DEBUG_PRINT(( "\n MHD_RCP_CMD_RECORD = %02x\n\n", (int)eventParameter ));
					break;   

				case MHD_RCP_CMD_FAST_FWD:
					TX_DEBUG_PRINT(( "\nFastfwd received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_REWIND:
					TX_DEBUG_PRINT(( "\nRewind received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_EJECT:
					TX_DEBUG_PRINT(( "\nEject received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_FWD:
					TX_DEBUG_PRINT(( "\nForward received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_BKWD:
					TX_DEBUG_PRINT(( "\nBackward received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_PLAY_FUNC:
					TX_DEBUG_PRINT(( "\nPlay Function received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_PAUSE_PLAY_FUNC:
					TX_DEBUG_PRINT(( "\nPause_Play Function received = %02x\n\n", (int)eventParameter ));
					break;

				case MHD_RCP_CMD_STOP_FUNC:
					TX_DEBUG_PRINT(( "\nStop Function received = %02x\n\n", (int)eventParameter ));
					break;

				default:

					break;
			}
				
                     rcpKeyCode = eventParameter;
    			SiiMhlTxRcpkSend(rcpKeyCode);
    			break;

    		case MHL_TX_EVENT_RCPK_RECEIVED:
    			printk(KERN_ERR "App: Received an RCPK = \n");
    			break;

    		case MHL_TX_EVENT_RCPE_RECEIVED:
    			printk(KERN_ERR "App: Received an RCPE = \n");
    			break;
                
    		default:
    			break;

    	}

}

/*===========================================================================

  FUNCTION SiI9244_interrupt_event

  DESCRIPTION
   When SII9244 H/W interrupt happen, call this event function
===========================================================================*/
void sii9244_interrupt_event(void)
{
       byte    event;
       byte    eventParameter;
       byte    flag;

       do {
            flag = 0;
            SiiMhlTxGetEvents( &event, &eventParameter );

            if( MHL_TX_EVENT_NONE != event )
            {
                    AppRcpDemo( event, eventParameter);
            }
 
            if(mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT)
            {
                	byte tmp;
                	tmp = I2C_ReadByte(PAGE_0_0X72, (0x74));   // read status
                	I2C_WriteByte(PAGE_0_0X72, (0x74), tmp);   // clear all interrupts
                	tmp = I2C_ReadByte(PAGE_0_0X72, 0x71);
                	I2C_WriteByte(PAGE_0_0X72, 0x71, tmp);

                	tmp = ReadByteCBUS(0x08);
                	WriteByteCBUS(0x08, tmp);  

                	tmp = ReadByteCBUS(0x1E);
                	WriteByteCBUS(0x1E, tmp);   
			TX_DEBUG_PRINT(("mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT\n"));
                	return ;
            }
            else if(((fwPowerState == TX_POWER_STATE_D0_MHL)||(fwPowerState == TX_POWER_STATE_D0_NO_MHL))&& mhl_cable_status) //NAGSM_Android_SEL_Kernel_Aakash_20101214
            {
                    byte tmp;

                    tmp = I2C_ReadByte(PAGE_0_0X72, (0x74));   // read status
                    flag |= (tmp&INTR_4_DESIRED_MASK);	  
		      printk("#1 (0x74) flag: %x\n",(int) flag );		

                    tmp = I2C_ReadByte(PAGE_0_0X72, 0x71);
                    flag |= (tmp&INTR_1_DESIRED_MASK);
		      printk("#1 (0x71) flag: %x\n",(int) flag );

                    if(mhlTxConfig.mhlConnected == MHL_TX_EVENT_DISCONNECTION)//(mhlTxConfig_status()== MHL_TX_EVENT_DISCONNECTION)//
                    {
                         tmp = ReadByteCBUS(0x08);
                         printk("#2 (ReadByteCBUS(0x08)) Temp: %x\n",(int) tmp );
                         WriteByteCBUS(0x08, tmp);  

                         tmp = ReadByteCBUS(0x1E);
                         printk("#2 (ReadByteCBUS(0x1E)) Temp: %x\n",(int) tmp );
                         WriteByteCBUS(0x1E, tmp);    
                    }
                    else
                    {
                         tmp = ReadByteCBUS(0x08);
                         flag |= (tmp&INTR_CBUS1_DESIRED_MASK);
                         printk("#1 (ReadByteCBUS(0x08)) Temp: %x\n",(int) flag );

                         tmp = ReadByteCBUS(0x1E);
                         flag |= (tmp&INTR_CBUS2_DESIRED_MASK);
                         printk("#1 (ReadByteCBUS(0x1E)) Temp: %x\n",(int) flag );              
                    }

                    if((flag == 0xFA)||(flag == 0xFF))
                    {
                        flag = 0;
                    }
            }
 
       }while(flag);
	   printk(KERN_ERR "[MHL]sii9244_interrupt_event :: flag: %x\n",(int) flag );
}

EXPORT_SYMBOL(sii9244_interrupt_event);

/*===========================================================================

===========================================================================*/


/*===========================================================================

===========================================================================*/
//------------------------------------------------------------------------------
// Function: I2C_WriteByte
// Description:
//------------------------------------------------------------------------------
void I2C_WriteByte(byte deviceID, byte offset, byte value)
{
	struct i2c_client* client_ptr = get_sii9244_client(deviceID);
	if(!client_ptr)
	{
		printk("[MHL]I2C_WriteByte error %x\n",deviceID); 
		return;	
	}
	
	if(deviceID == 0x72)
		sii9244_i2c_write(client_ptr,offset,value);
	else if(deviceID == 0x7A)
		sii9244_i2c_write(client_ptr,offset,value);
	else if(deviceID == 0x92)
		sii9244_i2c_write(client_ptr,offset,value);
	else if(deviceID == 0xC8)
		sii9244_i2c_write(client_ptr,offset,value);
}

byte I2C_ReadByte(byte deviceID, byte offset)
{
        byte number = 0;
        struct i2c_client* client_ptr = get_sii9244_client(deviceID);
        if(!client_ptr)
        {
        	printk("[MHL]I2C_ReadByte error %x\n",deviceID); 
        	return 0;	
        }


        if(deviceID == 0x72)
              number = sii9244_i2c_read(client_ptr,offset);
        else if(deviceID == 0x7A)
        	number = sii9244_i2c_read(client_ptr,offset);
        else if(deviceID == 0x92)
        	number = sii9244_i2c_read(client_ptr,offset);
        else if(deviceID == 0xC8)
        	number = sii9244_i2c_read(client_ptr,offset);

        return (number);

}

byte ReadByteTPI (byte Offset) 
{
	return I2C_ReadByte(PAGE_0_0X72, Offset);
}

void WriteByteTPI (byte Offset, byte Data) 
{
	I2C_WriteByte(PAGE_0_0X72, Offset, Data);
}

void ReadModifyWriteTPI(byte Offset, byte Mask, byte Data) 
{
	byte Temp;

	Temp = ReadByteTPI(Offset);		// Read the current value of the register.
	Temp &= ~Mask;					// Clear the bits that are set in Mask.
	Temp |= (Data & Mask);			// OR in new value. Apply Mask to Value for safety.
	WriteByteTPI(Offset, Temp);		// Write new value back to register.
}

byte ReadByteCBUS (byte Offset) 
{
	return I2C_ReadByte(PAGE_CBUS_0XC8, Offset);
}

void WriteByteCBUS(byte Offset, byte Data) 
{
	I2C_WriteByte(PAGE_CBUS_0XC8, Offset, Data);
}

void ReadModifyWriteCBUS(byte Offset, byte Mask, byte Value) 
{
        byte Temp;

        Temp = ReadByteCBUS(Offset);
        Temp &= ~Mask;
        Temp |= (Value & Mask);
        WriteByteCBUS(Offset, Temp);
}

void	AppVbusControl( bool powerOn )
{
	if( powerOn )
	{
		printk("App: Peer's POW bit is set. Turn the VBUS power OFF here.\n");
	}
	else
	{
		printk("App: Peer's POW bit is cleared. Turn the VBUS power ON here.\n");
	}
}

