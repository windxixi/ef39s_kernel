//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_GPIO.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_GPIO.h"
#include "../IO/TC317X_IO_UTIL.h"

void TC317X_Set_GPIO (t_TC3X_DEMODULE * pTC317X, TC3X_MainIF MIF, TC3X_SUBIF_Header SH)
{
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (pTC317X->MasterMode)
  {
    // GPIOC - MAIN IF
    if (MIF.MainIFDefine == TC3X_IF_I2C)
    {
      // GPIOT - SUB STREAM IF
      // diversity
      if (SH.SubIFDefine == TC3X_STREAM_IO_MAINIO)
      {
        TC317XREGSET_GPIO_ALT (pTC317X, 0x00);
        TC317XREGSET_GPIO_DR (pTC317X, 0x0000);
        TC317XREGSET_GPIO_LR (pTC317X, 0x00);
        TC317XREGSET_GPIO_DRV (pTC317X, 0x00);
        TC317XREGSET_GPIO_PE (pTC317X, 0x00);
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_PTS)
      {
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_STS)
      {
        TC317XREGSET_GPIO_ALT (pTC317X, 0x0e3C);
        TC317XREGSET_GPIO_DR (pTC317X, 0x000);
        TC317XREGSET_GPIO_LR (pTC317X, 0x00);
        TC317XREGSET_GPIO_DRV (pTC317X, 0x00);
        TC317XREGSET_GPIO_PE (pTC317X, 0x00);
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_SPIMS)
      {
        TC317XREGSET_GPIO_ALT (pTC317X, 0x0e34);
        TC317XREGSET_GPIO_DR (pTC317X, 0x000);
        TC317XREGSET_GPIO_LR (pTC317X, 0x00);
        TC317XREGSET_GPIO_DRV (pTC317X, 0x00);
        TC317XREGSET_GPIO_PE (pTC317X, 0x00);
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_SPISLV)
      {
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_HPI_HEADERON)
      {
      }
      else if (SH.SubIFDefine == TC3X_STREAM_IO_HPI_HEADEROFF)
      {
      }
      else
      {
      }
    }
    else if (MIF.MainIFDefine == TC3X_IF_SRAMLIKE)
    {
    }
    else if (MIF.MainIFDefine == TC3X_IF_CSPI)
    {
    }
    else if (MIF.MainIFDefine == TC3X_IF_SDIO1BIT || MIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    {
    }
    else
    {
    }
  }
  else
  {
#ifdef STREAM_MIXER
    TC317XREGSET_GPIO_ALT (pTC317X, 0x01c0);
    TC317XREGSET_GPIO_DR (pTC317X, 0x0000);
    TC317XREGSET_GPIO_LR (pTC317X, 0x00);
    TC317XREGSET_GPIO_DRV (pTC317X, 0x00);
    TC317XREGSET_GPIO_PE (pTC317X, 0x00);
#else
    TC317XREGSET_GPIO_ALT (pTC317X, 0x01c0);
    TC317XREGSET_GPIO_DR (pTC317X, 0x0000);
    TC317XREGSET_GPIO_LR (pTC317X, 0x00);
    TC317XREGSET_GPIO_DRV (pTC317X, 0x00);
    TC317XREGSET_GPIO_PE (pTC317X, 0x00);
#endif
  }
}
