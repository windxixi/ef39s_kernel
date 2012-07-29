//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Util.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/15   stkim              Draft
//=============================================================================
#include "../IO/TC317X_IO.h"
#include "TC317X_Core_Mailbox.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_Util.h"
#include "TC317X_deviceAPI.h"
#include "TC317X_Core.h"

//-----------------------------------------------------------------------------
// Stream Buffer Control Util
//

//#define MAXADDRCNT    (128)
#define MAXADDRCNT	(256)

#ifndef NULL
#define NULL 0
#endif

typedef struct
{
  uint8_t *addr[MAXADDRCNT];
  uint32_t widx;
  uint32_t ridx;
  uint32_t maxpktnum;
  int32_t       countpacket;
} t_buffctrl;

typedef struct
{
  uint32_t curridx;
  uint32_t maxpktnum;
} t_StreamBuffCtrl;

t_buffctrl buffctrl[MAX_TCC3X];
t_StreamBuffCtrl StreamBuffCtrl[MAX_TCC3X];

extern t_TC3X_DEMODULE **g_pTC3X;

void StreamBuffCtrl_Init (int32_t moduleidx, int32_t MAXCNT)
{
  StreamBuffCtrl[moduleidx].curridx = 0;
  StreamBuffCtrl[moduleidx].maxpktnum = MAXCNT;
}

void buffctrl_init (int32_t moduleidx, uint32_t maxpktnum) // recommand 128pktnum
{
  TC317X_IO_memset (&buffctrl[moduleidx], 0, sizeof (buffctrl[moduleidx]));

  if (maxpktnum > MAXADDRCNT)
    return;

  buffctrl[moduleidx].maxpktnum = maxpktnum;

  StreamBuffCtrl_Init (moduleidx, maxpktnum);
}

void buffctrl_push (int32_t moduleidx, uint8_t *ptr)
{
  buffctrl[moduleidx].addr[buffctrl[moduleidx].widx] = ptr;
  buffctrl[moduleidx].widx = (buffctrl[moduleidx].widx + 1) % buffctrl[moduleidx].maxpktnum;
  buffctrl[moduleidx].countpacket++;
}

uint8_t *buffctrl_get (int32_t moduleidx)
{
  uint8_t *ptr = NULL;

  if (buffctrl[moduleidx].widx == buffctrl[moduleidx].ridx)
  {
    return NULL;
  }

  if (buffctrl[moduleidx].addr[buffctrl[moduleidx].ridx] == NULL)
  {
    PRINTF_LV_0 ("# BufferCtrlErr[NULL ptr]\n");
    return NULL;
  }

  ptr = buffctrl[moduleidx].addr[buffctrl[moduleidx].ridx];
  buffctrl[moduleidx].ridx = (buffctrl[moduleidx].ridx + 1) % buffctrl[moduleidx].maxpktnum;
  buffctrl[moduleidx].countpacket--;

  return ptr;
}

uint32_t buffctrl_GetPacketNum (int32_t moduleidx)
{
  uint32_t retv = 0;

  if (buffctrl[moduleidx].widx == buffctrl[moduleidx].ridx)
    retv = 0;
  else if (buffctrl[moduleidx].widx > buffctrl[moduleidx].ridx)
    retv = buffctrl[moduleidx].widx - buffctrl[moduleidx].ridx;
  else
    retv = buffctrl[moduleidx].widx + (buffctrl[moduleidx].maxpktnum - buffctrl[moduleidx].ridx);

  return retv;
}

void buffctrl_flush (int32_t moduleidx)
{
  uint32_t temp;

  temp = buffctrl[moduleidx].maxpktnum;
  TC317X_IO_memset (&buffctrl[moduleidx], 0, sizeof (buffctrl[moduleidx]));
  buffctrl[moduleidx].maxpktnum = temp;
}

uint32_t buffctrl_GetAvailBurstWNum (int32_t moduleidx)
{
  uint32_t AvailBurstNum;

  AvailBurstNum = buffctrl[moduleidx].maxpktnum - buffctrl[moduleidx].widx;

  return AvailBurstNum;
}

uint32_t buffctrl_GetWriteIdx (int32_t moduleidx)
{
  uint32_t widx;

  widx = buffctrl[moduleidx].widx;

  return widx;
}

int32_t StreamBuffCtrl_GetCurr (int32_t moduleidx)
{
  int32_t       curridx;

  curridx = StreamBuffCtrl[moduleidx].curridx;

  return curridx;
}

int32_t StreamBuffCtrl_GetAvail (int32_t moduleidx)
{
  int32_t       ret;

  ret = StreamBuffCtrl[moduleidx].maxpktnum - StreamBuffCtrl[moduleidx].curridx;

  return ret;
}

int32_t StreamBuffCtrl_GetRoop (int32_t moduleidx, int32_t cnt)
{
  int32_t       ret;

  ret = (cnt / StreamBuffCtrl[moduleidx].maxpktnum);

  return ret;
}

void StreamBuffCtrl_PushCnt (int32_t moduleidx, int32_t cnt)
{
  StreamBuffCtrl[moduleidx].curridx = ((StreamBuffCtrl[moduleidx].curridx + cnt) % StreamBuffCtrl[moduleidx].maxpktnum);
}

uint32_t gsynccnt = 0;
int32_t TSSyncCheck (uint8_t *src)
{
  if (src[0] != 0x47)
  {
    gsynccnt = ((gsynccnt + 1) % 50);

    if (gsynccnt == 0)
    {
      //PRINTF_LV_1 ("# x[%02x]", src[0]);
    }
    return 1;
  }
  return 0;
}

#if defined (USE_ANDROID)
uint64_t TC317X_GetTickCnt (void)
{
  return (TC317X_IO_GET_TIMECNT_ms ());
}

uint64_t TC317X_GetInterval (uint64_t startTick)
{
  int64_t Cnt;
  if (TC317X_GetTickCnt () > startTick)
    Cnt = (TC317X_GetTickCnt () - startTick) * 1000;
  else
    Cnt = ((ULLONG_MAX - startTick) + TC317X_GetTickCnt () + 1) * 1000;

  return Cnt;
}
#else
uint32_t TC317X_GetTickCnt (void)
{
  return (TC317X_IO_GET_TIMECNT_us ());
}

uint32_t TC317X_GetInterval (uint32_t startTick)
{
  uint32_t Cnt;

  if (TC317X_GetTickCnt () > startTick)
    Cnt = TC317X_GetTickCnt () - startTick;
  else
    Cnt = ((0xFFFFFFFF - startTick) + TC317X_GetTickCnt () + 1);

  return Cnt;
}
#endif // USE_ANDROID

//---------------------------------------------------------------------------------------------
// Status Monitoring Util
//
// please input *1024! (<<10bit shift)
uint32_t TC317X_log10_ (unsigned int val)
{
  unsigned int i, SNR = 0;
  unsigned int snr_table[28] = {
    1024,
    1289,
    1622,
    2043,
    2572,
    3238,
    4076,
    5132,
    6461,
    8133,
    10240,
    12891,
    16229,
    20431,
    25721,
    32381,
    40766,
    51321,
    64610,
    81339,
    102400,
    128913,
    162293,
    204314,
    257217,
    323817,
    407661,
    513215
  };

  if (val < snr_table[0])
  {
    SNR = 0;
  }
  else if (val >= snr_table[27])
  {
    SNR = 27;
  }
  else
  {
    for (i = 0; i < 27; i++)
    {
      if (val >= snr_table[i] && val < snr_table[i + 1])
      {
        SNR = i;
        break;
      }
    }
  }

  return SNR;
}

uint32_t TC317X_Pow2 (int32_t pow)
{
  uint32_t result;

  if (pow < 0)
    result = 0;
  else if (pow == 0)
    result = 1;
  else
    result = 2 << (pow - 1);

  return result;
}

uint64_t TC317X_Div64(uint64_t x, uint64_t y)
{
  uint64_t a, b, q, counter;

  q = 0;
  if (y != 0) {
    while (x >= y) {
      a = x >> 1;
      b = y;
      counter = 1;
      while (a >= b) {
        b <<= 1;
        counter <<= 1;
      }
      x -= b;
      q += counter;
    }
  }
  return q;
}


uint32_t TC317X_moving_avg_d (uint32_t *pArray, uint32_t input,
    uint32_t cnt, int32_t MAX_ARRAY)
{
  int32_t j;
  uint32_t sum = 0;

  TC317X_IO_memcpy (pArray, &pArray[1], (MAX_ARRAY - 1) * sizeof (uint32_t));
  pArray[MAX_ARRAY - 1] = input;

  for (j = 0; j < (int) cnt; j++)
  {
    if (MAX_ARRAY - j - 1 < 0)
      break;

    sum += pArray[MAX_ARRAY - j - 1];
  }

  return (sum / cnt);
}

void Parse_TDMBSyncStat (t_TDMBLock * pTDMBLock, uint8_t input)
{
  pTDMBLock->AGC = (input) & 0x01;
  pTDMBLock->DC = 1;
  pTDMBLock->CTO = (input >> 1) & 0x01;
  pTDMBLock->CFO = (input >> 2) & 0x01;
  pTDMBLock->FFO = 1;
  pTDMBLock->FTO = (input >> 3) & 0x01;
  pTDMBLock->SYNC_BYTE = (input >> 4) & 0x01;
  pTDMBLock->OFDM_DETECT = (input >> 5) & 0x01;
}

//------------------------------------------------------------------------------
