//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_TDMB.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/05/07   stkim              Draft
//=============================================================================
#ifndef __TC3X_CORE_TDMB_H__
#define __TC3X_CORE_TDMB_H__

#include "TC317X_struc.h"

#define MAX_TDMBMSC_NUM     (6)

#define TDMB_MAX_MOV_AVG     32

#define _SIZE_STATHEADER_   0
#define _SIZE_STATDATA_     32
#define _SIZE_FICHEADER_    4
#define _SIZE_STATCRC_   4

typedef struct
{
  uint8_t EPHeaderData[4];
  uint8_t OPHeaderData[4];

  uint8_t FICHeader[_SIZE_STATHEADER_ + _SIZE_STATDATA_ + _SIZE_FICHEADER_];
  uint32_t FICHeaderIdx;
  uint32_t FICHeaderSize;

  uint32_t EP_Sync;
  uint32_t BuffIdx;
  uint32_t EPReserved;
  uint32_t EPDataSize;

  uint32_t OP_Sync;
  uint32_t SubchID;
  uint32_t OPDataSize;
} TDMBEPOPHeader_t;

typedef struct
{
  uint16_t CFO;
  uint16_t FFO;
  uint32_t SFE;
  uint32_t PRS_SNR;
  uint32_t PCBER;
#if defined(USE_ANDROID)
  int64_t RS_ERR_CNT;
#elif defined(USE_WINCE)
  unsigned __int64 RS_ERR_CNT;
#endif
  uint32_t RS_OVER_CNT;
  uint32_t RS_PKT_CNT;

  uint16_t Reserved0;
  uint8_t RF_LOOP_GAIN;
  uint8_t BB_LOOP_GAIN;

  uint8_t fig5_2_flag;
  uint8_t fig0_19_flag;
  uint8_t fig0_0_flag;
  uint32_t sync_status;

#if defined(USE_ANDROID)
  int64_t OLD_RS_ERR_CNT;
#elif defined(USE_WINCE)
  unsigned __int64 OLD_RS_ERR_CNT;
#endif
  uint32_t OLD_RS_OVER_CNT;
  uint32_t OLD_RS_PKT_CNT;

  uint32_t PktResynced;
} TDMB_StatusData;

typedef struct
{
  uint32_t SNR_;
  uint32_t SNRdb;
  uint32_t Moving_AVG;
  uint32_t  Array[TDMB_MAX_MOV_AVG];
  uint32_t cnt;
} tTDMB_SNR;

typedef struct
{
  uint32_t PCBER;
  uint32_t Moving_AVG;
  uint32_t Array[TDMB_MAX_MOV_AVG];
  uint32_t cnt;
} tTDMB_PCBER;

typedef struct
{
  uint32_t VITERBIBER;
  uint32_t Moving_AVG;
  uint32_t Array[TDMB_MAX_MOV_AVG];
  uint32_t cnt;
  uint32_t Old_VITERBIBER;
} tTDMB_VITERBIBER;

typedef struct
{
  uint32_t TSPER;
  uint32_t Moving_AVG;
  uint32_t Array[TDMB_MAX_MOV_AVG];
  uint32_t cnt;
  uint32_t Old_TSPER;
} tTDMB_TSPER;

typedef struct
{
  uint32_t RSSI;
  uint32_t Moving_AVG;
  uint32_t Array[TDMB_MAX_MOV_AVG];
  uint32_t cnt;
} tTDMB_RSSI;

typedef struct
{
  tTDMB_SNR SNRset;
  tTDMB_PCBER PCBERset;
  tTDMB_VITERBIBER VITERBIBERset;
  tTDMB_TSPER TSPERset;
  tTDMB_RSSI RSSIset;
} TDMB_MonitorStatus;

typedef struct
{
  uint32_t OPHeaderON;
  uint32_t EPHeaderON;

  int32_t       ParserState;
  int32_t       EPsynccnt;
  int32_t       OPsynccnt;
  int32_t       Datasize;
  int32_t       CurrDataSize;

  TDMBEPOPHeader_t TDMBEPOPHeader;

  uint32_t ServiceNum;                               // except FIC
  TDMBService_t TDMBService[MAX_TDMBMSC_NUM];
  uint32_t ProtocolData[MAX_TDMBMSC_NUM * 2];
  uint8_t bOnAir[MAX_TDMBMSC_NUM];

  int32_t DMBChannelID[MAX_TDMBMSC_NUM];
  int32_t MCICount;

  TDMB_StatusData TDMBStatData;
  TDMB_MonitorStatus MonStat;

  uint16_t Flag_ChannelReconfig;
  uint16_t Flag_EWS_Announce;
} TDMBCtrl_t;

typedef struct _BUFF
{
  uint8_t *p;
  uint32_t rptr;
  uint32_t wptr;
  uint32_t size;
} BUFF;

// Structure for DAB only use
typedef enum
{
  ENUM_FIND_EP_SYNC = 0,
  ENUM_FIND_OP_SYNC,
  ENUM_OP_SYNC_GET,
  ENUM_EP_SYNC_GET,
  ENUM_OP_FIC_GET,
  ENUM_OP_MSC_GET,
  ENUM_DATA_GET_S
} enum_DABParsingStat;


void      Init_TDMB_Process (t_TC3X_DEMODULE * pTC317X);
void      TC317X_TDMBStreamPause (int32_t moduleidx);
void      TC317X_TDMBStreamRestart (int32_t moduleidx);
void      Stop_TDMB_Process (t_TC3X_DEMODULE * pTC317X);
void      TDMB_Alloc (int32_t moduleidx, uint32_t size);
void      TDMB_DeAlloc (int32_t moduleidx);
void      TC317X_InitTDMBService (int32_t moduleidx);
int32_t   GetFIFOThr_TDMB (int32_t moduleidx);
void      GetTDMBStream (int32_t moduleidx, uint32_t interrupt_stat);
int32_t   Parse_TDMB_OPHEADER (int32_t moduleidx, uint32_t bufferidx, uint8_t *p, TDMBEPOPHeader_t * pEPOPHeader);
void      TDMBStreamControlSub (int32_t moduleidx, uint8_t *p, uint32_t size);
void      TDMBStreamControl (int32_t moduleidx, uint8_t *p, uint32_t size, int32_t BuffIdx);
void      SetTDMBMSCFormat (t_TC3X_DEMODULE * pTC317X, uint32_t ch_cfg);
void      SetTDMBStreamOn (t_TC3X_DEMODULE * pTC317X, int32_t FIC_ON, int32_t DAB_ON, int32_t DMB_ON);
int32_t   TC317X_TDMB_RegService (int32_t moduleidx, TDMBService_t * pTDMBService);
int32_t   TC317X_TDMB_UnRegService (int32_t moduleidx, TDMBService_t * pTDMBService, int32_t idx);
int32_t   TC317X_SET_TDMB_Service (int32_t moduleidx, uint32_t RegFlag, TDMBService_t * pTDMBService);
void      TDMB_SPI_Handler (int32_t moduleidx, int32_t StreamBuffIdx, uint8_t *pStream, int32_t PktSize, int32_t PktThrNum);
void      TDMB_Stream_Handler (int32_t moduleidx, uint8_t *pStream, uint32_t Size, uint32_t Type, uint32_t DABSubChId, uint32_t Idx);
int32_t   TC317X_TDMBStreamRead (int32_t moduleidx, uint32_t Size);
void      Get_TDMB_LockStatus (int32_t moduleidx, t_TDMBLock * pLockStat);
void      Get_TDMB_SNR (int32_t moduleidx, uint32_t *pSNR, uint32_t *pSNRdb, uint32_t *pMovingAVGSNRdb);
void      Get_TDMB_PCBER (int32_t moduleidx, uint32_t *pPCBER, uint32_t *pMovingAVGPCBER);
void      Get_TDMB_VITERBIBER (int32_t moduleidx, uint32_t *pVITERBIBER, uint32_t *pMovingAVGVITERBIBER);
void      Get_TDMB_TSPER (int32_t moduleidx, uint32_t *pTSPER, uint32_t *pMovingAVGTSPER);
void      Get_TDMB_RSSI (int32_t moduleidx, uint32_t *pRSSI, uint32_t *pMovingAVGRSSI);
void      TDMB_Reset_MonitorValue(int32_t moduleidx);
int32_t   ParseNUpdateTDMBStatus (int32_t moduleidx, uint8_t *p, uint32_t Size);
void      TC3X_TDMB_ReRegService (t_TC3X_DEMODULE * pTC317X);
int32_t   GetTDMBServMailIdx (int32_t moduleidx, int32_t SubChID);
extern uint8_t **gp_TDMB_Stream;

#endif
