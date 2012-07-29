//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_H__
#define __TC317X_CORE_H__

#include "../IO/TC317X_IO.h"
#include "TC317X_deviceAPI.h"
#include "TC317X_struc.h"

#define	TC3X_I2C_DEV_ADDR   0xA0
#define	TC3X_I2C_DEV_ADDR2  0xA2

#define CB_OP_BIN_NOT_READY	-1
#define CB_OP_BIN_CRC_FAIL	-2

// TC3X Driver core Define
// -----------------------------------------------------------------
// IO Memory & MemoryMap
//

#define PHY_BASE_ADDR           (0x80000000)
#define PHY_MEM_FIFO_START_ADDR (0x00000000)

#define FIFO_A_SIZE           MAX_FIFO_SIZE
#define FIFO_B_SIZE           (0x0)
#define FIFO_C_SIZE           (0x0)
#define FIFO_D_SIZE           (0x0)

#define TC317X_STATUS_SIZE (32+4)
#define TC317X_FIC_SIZE    (384+4)
#define TC317X_TS_SIZE     (188+4)

#ifdef TC317X_SPLIT_MSC_BUFFER
#define CACHED_BUFFER_SIZE_DUAL (MAX_FIFO_SIZE>>2)
#define CACHED_BUFFER_SIZE_MSC  (MAX_FIFO_SIZE>>1)
#define BUFFER_SIZE_MSC_FIC     (MAX_FIFO_SIZE>>1)
#else
#define CACHED_BUFFER_SIZE_DUAL (MAX_FIFO_SIZE)
#define CACHED_BUFFER_SIZE_MSC  CACHED_BUFFER_SIZE_DUAL
#define BUFFER_SIZE_MSC_FIC     (MAX_FIFO_SIZE>>1)
#endif

//total 16kbyte
#if defined(TC317X_ES)
#define PHY_MEM_ADDR_A_START	(PHY_BASE_ADDR + 0xc000)
#elif defined(TC317X_SR)
#define PHY_MEM_ADDR_A_START	(PHY_BASE_ADDR + 0xa000)
#endif
#define PHY_MEM_ADDR_A_END		(PHY_MEM_ADDR_A_START+FIFO_A_SIZE-1)
#define PHY_MEM_ADDR_B_START	(PHY_MEM_ADDR_A_END+1)
#define PHY_MEM_ADDR_B_END		(PHY_MEM_ADDR_B_START+FIFO_B_SIZE-1)
#define PHY_MEM_ADDR_C_START	(PHY_MEM_ADDR_B_END+1)
#define PHY_MEM_ADDR_C_END		(PHY_MEM_ADDR_C_START+FIFO_C_SIZE-1)
#define PHY_MEM_ADDR_D_START	(PHY_MEM_ADDR_C_END+1)
#define PHY_MEM_ADDR_D_END		(PHY_MEM_ADDR_D_START+FIFO_D_SIZE-1)

#define DP_CFG_OPSET1         (1144)
#define DP_CFG_OPSET2         (204)

#define DP_CFG_1_DATA0				(PHY_BASE_ADDR + 0xd000)
#define DP_CFG_2_DATA0				(DP_CFG_1_DATA0 + DP_CFG_OPSET1)
#define DP_CFG_1_DATA1				(DP_CFG_2_DATA0 + DP_CFG_OPSET2)
#define DP_CFG_2_DATA1				(DP_CFG_1_DATA1 + DP_CFG_OPSET1)
#define DP_CFG_1_DATA2				(DP_CFG_2_DATA1 + DP_CFG_OPSET2)
#define DP_CFG_2_DATA2				(DP_CFG_1_DATA2 + DP_CFG_OPSET1)
#define DP_CFG_1_DATA3				(DP_CFG_2_DATA2 + DP_CFG_OPSET2)
#define DP_CFG_2_DATA3				(DP_CFG_1_DATA3 + DP_CFG_OPSET1)


// CODE Memory Setting
#define	TCC3170_START_PC                (0x0000)
#define	TCC3170_START_PC_opset          (0x8000)
#define	TC317X_CODEMEMBASE              (PHY_BASE_ADDR+TCC3170_START_PC)
#define	TC317X_CODEMEMBASE_8000         (PHY_BASE_ADDR+TCC3170_START_PC_opset)
#define	TC317X_CODE_TABLEBASE_RAND      (0xF0020000)
#define	TC317X_CODE_TABLEBASE_DINT      (0xF0024000)
#define	TC317X_CODE_TABLEBASE_DAGU      (0xF0028000)
#define	TC317X_CODE_TABLEBASE_COL_ORDER (0xF002C000)

#define STREAM_BUFFER_DATA_ENABLE  (0x1<<7)
#define STREAM_BUFFER_HEADER_ON    (0x1<<6)
#define STREAM_BUFFER_MASK_BUFFERD (0x1<<3)
#define STREAM_BUFFER_MASK_BUFFERC (0x1<<2)
#define STREAM_BUFFER_MASK_BUFFERB (0x1<<1)
#define STREAM_BUFFER_MASK_BUFFERA (0x1<<0)

#define STREAM_BUFFER_ALL (      \
    STREAM_BUFFER_DATA_ENABLE  | \
    STREAM_BUFFER_HEADER_ON    | \
    STREAM_BUFFER_MASK_BUFFERD | \
    STREAM_BUFFER_MASK_BUFFERC | \
    STREAM_BUFFER_MASK_BUFFERB | \
    STREAM_BUFFER_MASK_BUFFERA)

#define STREAM_CMD_FIFO_MSB     (0x1<<0)
#define STREAM_CMD_FIFO_ENABLE  (0x1<<1)
#define STREAM_CMD_FIFO_INIT    (0x1<<4)
#define STREAM_LATCH_TOTAL_SIZE (0x1<<5)

#define CHANNEL_CFG_DAB_PLUS   (0x1<<6)
#define CHANNEL_CFG_EPM        (0x1<<5)
#define CHANNEL_CFG_DMB        (0x1<<4)
#define CHANNEL_CFG_DAB        (0x1<<3)
#define CHANNEL_CFG_FIC        (0x1<<2)
#define CHANNEL_CFG_FIB_FILTER (0x1<<1)
#define CHANNEL_CFG_STATUS     (0x1<<0)
#define CHANNEL_CFG_ALL     \
    (CHANNEL_CFG_DAB_PLUS  |\
    CHANNEL_CFG_EPM        |\
    CHANNEL_CFG_DMB        |\
    CHANNEL_CFG_DAB        |\
    CHANNEL_CFG_FIC        |\
    CHANNEL_CFG_FIB_FILTER |\
    CHANNEL_CFG_STATUS)

#define IRQ_ERR_MAILBOX (0x1<<0)
#define IRQ_ERR_I2C     (0x1<<1)
#define IRQ_ERR_DATA    (0x1<<3)
#define IRQ_ERR_BUFFERA (0x1<<4)
#define IRQ_ERR_BUFFERB (0x1<<5)
#define IRQ_ERR_BUFFERC (0x1<<6)
#define IRQ_ERR_BUFFERD (0x1<<7)

//-----------------------------------------------------------------------------
// Queue Commands
enum
{
  CMDTC3X_INTERRUPT = 700,
  CMDTC3X_SPI_STS_DATA_COME,
  CMDTC3X_RESERVED0
};

void TC317X_CoreAttach (int32_t moduleidx, int32_t num);
void TC317X_CoreDetach (int32_t moduleidx);
void TC317X_Stream_DeAlloc (int32_t moduleidx);
t_TC3X_DEMODULE *GetPointerTC317X (int32_t moduleidx, int32_t idx);
int	TC317X_GetChipID (int32_t moduleidx, int32_t idx);
void TC317X_CoreInit (int32_t moduleidx, int32_t Reserved0);
void TC317X_CoreIOInit (int32_t moduleidx, TC3X_DNUM hBB, int32_t MainIO, int32_t StreamIO, int32_t PktSize, int32_t PktThrNum);
void TC317X_PLL_SET (t_TC3X_DEMODULE * pTC317X);
uint32_t	TC317X_WarmBoot (t_TC3X_DEMODULE * pTC317X, int32_t bStreamStart);
void TC317X_Set_AGC_Table_Vhf (t_TC3X_DEMODULE * pTC317X);
void TC317X_Set_AGC_Table_Lband(t_TC3X_DEMODULE * pTC317X);
void TC317X_Set_FP_config (t_TC3X_DEMODULE * pTC317X);
void TC317X_Set_FP_IIR_Coeff (t_TC3X_DEMODULE * pTC317X, int32_t );
int32_t TC317X_CoreColdBoot (int32_t moduleidx,
    uint8_t *pCOLDBOOT, uint32_t COLDBOOTSize,
    uint8_t *pDAGU, uint32_t DAGUSize,
    uint8_t *pDINT, uint32_t DINTSize,
    uint8_t *pRAND, uint32_t RANDSize, uint8_t *pCOL_ORDER, uint32_t COL_ORDERSize);
void      TC317X_CoreStreamIOInit (t_TC3X_DEMODULE * pTC317X);
void      TC317X_CoreSetInterruptMode (int32_t moduleidx, int32_t idx);
void      TC317X_Set_INT_Enable (int32_t moduleidx);
void      TC317X_CoreGPIOnEnableSystem (int32_t moduleidx, TC3X_DNUM hBB);
void      Start_Standard_Process (t_TC3X_DEMODULE * pTC317X);
void      Stop_Standard_Process (t_TC3X_DEMODULE * pTC317X);
int32_t   TC317X_CoreSetFrequency (int32_t moduleidx, TC3X_DNUM hBB, int32_t freq_khz, int32_t bw_khz, tFreqSet_Option * pOption);
void      TC3X_Core_AutoScan (int32_t moduleidx, TC3X_DNUM hBB, uint32_t bandwidth, uint32_t frequency, uint32_t e_frequency, tFreqSet_Option * pOption, uint32_t *pResult);
void      TC317X_Stream_StartSet (int32_t moduleidx);
void      TC317X_Stream_Stop (int32_t moduleidx);
void      TC317XTask_Start (int32_t moduleidx);
void      TC317XTask_Stop (int32_t moduleidx);
void      TC317XQue_Init (int32_t moduleidx);
void      TC317XQue_Delete (int32_t moduleidx);
void      TC317X_OP_Enable (t_TC3X_DEMODULE * pTC317X, uint32_t AdditionalOpt);
void      TC317X_OP_Disable (t_TC3X_DEMODULE * pTC317X, uint32_t AdditionalOpt);
void      TC317X_OutBuffer_Init_En (t_TC3X_DEMODULE * pTC317X);
TC3X_IO_Err TC317X_DRV_Mem_Write (t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize, int32_t DMAOPT, int32_t CIRMODE);
TC3X_IO_Err TC317X_DRV_Mem_Read (t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize, int32_t DMAOPT, int32_t CIRMODE);
TC3X_IO_Err TC317X_DRV_Stream_Read (t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize);
void      TC317X_DRV_OP_Write (t_TC3X_DEMODULE * pTC317X, uint8_t *data, int32_t iSize);
void      TC317X_DRV_BootTable_Write (t_TC3X_DEMODULE * pTC317X, uint32_t addr, uint8_t *data, int32_t iSize);
int32_t   TC317X_INT_Mode (t_TC3X_DEMODULE * pTC317X, int32_t value);
int32_t   TC317X_INT_Enable (t_TC3X_DEMODULE * pTC317X, uint32_t value);
int32_t   TC317X_INT_Disable (t_TC3X_DEMODULE * pTC317X);
int32_t   TC317X_Stream_Cfg(int32_t moduleidx, int32_t idx, int32_t threshold);
int32_t   TC317X_Stream_Reset(int32_t moduleidx, int32_t idx);
//----------------------------
// user function

void      UF_RestartStream (int32_t moduleidx);
void      UF_PauseStream (int32_t moduleidx);
void      UF_GetBootCodeVersion (int32_t moduleidx, uint32_t *pVersion);
int32_t   UF_WarmBoot (int32_t moduleidx, TC3X_DNUM hBB);
uint32_t  Uf_RegRead (int32_t moduleidx, TC3X_DNUM hBB, int32_t Addr);
void      Uf_RegWrite (int32_t moduleidx, TC3X_DNUM hBB, int32_t Addr, int32_t Data);
void      Uf_MailBoxRead (int32_t moduleidx, TC3X_DNUM hBB, int32_t cmd, void *pmailbox);
void      Uf_MailBoxWrite (int32_t moduleidx, TC3X_DNUM hBB, int32_t cmd, uint32_t *DataArray, int32_t wordcnt);
uint32_t  Uf_GetChipID (int32_t moduleidx);
uint32_t  Uf_GetProgramID (int32_t moduleidx);
int32_t   UF_CheckOP (int32_t moduleidx);

int32_t   Uf_REG_TDMB_Service (int32_t moduleidx, TC3X_DNUM hBB, TDMBService_t * pTDMBService);
int32_t   Uf_UnREG_TDMB_Service (int32_t moduleidx, TC3X_DNUM hBB, TDMBService_t * pTDMBService);
void      Uf_GetTDMB_LOCK (int32_t moduleidx, TC3X_DNUM hBB, t_TDMBLock * pLock);
void      Uf_GetTDMB_SNR_SET (int32_t moduleidx, TC3X_DNUM hBB, uint32_t *param1, uint32_t *param2, uint32_t *param3);
void      Uf_GetTDMB_PCBER_SET (int32_t moduleidx, TC3X_DNUM hBB, uint32_t *param1, uint32_t *param2);
void      Uf_GetTDMB_VITERBIBER_SET (int32_t moduleidx, TC3X_DNUM hBB, uint32_t *param1, uint32_t *param2);
void      Uf_GetTDMB_TSPER_SET (int32_t moduleidx, TC3X_DNUM hBB, uint32_t *param1, uint32_t *param2);
void      Uf_GetTDMB_RSSI_SET (int32_t moduleidx, TC3X_DNUM hBB, uint32_t *param1, uint32_t *param2);
int32_t   Uf_PushTDMBStatData (int32_t moduleidx, TC3X_DNUM hBB, uint8_t *pData, uint32_t Size);
void      Uf_PushTDMB_RawData (int32_t moduleidx, TC3X_DNUM hBB, uint8_t *pData, uint32_t Size);
void      Uf_PushTDMB_ResetMonitoringValue (int32_t moduleidx, TC3X_DNUM hBB);

void      Uf_SetMEMWrite (int32_t moduleidx, uint8_t *pData, uint32_t Addr, uint32_t Size);
void      Uf_GetMEMRead (int32_t moduleidx, uint8_t *pData, uint32_t Addr, uint32_t Size);
void      Uf_SetPLLSet (int32_t moduleidx, uint8_t PLL_WaitTime, uint8_t PLL_P, uint8_t PLL_M, uint8_t PLL_S, uint32_t OSC_Khz);

void      Uf_SetFifoSize(int32_t moduleidx, TC3X_DNUM hBB, int32_t size);
extern t_TC3X_DEMODULE **g_pTC3X;
extern t_StreamSet g_tStreamSet[MAX_TCC3X];
#endif
