/* customer definition for MTP (Media Transfer Protocol)
 *
 * $Id: f_mtpDroid.h, v1.00, 2010.04.08:15:28:582$
 *
 * Initial work by:
 *   (c) 2007 Insight-kr Lee Hyun Wook (wondo@insight-kr.com)
 *
                        EDIT HISTORY FOR MODULE
 
when            who          what, where, why
--------      ------     ----------------------------------------------------------
2007/11/27   wondo      Initial work............^^"
2011/03/23   wondo      Pantech [APACHE : ??] Porting
2011/05/04   wondo      Pantech [MINI : P9060] Porting



-----------------------------------------------------------------------------*/
#ifndef __USB_FUNCTION_MTP_H__
#define __USB_FUNCTION_MTP_H__   

/* customer Define----------------------------------------------*/
#define FEATURE_MINI_DEVICE		1  //..for pantech
#define FEATURE_XXX_SHIT 			0
#define FEATURE_SETUP_RESPONSE	1



/* Debug Define------------------------------------------------*/
//#define __DEBUG_iMTP_INIT
//#define __DEBUG_iMTP_SETUP
//#define __DEBUG_iMTP_READ
//#define __DEBUG_iMTP_WRITE
//#define __DEBUG_iMTP_IOCTL

//..#undef __DEBUG_iMTP_INIT
//..#undef __DEBUG_iMTP_SETUP
//..#undef __DEBUG_iMTP_READ
//..#undef __DEBUG_iMTP_WRITE
//..#undef __DEBUG_iMTP_IOCTL
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
#ifdef __DEBUG_iMTP_SETUP
#define dbglog_i(format, args...) printk("MT_i> " format, ##args)
#else
#define dbglog_i(format, args...) do {} while(0)
#endif

#ifdef __DEBUG_iMTP_SETUP
#define dbglog_s(format, args...) printk("MT_s> " format, ##args)
#else
#define dbglog_s(format, args...) do {} while(0)
#endif

#ifdef __DEBUG_iMTP_READ
#define dbglog_r(format, args...) printk("MT_r> " format, ##args)
#else
#define dbglog_r(format, args...) do {} while(0)
#endif

#ifdef __DEBUG_iMTP_WRITE
#define dbglog_w(format, args...) printk("MT_w> " format, ##args)
#else
#define dbglog_w(format, args...) do {} while(0)
#endif

#ifdef __DEBUG_iMTP_IOCTL
#define dbglog_io(format, args...) printk("MT_io> " format, ##args)
#else
#define dbglog_io(format, args...) do {} while(0)
#endif

#define true 1
#define false 0

/*------------------------------------------------------------*/
typedef struct{
	int Mode;
	int Len;
	char Buff[50];
}__attribute__ ((packed)) SCRIPT_TYPE;

/*------------------------------------------------------------*/

#define MTP_SET_STATUS			_IOW('g', 0x21, int)
#define MTP_GET_STATUS			_IOR('g', 0x22, int *)
#define MTP_END_STATUS			_IOW('g', 0x23, int)
#define MTP_CLEAR_HALT			_IOW('g', 0x26, int)

#define MTP_SET_SCRIPT			_IOW('g', 0x31, SCRIPT_TYPE)
#define MTP_SET_PIPE_INIT		_IOW('g', 0x32, int)
#define MTP_SET_LIST_INIT		_IOW('g', 0x33, int)
/*------------------------------------------------------------*/

typedef enum{
	UM_NONE,
	UM_START,
	UM_ACTIVE,
	UM_DEACTIVE
}USB_MTP_STATE;

typedef enum
{
    MTP_OK,
    MTP_BUSY,
    MTP_CANCEL
} MTP_DEVICE_STATUS;
/*------------------------------------------------------------*/


#endif  /* __USB_FUNCTION_MTP_H__ */

