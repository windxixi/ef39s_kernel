//----------------------------------------------------------------------------
//   FileName    : TC3X_IO_UserDefine.h
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#ifndef __TC317X_IO_USERDEFINE_h__
#define __TC317X_IO_USERDEFINE_h__

#define FEATURE_APPLY_EF40K

#ifdef FEATURE_APPLY_EF40K
#include <linux/slab.h>
#include "../../tdmb_comdef.h"
#include "../../tdmb_gpio.h"
#ifdef FEATURE_QTDMB_SPI_IF
#include "../../tdmb_spi.h"
#endif
#endif

#define USE_ANDROID
//---------------------------
// Special Platform Define
//
#define USE_ANDROID
//#define TC_AP
#define TC317X_SR
//#define TC317X_ES

#define TC317X_SPLIT_MSC_BUFFER
#define TC317X_USING_WORKQUEUE
//#define TC317X_USING_KTHREAD
#define TC317X_AGC_IN_BOOT
#define TC317X_REDUCED_IO

#if defined(TC317X_ES)
#define MAX_FIFO_SIZE           (0x2000)
#elif defined(TC317X_SR)
#define MAX_FIFO_SIZE           (0x4000)
#define	TDMB_CSPI_READ_SIZE_FIC	(388+4)      //(FIC + HEADER)
#endif

//--------------------------------
// TCC3170 Defines!!!!!!!!!!!!!!!!
//
//#define       STREAM_MIXER

#define USE_HOST_FREQ_SET_RF
#define DEF_OSCCLK_RF   19200

//---------------------------
#define DUAL_BB_AVAIL

#ifdef DUAL_BB_AVAIL
#define MAX_TCC3X		    2
#define TCC3X_DEVICE0
#define TCC3X_DEVICE1
#else
#define MAX_TCC3X		    1
#define TCC3X_DEVICE0
#endif

//---------------------------
//#define TC317X_MULTI_DMB

/* Interface Define */
#ifdef FEATURE_QTDMB_SPI_CMD
#define USE_IF_CSPI
#else
#define USE_IF_I2C
#endif

#ifdef FEATURE_APPLY_EF40K
#define PRINTF_LV_0(fmt, arg...) TDMB_KERN_MSG_ALERT(fmt, ##arg) 
#else
#define PRINTF_LV_0(fmt, arg...) printk 
#endif
#define PRINTF_LV_1 //LOGD
#define PRINTF_LV_2 //LOGD

/* Packet size & Packet Num Define (Recommand) */
#define TC3X_TDMB_PKTSIZE       (188)   // for fifo
#define TC3X_TDMB_PKTNUM        (20)
#define TC3X_TDMB_PKTNUM_CMDIO  (20)

/* lock check time definition */
#define TDMB_OFDMDETECT_LOCK    (100)
#define TDMB_OFDMDETECT_RETRY   (1)
#define TDMB_CTO_LOCK           (100)
#define TDMB_CTO_RETRY          (3)
#define TDMB_CFO_LOCK           (20)
#define TDMB_CFO_RETRY          (3)
// invalid min : 100
// valid min : 220
// worst : 760

#endif
