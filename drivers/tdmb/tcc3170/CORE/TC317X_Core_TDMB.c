//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_TDMB.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/05/07   stkim              Draft
//=============================================================================
#include "../IO/TC317X_IO.h"
#include "TC317X_deviceAPI.h"
#include "../IO/TC317X_IO_UTIL.h"
#include "TC317X_Core.h"
#include "TC317X_Core_Mailbox.h"
#include "TC317X_Core_TDMB.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_Util.h"
#include "TC317X_Core_Interrupt.h"

#if defined(DEBUG_DUMP_FILE)
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/file.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#endif

#define _TDMB_MIN_SNR_  0
#define _TDMB_MAX_SNR_  25

#define _TDMB_MIN_PCBER_  0
#define _TDMB_MAX_PCBER_  1

#define _TDMB_MIN_VITERBIBER_  0
#define _TDMB_MAX_VITERBIBER_  1

#define _TDMB_MIN_TSPER_  0
#define _TDMB_MAX_TSPER_  1

#define _TDMB_MIN_RSSI_  (-105)
#define _TDMB_MAX_RSSI_  3

#ifdef STREAM_MIXER
int32_t ksj_cnt = 0;
#define dmb_module_num  2
#else
#define dmb_module_num  1
#endif

static uint32_t post_read_size = 0;
static uint32_t dmb_chip_index = 0;
static uint32_t garbage_size = 0;
static BUFF dual_dmb_buffer[dmb_module_num];
static uint32_t post_subch_id = -1;
static uint32_t post_read_type = -1;

#ifdef TC317X_SPLIT_MSC_BUFFER
static BUFF msc_buffer[dmb_module_num];
#endif

extern void TC317X_IO_HPI_ReadStream(int32_t moduleidx, uint8_t *data, int32_t iSize);

// -------------------------------------------------------
// Gloval variables
//

TDMBCtrl_t gtTDMBCtrl[MAX_TCC3X];
uint8_t **gp_TDMB_Stream = NULL;     // for DAB : approx 16k
uint8_t **gp_MSCFICData = NULL;      // for DAB : Header Parsing & Send Data
extern uint32_t tc317x_cspi_read_size;

uint32_t data_buff_check (BUFF * Q)
{
  uint32_t size;

  if (Q->rptr > Q->wptr)
    size = Q->size - Q->rptr + Q->wptr;
  else
    size = Q->wptr - Q->rptr;

  return size;
}

uint32_t buffer_space_check (BUFF * Q)
{
  uint32_t size;

  if (Q->rptr > Q->wptr)
    size = Q->rptr - Q->wptr;
  else
    size = Q->size - Q->wptr + Q->rptr;

  return size;
}

void stream_upload (uint32_t size, BUFF * q, uint32_t type, int32_t moduleidx, int32_t subchid)
{
  uint32_t r_size;
  int32_t ServMailIdx;

  if (q->wptr > q->rptr)
  {
    r_size = q->wptr - q->rptr;
    if (r_size < size)
    {
      PRINTF_LV_0 ("size underflow\n");
    }
    TC317X_IO_memcpy (&gp_MSCFICData[moduleidx][0], &q->p[q->rptr], size);
  }
  else
  {
    r_size = q->size - q->rptr;
    if (r_size >= size)
    {
      TC317X_IO_memcpy (&gp_MSCFICData[moduleidx][0], &q->p[q->rptr], size);
    }
    else
    {
      TC317X_IO_memcpy (&gp_MSCFICData[moduleidx][0], &q->p[q->rptr], r_size);
      TC317X_IO_memcpy (&gp_MSCFICData[moduleidx][r_size], &q->p[0], size - r_size);
    }
  }

  if( type == SRVTYPE_DMB)
  {
    ServMailIdx = GetTDMBServMailIdx (moduleidx, subchid);
    if(ServMailIdx>=0)
    {
      TDMB_Stream_Handler (moduleidx, &gp_MSCFICData[moduleidx][0], 
        size, type, subchid, ServMailIdx);
    }
    else
    {
      TDMB_Stream_Handler (moduleidx, &gp_MSCFICData[moduleidx][0], 
        size, type, 0, 0);
    }
  }
  else
  {
    TDMB_Stream_Handler (moduleidx, &gp_MSCFICData[moduleidx][0], 
      size, type, subchid, 0);
  }
}

void buffer_reset(void)
{
  int i;
  for (i = 0; i < dmb_module_num; i++)
  {
    dual_dmb_buffer[i].rptr = 0;
    dual_dmb_buffer[i].wptr = 0;
#ifdef TC317X_SPLIT_MSC_BUFFER
    msc_buffer[i].rptr = 0;
    msc_buffer[i].wptr = 0;
#endif
  }
}

uint32_t buffer_write (uint32_t size, BUFF * q, uint8_t *in)
{
  uint32_t r_size;

  TCASSERT(q->size < size);
  if(q->size < size) return 0;

  r_size = buffer_space_check(q);
  if (r_size < size)
  {
    PRINTF_LV_0 ("buffer over rsize:%d, in size:%d, total:%d\n", 
        r_size, size, q->size);
  }

  r_size = q->size - q->wptr;
  if (r_size >= size)
  {
    TC317X_IO_memcpy (&q->p[q->wptr], in, size);
  }
  else
  {
    TC317X_IO_memcpy (&q->p[q->wptr], in, r_size);
    TC317X_IO_memcpy (&q->p[0], in + r_size, size - r_size);
  }

  q->wptr += size;
  q->wptr %= q->size;

  return size;
}

void Init_TDMB_Process (t_TC3X_DEMODULE * pTC317X)
{
  TDMB_Alloc (pTC317X->moduleidx, CACHED_BUFFER_SIZE_MSC);
  TC317X_InitTDMBService (pTC317X->moduleidx);
  SetTDMBMSCFormat (pTC317X, CHANNEL_CFG_FIC);
  TC317X_Stream_Cfg(pTC317X->moduleidx, 0, TDMB_CSPI_READ_SIZE_FIC);
}

void Stop_TDMB_Process (t_TC3X_DEMODULE * pTC317X)
{
  //stream stop
  TC317X_InitTDMBService (pTC317X->moduleidx);
  TDMB_DeAlloc (pTC317X->moduleidx);
}

void TDMB_Alloc (int32_t moduleidx, uint32_t size)
{
  int32_t       i;
  int32_t       maxbb;

  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (!gp_TDMB_Stream)
  {
    gp_TDMB_Stream = (uint8_t **) TC317X_IO_malloc (maxbb);
    TC317X_IO_memset (&gp_TDMB_Stream[0], 0, maxbb * sizeof (uint8_t *));
  }

  if (!gp_TDMB_Stream[moduleidx])
  {
    gp_TDMB_Stream[moduleidx] = (uint8_t *) TC317X_IO_malloc (size);
  }

  if (!gp_MSCFICData)
  {
    gp_MSCFICData = (uint8_t **) TC317X_IO_malloc (maxbb);
    TC317X_IO_memset (&gp_MSCFICData[0], 0, maxbb * sizeof (uint8_t *));
  }

  if (!gp_MSCFICData[moduleidx])
  {
    gp_MSCFICData[moduleidx] = (uint8_t *) TC317X_IO_malloc (BUFFER_SIZE_MSC_FIC);
    for (i = 0; i < dmb_module_num; i++)
    {
      dual_dmb_buffer[i].p = (uint8_t *) TC317X_IO_malloc (CACHED_BUFFER_SIZE_DUAL);
      dual_dmb_buffer[i].rptr = 0;
      dual_dmb_buffer[i].wptr = 0;
      dual_dmb_buffer[i].size = CACHED_BUFFER_SIZE_DUAL;
#ifdef TC317X_SPLIT_MSC_BUFFER
      msc_buffer[i].p = (uint8_t *) TC317X_IO_malloc (CACHED_BUFFER_SIZE_MSC);
      msc_buffer[i].rptr = 0;
      msc_buffer[i].wptr = 0;
      msc_buffer[i].size = CACHED_BUFFER_SIZE_MSC;
#endif
    }

    post_read_size = 0;
    dmb_chip_index = 0;
  }

  //PRINTF_LV_1 ("# Mem Alloc\n");
}

void TDMB_DeAlloc (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;

  if (!gp_TDMB_Stream)
    return;

  if (gp_TDMB_Stream[moduleidx])
  {
    TC317X_IO_free (&gp_TDMB_Stream[moduleidx][0]);
  }
  gp_TDMB_Stream[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (gp_TDMB_Stream[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (gp_TDMB_Stream);
    gp_TDMB_Stream = NULL;
  }

  // MSC
  if (!gp_MSCFICData)
    return;

  if (gp_MSCFICData[moduleidx])
  {
    TC317X_IO_free (&gp_MSCFICData[moduleidx][0]);
  }
  gp_MSCFICData[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (gp_MSCFICData[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (gp_MSCFICData);
    gp_MSCFICData = NULL;
    {
      for (i = 0; i < dmb_module_num; i++)
      {
        TC317X_IO_free (dual_dmb_buffer[i].p);
        dual_dmb_buffer[i].p = NULL;
#ifdef TC317X_SPLIT_MSC_BUFFER
        TC317X_IO_free (msc_buffer[i].p);
        msc_buffer[i].p = NULL;
#endif
      }
    }
  }

  //    PRINTF_LV_0 ("# Mem dealloc\n");
}

void Init_TDMB_Status (int32_t moduleidx)
{
  TC317X_IO_memset (&gtTDMBCtrl[moduleidx].TDMBStatData, 0, sizeof (TDMB_StatusData));
  TC317X_IO_memset (&gtTDMBCtrl[moduleidx].MonStat, 0, sizeof (TDMB_MonitorStatus));
  TC317X_IO_memset (&gtTDMBCtrl[moduleidx].TDMBStatData, 0, sizeof (TDMB_StatusData));
}

void TC317X_InitTDMBService (int32_t moduleidx)
{
  TC317X_IO_memset (&gtTDMBCtrl[moduleidx], 0, sizeof (TDMBCtrl_t));
  Init_TDMB_Status (moduleidx);
}

void TC317X_TDMBStreamPause (int32_t moduleidx)
{
  switch (g_pTC3X[moduleidx][0].SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_PTS:
    case TC3X_STREAM_IO_STS:
    case TC3X_STREAM_IO_SPIMS:
    case TC3X_STREAM_IO_SPISLV:
      TC317X_IO_Stream_SPI_Ctrl (
          moduleidx, CMD_SPI_STOP, &g_tStreamSet[moduleidx].pStream,
          g_tStreamSet[moduleidx].PktSize, g_tStreamSet[moduleidx].PktThrNum,
          g_pTC3X[moduleidx][0].SubIF.SubIFDefine);
      break;
  }

  SetTDMBStreamOn (&g_pTC3X[moduleidx][0], 0, 0, 0);
}

void TC317X_TDMBStreamRestart (int32_t moduleidx)
{
  switch (g_pTC3X[moduleidx][0].SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_PTS:
    case TC3X_STREAM_IO_STS:
    case TC3X_STREAM_IO_SPIMS:
    case TC3X_STREAM_IO_SPISLV:
      TC317X_IO_Stream_SPI_Ctrl (
          moduleidx, CMD_SPI_START, &g_tStreamSet[moduleidx].pStream,
          g_tStreamSet[moduleidx].PktSize, g_tStreamSet[moduleidx].PktThrNum,
          g_pTC3X[moduleidx][0].SubIF.SubIFDefine);
      InitSPICtrlSet (moduleidx);
      break;
  }

  gtTDMBCtrl[moduleidx].ParserState = ENUM_FIND_EP_SYNC;
  SetTDMBStreamOn (&g_pTC3X[moduleidx][0], 1, 1, 1);
}

uint32_t Cal_TDMB_SNR (TDMB_StatusData * pDMBStatData, uint32_t *pSNR_)
{
  uint32_t SNR;
  uint32_t PRS_SNR_mod_2_10;

  PRS_SNR_mod_2_10 = pDMBStatData->PRS_SNR;

  if (PRS_SNR_mod_2_10 < 0)
  {
    SNR = _TDMB_MAX_SNR_;
    pSNR_[0] = PRS_SNR_mod_2_10>>10;
  }
  else
  {
    uint32_t logi;
    logi = PRS_SNR_mod_2_10;
    pSNR_[0] = PRS_SNR_mod_2_10>>10;

    // org cal.
    // SNR = 10 * TC3X_log10_ (prs_snr/(2^10)); => SNR = 10 * TC3X_log10_ (PRS_SNR_mod_2_10));
    // can change as..
    //SNR = 10 * TC3X_log10_ (PRS_SNR_mod_2_10*10^3) - TC3X_log10_(10^3);

    if (logi == 0)
    {
      SNR = _TDMB_MIN_SNR_;
    }
    else
    {
      SNR = TC317X_log10_ (logi);      // TC3X_log10_ return *10
    }
  }

  if (SNR < _TDMB_MIN_SNR_)
    SNR = _TDMB_MIN_SNR_;
  else if (SNR > _TDMB_MAX_SNR_)
    SNR = _TDMB_MAX_SNR_;

  return SNR;
}

#define SCALE_FACTOR 1000000
uint32_t Cal_TDMB_PCBER (TDMB_StatusData * pDMBStatData)
{
  uint32_t PCBER;
  uint64_t over;

  over = (uint64_t)pDMBStatData->PCBER*SCALE_FACTOR;
  PCBER = (uint32_t)(over>>16);

  if (PCBER < _TDMB_MIN_PCBER_)
    PCBER = _TDMB_MIN_PCBER_;
  else if (PCBER > _TDMB_MAX_PCBER_*SCALE_FACTOR)
    PCBER = _TDMB_MAX_PCBER_*SCALE_FACTOR;

  return PCBER;
}

uint32_t Cal_TDMB_VITERBIBER (TDMB_StatusData * pDMBStatData, uint32_t OldVITERBIBER)
{
  uint32_t VITERBIBER;
  uint32_t over;
  uint32_t under;
  uint32_t temp;

  if (pDMBStatData->PktResynced)
  {
    VITERBIBER = _TDMB_MAX_VITERBIBER_*SCALE_FACTOR;
    return VITERBIBER;
  }

  if (!(pDMBStatData->PktResynced) && pDMBStatData->RS_PKT_CNT == 0)
  {
    VITERBIBER = _TDMB_MIN_VITERBIBER_;
    return VITERBIBER;
  }

  if ( !pDMBStatData->PktResynced &&
      (pDMBStatData->RS_PKT_CNT == pDMBStatData->OLD_RS_PKT_CNT) )
  {
    VITERBIBER = OldVITERBIBER;
    return VITERBIBER;
  }

  over = pDMBStatData->RS_ERR_CNT - pDMBStatData->OLD_RS_ERR_CNT;
  temp = pDMBStatData->RS_PKT_CNT - pDMBStatData->OLD_RS_PKT_CNT;
  under = temp * 204 * 8;

  over *= SCALE_FACTOR;

  VITERBIBER = (over / under);

  if (VITERBIBER < _TDMB_MIN_VITERBIBER_)
    VITERBIBER = _TDMB_MIN_VITERBIBER_;
  else if (VITERBIBER > _TDMB_MAX_VITERBIBER_*SCALE_FACTOR)
    VITERBIBER = _TDMB_MAX_VITERBIBER_*SCALE_FACTOR;

  return VITERBIBER;
}

uint32_t Cal_TDMB_TSPER (TDMB_StatusData * pDMBStatData, uint32_t OldTSPER)
{
  uint32_t TSPER;
  uint32_t over;
  uint32_t under;

  if (pDMBStatData->PktResynced)
  {
    TSPER = _TDMB_MAX_TSPER_*SCALE_FACTOR;
    return TSPER;
  }

  if (!(pDMBStatData->PktResynced) && pDMBStatData->RS_PKT_CNT == 0)
  {
    TSPER = _TDMB_MIN_TSPER_;
    return TSPER;
  }

  if ( !(pDMBStatData->PktResynced) &&
      pDMBStatData->RS_PKT_CNT == pDMBStatData->OLD_RS_PKT_CNT)
  {
    TSPER = OldTSPER;
    return TSPER;
  }

  over = (pDMBStatData->RS_OVER_CNT - pDMBStatData->OLD_RS_OVER_CNT);
  under = (pDMBStatData->RS_PKT_CNT - pDMBStatData->OLD_RS_PKT_CNT);

  over *= SCALE_FACTOR;

  TSPER = (over / under);

  if (TSPER < _TDMB_MIN_TSPER_)
    TSPER = _TDMB_MIN_TSPER_;
  else if (TSPER > _TDMB_MAX_TSPER_*SCALE_FACTOR)
    TSPER = _TDMB_MAX_TSPER_*SCALE_FACTOR;

  return TSPER;
}

uint32_t Cal_TDMB_RSSI (t_TC3X_DEMODULE * pTC317X, TDMB_StatusData * pDMBStatData)
{
  int32_t RSSI;

  if (pDMBStatData->RF_LOOP_GAIN <= 120)
    RSSI = 1500 - ((int32_t)pDMBStatData->BB_LOOP_GAIN)*37 - 27*((int32_t)pDMBStatData->RF_LOOP_GAIN);
  else if (pDMBStatData->RF_LOOP_GAIN <= 216)
    RSSI = 1500 - ((int32_t)pDMBStatData->BB_LOOP_GAIN)*35 - 24*((int32_t)pDMBStatData->RF_LOOP_GAIN);
  else
    RSSI = 1500 - ((int32_t)pDMBStatData->BB_LOOP_GAIN)*33 - 22*((int32_t)pDMBStatData->RF_LOOP_GAIN);

  RSSI /= 100;
  if (RSSI < _TDMB_MIN_RSSI_)
    RSSI = _TDMB_MIN_RSSI_;
  else if (RSSI > _TDMB_MAX_RSSI_)
    RSSI = _TDMB_MAX_RSSI_;

  return RSSI;
}

void Update_TDMB_Status (int32_t moduleidx, TDMB_StatusData * pDMBStatData)
{
  pDMBStatData->OLD_RS_PKT_CNT =
    gtTDMBCtrl[moduleidx].TDMBStatData.OLD_RS_PKT_CNT;
  pDMBStatData->OLD_RS_ERR_CNT =
    gtTDMBCtrl[moduleidx].TDMBStatData.OLD_RS_ERR_CNT;
  pDMBStatData->OLD_RS_OVER_CNT =
    gtTDMBCtrl[moduleidx].TDMBStatData.OLD_RS_OVER_CNT;
  pDMBStatData->PktResynced = gtTDMBCtrl[moduleidx].TDMBStatData.PktResynced;

  if (pDMBStatData->RS_PKT_CNT == pDMBStatData->OLD_RS_PKT_CNT) {
    // same as previouse
  } else {
    pDMBStatData->PktResynced = 0;
    if (pDMBStatData->RS_PKT_CNT == 0) {
      if (pDMBStatData->OLD_RS_PKT_CNT) {
        pDMBStatData->PktResynced = 1;
      }
    } else if (pDMBStatData->RS_PKT_CNT != 0 &&
        pDMBStatData->RS_PKT_CNT < pDMBStatData->OLD_RS_PKT_CNT) {
      if (pDMBStatData->OLD_RS_PKT_CNT < 0x80000000) {
        // auto resync situation
        pDMBStatData->PktResynced = 1;
      } else {
        // rotate
        pDMBStatData->OLD_RS_PKT_CNT = 0;
        pDMBStatData->OLD_RS_ERR_CNT = 0;
        pDMBStatData->OLD_RS_OVER_CNT = 0;
      }
    }
  }

  // Update SNR Set
  if (gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt < TDMB_MAX_MOV_AVG)
    gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt++;

  gtTDMBCtrl[moduleidx].MonStat.SNRset.SNRdb =
    Cal_TDMB_SNR (pDMBStatData, &gtTDMBCtrl[moduleidx].MonStat.SNRset.SNR_);
  gtTDMBCtrl[moduleidx].MonStat.SNRset.Moving_AVG =
    TC317X_moving_avg_d (
        gtTDMBCtrl[moduleidx].MonStat.SNRset.Array,
        gtTDMBCtrl[moduleidx].MonStat.SNRset.SNRdb,
        gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt,
        TDMB_MAX_MOV_AVG);

  // Update PCBER Set
  if (gtTDMBCtrl[moduleidx].MonStat.PCBERset.cnt < TDMB_MAX_MOV_AVG)
    gtTDMBCtrl[moduleidx].MonStat.PCBERset.cnt++;

  gtTDMBCtrl[moduleidx].MonStat.PCBERset.PCBER = Cal_TDMB_PCBER (pDMBStatData);
  gtTDMBCtrl[moduleidx].MonStat.PCBERset.Moving_AVG =
    TC317X_moving_avg_d (
        gtTDMBCtrl[moduleidx].MonStat.PCBERset.Array,
        gtTDMBCtrl[moduleidx].MonStat.PCBERset.PCBER,
        gtTDMBCtrl[moduleidx].MonStat.PCBERset.cnt,
        TDMB_MAX_MOV_AVG);

  // Update VITERBIBER Set
  if (gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.cnt < TDMB_MAX_MOV_AVG)
    gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.cnt++;

  gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.VITERBIBER =
    Cal_TDMB_VITERBIBER ( pDMBStatData,
        gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Old_VITERBIBER);
  gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Moving_AVG =
    TC317X_moving_avg_d (
        gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Array,
        gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.VITERBIBER,
        gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.cnt,
        TDMB_MAX_MOV_AVG);

  // Update TSPER Set
  if (gtTDMBCtrl[moduleidx].MonStat.TSPERset.cnt < TDMB_MAX_MOV_AVG)
    gtTDMBCtrl[moduleidx].MonStat.TSPERset.cnt++;

  gtTDMBCtrl[moduleidx].MonStat.TSPERset.TSPER =
    Cal_TDMB_TSPER (pDMBStatData, gtTDMBCtrl[moduleidx].MonStat.TSPERset.Old_TSPER);
  gtTDMBCtrl[moduleidx].MonStat.TSPERset.Moving_AVG =
    TC317X_moving_avg_d (
        gtTDMBCtrl[moduleidx].MonStat.TSPERset.Array,
        gtTDMBCtrl[moduleidx].MonStat.TSPERset.TSPER,
        gtTDMBCtrl[moduleidx].MonStat.TSPERset.cnt,
        TDMB_MAX_MOV_AVG);

  // Update RSSI Set
  if (gtTDMBCtrl[moduleidx].MonStat.RSSIset.cnt < TDMB_MAX_MOV_AVG)
    gtTDMBCtrl[moduleidx].MonStat.RSSIset.cnt++;

  gtTDMBCtrl[moduleidx].MonStat.RSSIset.RSSI =
    Cal_TDMB_RSSI (&g_pTC3X[moduleidx][0], pDMBStatData);
  gtTDMBCtrl[moduleidx].MonStat.RSSIset.Moving_AVG =
    TC317X_moving_avg_d (
        gtTDMBCtrl[moduleidx].MonStat.RSSIset.Array,
        gtTDMBCtrl[moduleidx].MonStat.RSSIset.RSSI,
        gtTDMBCtrl[moduleidx].MonStat.RSSIset.cnt,
        TDMB_MAX_MOV_AVG);

  pDMBStatData->OLD_RS_PKT_CNT = pDMBStatData->RS_PKT_CNT;
  pDMBStatData->OLD_RS_ERR_CNT = pDMBStatData->RS_ERR_CNT;
  pDMBStatData->OLD_RS_OVER_CNT = pDMBStatData->RS_OVER_CNT;

  TC317X_IO_memcpy (&gtTDMBCtrl[moduleidx].TDMBStatData,
      pDMBStatData, sizeof(TDMB_StatusData));
}

uint32_t getmaskx (uint32_t input)
{
  uint32_t valx[] = { 
    0x00, 
    0x01, 
    0x03, 
    0x07, 
    0x0f,
    0x1f, 
    0x3f, 
    0x7f, 
    0xff, 
    0x1ff,
    0x3ff, 
    0x7ff, 
    0xfff, 
    0x1fff, 
    0x3fff,
    0x7fff, 
    0xffff
  };

  if (input > 16)
    return 0;

  return (valx[input]);
}

int32_t ParseNUpdateTDMBStatus (
    int32_t moduleidx, uint8_t *stat, uint32_t Size)
{
#define OFFSET_RF_LOOP_GAIN   4
#define OFFSET_BB_LOOP_GAIN   5
#define OFFSET_PRS_SNR        8
#define OFFSET_PCBER         12
#define OFFSET_RS_PKT_CNT    16
#define OFFSET_RS_OVER_CNT   20
#define OFFSET_RS_ERR_CNT_LO 24
#define OFFSET_RS_ERR_CNT_HI 28
  //uint8_t StatusData[_SIZE_STATDATA_];
  TDMB_StatusData tmpDMBStatData;

  if (Size < _SIZE_STATDATA_)
  {
    PRINTF_LV_0 ("# [E]InputSizeErr[%d]\n", Size);
    return -1;
  }

  tmpDMBStatData.sync_status = *((uint32_t*)stat);
  tmpDMBStatData.RF_LOOP_GAIN = stat[OFFSET_RF_LOOP_GAIN];
  tmpDMBStatData.BB_LOOP_GAIN = stat[OFFSET_BB_LOOP_GAIN];

  tmpDMBStatData.PRS_SNR     = *((uint32_t*)(stat+OFFSET_PRS_SNR));
  tmpDMBStatData.PCBER       = *((uint32_t*)(stat+OFFSET_PCBER));
  tmpDMBStatData.RS_OVER_CNT = *((uint32_t*)(stat+OFFSET_RS_OVER_CNT));
  tmpDMBStatData.RS_PKT_CNT  = *((uint32_t*)(stat+OFFSET_RS_PKT_CNT));
  tmpDMBStatData.RS_ERR_CNT  = *((uint64_t*)(stat+OFFSET_RS_ERR_CNT_LO));
  tmpDMBStatData.RS_ERR_CNT |= (*((uint64_t*)(stat+OFFSET_RS_ERR_CNT_HI))) <<32;


  Update_TDMB_Status (moduleidx, &tmpDMBStatData);

  return 0;
}

int32_t Parse_TDMB_OPHEADER (int32_t moduleidx, uint32_t bufferidx, uint8_t *p, TDMBEPOPHeader_t * pEPOPHeader)
{
  // receive 1 frame at once. (dab & fic use circular buffer)
  /* DAB
   * [31:24] 0x5C
   * [22:16] SUBCH ID
   * [15:0] data size (word ??')
   */

  /* FIC
   * Status Data Header 1word
   * Status Data 9word
   *
   * FIC Header 1word
   * FIC Data
   *
   * Return FIC Data idx
   */

  int32_t       SubChID;

  if (bufferidx == 3) // dab
  {
    uint8_t tempb[4];

    TC317X_IO_memcpy (&tempb[0], p, 4);

    if (tempb[0] != 0x5C)
      return (0);

    SubChID = (tempb[1] & 0x3F);

    pEPOPHeader->OP_Sync = 0x5C;
    pEPOPHeader->SubchID = SubChID;
    pEPOPHeader->OPDataSize = ((tempb[2] << 8 | tempb[3]) << 2);

    if (gtTDMBCtrl[moduleidx].EPHeaderON)
    {
      if (pEPOPHeader->EPDataSize < pEPOPHeader->OPDataSize + 4)
      {
        PRINTF_LV_0 ("# [E]OPDataSize>EpheaderDatasize\n");
        return 0;
      }
    }

    if (pEPOPHeader->OPDataSize >= 1024 * 4)
    {
      PRINTF_LV_0 ("# [E]OPDataSizeOver4k\n");
      return 0;
    }
  }
  else if (bufferidx == 2)    // fic
  {
    uint8_t FICHeader[_SIZE_FICHEADER_];
    uint8_t Temp4Header[4];
    int32_t       FICSize;
    int32_t       UsedByte;

    TC317X_IO_memcpy (&Temp4Header[0], &p[0], 4);

    if (Temp4Header[0] == 0x5d)     // Status Header
    {
      int32_t       retv;

      TC317X_IO_memcpy (&FICHeader[0], &p[_SIZE_STATHEADER_ + _SIZE_STATDATA_], 4);

      retv = ParseNUpdateTDMBStatus (moduleidx, &p[0], _SIZE_STATHEADER_ + _SIZE_STATDATA_);
      if (retv == -1)
      {
        //PRINTF_LV_2 ("# [E]ParseNUpdateTDMBStatus\n");
        return 0;
      }
      UsedByte = _SIZE_STATHEADER_ + _SIZE_STATDATA_ + _SIZE_FICHEADER_;

      gtTDMBCtrl[moduleidx].Flag_ChannelReconfig = ((p[_SIZE_STATHEADER_ + 34] >> 1) & 0x01);
      gtTDMBCtrl[moduleidx].Flag_EWS_Announce = (p[_SIZE_STATHEADER_ + 34] & 0x01);
    }
    else if (Temp4Header[0] == 0x5e)        // FIC Header
    {
      TC317X_IO_memcpy (&FICHeader[0], &p[0], 4);
      UsedByte = _SIZE_FICHEADER_;
    }
    else
    {
      PRINTF_LV_0 ("# [E]StatusDataHeader! or FICHeader\n");
      return 0;
    }

    FICSize = (FICHeader[3] | FICHeader[2] << 8) << 2;
    pEPOPHeader->OPDataSize = FICSize;

    if (gtTDMBCtrl[moduleidx].EPHeaderON)
    {
      if (pEPOPHeader->EPDataSize < pEPOPHeader->OPDataSize + UsedByte + _SIZE_STATCRC_)
      {
        PRINTF_LV_0 ("# [E]OPDataSize>EpheaderDatasize\n");
        return 0;
      }
    }

    if (pEPOPHeader->OPDataSize >= 1024 * 4)
    {
      PRINTF_LV_0 ("# [E]OPDataSizeOver4k\n");
      return 0;
    }
    else
    {
      return UsedByte;
    }
  }
  else
  {
    PRINTF_LV_0 ("# [E]It must have not OPHeader!\n");
    return 0;
  }

  return 1;
}

int32_t GetTDMBServMailIdx (int32_t moduleidx, int32_t SubChID)
{
  int32_t       i;
  int32_t       findsubch = -1;

  for (i = 0; i < MAX_TDMBMSC_NUM; i++)
  {
    if ((SubChID == gtTDMBCtrl[moduleidx].TDMBService[i].SubChID) && (gtTDMBCtrl[moduleidx].bOnAir[i]))
    {
      findsubch = i;
      break;
    }
  }

  if (findsubch >= 0)
  {
    return findsubch;
  }
  else
  {
    return -1;
  }

  return -1;
}

int32_t GetTDMBServiceType (int32_t moduleidx, int32_t idx)
{
  return gtTDMBCtrl[moduleidx].TDMBService[idx].ServType;
}

uint8_t TDMBEP_Header_check (uint8_t *in)
{
  uint32_t i, k;
  uint8_t p = 0;

  for (i = 0; i < 4; i++)
  {
    for (k = 0; k < 8; k++)
    {
      p = (p + ((in[i] >> k) & 1)) & 1;
    }
  }

  if (p == 0)
    PRINTF_LV_0 ("Odd parity error\n");
  return p;
}

inline int32_t find_header(uint8_t *p, uint32_t size)
{
  int32_t first = 0, ok = 0;
  int32_t i = 0, j = 0, ch_size = 0;

  for(i = 0; i < size; i++)
  {
    if(p[i] == 0x5C && i >= 3)
    {
      if(TDMBEP_Header_check (&p[i-3]) != 1)
      {
        PRINTF_LV_0("parity error, i=%d %04X\n", 
              i, SWAP32(*((uint32_t*)&p[i-3])) );
        continue;
      }
      else
      {
        //PRINTF_LV_0("parity ok\n");
        ok++;
      }

      first = j = i-3;
      do 
      {
        ch_size = (p[j + 1] << 7 | (p[j+0] >> 1)) << 2;
        j += (ch_size+4);

        if( (j < size) && 
            (p[j + 3] == 0x5C) && 
            (TDMBEP_Header_check (p+j) == 1)) 
        {
          //PRINTF_LV_0("header ok \n"); 
          ok++;
        }
        else
        {
          PRINTF_LV_0("Header ERR!! [%02X] j = %d, size=%d, ch_size:%d\n", 
            SWAP32(*((uint32_t*)(p+j))), j, size, ch_size);
          break;
        }
        if(ok > 2) break;
      } while(j < size);

      if(ok > 0) 
      {
        //PRINTF_LV_0("header ok count : %d\n", ok);
        return first;
      }
    }
  }

  return -i;
}

#if defined(DEBUG_DUMP_FILE)
void file_dump(uint8_t *p, uint32_t size)
{
  struct file *flip = NULL;
  char *filename = "/data/debug_dump.bin";
  mm_segment_t old_fs;

  flip = filp_open(filename, O_CREAT | O_RDWR | O_APPEND | O_LARGEFILE, 0);
  if (flip == NULL)
  {
    PRINTF_LV_0("%s open failed\n", filename);
    return;
  }

  old_fs=get_fs();
  set_fs(KERNEL_DS);

  if(flip->f_op == NULL || flip->f_op->write == NULL)
  {
    PRINTF_LV_0("%s: File has no file operations registered!\n", __func__);
    filp_close(flip, NULL);
    return;
  }
  flip->f_op->write(flip, p, size, &flip->f_pos);
  set_fs(old_fs);

  filp_close(flip, NULL);
}
#endif

#ifdef DEBUG_MSC_BUFFER
static uint32_t debug_buf=0;
static uint32_t pre_data_mode = 0;
#endif
void TDMBStreamControlSub (int32_t moduleidx, uint8_t *p, uint32_t size)
{
  uint32_t i = 0, k;
  uint32_t point, stream_type = 0, id = 0, ch_size = 0;
  uint32_t *prot_data = gtTDMBCtrl[moduleidx].ProtocolData;
  int32_t RemainSize;
  int32_t done, err_cnt = 0;
  int32_t hdr_cnt = 0;
  static uint32_t data_mode;
#ifdef TC317X_SPLIT_MSC_BUFFER
  static uint32_t msc_ch_size = 0;
  uint32_t subch_id = 0;

  if((int32_t)(prot_data[0*2+1]) != 0)
    data_mode = (prot_data[0*2+1]>>16) & 0x3;

#ifdef DEBUG_MSC_BUFFER
  if(pre_data_mode != data_mode) 
  {
    buffer_reset();
    debug_buf = 1; 
  }
  pre_data_mode = data_mode;
#endif
#endif /*TC317X_SPLIT_MSC_BUFFER*/

  if (!gp_MSCFICData[moduleidx])      // safety code
    return;

  RemainSize = size;
  point = 0;

#if defined(DEBUG_MSC_BUFFER)
  PRINTF_LV_0("post_read_size:%d, size:%d\n", post_read_size, size);
#endif
  for(k = post_read_size; k < size; )
  {
    stream_type = ((p[k + 2] & 0xc0) >> 6);
    ch_size = (p[k + 1] << 7 | (p[k+0] >> 1)) << 2;
    if(p[k + 3] == 0x5c)
    {
      id = p[k + 2] & 0x3f;
#if defined(DEBUG_MSC_BUFFER)
      if(stream_type == 1 && id == 4)
      {
        PRINTF_LV_0("  [%08X]::%d:%d:%d\n", 
          SWAP32(*((uint32_t*)(p+k))), stream_type, ch_size, id);
      }
      else
      {
        PRINTF_LV_0("  [%08X]::%d:%d:%d\n", 
          SWAP32(*((uint32_t*)(p+k))), stream_type, ch_size, id);
      }
#endif /*DEBUG_MSC_BUFFER*/
      hdr_cnt++;
      if(hdr_cnt > 2) 
      {
        //PRINTF_LV_0("%d:%d\n", id, ch_size);
        break;
      }
      k += (ch_size+4);
    }
    else 
    {
      //PRINTF_LV_0("wrong hdr_cnt:%d\n", hdr_cnt);
      buffer_reset();
      point = find_header(p, size);
      if((int32_t)point < 0)
      {
        PRINTF_LV_0("no header!! ignore data! %d\n", (int32_t)point);
#if defined(DEBUG_DUMP_FILE)
        file_dump(p, size);
#endif /*DEBUG_DUMP_FILE*/
        return;
      }

      post_read_size = 0;
      RemainSize = size - point;
      //PRINTF_LV_0("found header!! %d, Remain:%d\n", point, RemainSize);
      break;
    }
  }

  if (post_read_size)
  {
    if (dmb_chip_index == 0)
      i = 0;
    else if (dmb_chip_index == 1)
      i = 1;
    else
      PRINTF_LV_0 ("errrrrrrrrrrrr\n");

    if((int32_t)post_read_size > RemainSize)
    {
      PRINTF_LV_0("WARN!! post_read_size:%d, RemainSize:%d\n",
        post_read_size, RemainSize);
      post_read_size = 0;
      return; 
    }
    else
    {
#if defined(TC317X_SPLIT_MSC_BUFFER)
      if(post_read_type == 0)
        buffer_write (post_read_size, &msc_buffer[i], &p[0]);
      else
#endif //TC317X_SPLIT_MSC_BUFFER
      if(post_read_type == 1 && post_subch_id == 4);
        //PRINTF_LV_0("post garbage!\n");
      else
        buffer_write (post_read_size, &dual_dmb_buffer[i], &p[0]);

      point += post_read_size;
      RemainSize -= post_read_size;
      post_read_size = 0;
    }
  }
  else
    post_read_size = 0;

  post_subch_id = -1;
  post_read_type = -1;

  while (RemainSize > 0)
  {
    uint32_t w_size = 0, idx;

    if (p[point + 3] == 0x5c && TDMBEP_Header_check (&p[point]) == 1)
    {
      //header parse
      stream_type = ((p[point + 2] & 0xc0) >> 6);
      id = p[point + 2] & 0x3f;
      ch_size = (p[point + 1] << 7 | (p[point + 0] >> 1)) << 2;

      if (stream_type == 3) //remove other chip header
      {
        idx = 1;
        point += 4;
        RemainSize -= 4;
        TCDBG("other chip!?, stream_type:%d, size:%d\n", stream_type, ch_size);
        continue;
      }
      else
      {
        idx = 0;
      }

      if ((int)ch_size >= RemainSize)
      {
        w_size = RemainSize;
        post_read_size = (ch_size - w_size);
        post_read_type = stream_type;
        post_subch_id  = id;
        if (idx == 0) 
        {
          dmb_chip_index = 0;
          post_read_size += 4;
        }
        if (idx == 1) 
        {
          dmb_chip_index = 1;
        }
      }
      else
      {
        w_size = ch_size;
        post_read_size = 0;
        post_read_type = -1;
        if (idx == 0) w_size += 4;
      }

      if (RemainSize-w_size < 0)
      {
        PRINTF_LV_0 ("errr999\n");
        break;
      }

#if defined(TC317X_SPLIT_MSC_BUFFER)
      if(stream_type == 0)
      {
        buffer_write (w_size-4, &msc_buffer[idx], &p[point+4]);
        subch_id = id;
        msc_ch_size = ch_size;
      }
      else
#endif /*TC317X_SPLIT_MSC_BUFFER*/
      if(stream_type == 1 && id == 4) ;
        //PRINTF_LV_0("garbage!\n");
      else 
        buffer_write (w_size, &dual_dmb_buffer[idx], &p[point]);

      RemainSize -= w_size;
      point += (w_size);
    }
    else
    {
      //PRINTF_LV_0 ("e[%X]",p[point + 3]);
      RemainSize -= 4;
      point += 4;
      post_read_type = -1;
      err_cnt++;
    }
  }

#ifdef DEBUG_MSC_BUFFER
  if(err_cnt)
  {
    PRINTF_LV_0("hdr err_cnt : %d \n", err_cnt);
  }
  debug_buf = 0; 
#endif /*DEBUG_MSC_BUFFER*/

  for (i = 0; i < dmb_module_num; i++)
  {
    uint32_t rsize;
    BUFF *pficbuf = &dual_dmb_buffer[i];
#ifdef  TC317X_SPLIT_MSC_BUFFER
    uint32_t srv_type = SRVTYPE_DMB;
    BUFF *pmscbuf = &msc_buffer[i];
#endif /*TC317X_SPLIT_MSC_BUFFER*/

    done = 0;
    while (!done)
    {
      rsize = data_buff_check (pficbuf);

      if (rsize <= 4) { done = 1; break; }

      if (pficbuf->p[pficbuf->rptr + 3] == 0x5c)
      {
        //header parse
        stream_type = ((pficbuf->p[2+pficbuf->rptr] & 0xc0) >> 6);
        id = pficbuf->p[2 + pficbuf->rptr] & 0x3f;
        ch_size = (pficbuf->p[1 + pficbuf->rptr] << 7 | 
            (pficbuf->p[0 + pficbuf->rptr] >> 1)) << 2;
      }
      else
      {
        //PRINTF_LV_0("header error!!!!\n");
      }

      if (rsize <= ch_size) { done = 1; break; }

      pficbuf->rptr += 4;
      pficbuf->rptr %= pficbuf->size;
#if !defined(STREAM_MIXER)
      if (stream_type == 0)//msc
      {
#if defined(TC317X_SPLIT_MSC_BUFFER)
#else //TC317X_SPLIT_MSC_BUFFER
        for(k = 0; k<MAX_TDMBMSC_NUM; k++)
        {
          if(id == ((prot_data[k*2] & 0x03f00000) >> 20))
          {
            data_mode = (prot_data[k*2+1] >> 16) & 0x3;
            if(data_mode == 0)//dab
            {
              stream_upload(ch_size, pficbuf, SRVTYPE_DAB, moduleidx, id);
            }
            else if(data_mode == 1)//dmb
              stream_upload(ch_size, pficbuf, SRVTYPE_DMB, moduleidx, id);
            else if(data_mode == 2)//epm
              stream_upload(ch_size, pficbuf, SRVTYPE_DMB, moduleidx, id);
            else if(data_mode == 3)//dab+
            { // for word align
              stream_upload(ch_size, pficbuf, SRVTYPE_DABPLUS, moduleidx, id);
            }
            break;
          }
        }
#endif //TC317X_SPLIT_MSC_BUFFER
      }
      else if (stream_type == 1)//fic
      {
        stream_upload (ch_size - 4, pficbuf, SRVTYPE_FIC, moduleidx, 0);
      }
      else if (stream_type == 2)//status
      {
        stream_upload (ch_size, pficbuf, SRVTYPE_STATUS, moduleidx, 0);
      }
      else
      {
        PRINTF_LV_0 ("stream_type error\n");
      }
#else //STREAM_MIXER
// {{{
      if (stream_type == 0)//msc
      {
        if(i == 0)
          stream_upload (ch_size, &dual_dmb_buffer[i], SRVTYPE_DMB, moduleidx, id);
        else
        {
          if(ksj_cnt==3)
          {
            if(id == 0)
            {
              stream_upload (ch_size, &dual_dmb_buffer[i], SRVTYPE_DMB, moduleidx, 6);
            }
          }
          else if(ksj_cnt>=4)
          {
            if(id == 0)
            {
              stream_upload (ch_size, &dual_dmb_buffer[i], SRVTYPE_DMB, moduleidx, 6);
            }
            else
            {
              stream_upload (ch_size, &dual_dmb_buffer[i], SRVTYPE_DMB, moduleidx, 7);
            }
          }
        }
      }
      else if (stream_type == 1)//fic
      {
        if(i == 0)
          stream_upload (ch_size - 4, &dual_dmb_buffer[i], SRVTYPE_FIC, moduleidx, 0);
      }
      else if (stream_type == 2)//status
      {
        if(i == 0)
          stream_upload (ch_size, &dual_dmb_buffer[i], SRVTYPE_STATUS, moduleidx, 0);
      }
      else
      {
        PRINTF_LV_0 ("stream_type error\n");
      }
//}}}
#endif 
      pficbuf->rptr += ch_size;
      pficbuf->rptr %= pficbuf->size;
    }

#ifdef  TC317X_SPLIT_MSC_BUFFER
    rsize = data_buff_check (pmscbuf);
    if(!rsize) continue;

    if(data_mode == 1) 
    {
      srv_type = SRVTYPE_DMB;
      rsize -= (rsize%188);
      //PRINTF_LV_0("data_mode :%d, %d bytes upload, total %d bytes\n",
      //  data_mode, rsize-rsize%188, rsize);
    }
    else if(data_mode == 0 || data_mode == 3)
    {
      srv_type = (data_mode==0) ? SRVTYPE_DAB : SRVTYPE_DABPLUS;
      TCASSERT(msc_ch_size == 0);
      rsize -= (rsize%msc_ch_size);
      //PRINTF_LV_0("data_mode :%d, %d bytes upload, msc_ch_size:%d\n", 
      //  data_mode, rsize, msc_ch_size);
    }

    if(!rsize) continue;

    stream_upload(rsize, pmscbuf, srv_type, moduleidx, subch_id);
    pmscbuf->rptr += rsize;
    TCASSERT(pmscbuf->size == 0);
    pmscbuf->rptr %= pmscbuf->size;
#endif /*TC317X_SPLIT_MSC_BUFFER*/
  }
}

// header & etc parsing & send data
void TDMBStreamControl (int32_t moduleidx, uint8_t *p, uint32_t size, int32_t BuffIdx)
{
  gtTDMBCtrl[moduleidx].EPHeaderON = 1;

  if (gtTDMBCtrl[moduleidx].EPHeaderON)
  {
    TDMBStreamControlSub (moduleidx, p, size);
  }
  else
  {
    // pass through, push to usb...
    if (BuffIdx == 1)
    {
      TDMB_Stream_Handler (moduleidx, p, size, SRVTYPE_DMB, 0, 0);
    }
    else if (BuffIdx == 2)
    {
      if (gtTDMBCtrl[moduleidx].OPHeaderON)
      {
        uint8_t HeaderData[_SIZE_STATHEADER_ + _SIZE_STATDATA_ + _SIZE_FICHEADER_];
        TDMBEPOPHeader_t EPOPHeader;
        int32_t       ret;

        uint8_t CRCStat[4];
        uint32_t CRCValue;
        uint32_t NumOfFIBs;
        uint32_t CRCResult;
        uint32_t mask;
        TC317X_IO_memcpy (&HeaderData[0], p, _SIZE_STATHEADER_ + _SIZE_STATDATA_ + _SIZE_FICHEADER_);
        ret = Parse_TDMB_OPHEADER (moduleidx, BuffIdx, &HeaderData[0], &EPOPHeader);
        if (ret == 0)
        {
          //PRINTF_LV_1 ("# OPHeaderError\n");
          return;
        }

        if (EPOPHeader.OPDataSize > size)
        {
          PRINTF_LV_0 ("# OPHeaderSizeOver\n");
          return;
        }

        if (gtTDMBCtrl[moduleidx].Flag_ChannelReconfig)
        {
          //PRINTF_LV_2 ("# [A]ChannelReconfig Exist\n");
        }

        if (gtTDMBCtrl[moduleidx].Flag_EWS_Announce)
        {
          //PRINTF_LV_2 ("# [A]EWS Announce Exist\n");
        }

        if (gtTDMBCtrl[moduleidx].Flag_ChannelReconfig)
        {
          uint32_t data = 0;
          TC317X_MB_TxOnly (&g_pTC3X[moduleidx][0], MBPARA_SYS_DAB_RECONFIG_CLR, &data, 1);
          //PRINTF_LV_2 ("# CLR_ReconfigFlag\n");
        }

        TC317X_IO_memcpy (&CRCStat[0], &p[ret + EPOPHeader.OPDataSize], _SIZE_STATCRC_);

        //CRCValue = (CRCStat[0]<<24) | (CRCStat[1]<<16) | (CRCStat[2]<<8) | (CRCStat[3]);
        CRCValue = (CRCStat[3] << 24) | (CRCStat[2] << 16) | (CRCStat[1] << 8) | (CRCStat[0]);
        NumOfFIBs = CRCValue >> 16 & 0xF;
        mask = getmaskx (NumOfFIBs);
        if (!mask)
        {
          //PRINTF_LV_1 ("# MaskError\n");
        }
        CRCResult = CRCValue & mask;

        if (CRCResult)
        {
          //PRINTF_LV_1 ("# FIC CRC Error Found[%d][%x]\n", NumOfFIBs, CRCResult);
          //PRINTF_LV_1 ("#[%d]F[0x%x]\n", moduleidx, CRCResult);
        }

        if (CRCResult)  // crc error exist
        {
          TDMB_Stream_Handler (moduleidx, &p[ret], EPOPHeader.OPDataSize, SRVTYPE_FIC_WITH_ERR, 0, CRCResult);
        }
        else    // no crc error
        {
          if (gtTDMBCtrl[moduleidx].Flag_ChannelReconfig && gtTDMBCtrl[moduleidx].Flag_EWS_Announce)
            TDMB_Stream_Handler (moduleidx, &p[ret], EPOPHeader.OPDataSize, EWS_ANNOUNCE_RECONFIGURATION_FLAG, 0, 0);
          else if (gtTDMBCtrl[moduleidx].Flag_ChannelReconfig)
            TDMB_Stream_Handler (moduleidx, &p[ret], EPOPHeader.OPDataSize, RECONFIGURATION_FLAG, 0, 0);
          else if (gtTDMBCtrl[moduleidx].Flag_EWS_Announce)
            TDMB_Stream_Handler (moduleidx, &p[ret], EPOPHeader.OPDataSize, EWS_ANNOUNCE_FLAG, 0, 0);
          else
            TDMB_Stream_Handler (moduleidx, &p[ret], EPOPHeader.OPDataSize, SRVTYPE_FIC, 0, 0);
        }
      }
      else
      {   // Pass Through
        TDMB_Stream_Handler (moduleidx, p, size, SRVTYPE_FIC, 0, 0);
      }
    }
    else if (BuffIdx == 3)
    {
      // receive 1 frame at once. (dab use circular buffer)
      /*
       * [31:23] 0x5C
       * [22:16] SUBCH ID
       * [15:0] data size (word ??')
       */
      if (gtTDMBCtrl[moduleidx].OPHeaderON)
      {
        uint8_t HeaderData[4];
        TDMBEPOPHeader_t EPOPHeader;
        int32_t       ret;
        int32_t       ServMailIdx;

        TC317X_IO_memcpy (&HeaderData[0], p, 4);
        ret = Parse_TDMB_OPHEADER (moduleidx, BuffIdx, &HeaderData[0], &EPOPHeader);
        if (ret != 1)
        {
          //PRINTF_LV_1 ("# OPHeaderError\n");
          return;
        }

        if (EPOPHeader.OPDataSize > size)
        {
          PRINTF_LV_0 ("# OPHeaderSizeOver\n");
          return;
        }

        ServMailIdx = GetTDMBServMailIdx (moduleidx, EPOPHeader.SubchID);
        if (ServMailIdx != -1)
        {
          int32_t       servicetype;
          servicetype = GetTDMBServiceType (moduleidx, ServMailIdx);
          TDMB_Stream_Handler (moduleidx, &p[4], EPOPHeader.OPDataSize, servicetype, EPOPHeader.SubchID, ServMailIdx);
        }
      }
      else
      {   // Pass Through
        TDMB_Stream_Handler (moduleidx, p, size, SRVTYPE_DAB, 0, 0);
      }
    }
  }
}

void SetTDMBMSCFormat (t_TC3X_DEMODULE * pTC317X, uint32_t ch_cfg)
{
  gtTDMBCtrl[pTC317X->moduleidx].OPHeaderON = 1;
  //TCDBG("channel cfg: 0x%X\n", ch_cfg);

  TC317X_MB_TxOnly (pTC317X, MBPARA_SYS_DAB_STREAM_SET, &ch_cfg, 1);
}

void SetTDMBStreamOn (t_TC3X_DEMODULE * pTC317X, int32_t FIC_ON, int32_t DAB_ON, int32_t DMB_ON)
{

}

void SetTDMBStreamOnOff (t_TC3X_DEMODULE * pTC317XD, uint32_t data)
{

}

void ksj_func_init (void)
{
  post_read_size = 0;
  dmb_chip_index = 0;
}

void test_stream_mixer_force(t_TC3X_DEMODULE * pTC317X)
{
  uint32_t protocoldata[12], data;
  TC317X_IO_memset(protocoldata, 0x00, 4*12);

  //MBC
  protocoldata[0] =   2<<26  | 0 << 20 |0x198 << 10 | 0x00;
  protocoldata[1] =   0<< 20 | 0<< 18  |1<< 16    | 1<< 11    | 2 << 8 | 0x44;//dmb

  protocoldata[2] =   2<<26  | 1 << 20 |0x78 << 10 | 0x258;
  protocoldata[3] =   1<< 20 | 0<< 18  |1<< 16    | 1<< 11    | 2 << 8 | 0x14;//dmb

  data = 2;
  TC317X_MB_TxOnly (pTC317X, MBPARA_SEL_CH_INFO_PRAM, protocoldata, 6);
  TC317X_MB_TxOnly (pTC317X, MBPARA_SYS_DAB_MCI_UPDATE, &data, 1);
}


void PushMCI_SortStartCU(int32_t moduleidx, uint32_t *pPushDatas)
{
  int32_t i;
  int32_t j;
  int32_t StartCU[MAX_TDMBMSC_NUM];
  uint32_t RemapData[MAX_TDMBMSC_NUM*2];
  //int32_t Seed = 65535;

  memcpy(&RemapData[0], pPushDatas, sizeof(uint32_t)*MAX_TDMBMSC_NUM*2);

  for(i=0; i<MAX_TDMBMSC_NUM; i++)
  {
    if(pPushDatas[i*2]!=0)
      StartCU[i] = (pPushDatas[i*2] & 0x3FF);
    else
      StartCU[i] = 0x3FF;
  }

  for(i=0; i<MAX_TDMBMSC_NUM; i++)
  {
    for(j=i+1; j<MAX_TDMBMSC_NUM; j++)
    {
      if(StartCU[i]>StartCU[j])
      {
        uint32_t Data[2];
        int32_t Temp;

        Temp = StartCU[i];
        Data[0] = RemapData[i*2];
        Data[1] = RemapData[i*2+1];

        StartCU[i] = StartCU[j];
        RemapData[i*2] = RemapData[j*2];
        RemapData[i*2+1] = RemapData[j*2+1];

        StartCU[j] = Temp;
        RemapData[j*2] = Data[0];
        RemapData[j*2+1] = Data[1];
      }
    }
  }

  TC317X_MB_TxOnly (&g_pTC3X[moduleidx][0], MBPARA_SEL_CH_INFO_PRAM, &RemapData[0], 6);
  TC317X_MB_TxOnly (&g_pTC3X[moduleidx][0], MBPARA_SEL_CH_INFO_PRAM+1, &RemapData[6], 6);
#if 0
  PRINTF_LV_0("0: 0x%08x\n", RemapData[0]);
  PRINTF_LV_0("1: 0x%08x\n", RemapData[1]);
  PRINTF_LV_0("2: 0x%08x\n", RemapData[2]);
  PRINTF_LV_0("3: 0x%08x\n", RemapData[3]);
  PRINTF_LV_0("4: 0x%08x\n", RemapData[4]);
  PRINTF_LV_0("5: 0x%08x\n", RemapData[5]);
  PRINTF_LV_0("6: 0x%08x\n", RemapData[6]);
  PRINTF_LV_0("7: 0x%08x\n", RemapData[7]);
  PRINTF_LV_0("8: 0x%08x\n", RemapData[8]);
  PRINTF_LV_0("9: 0x%08x\n", RemapData[9]);
  PRINTF_LV_0("a: 0x%08x\n", RemapData[10]);
  PRINTF_LV_0("b: 0x%08x\n", RemapData[11]);
#endif
}

typedef enum
{
  GLOBALSRVTYPE_DAB = 4,
  GLOBALSRVTYPE_DABPLUS,
  GLOBALSRVTYPE_DMB,
  GLOBALSRVTYPE_TPEG,
  GLOBALSRVTYPE_EPG
} SRV_TYPE_E;   // inside of muse

int32_t TC317X_TDMB_RegService (int32_t moduleidx, TDMBService_t * pTDMBService)
{
  int32_t       i;//, j;
  int32_t       ret;
  int32_t       idx = 0;
  int32_t       sindx= 0;
  uint32_t data = 0;
  uint32_t protocoldata[12];
  int32_t DataMode, Soft_bit;
  int32_t threshold = 0;
  int32_t br_uep[] = 
    {32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384};

  switch(pTDMBService->ServType)
  {
    case SRVTYPE_DAB:
      DataMode = 0;
      break;

    case SRVTYPE_DABPLUS:
      DataMode = 3;
      break;

    case SRVTYPE_DATA:
      DataMode = 0;
      break;

    case SRVTYPE_DMB:
      DataMode = 1;
      break;

    default:
      DataMode = 2;
      break;
  }

  TC317X_IO_memset (protocoldata, 0x00, 4 * 12);

  // find reg idx
  for (i = 0; i < MAX_TDMBMSC_NUM; i++)
  {
    if (gtTDMBCtrl[moduleidx].bOnAir[i] == 0)
    {
      idx = i * 2;
      sindx = i;
      break;
    }
  }

  ret = sindx;

  Soft_bit = 0;

  TC317X_IO_memcpy (&gtTDMBCtrl[moduleidx].TDMBService[sindx], pTDMBService, sizeof (TDMBService_t));

  gtTDMBCtrl[moduleidx].bOnAir[sindx] = 1;
  gtTDMBCtrl[moduleidx].MCICount++;
  gtTDMBCtrl[moduleidx].DMBChannelID[sindx] = sindx;

  gtTDMBCtrl[moduleidx].ProtocolData[idx] = 
    pTDMBService->ReConfig << 26
    | pTDMBService->SubChID << 20
    | pTDMBService->CUSize << 10
    | pTDMBService->StartCU;
  gtTDMBCtrl[moduleidx].ProtocolData[idx + 1] = 
    gtTDMBCtrl[moduleidx].DMBChannelID[sindx] << 20
    | Soft_bit << 18
    | DataMode << 16
    | pTDMBService->PType << 11
    | pTDMBService->PLevel << 8
    | pTDMBService->BitRate;
  //PRINTF_LV_2 ("subchid:%d, cusize:%d, startcu:%d, ptype:%d, plevel:%d, bitrate:%d, rs=%d, soft_bit=%d, reconfg=%d, DataMode=%d\n", pTDMBService->SubChID, pTDMBService->CUSize, pTDMBService->StartCU, pTDMBService->PType, pTDMBService->PLevel, pTDMBService->BitRate, pTDMBService->RS_ON, Soft_bit, pTDMBService->ReConfig, DataMode);

#ifdef  STREAM_MIXER
  ///test code
  ksj_cnt++;
  if(ksj_cnt >=3)
    return ret;
#endif

  /* calculate interrupt threshold */
  switch(pTDMBService->PType)
  {
    case 0x00: //UEP
      PRINTF_LV_0("UEP %d, ", pTDMBService->BitRate);
      threshold = (br_uep[pTDMBService->BitRate]*6)<<2;
      break;
    case 0x01: //EEP
      PRINTF_LV_0("EEP %d, ", pTDMBService->BitRate);
      threshold = (pTDMBService->BitRate*6) << 5;
      break;
  }
  if( (MAX_FIFO_SIZE>>1) < threshold) 
    threshold = (MAX_FIFO_SIZE>>1);

  /* calculate garabage size */
  garbage_size = (threshold)>>3;
  PRINTF_LV_0("threshold:%d \n", threshold);

  if(pTDMBService->ServType == SRVTYPE_DMB)
    threshold -= TC317X_TS_SIZE;
  else
    threshold -= (TC317X_FIC_SIZE + TC317X_STATUS_SIZE);

  /* 1. Disable interrupt */
  TC317XREGSET_IRQEN(&g_pTC3X[moduleidx][idx], 0);
  TC317XREGSET_IRQCLR (&g_pTC3X[moduleidx][idx], TC3XREG_IRQ_STATCLR_ALL);

  /* 2. Change stream type */
  SetTDMBMSCFormat (&g_pTC3X[moduleidx][0], 
    (garbage_size<<16)|(1<<7)|
      (CHANNEL_CFG_DMB|CHANNEL_CFG_DAB|CHANNEL_CFG_FIC|CHANNEL_CFG_STATUS)
      /*CHANNEL_CFG_ALL*/);

  /* 3. Write channel data */
  gtTDMBCtrl[moduleidx].ServiceNum++;
  if(Soft_bit == 1 || Soft_bit == 2)
    data = gtTDMBCtrl[moduleidx].MCICount+(1<<16);
  else
    data = gtTDMBCtrl[moduleidx].MCICount;
  PushMCI_SortStartCU(moduleidx, &gtTDMBCtrl[moduleidx].ProtocolData[0]);
  TC317X_MB_TxOnly (&g_pTC3X[moduleidx][0], MBPARA_SYS_DAB_MCI_UPDATE, &data, 1);
  gtTDMBCtrl[moduleidx].ProtocolData[idx] &= 0xf3ffffff;

  /* 4. Change interrupt threshold */
  TC317X_Stream_Cfg(moduleidx, 0, threshold);

  /* 5. Enable interrupt */
  TC317XREGSET_IRQEN(&g_pTC3X[moduleidx][idx], 0x8);

  return ret;
}

int32_t TC317X_TDMB_UnRegService (int32_t moduleidx, TDMBService_t * pTDMBService, int32_t idx)
{
  int32_t       ret;
  uint32_t data = 0;

  TC317X_IO_memset (&gtTDMBCtrl[moduleidx].TDMBService[idx], 0x00, sizeof (TDMBService_t));
  gtTDMBCtrl[moduleidx].ProtocolData[idx * 2] = 0x00;
  gtTDMBCtrl[moduleidx].ProtocolData[idx * 2 + 1] = 0x00;
  gtTDMBCtrl[moduleidx].ServiceNum--;
  gtTDMBCtrl[moduleidx].bOnAir[idx] = 0;
  gtTDMBCtrl[moduleidx].MCICount--;
  data = gtTDMBCtrl[moduleidx].MCICount;

  PushMCI_SortStartCU(moduleidx, &gtTDMBCtrl[moduleidx].ProtocolData[0]);
  TC317X_MB_TxOnly (&g_pTC3X[moduleidx][0], MBPARA_SYS_DAB_MCI_UPDATE, &data, 1);

  // Display
  //PRINTF_LV_2 ("# idx[%d]\n", idx);

  ret = idx;
  return ret;
}

int32_t TC317X_SET_TDMB_Service (int32_t moduleidx, uint32_t RegFlag, TDMBService_t * pTDMBService)
{
  uint32_t i;
  uint32_t idx = 0;
  int32_t       ret = 0;
  int32_t       findid = 0;

  if (RegFlag)
  {
    //PRINTF_LV_2 ("# TDMB RegCh [%d]\n", pTDMBService->SubChID);

    if (gtTDMBCtrl[moduleidx].ServiceNum > (MAX_TDMBMSC_NUM - 1))
    {
      PRINTF_LV_0 ("# MAX Service Over\n");
      return -1;
    }

    for (i = 0; i < MAX_TDMBMSC_NUM; i++)
    {       // same service exist. return;
      if ((pTDMBService->SubChID == gtTDMBCtrl[moduleidx].TDMBService[i].SubChID) && gtTDMBCtrl[moduleidx].bOnAir[i] == 1)
      {
        //PRINTF_LV_1 ("# Already Exist Service[%d]\n", pTDMBService->SubChID);
        return -1;
      }
    }
  }
  else
  {
    //PRINTF_LV_2 ("# TDMB UnRegCh [%d]\n", pTDMBService->SubChID);

    if (gtTDMBCtrl[moduleidx].ServiceNum < (1))
    {
      //PRINTF_LV_1 ("# Current, No Service.\n");
      return -1;
    }

    // no service, return
    for (i = 0; i < MAX_TDMBMSC_NUM; i++)
    {
      if ((pTDMBService->SubChID == gtTDMBCtrl[moduleidx].TDMBService[i].SubChID) && gtTDMBCtrl[moduleidx].bOnAir[i] == 1)
      {
        findid = 1;
        idx = i;
        break;
      }
    }
#ifndef multi_dmb
    if (!findid)
    {
      //PRINTF_LV_1 ("# Can't Find old Serivce\n");
      return -1;
    }
#endif
  }

  switch (RegFlag)
  {
    case 0:    // un reg
#ifndef multi_dmb
      ret = TC317X_TDMB_UnRegService (moduleidx, pTDMBService, idx);
#endif
      break;

    case 1:    // reg
      ret = TC317X_TDMB_RegService (moduleidx, pTDMBService);
      break;
  }

  return ret;
}

int32_t GetFIFOThr_TDMB (int32_t moduleidx)
{
  int32_t       retv;

  switch (g_pTC3X[moduleidx][0].SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_MAINIO:
    case TC3X_STREAM_IO_SPISLV:
    case TC3X_STREAM_IO_HPI_HEADERON:
    case TC3X_STREAM_IO_HPI_HEADEROFF:
      retv = ((g_tStreamSet[moduleidx].PktSize * g_tStreamSet[moduleidx].PktThrNum) >> 2);
      break;

    case TC3X_STREAM_IO_PTS:
    case TC3X_STREAM_IO_STS:
    case TC3X_STREAM_IO_SPIMS:
      retv = ((g_tStreamSet[moduleidx].PktSize * g_tStreamSet[moduleidx].PktThrNum) >> 2);
      break;

    default:
      retv = ((g_tStreamSet[moduleidx].PktSize * g_tStreamSet[moduleidx].PktThrNum) >> 2);
      break;
  }

  return retv;
}

void GetTDMBStream (int32_t moduleidx, uint32_t interrupt_stat)
{
  uint32_t Size;

  Size = (tc317x_cspi_read_size);
  TC317X_TDMBStreamRead (moduleidx, Size);
  //TDMBStreamControl (moduleidx, gp_TDMB_Stream[moduleidx], Size, 0);
  TDMBStreamControlSub (moduleidx, gp_TDMB_Stream[moduleidx], Size);
}

int32_t TC317X_TDMBStreamRead (int32_t moduleidx, uint32_t Size)
{
  TC317X_DRV_Stream_Read (&g_pTC3X[moduleidx][0], 0,
      &gp_TDMB_Stream[moduleidx][0], Size);
  return 0;
}

void TDMB_SPI_Handler (int32_t moduleidx, int32_t StreamBuffIdx, uint8_t *pStream, int32_t PktSize, int32_t PktThrNum)
{
  int32_t       num;
  uint32_t nStart;
  uint32_t PktBurstSize;

  uint8_t *src;

  if (!gp_TDMB_Stream)
    return;

  num = (PktThrNum >> 1);

  PktBurstSize = PktSize * num;
  nStart = (StreamBuffIdx == 0) ? (PktThrNum >> 1) : 0;
  src = pStream + nStart * PktSize;

  TDMBStreamControl (moduleidx, src, PktBurstSize, 0);
}

void TDMB_Stream_Handler (int32_t moduleidx,
    uint8_t *pStream, uint32_t Size, uint32_t Type,
    uint32_t DABSubChId, uint32_t Idx)       // same as TDMB_SPI_Handler
{
  uint8_t *pData;
  uint32_t sSize;
  uint32_t sIdx;
  uint32_t Formatted;

  pData = pStream;
  sSize = Size;
  sIdx = Idx;
  Formatted = (Type << 8) | DABSubChId;

  g_pTC3XCTRL[moduleidx][0].BBEventFn (moduleidx, 0, CMD_STREAM_TDMB, pData, &sSize, &Formatted, &sIdx);      // fic/dabmsc/dmbmsc, type,subchID, servicemailboxidx
}

void Get_TDMB_Moving_AVGSet (int32_t moduleidx,
    uint32_t *pAVGSNR, uint32_t *pAVGPCBER,
    uint32_t *pAVGVITERBIBER, uint32_t *pAVGTSPER, uint32_t *pAVGRSSI)
{
  if (pAVGSNR)
    pAVGSNR[0] = gtTDMBCtrl[moduleidx].MonStat.SNRset.Moving_AVG;

  if (pAVGPCBER)
    pAVGPCBER[0] = gtTDMBCtrl[moduleidx].MonStat.PCBERset.Moving_AVG;

  if (pAVGVITERBIBER)
    pAVGVITERBIBER[0] = gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Moving_AVG;

  if (pAVGTSPER)
    pAVGTSPER[0] = gtTDMBCtrl[moduleidx].MonStat.TSPERset.Moving_AVG;

  if (pAVGRSSI)
    pAVGRSSI[0] = gtTDMBCtrl[moduleidx].MonStat.RSSIset.Moving_AVG;
}

void Get_TDMB_LockStatus (int32_t moduleidx, t_TDMBLock * pLockStat)
{
  uint8_t input;
  input = (uint8_t) TC317XREGGET_PROGRAMID(&g_pTC3X[moduleidx][0]);
  Parse_TDMBSyncStat (pLockStat, input);
}

void Get_TDMB_SNR (int32_t moduleidx,
    uint32_t *pSNR, uint32_t *pSNRdb, uint32_t *pMovingAVGSNRdb)
{
  if (pSNR)
    pSNR[0] = gtTDMBCtrl[moduleidx].MonStat.SNRset.SNR_;
  if (pSNRdb)
    pSNRdb[0] = gtTDMBCtrl[moduleidx].MonStat.SNRset.SNRdb;
  if (pMovingAVGSNRdb)
    pMovingAVGSNRdb[0] = gtTDMBCtrl[moduleidx].MonStat.SNRset.Moving_AVG;
}

void Get_TDMB_PCBER (int32_t moduleidx, uint32_t *pPCBER, uint32_t *pMovingAVGPCBER)
{
  if (pPCBER)
    pPCBER[0] = gtTDMBCtrl[moduleidx].MonStat.PCBERset.PCBER;
  if (pMovingAVGPCBER)
    pMovingAVGPCBER[0] = gtTDMBCtrl[moduleidx].MonStat.PCBERset.Moving_AVG;
}

void Get_TDMB_VITERBIBER (int32_t moduleidx,
    uint32_t *pVITERBIBER, uint32_t *pMovingAVGVITERBIBER)
{
  if (pVITERBIBER)
    pVITERBIBER[0] = gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.VITERBIBER;
  if (pMovingAVGVITERBIBER)
    pMovingAVGVITERBIBER[0] = gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Moving_AVG;
}

void Get_TDMB_TSPER (int32_t moduleidx, uint32_t *pTSPER, uint32_t *pMovingAVGTSPER)
{
  if (pTSPER)
    pTSPER[0] = gtTDMBCtrl[moduleidx].MonStat.TSPERset.TSPER;
  if (pMovingAVGTSPER)
    pMovingAVGTSPER[0] = gtTDMBCtrl[moduleidx].MonStat.TSPERset.Moving_AVG;
}

void Get_TDMB_RSSI (int32_t moduleidx, uint32_t *pRSSI, uint32_t *pMovingAVGRSSI)
{
  if (pRSSI)
    pRSSI[0] = gtTDMBCtrl[moduleidx].MonStat.RSSIset.RSSI;
  if (pMovingAVGRSSI)
    pMovingAVGRSSI[0] = gtTDMBCtrl[moduleidx].MonStat.RSSIset.Moving_AVG;
}

void TDMB_Reset_MonitorValue(int32_t moduleidx)
{
  // PCBER
  gtTDMBCtrl[moduleidx].MonStat.PCBERset.cnt = 1;
  TC317X_IO_memset(&gtTDMBCtrl[moduleidx].MonStat.PCBERset.Array[0], 0x00, sizeof(uint32_t)*TDMB_MAX_MOV_AVG);

  gtTDMBCtrl[moduleidx].MonStat.PCBERset.PCBER = _TDMB_MAX_PCBER_*SCALE_FACTOR;
  gtTDMBCtrl[moduleidx].MonStat.PCBERset.Moving_AVG =
    TC317X_moving_avg_d (&gtTDMBCtrl[moduleidx].MonStat.PCBERset.Array[0], gtTDMBCtrl[moduleidx].MonStat.PCBERset.PCBER,
        gtTDMBCtrl[moduleidx].MonStat.PCBERset.cnt, TDMB_MAX_MOV_AVG);
  // SNR
  gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt = 1;
  TC317X_IO_memset(&gtTDMBCtrl[moduleidx].MonStat.SNRset.Array[0], 0x00, sizeof(uint32_t)*TDMB_MAX_MOV_AVG);

  gtTDMBCtrl[moduleidx].MonStat.SNRset.SNRdb = _TDMB_MIN_SNR_;
  gtTDMBCtrl[moduleidx].MonStat.SNRset.Moving_AVG =
    TC317X_moving_avg_d (&gtTDMBCtrl[moduleidx].MonStat.SNRset.Array[0], gtTDMBCtrl[moduleidx].MonStat.SNRset.SNRdb,
        gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt, TDMB_MAX_MOV_AVG);
  // TSPER
  gtTDMBCtrl[moduleidx].MonStat.TSPERset.cnt = 1;
  TC317X_IO_memset(&gtTDMBCtrl[moduleidx].MonStat.TSPERset.Array[0], 0x00, sizeof(uint32_t)*TDMB_MAX_MOV_AVG);

  gtTDMBCtrl[moduleidx].MonStat.TSPERset.TSPER = _TDMB_MAX_TSPER_*SCALE_FACTOR;
  gtTDMBCtrl[moduleidx].MonStat.TSPERset.Moving_AVG =
    TC317X_moving_avg_d (&gtTDMBCtrl[moduleidx].MonStat.TSPERset.Array[0], gtTDMBCtrl[moduleidx].MonStat.TSPERset.TSPER,
        gtTDMBCtrl[moduleidx].MonStat.TSPERset.cnt, TDMB_MAX_MOV_AVG);
  // VITERBI
  gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.cnt = 1;
  TC317X_IO_memset(&gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Array[0], 0x00, sizeof(uint32_t)*TDMB_MAX_MOV_AVG);

  gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.VITERBIBER = _TDMB_MAX_VITERBIBER_*SCALE_FACTOR;
  gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Moving_AVG =
    TC317X_moving_avg_d (&gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.Array[0], gtTDMBCtrl[moduleidx].MonStat.VITERBIBERset.VITERBIBER,
        gtTDMBCtrl[moduleidx].MonStat.SNRset.cnt, TDMB_MAX_MOV_AVG);
  // RSSI
  gtTDMBCtrl[moduleidx].MonStat.RSSIset.cnt = 1;
  TC317X_IO_memset(&gtTDMBCtrl[moduleidx].MonStat.RSSIset.Array[0], 0x00, sizeof(uint32_t)*TDMB_MAX_MOV_AVG);

  gtTDMBCtrl[moduleidx].MonStat.RSSIset.RSSI = _TDMB_MIN_RSSI_;
  gtTDMBCtrl[moduleidx].MonStat.RSSIset.Moving_AVG =
    TC317X_moving_avg_d (&gtTDMBCtrl[moduleidx].MonStat.RSSIset.Array[0], gtTDMBCtrl[moduleidx].MonStat.RSSIset.RSSI,
        gtTDMBCtrl[moduleidx].MonStat.RSSIset.cnt, TDMB_MAX_MOV_AVG);
}

void TCC317X_MB_MCI_UPDATE (t_TC3X_DEMODULE * pTC3XD, uint32_t mci_no)
{
  TC317X_MB_TxOnly (pTC3XD, MBPARA_SYS_DAB_MCI_UPDATE, &mci_no, 1);
}

void TCC317X_MB_CH_CFG (t_TC3X_DEMODULE * pTC3XD, uint32_t sm_sa_sf_fe_ss)
{
  TC317X_MB_TxOnly (pTC3XD, MBPARA_SYS_DAB_STREAM_SET, &sm_sa_sf_fe_ss, 1);
}

void TCC317X_MB_DP_FILTER_CFG (t_TC3X_DEMODULE * pTC3XD, uint32_t sm_sa_sf_fe_ss)
{
  TC317X_MB_TxOnly (pTC3XD, MBPARA_SYS_DAB_STREAM_SET, &sm_sa_sf_fe_ss, 1);
}

void TCC317X_MB_RESYNC_PARAM_READ (t_TC3X_DEMODULE * pTC3XD, uint32_t sm_sa_sf_fe_ss)
{
  TC317X_MB_TxOnly (pTC3XD, MBPARA_SYS_DAB_STREAM_SET, &sm_sa_sf_fe_ss, 1);
}

void TCC317X_MB_RESYNC_PARAM_RESULT (t_TC3X_DEMODULE * pTC3XD, uint32_t sm_sa_sf_fe_ss)
{
  TC317X_MB_TxOnly (pTC3XD, MBPARA_SYS_DAB_STREAM_SET, &sm_sa_sf_fe_ss, 1);
}

