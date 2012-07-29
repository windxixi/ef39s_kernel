//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TCC3170.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#include "TCC3170RF.h"
#include "../IO/TC317X_IO.h"
#include "TC317X_struc.h"
#include "TC317X_Core_RegCtrl.h"

//--------------------------------------------------------------------
// TC3X_DNUM : It used for BB numering.  0:master 1:slave1, 2: slave2, ...

tTCC3170RF_GVar **pTCC3170RF_GVar = NULL;

typedef uint32_t (*TCC3170RF_ReadIIC_For_BB_fn_t) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t Addr);
typedef void      (*TCC3170RF_WriteIIC_For_BB_fn_t) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t Addr, uint32_t Data);
typedef void      (*TCC3170RF_WriteIICEx_For_BB_fn_t) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t *pAddr, uint32_t *pData, int32_t cnt);

uint64_t TC317X_Div64(uint64_t x, uint64_t y);

TCC3170RF_ReadIIC_For_BB_fn_t TCC3170RF_ReadIIC_For_BB = NULL;
TCC3170RF_WriteIIC_For_BB_fn_t TCC3170RF_WriteIIC_For_BB = NULL;
TCC3170RF_WriteIICEx_For_BB_fn_t TCC3170RF_WriteIICEx_For_BB = NULL;


#ifndef Bit0
#define	Bit31		0x80000000
#define	Bit30		0x40000000
#define	Bit29		0x20000000
#define	Bit28		0x10000000
#define	Bit27		0x08000000
#define	Bit26		0x04000000
#define	Bit25		0x02000000
#define	Bit24		0x01000000
#define	Bit23		0x00800000
#define	Bit22		0x00400000
#define	Bit21		0x00200000
#define	Bit20		0x00100000
#define	Bit19		0x00080000
#define	Bit18		0x00040000
#define	Bit17		0x00020000
#define	Bit16		0x00010000
#define	Bit15		0x00008000
#define	Bit14		0x00004000
#define	Bit13		0x00002000
#define	Bit12		0x00001000
#define	Bit11		0x00000800
#define	Bit10		0x00000400
#define	Bit9		0x00000200
#define	Bit8		0x00000100
#define	Bit7		0x00000080
#define	Bit6		0x00000040
#define	Bit5		0x00000020
#define	Bit4		0x00000010
#define	Bit3		0x00000008
#define	Bit2		0x00000004
#define	Bit1		0x00000002
#define	Bit0		0x00000001
#define	BitNONE 	0x00000000
#endif

#ifndef BITSET
#define	BITSET(X, MASK)				( (X) |= (uint32_t)(MASK) )
#endif
#ifndef BITCLR
#define	BITCLR(X, MASK)				( (X) &= ~((uint32_t)(MASK)) )
#endif

TC3X_DNUM TCC3170RF_Entry (int32_t moduleidx, TC3X_DNUM hBB,
    FN_i_ihi * RF_Init,
    FN_v_ih * RF_Close, FN_v_ihii * RF_SetFrequency, FN_v_ihi * RF_CtrlPower, FN_i_ihivvvv * RF_UserFunction)
{
  pTCC3170RF_GVar[moduleidx][hBB].hBB = hBB;
  pTCC3170RF_GVar[moduleidx][hBB].hRF = hBB;

  if (RF_Init)
    *RF_Init = TCC3170RF_RF_Init;
  if (RF_Close)
    *RF_Close = TCC3170RF_RF_Close;
  if (RF_SetFrequency)
    *RF_SetFrequency = TCC3170RF_RF_SetFrequency;
  if (RF_CtrlPower)
    *RF_CtrlPower = TCC3170RF_RF_CtrlPower;
  if (RF_UserFunction)
    *RF_UserFunction = TCC3170RF_RF_UserFunction;

  return pTCC3170RF_GVar[moduleidx][hBB].hRF;
}

void TCC3170RF_AttachDevice (int32_t moduleidx, int32_t num)
{
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (!pTCC3170RF_GVar)
  {
    pTCC3170RF_GVar = (tTCC3170RF_GVar **) TC317X_IO_malloc (maxbb * sizeof (tTCC3170RF_GVar));
    TC317X_IO_memset (&pTCC3170RF_GVar[0], 0, maxbb * sizeof (tTCC3170RF_GVar));
  }

  if (!pTCC3170RF_GVar[moduleidx])
  {
    pTCC3170RF_GVar[moduleidx] = (tTCC3170RF_GVar *) TC317X_IO_malloc (sizeof (tTCC3170RF_GVar) * num);
  }

  TC317X_IO_memset (&pTCC3170RF_GVar[moduleidx][0], 0, sizeof (tTCC3170RF_GVar) * num);
}

void TCC3170RF_DetachDevice (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;

  if (pTCC3170RF_GVar[moduleidx])
    TC317X_IO_free (pTCC3170RF_GVar[moduleidx]);
  pTCC3170RF_GVar[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (pTCC3170RF_GVar[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (pTCC3170RF_GVar);
    pTCC3170RF_GVar = NULL;
  }
}

int32_t TCC3170RF_RF_Init (int32_t moduleidx, TC3X_DNUM hRF, int32_t band)
{
  //uint32_t idata, i;

  if(band== 0)		/* band III */
  {
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x02, 0x00030000);

    TCC3170RF_WriteIIC (moduleidx, hRF, 0x04, 0x00002002);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x05, 0x5500640b);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x06, 0x1AADA754);
    //		TCC3170RF_WriteIIC (moduleidx, hRF, 0x06, 0x0AADA754);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x07, 0x00002000);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x08, 0x42222249);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x09, 0x000d2299);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0a, 0xD87060DD);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0b, 0x00000e72);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0c, 0x02AE7077);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0d, 0x00240376);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0e, 0x7F7F0711);
#if 0
    for(i = 0 ; i < 0xf; i ++)
    {
      idata = TCC3170RF_ReadIIC(moduleidx, hRF, i);
      LOGD("[%s:%d] addr:0x%02X, data:%08X \n", __func__, __LINE__, i, idata);
    }
#endif
  }
  else		/* LBAND */
  {
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x02, 0x00030000);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x04, 0x00004001);
    //		TCC3170RF_WriteIIC (moduleidx, hRF, 0x05, 0x5500640b);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x06, 0x1AADA754);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x06, 0x0AADA754);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x07, 0x00002000);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x08, 0x42222249);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x09, 0x000d2299);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0a, 0xD87060DD);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0b, 0x00000e72);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0c, 0x02AE7077);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0d, 0x00240376);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0e, 0x7F7F0907);
  }
  //    LOGD("%s:%d band=%d\n", __func__, __LINE__, band);

  return 0;
}

void TCC3170RF_RF_Close (int32_t moduleidx, TC3X_DNUM hRF)
{
  TCC3170RF_DetachDevice (moduleidx);
}

#define Scale       22
#define Fixed(x)    (x<<Scale)
#define Mul(A,B)    ((A*B)>>Scale)
#define Div(A,B)    (TC317X_Div64((A<<Scale), B))

void TCC3170RF_RF_SetFrequency (int32_t moduleidx, TC3X_DNUM hRF,
    int32_t freq_khz, int32_t bw_khz)
{
  uint64_t N, F;
  uint64_t Flo, VCO_DIV, FOffset, Fvco, FR, PLL_MODE;
  uint64_t N_int, intF, intVCO_DIV;//, REG_VCO_DIV, RCNT_RDC;
  uint64_t fXtal, fpfd, f_freq_khz;
  uint64_t Temp, Temp1, Temp2;
  uint32_t OSCCLK;
  uint32_t DIV_MODE;
  int32_t RST_PLL = 1;
  int32_t band = 0;

  FOffset = 0;

  if(freq_khz > 1000000)//l-band
    band = 1;

#if 0
  TCC3170RF_RF_Init(moduleidx, hRF, band);
#else
  if(band== 0)    /* band III */
  {
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x04, 0x00002002);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0e, 0x7F7F0711);
  }
  else    /* LBAND */
  {
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x04, 0x00004001);
    TCC3170RF_WriteIIC (moduleidx, hRF, 0x0e, 0x7F7F0907);
  }
#endif

#if 1
  Temp = TCC3170RF_ReadIIC (moduleidx, hRF, 0x06);
  if(((Temp>>28)&0x01)==0)
    FR = 0;
  else
    FR = 1;
#else
  FR = 1;
#endif

#if 1
  if(((Temp>>31)&0x01)==0)
    PLL_MODE = 2;
  else
    PLL_MODE = 4;
#else
  PLL_MODE = 2;
#endif

#if 0
  Temp = TCC3170RF_ReadIIC (moduleidx, hRF, 0x08);
  if(((Temp>>7)&0x01)==0)
    VCO_DIV = 16;
  else
    VCO_DIV = 2;
#endif

  OSCCLK = DEF_OSCCLK_RF;
  fXtal = OSCCLK;
  f_freq_khz = freq_khz;

  // Calculate PLL
  if (f_freq_khz < 250000) {
    // VHF
    DIV_MODE = 0x00;
    fpfd = fXtal >> FR;
    VCO_DIV = 16;

    Flo = f_freq_khz - FOffset;
    Fvco = Flo * VCO_DIV;

    Temp1 = Fvco<<FR;
    Temp2 = PLL_MODE*fXtal;
    N = Div(Temp1, Temp2);
    N_int = (N>>Scale) << Scale;
    F = ((N-N_int) * (2 << 21)) >> Scale;

    if (N_int < (19<<Scale) ) {
      FR = 1;
      fpfd = fXtal >> FR;
      VCO_DIV = 16;
      Flo = f_freq_khz - FOffset;
      Fvco = Flo * VCO_DIV;

      Temp1 = Fvco*FR;
      Temp2 = PLL_MODE*fXtal;
      N = Div(Temp1, Temp2);
      N_int = (N>>Scale) << Scale;
      F = ((N - N_int) * (2<<21)) >> Scale;
    }
    intF = F;
    intVCO_DIV = VCO_DIV;
  }
  else {
    // LBAND
    DIV_MODE = 0x01;
    fpfd = fXtal >> FR;
    VCO_DIV = 2;

    Flo = f_freq_khz - FOffset;
    Fvco = Flo * VCO_DIV;

    Temp1 = Fvco << FR;
    Temp2 = PLL_MODE*fXtal;
    N = Div(Temp1, Temp2);//Div(Temp1, Temp2);
    N_int = (N>>Scale) << Scale;
    F = ((N-N_int) * (2<<21)) >> Scale;

    if (N_int < (19<<Scale)) {
      FR = 1;

      VCO_DIV = 2;
      Flo = f_freq_khz - FOffset;
      Fvco = Flo * VCO_DIV;

      Temp1 = Fvco<<FR;
      Temp2 = PLL_MODE*fXtal;
      N = Div(Temp1, Temp2);
      N_int = (N>>Scale) << Scale;
      F = ((N-N_int) * (2<<21)) >> Scale;
    }
    intF = F;
    intVCO_DIV = VCO_DIV;
  }

  Temp = ((N_int>>Scale) & 0xFF) ;
  Temp |= ((intF & 0x3FFFFF)<<8);
  Temp |= ((RST_PLL&0x01) << 30);
  Temp |= ((DIV_MODE&0x01) << 31);
  TCC3170RF_WriteIIC (moduleidx, hRF, 0x08, Temp);

  Temp = TCC3170RF_ReadIIC (moduleidx, hRF, 0x06);
  if(FR==0)
    BITCLR(Temp, Bit28);
  else
    BITSET(Temp, Bit28);

  TCC3170RF_WriteIIC (moduleidx, hRF, 0x06, Temp);
  //LOGD("[%s:%d] end\n", __func__, __LINE__);
}

void TCC3170RF_RF_CtrlPower (int32_t moduleidx, TC3X_DNUM hRF, int32_t param1)    // param1 1:on 0:off
{
}

int32_t TCC3170RF_RF_UserFunction (int32_t moduleidx, TC3X_DNUM hRF, int32_t message, void *param1, void *param2, void *param3, void *param4)
{
  int32_t       ret = 0;

  switch (message)
  {
    case CMD_RF_READ_REG_SINGLE:
      {
        // param1 : addr
        uint8_t addr;
        addr = *(uint8_t *) param1;
        ret = TCC3170RF_ReadIIC (moduleidx, hRF, addr);
      }
      break;

    case CMD_RF_WRITE_REG_SINGLE:
      {
        // param1 : addr, param2 : data
        uint8_t addr;
        uint8_t data;
        addr = *(uint8_t *) param1;
        data = *(uint8_t *) param2;
        TCC3170RF_WriteIIC (moduleidx, hRF, addr, data);
      }
      break;

    case CMD_RF_GET_PLL_LOCKSTAT:
      break;

    case MSG_SET_FN_RF_READ:
      TCC3170RF_ReadIIC_For_BB = (TCC3170RF_ReadIIC_For_BB_fn_t)param1;//(uint8_t (*)(int, TC3X_DNUM, uint8_t, uint8_t)) param1;
      break;

    case MSG_SET_FN_RF_WRITE:
      TCC3170RF_WriteIIC_For_BB = (TCC3170RF_WriteIIC_For_BB_fn_t)param1;//(void (*)(int, TC3X_DNUM, uint8_t, uint8_t, uint8_t)) param1;
      break;

    case MSG_SET_FN_RF_WRITEEX:
      TCC3170RF_WriteIICEx_For_BB = (TCC3170RF_WriteIICEx_For_BB_fn_t)param1;//(void (*)(int, TC3X_DNUM, uint8_t, uint8_t *, uint8_t *, int)) param1;
      break;

    default:
      break;
  }
  //LOGD("[%s:%d] message:%d\n", __func__, __LINE__, message);
  return ret;
}

uint32_t TCC3170RF_ReadIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr)
{
  if(TCC3170RF_ReadIIC_For_BB == NULL) return -1;

  return (TCC3170RF_ReadIIC_For_BB (moduleidx, pTCC3170RF_GVar[moduleidx][hRF].hBB, 0x00, Addr));
}

void TCC3170RF_WriteIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr, uint32_t Data)
{
  if(TCC3170RF_WriteIIC_For_BB == NULL) return;

  TCC3170RF_WriteIIC_For_BB (moduleidx, pTCC3170RF_GVar[moduleidx][hRF].hBB, 0x00, Addr, Data);
}

void TCC3170RF_WriteIICEx (int32_t moduleidx, TC3X_DNUM hRF, uint8_t *pAddr, uint32_t *pData, int32_t cnt)
{
  if(TCC3170RF_WriteIICEx_For_BB == NULL) return;

  TCC3170RF_WriteIICEx_For_BB (moduleidx, pTCC3170RF_GVar[moduleidx][hRF].hBB, 0x00, pAddr, pData, cnt);
}

void TCC3170RF_WriteIIC_RegData (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Taddr, uint32_t Tdata, int32_t bitposition, int32_t bitcount)
{
  uint32_t       Data;
  int32_t       bitmask, i, bitmask2;
  bitmask = 0xff;
  bitmask2 = 0x0;
  for (i = bitposition; i < bitposition + bitcount; i++)
  {
    bitmask = bitmask - TCC3170RF_pow2_user (i);
    bitmask2 = bitmask2 + TCC3170RF_pow2_user (i);
  }
  bitmask2 = bitmask2 / (TCC3170RF_pow2_user (bitposition));
  Tdata = Tdata & bitmask2;
  Data = TCC3170RF_ReadIIC (moduleidx, hRF, Taddr);
  Data = (Data & bitmask) + (Tdata << bitposition);
  TCC3170RF_WriteIIC (moduleidx, hRF, Taddr, Data);
}

uint32_t TCC3170RF_ReadIIC_RegData (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Taddr, int32_t bitposition, int32_t bitcount)
{
  int32_t       Data;
  int32_t       bitmask, i;
  bitmask = 0x00;
  for (i = bitposition; i < bitposition + bitcount; i++)
  {
    bitmask = bitmask + (int) (TCC3170RF_pow2_user (i));
  }
  Data = TCC3170RF_ReadIIC (moduleidx, hRF, Taddr);
  Data = (Data & bitmask) >> bitposition;
  return Data;
}

void TCC3170RF_WriteIIC_FormedData (int32_t moduleidx, TC3X_DNUM hRF, uint32_t *pData, uint32_t length)
{
  // max16
  int32_t       j;
  int32_t       cnt;
  int32_t       remain;
  int32_t       i;
  uint8_t TargetAddr[16];
  uint32_t Data[16];
  int32_t       maxcnt;

  //maxcnt = 4;
  maxcnt = 16;

  cnt = length / maxcnt;
  remain = length % maxcnt;

  if (cnt)
  {
    for (j = 0; j < cnt; j++)
    {
      for (i = 0; i < maxcnt; i++)
      {
        TargetAddr[i] = pData[j * maxcnt * 2 + i * 2];
        Data[i] = pData[j * maxcnt * 2 + i * 2 + 1];
      }
      TCC3170RF_WriteIICEx (moduleidx, hRF, &TargetAddr[0], &Data[0], maxcnt);
    }
  }

  if (remain)
  {
    for (i = 0; i < remain; i++)
    {
      TargetAddr[i] = pData[cnt * maxcnt * 2 + i * 2];
      Data[i] = pData[cnt * maxcnt * 2 + i * 2 + 1];
    }
    TCC3170RF_WriteIICEx (moduleidx, hRF, &TargetAddr[0], &Data[0], remain);
  }
}

void TCC3170RF_SetOutBW (int32_t moduleidx, TC3X_DNUM hRF, int32_t bw_khz)
{
  switch (bw_khz)
  {
    case 8000:
      break;
    case 7000:
      break;
    case 6000:
      break;
    case 5000:
      break;
    default:
      break;
  }
}

int32_t TCC3170RF_pow2_user (int32_t nval)                           // added by jhj
{
  int32_t       retv;

  if (nval == 0)
  {
    retv = 1;
  }
  else if (nval == 1)
  {
    retv = 2;
  }
  else
  {
    retv = 2 << (nval - 1);
  }
  return retv;
}
