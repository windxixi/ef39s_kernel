//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Mailbox.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_MAILBOX_H__
#define __TC317X_CORE_MAILBOX_H__

#include "TC317X_struc.h"

#define MAX_MAILBOX_RETRY (3)   // continuously

#define MB_HOSTMAIL		0x47
#define MB_SLAVEMAIL 	0x74

#define MB_ERR_OK		0x00
#define MB_ERR_CMD		0x01
#define MB_ERR_PARA		0x02

#define MBCMD_SYS						(0x00<<11)
#define MBPARA_SYS_WARMBOOT 		(MBCMD_SYS | 0x00)
#define MBPARA_SYS_START 			(MBCMD_SYS | 0x01)
#define MBPARA_SYS_SYNC 			(MBCMD_SYS | 0x02)

#define MBPARA_SYS_DAB_STREAM_SET 	(MBCMD_SYS | 0x0D)
#define MBPARA_SYS_DAB_STREAM_ON 	(MBCMD_SYS | 0x0E)
#define MBPARA_SYS_DAB_RECONFIG_CLR (MBCMD_SYS | 0x12)

#define MBPARA_SYS_DAB_MCI_UPDATE 			(MBCMD_SYS | 0x0c)
#define MBPARA_SYS_DAB_DP_FLT 				(MBCMD_SYS | 0x0f)
#define MBPARA_SYS_DAB_RESYNC_PARAM 		(MBCMD_SYS | 0x10)
#define MBPARA_SYS_DAB_RESYNC_RESULT 		(MBCMD_SYS | 0x11)
#define MBPARA_SYS_DAB_RF_FREQ_NO 			(MBCMD_SYS | 0x20)
#define MBPARA_SYS_DAB_RF_FREQ 				(MBCMD_SYS | 0x21)
#define MBPARA_SYS_DAB_ASM_VER 				(MBCMD_SYS | 0xff)

#define MBCMD_AGC_DAB						(0x01<<11)
#define MBCMD_AGC_DAB_CFG				(MBCMD_AGC_DAB | 0)
#define MBCMD_AGC_DAB_GAIN				(MBCMD_AGC_DAB | 1)
#define MBCMD_AGC_DAB_JAM				(MBCMD_AGC_DAB | 2)
#define MBCMD_AGC_DAB_3				(MBCMD_AGC_DAB | 3)
#define MBCMD_AGC_DAB_4				(MBCMD_AGC_DAB | 4)
#define MBCMD_AGC_DAB_5			(MBCMD_AGC_DAB | 5)
#define MBCMD_AGC_DAB_6				(MBCMD_AGC_DAB | 6)
#define MBCMD_AGC_DAB_7				(MBCMD_AGC_DAB | 7)
#define MBCMD_AGC_DAB_8				(MBCMD_AGC_DAB | 8)
#define MBCMD_AGC_DAB_9				(MBCMD_AGC_DAB | 9)
#define MBCMD_AGC_DAB_A				(MBCMD_AGC_DAB | 10)
#define MBCMD_AGC_DAB_B				(MBCMD_AGC_DAB | 11)
#define MBCMD_AGC_DAB_C				(MBCMD_AGC_DAB | 12)
#define MBCMD_AGC_DAB_D				(MBCMD_AGC_DAB | 13)
#define MBCMD_AGC_DAB_E				(MBCMD_AGC_DAB | 14)
#define MBCMD_AGC_DAB_F				(MBCMD_AGC_DAB | 15)

#define MBCMD_FP_DAB					(0x02 << 11)
#define MBCMD_FP_DAB_CFG				(MBCMD_FP_DAB | 0)
#define MBCMD_FP_DAB_IIR				(MBCMD_FP_DAB | 1)

#define MBCMD_CTO_DAB						(0x04<<11)
#define MBCMD_CTO_DAB_PARAM				(MBCMD_CTO_DAB | 0x00)
#define MBCMD_CTO_DAB_RESULT				(MBCMD_CTO_DAB | 0x01)

#define MBCMD_CFO_DAB						(0x06<<11)
#define MBCMD_CFO_DAB_PARAM				(MBCMD_CFO_DAB | 0x00)
#define MBCMD_CFO_DAB_RESULT				(MBCMD_CFO_DAB | 0x01)

#define MBCMD_FFO_DAB						(0x07<<11)
#define MBCMD_FTO_DAB						(0x0A<<11)

#define MBCMD_SFE_DAB						(0x0B<<11)
#define MBCMD_SFE_DAB_PARAM				(MBCMD_SFE_DAB | 0x00)
#define MBCMD_SFE_DAB_RESULT				(MBCMD_SFE_DAB | 0x01)

#define MBCMD_CIR_DAB						(0x0C<<11)
#define MBCMD_CIR_DAB_PARAM				(MBCMD_CIR_DAB | 0x00)
#define MBCMD_CIR_DAB_RESULT				(MBCMD_CIR_DAB | 0x01)

#define MBCMD_DP_DAB						(0x10<<11)
#define MBCMD_DP_DAB_RESULT				(MBCMD_DP_DAB | 0x00)
#define MBCMD_DP_DAB_CFG1					(MBCMD_DP_DAB | 0x01)
#define MBCMD_DP_DAB_CFG2					(MBCMD_DP_DAB | 0x02)

#define MBCMD_DEBUG_DAB					(0x1F<<11)

#define MBPARA_SYS_VERSION 	        (MBCMD_SYS | 0xFF)

// Freqset
#define MBPARA_SYS_NUM_FREQ 	(MBCMD_SYS | 0x20)
#define MBPARA_SYS_FREQ_0_6    	(MBCMD_SYS | 0x21)
#define MBPARA_SYS_FREQ_7_13   	(MBCMD_SYS | 0x22)
#define MBPARA_SYS_FREQ_14_20  	(MBCMD_SYS | 0x23)
#define MBPARA_SYS_FREQ_21_27  	(MBCMD_SYS | 0x24)

#define MBCMD_SEL_CH_INFO				(0x13<<11)
#define MBPARA_SEL_CH_INFO_PRAM 	(MBCMD_SEL_CH_INFO | 0x00)
#define MBPARA_SEL_CH_INFO_RESULT 	(MBCMD_SEL_CH_INFO | 0x01)

#define MBCMD_VARI_OSC				(0x15<<11)
#define MBCMD_VARI_OSC_SPUR 	    (MBCMD_VARI_OSC | 0x00)
#define MBCMD_VARI_OSC_RCSTEP 	    (MBCMD_VARI_OSC | 0x01)
#define MBCMD_VARI_OSC_RFPLL 	    (MBCMD_VARI_OSC | 0x02)

// Mailbox protocol
// dir : 8bit
// size : 4bit
// RW
// error/ok : 12bit
// cmd : 5bit
// param : 3bit

#ifndef DEF_MAILBOX_T
#define DEF_MAILBOX_T
typedef struct
{
  uint32_t cmd;
  uint32_t word_cnt;
  uint32_t status;
  uint32_t data_array[7];
} tMAILBOX;
#endif //DEF_MAILBOX_T

#define MB_CMD_READ	0
#define MB_CMD_WRITE	1

void      TC317X_Mailbox_Error_Handler (t_TC3X_DEMODULE * pTC317X);
int32_t       TC317X_MB_TxOnly (t_TC3X_DEMODULE * pTC317X, int32_t cmd, uint32_t *data_array, int32_t word_cnt);
int32_t       TC317X_MB_TxnRead (t_TC3X_DEMODULE * pTC317X, tMAILBOX * p_mailbox, int32_t cmd, uint32_t *data_array, int32_t word_cnt, int32_t warmboot);
uint32_t TC317X_GetAccessMail (t_TC3X_DEMODULE * pTC317X);

#endif
