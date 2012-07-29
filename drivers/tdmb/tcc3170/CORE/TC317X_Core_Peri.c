//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_PERI.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_Peri.h"

extern t_TC3X_DEMODULE **g_pTC3X;

void TC317X_Set_PERIperalForStream (t_TC3X_DEMODULE * pTC317X, TC3X_SUBIF_Header SH)
{
  int32_t       optionvalue0;
  int32_t       optionvalue1;
  int32_t       optionvalue2;

  if (SH.SubIFValue == TC3XREG_PERI_SEL_SPI)
  {
    optionvalue0 = SH.oSPI.Protocol | SH.oSPI.SlaveON | SH.oSPI.FastMode | SH.oSPI.Size;
    optionvalue1 = SH.oSPI.SPH | SH.oSPI.SPO | SH.oSPI.ByteMSB | SH.oSPI.BitMSB;
    TC317XREGSET_PERI_SPI_MODE0 (pTC317X, optionvalue0, SH.oSPI.DLR);
    TC317XREGSET_PERI_SPI_MODE1 (pTC317X, optionvalue1, SH.oSPI.ReadyTime, SH.oSPI.WaitTime);
    TC317XREGSET_PERI_CTRL (pTC317X, 0x90);     //ksj 0x00
  }
  else if (SH.SubIFValue == TC3XREG_PERI_SEL_TS)
  {
    optionvalue0 = SH.SubIFValue | SH.HeaderON | SH.oTS.MSM_Tssync | SH.oTS.Parallel | SH.oTS.FastMode | SH.oTS.TSClkMask;
    optionvalue1 = SH.oTS.TSClk_POL_Low | SH.oTS.TSSync_POL_Low | SH.oTS.TSEN_POL_Low | SH.oTS.TSStreamWaitON;
    optionvalue2 = SH.oTS.ByteMSB | SH.oTS.BitMSB;
    TC317XREGSET_PERI_TS_MODE0 (pTC317X, optionvalue0, SH.oTS.DLR);
    TC317XREGSET_PERI_TS_MODE1 (pTC317X, optionvalue1, SH.oTS.TSStreamWaitTime);
    TC317XREGSET_PERI_TS_MODE2 (pTC317X, optionvalue2, SH.oTS.MSMTSSyncHighTime, SH.oTS.MSMTSSyncLowTime);
    TC317XREGSET_PERI_TS_MODE3 (pTC317X, TC3XREG_PERI_TS_ERR_SIG_OFF);
    //TC317XREGSET_PERI_CTRL (pTC317X, 0xA0);     //ksj 0x00
  }
  else if (SH.SubIFValue == TC3XREG_PERI_SEL_HPI)
  {
    optionvalue0 = SH.oHPI.IntelMode | SH.oHPI.ByteMSB | SH.oHPI.BitMSB;
    TC317XREGSET_PERI_HPI_MODE0 (pTC317X, optionvalue0);
  }
  else
  {
    TC317XREGSET_PERI_CTRL (pTC317X, 0x80);     //ksj 0x00
  }
}

// only Set Master
void TC317X_PERI_EN (t_TC3X_DEMODULE * pTC317X)
{
  switch (g_pTC3X[pTC317X->moduleidx][0].SubIF.SubIFValue)
  {
    case TC3X_STREAM_IO_MAINIO:
      TC317XREGSET_PERI_CTRL (pTC317X, 0x80);     //ksj 0x00
      break;

    case TC3XREG_PERI_SEL_SPI:
    case TC3XREG_PERI_SEL_TS:
    case TC3XREG_PERI_SEL_HPI:
      TC317XREGSET_PERI_CTRL (pTC317X, 
        TC3XREG_PERI_EN | 
        pTC317X->SubIF.SubIFValue | 
        TC3XREG_PERI_INIT_AUTOCLR | 
        pTC317X->SubIF.HeaderON);
      break;
  }
}

void TC317X_PERI_DN (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;

  value = TC317XREGGET_PERI_CTRL (pTC317X);
  value &= ~(TC3XREG_PERI_EN);
  TC317XREGSET_PERI_CTRL (pTC317X, value);
}
