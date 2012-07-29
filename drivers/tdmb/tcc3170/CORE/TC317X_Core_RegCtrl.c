//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_RegCtrl.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#include "TC317X_Core_RegCtrl.h"

#include "../IO/TC317X_IO.h"
#if defined (USE_IF_SDIO)
#include "../IO/TC317X_IO_SDIO.h"
#endif // USE_IF_SDIO

extern t_TC3X_DEMODULE **g_pTC3X;

//====================================================================
//
// Manual Control Register
//
//====================================================================
void TC317XREGSET_Manual (t_TC3X_DEMODULE * pTC317X, uint32_t addr, uint32_t value, int32_t innersem)
{
  if (addr == TC3XREG_SYS_EN)
  {
    TC317XREGSET_SYSEN (pTC317X, value, innersem);
  }
  else
  {
    if (innersem)
      pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, addr, value);
    else
      pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, addr, value);
  }
}

uint32_t TC317XREGGET_Manual (t_TC3X_DEMODULE * pTC317X, uint32_t addr, int32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, addr, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, addr, NULL);

  return value;
}

//====================================================================
//
// System Control Register
//
//====================================================================

void TC317XREGSET_SYSEN (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t innersem)
{
  TC317X_IO_OP_Mailbox_LOCK(pTC317X->moduleidx);
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_SYS_EN, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_SYS_EN, value);

  pTC317X->OP_Status = value;
  TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
}

uint32_t TC317XREGGET_SYSEN (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_SYS_EN, NULL);
  return value;
}

void TC317XREGSET_SYSRESET (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_SYS_RESET, value);
}

void TC317XREGSET_IRQMODE (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_MODE, value);
}

void TC317XREGSET_IRQEN (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_EN, value);
}

uint32_t TC317XREGGET_IRQEN (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_EN, NULL);
  return value;
}

void TC317XREGSET_IRQCLR (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_STAT_CLR, value);
}

uint32_t TC317XREGGET_IRQSTAT (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value = 0;

  TC317XREGSET_LATCH_IRQSTAT (pTC317X);
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_STAT_CLR, NULL);
#else
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_IRQ_STAT_CLR, (uint8_t*)&value, 2);
#endif
  return SWAP16(value);
}

uint32_t TC317XREGGET_IRQERROR (t_TC3X_DEMODULE * pTC317X)
{
  return pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_ERROR, NULL);
}

void TC317XREGSET_IRQERROR (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_IRQ_ERROR, value);
}

void TC317XREGSET_PLL_6 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PLL_6, value);
}

void TC317XREGSET_PLL_7 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PLL_7, value);
}

void TC317XREGSET_PLL_8 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PLL_8, value);
}

void TC317XREGSET_PLL_9 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PLL_9, value);
}

void TC317XREGSET_REMAP (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_REMAP, value);
}

void TC317XREGSET_REMAP_PC (t_TC3X_DEMODULE * pTC317X, uint32_t remap_value, uint32_t pc_value8, uint32_t pc_value0)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_REMAP, remap_value);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC8, pc_value8);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC0, pc_value0);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_SYS_RESET, TC3XREG_SYS_RESET_DSP);
}

void TC317XREGSET_PC (t_TC3X_DEMODULE * pTC317X, uint32_t pc_value8, uint32_t pc_value0)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC8, pc_value8);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC0, pc_value0);
}

uint32_t TC317XREGGET_REMAP (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value, v1, v2, v3;

  v1 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_REMAP, NULL);
  v2 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC8, NULL);
  v3 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_INIT_PC0, NULL);
  value = (v1 << 16) | (v2 << 8) | v3;

  return value;
}

void TC317XREGSET_CHIPAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CHIPADDR, value);
}

uint32_t TC317XREGGET_PROGRAMID (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PROGRAMID, NULL);

  return value;
}

uint32_t TC317XREGGET_CHIPID (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CHIPID, NULL);

  return value;
}

void TC317XREGSET_LATCH_IRQSTAT (t_TC3X_DEMODULE * pTC317X)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_READ_IRQSTAT, 0x5E);        // for read syncronized
}

void TC317XREGSET_GPIO_ALT (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_ALT8, ((value >> 8) & 0xff));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_ALT0, (value & 0xff));
}

void TC317XREGSET_GPIO_DR (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_DR8, ((value >> 8) & 0xff));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_DR0, (value & 0xff));
}

void TC317XREGSET_GPIO_LR (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_LR8, ((value >> 8) & 0xff));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_LR0, (value & 0xff));
}

void TC317XREGSET_GPIO_DRV (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_DRV8, ((value >> 8) & 0xff));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_DRV0, (value & 0xff));
}

void TC317XREGSET_GPIO_PE (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_PE8, ((value >> 8) & 0xff));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_GPIO_PE0, (value & 0xff));
}

void TC317XREGSET_DIVIO (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_DIVIO, value);
}

void TC317XREGSET_STREAMMIX (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAMMIX_CFG0, value);
}

void TC317XREGRESET_CMD_FIFO(t_TC3X_DEMODULE * pTC317X)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG3, 0x12);
}

void TC317XREGSET_STREAM_CFG (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO 
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG0, (value >> 24) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG1, (value >> 16) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG2, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG3, (value >> 0) & 0xff);
#else
  value = SWAP32(value);
  pTC317X->TC3X_Reg_WriteEx(pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_STREAM_CFG0, (unsigned char*)&value, 4);
#endif
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x30, 0x80);
}

uint32_t TC317XREGGET_STREAM_DATA_SIZE(t_TC3X_DEMODULE * pTC317X)
{
  int32_t value;
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx,
      pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG3, 0x22); //latch total size

  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx,
      pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG1, NULL) << 8;
  value |= pTC317X->TC3X_Reg_Read (pTC317X->moduleidx,
      pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG2, NULL);

  value <<= 2;
  return value;
}
//====================================================================
//
// Command DMA Register
//
//====================================================================
void TC317XREGSET_CMDDMA_CTRL (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CTRL, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CTRL, value);
}

void TC317XREGSET_CMDDMA_SourceAddr (t_TC3X_DEMODULE * pTC317X, uint32_t addr, int32_t innersem)
{
#ifndef TC317X_REDUCED_IO 
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_24, (addr >> 24) & 0xff);
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_16, (addr >> 16) & 0xff);
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_8, (addr >> 8) & 0xff);
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_0, addr & 0xff);
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_24, (addr >> 24) & 0xff);
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_16, (addr >> 16) & 0xff);
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_8, (addr >> 8) & 0xff);
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SADDR_0, addr & 0xff);
  }
#else
  uint32_t value;
  value = SWAP32(addr);
  if (innersem)
    pTC317X->TC3X_Reg_WriteEx_InnerSem(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_SADDR_24, (unsigned char*)&value, 4);
  else
    pTC317X->TC3X_Reg_WriteEx(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_SADDR_24, (unsigned char*)&value, 4);
#endif
}

void TC317XREGSET_CMDDMA_Size (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
#ifndef TC317X_REDUCED_IO 
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SIZE8, ((value >> 8) & 0xff));
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SIZE0, (value & 0xff));
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SIZE8, ((value >> 8) & 0xff));
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_SIZE0, (value & 0xff));
  }
#else
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  if (innersem)
    pTC317X->TC3X_Reg_WriteEx_InnerSem(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_SIZE8, (unsigned char*)&value16, 2);
  else
    pTC317X->TC3X_Reg_WriteEx(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_SIZE8, (unsigned char*)&value16, 2);
#endif
}

void TC317XREGSET_CMDDMA_InitNStart (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_STARTCTRL, value);
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_STARTCTRL, value);
  }
}

void TC317XREGSET_TGTBUFF_CIR_MODE (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_TGTBUFF_CIR_MODE, (value & 0xff));
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_TGTBUFF_CIR_MODE, (value & 0xff));
  }
}

uint32_t TC317XREGGET_CMDDMA_CRC32 (t_TC3X_DEMODULE * pTC317X, int32_t innersem)
{
  uint32_t value;

#ifndef TC317X_REDUCED_IO 
  if (innersem)
  {
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC0, NULL);
    value |= (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC8, NULL) << 8);
    value |= (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC16, NULL) << 16);
    value |= (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC24, NULL) << 24);
  }
  else
  {
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC0, NULL);
    value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC8, NULL) << 8);
    value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC16, NULL) << 16);
    value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_CRC24, NULL) << 24);
  }
#else
  if (innersem)
    pTC317X->TC3X_Reg_ReadEx_InnerSem(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_CRC24, (unsigned char*)&value, 4);
  else
    pTC317X->TC3X_Reg_ReadEx(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_CMDDMA_CRC24, (unsigned char*)&value, 4);
#endif

  return SWAP32(value);
}

//====================================================================
//
// PERIperal for stream data Register
//
//====================================================================
void TC317XREGSET_PERI_CTRL (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_CTRL, value);
}

uint32_t TC317XREGGET_PERI_CTRL (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;

  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_CTRL, NULL);

  return value;
}

void TC317XREGSET_PERI_SPI_MODE0 (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t DLR)
{
  uint32_t value2;

  value2 = (value | (DLR << 2));
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE0, value2);
}

void TC317XREGSET_PERI_SPI_MODE1 (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t readytimeMinus1, uint32_t waittimeMinus1)
{
  uint32_t value2;

  value2 = value | (readytimeMinus1 << 2) | waittimeMinus1;
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE1, value2);
}

void TC317XREGSET_PERI_TS_MODE0 (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t DLR)
{
  uint32_t value2;

  value2 = (value | DLR);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE0, value2);
}

void TC317XREGSET_PERI_TS_MODE1 (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t TSStreamWaitTimeMinus1)
{
  uint32_t value2;

  value2 = value | TSStreamWaitTimeMinus1;
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE1, value2);
}

void TC317XREGSET_PERI_TS_MODE2 (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t MSM_TS_SyncHighTimeMinus1,
    uint32_t MSM_TS_SyncLowTimeMinus1)
{
  uint32_t value2;

  value2 = value | (MSM_TS_SyncHighTimeMinus1 << 3) | (MSM_TS_SyncLowTimeMinus1);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE2, value2);
}

void TC317XREGSET_PERI_TS_MODE3 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE3, value);
}

void TC317XREGSET_PERI_HPI_MODE0 (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_PERI_MODE0, value);
}

//====================================================================
//
// MAILBOX Register
//
//====================================================================
void TC317XREGSET_MailBox_CTRL (t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t InterruptLevel)
{
  uint32_t value2;

  value2 = value | InterruptLevel;
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_CTRL, value2);
}

uint32_t TC317XREGGET_MailBox_FIFO_ReadStat (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;

  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_R, 0x5E); // for read syncronized
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_R, NULL);

  return value;
}

uint32_t TC317XREGGET_MailBox_FIFO_WriteStat (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t value;

  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_W, 0x5E); // for read syncronized
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_W, NULL);

  return value;
}

void TC317XREGSET_MailBox_FIFOWindow (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_WIND | Bit7, value);
}

void TC317XREGGET_MailBox_FIFO_WordData (t_TC3X_DEMODULE * pTC317X, uint32_t *outdata, int32_t wordsize)
{
  TC317X_IO_IF_LOCK (pTC317X->moduleidx);
#if defined (USE_SDIO_DATA_INT)
  if (g_pTC3X[pTC317X->moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || g_pTC3X[pTC317X->moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x02, 1);
#endif // USE_SDIO_DATA_INT

  pTC317X->TC3X_Reg_ReadEx_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_MAIL_FIFO_WIND, (uint8_t *) outdata, (wordsize << 2));

#if defined (USE_SDIO_DATA_INT)
  if (g_pTC3X[pTC317X->moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || g_pTC3X[pTC317X->moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x03, 1);
#endif // USE_SDIO_DATA_INT

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
}

//====================================================================
//
// I2C Master/Slave Register
//
//====================================================================
void TC317XREGSET_I2CMST_CTRL (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_CTRL, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_CTRL, value);
}

void TC317XREGSET_I2CMST_FILTER (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_FILTER, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_FILTER, value);
}

void TC317XREGSET_I2CMST_PRER (t_TC3X_DEMODULE * pTC317X, int32_t value, int32_t innersem)
{
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_PRER_H, ((value >> 8) & 0xff));
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_PRER_L, (value & 0xff));
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_PRER_H, ((value >> 8) & 0xff));
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_PRER_L, (value & 0xff));
  }
}

void TC317XREGSET_I2CMST_DevAddr (t_TC3X_DEMODULE * pTC317X, unsigned value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_DEV_ADDR, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_DEV_ADDR, value);
}

void TC317XREGSET_I2CMST_AutoTargetAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TGT_ADDR, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TGT_ADDR, value);
}

void TC317XREGSET_I2CMST_AutoConfig (t_TC3X_DEMODULE * pTC317X, int32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_AUTOCONFIG, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_AUTOCONFIG, value);
}

uint32_t TC317XREGGET_I2CMST_AutoConfig (t_TC3X_DEMODULE * pTC317X, int32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_AUTOCONFIG, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_AUTOCONFIG, NULL);

  return value;
}

void TC317XREGSET_I2CMST_TXRRXR (t_TC3X_DEMODULE * pTC317X, int32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TXRX, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TXRX, value);
}

uint32_t TC317XREGGET_I2CMST_TXRRXR (t_TC3X_DEMODULE * pTC317X, int32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TXRX, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_TXRX, NULL);

  return value;
}

void TC317XREGSET_I2CMST_Start (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_START, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_START, value);
}

void TC317XREGSET_I2CMST_Stat0 (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT0, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT0, value);
}

void TC317XREGSET_I2CMST_Stat1 (t_TC3X_DEMODULE * pTC317X, uint32_t value, int32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT1, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT1, value);
}

uint32_t TC317XREGGET_I2CMST_Stat0 (t_TC3X_DEMODULE * pTC317X, int32_t innersem)
{
  uint32_t value;

  TC317XREGSET_I2CMST_Stat0 (pTC317X, 0x5E, innersem);    // for read syncronized
  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT0, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT0, NULL);

  return value;
}

uint32_t TC317XREGGET_I2CMST_Stat1 (t_TC3X_DEMODULE * pTC317X, int32_t innersem)
{
  uint32_t value;

  TC317XREGSET_I2CMST_Stat1 (pTC317X, 0x5E, innersem);    // for read syncronized
  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT1, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_I2CMST_STAT1, NULL);

  return value;
}

//====================================================================
//
// OUTPUT Buffer Management Register
//
//====================================================================
void TC317XREGSET_OBUFF_Config (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_CONFIG, value);
}

void TC317XREGSET_OBUFF_Init (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_INIT, value);
}

void TC317XREGSET_OBUFF_A_SAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO 
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_SADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_SADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO 
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_SADDR0, (uint8_t*)&value16, 2);
#endif
}

void TC317XREGSET_OBUFF_A_EAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_EADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_EADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO 
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_EADDR0, (uint8_t*)&value16, 2);
#endif
}

void TC317XREGSET_OBUFF_A_FIFO_Thr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO 
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_FIFO_THR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_FIFO_THR1, (value) & 0xff);
#else //TC317X_REDUCED_IO 
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_FIFO_THR0, (uint8_t*)&value16, 2);
#endif
}

uint32_t TC317XREGGET_OBUFF_A_CirBuff_Data_Addr (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO 
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_CIRBUFF_DATA_ADDR1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_CIRBUFF_DATA_ADDR0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO 
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_CIRBUFF_DATA_ADDR0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

uint32_t TC317XREGGET_OBUFF_A_FIFO_Stat (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;

  // for read syncronized
  TC317XREGSET_OBUFF_Init (pTC317X, TC3XREG_OBUFF_ABUFF_STATLATCH);

#ifndef TC317X_REDUCED_IO 
  value = (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_FIFO_STAT0, NULL) << 8);
  value |= pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_FIFO_STAT1, NULL);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_FIFO_STAT0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

uint32_t TC317XREGGET_OBUFF_A_CirBuff_Data_Size (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_CIRBUFF_DATA_SIZE1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_A_CIRBUFF_DATA_SIZE0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_A_CIRBUFF_DATA_SIZE0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

void TC317XREGSET_OBUFF_B_SAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_SADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_SADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_B_SADDR0, (uint8_t*)&value16, 2);
#endif
}

void TC317XREGSET_OBUFF_B_EAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_EADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_EADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_B_EADDR0, (uint8_t*)&value16, 2);
#endif
}

uint32_t TC317XREGGET_OBUFF_B_CirBuff_Data_Addr (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_CIRBUFF_DATA_ADDR1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_CIRBUFF_DATA_ADDR0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_B_CIRBUFF_DATA_ADDR0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

uint32_t TC317XREGGET_OBUFF_B_CirBuff_Data_Size (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_CIRBUFF_DATA_SIZE1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_B_CIRBUFF_DATA_SIZE0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_B_CIRBUFF_DATA_SIZE0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

void TC317XREGSET_OBUFF_C_SAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_SADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_SADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_C_SADDR0, (uint8_t*)&value16, 2);
#endif
}

void TC317XREGSET_OBUFF_C_EAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_EADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_EADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_C_EADDR0, (uint8_t*)&value16, 2);
#endif
}

uint32_t TC317XREGGET_OBUFF_C_CirBuff_Data_Addr (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_CIRBUFF_DATA_ADDR1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_CIRBUFF_DATA_ADDR0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_C_CIRBUFF_DATA_ADDR0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

uint32_t TC317XREGGET_OBUFF_C_CirBuff_Data_Size (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_CIRBUFF_DATA_SIZE1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_C_CIRBUFF_DATA_SIZE0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_C_CIRBUFF_DATA_SIZE0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

void TC317XREGSET_OBUFF_D_SAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_SADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_SADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_D_SADDR0, (uint8_t*)&value16, 2);
#endif
}

void TC317XREGSET_OBUFF_D_EAddr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_EADDR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_EADDR1, (value) & 0xff);
#else //TC317X_REDUCED_IO
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_D_EADDR0, (uint8_t*)&value16, 2);
#endif
}

uint32_t TC317XREGGET_OBUFF_D_CirBuff_Data_Addr (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_CIRBUFF_DATA_ADDR1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_CIRBUFF_DATA_ADDR0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_C_CIRBUFF_DATA_SIZE0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

uint32_t TC317XREGGET_OBUFF_D_CirBuff_Data_Size (t_TC3X_DEMODULE * pTC317X)
{
  uint16_t value;
#ifndef TC317X_REDUCED_IO
  value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_CIRBUFF_DATA_SIZE1, NULL);
  value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_CIRBUFF_DATA_SIZE0, NULL) << 8);

  return value;
#else //TC317X_REDUCED_IO
  pTC317X->TC3X_Reg_ReadEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_D_CIRBUFF_DATA_SIZE0, (uint8_t*)&value, 2);
  return SWAP16(value);
#endif
}

void TC317XREGSET_OBUFF_D_FIFO_Thr (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
#ifndef TC317X_REDUCED_IO 
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_FIFO_THR0, (value >> 8) & 0xff);
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OBUFF_D_FIFO_THR1, (value) & 0xff);
#else //TC317X_REDUCED_IO 
  uint16_t value16 = (uint16_t)value;
  value16 = SWAP16(value16);
  pTC317X->TC3X_Reg_WriteEx (pTC317X->moduleidx, 
    (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
    TC3XREG_OBUFF_D_FIFO_THR0, (uint8_t*)&value16, 2);
#endif
}

uint32_t TC317XREGGET_PC (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t v1, v2, v3;

  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x78, 0x55);
  v1 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x78, NULL);
  v2 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x79, NULL);
  v3 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x7a, NULL);

  return (v1 << 16 | v2 << 8 | v3);
}


void TC317XREGSET_RFCfg(t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG0, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG0, value);
}

uint32_t TC317XREGGET_RFCfg(t_TC3X_DEMODULE * pTC317X, uint32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG0, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG0, NULL);

  return value;
}

void TC317XREGSET_RFAction(t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG1, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG1, value);
}

uint32_t TC317XREGGET_RFAction(t_TC3X_DEMODULE * pTC317X, uint32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG1, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG1, NULL);

  return value;
}

void TC317XREGSET_RFAddr(t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t innersem)
{
  if (innersem)
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG2, value);
  else
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG2, value);
}

uint32_t TC317XREGGET_RFAddr(t_TC3X_DEMODULE * pTC317X, uint32_t innersem)
{
  uint32_t value;

  if (innersem)
    value = pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG2, NULL);
  else
    value = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG2, NULL);

  return value;
}

void TC317XREGSET_RFData(t_TC3X_DEMODULE * pTC317X, uint32_t value, uint32_t innersem)
{
#ifndef TC317X_REDUCED_IO
  if (innersem)
  {
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG3, ((value>>24) & 0xFF));
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG4, ((value>>16) & 0xFF));
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG5, ((value>>8) & 0xFF));
    pTC317X->TC3X_Reg_Write_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG6, (value & 0xFF));
  }
  else
  {
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG3, ((value>>24) & 0xFF));
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG4, ((value>>16) & 0xFF));
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG5, ((value>>8) & 0xFF));
    pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG6, (value & 0xFF));
  }
#else //TC317X_REDUCED_IO
  value = SWAP32(value);
  if (innersem)
    pTC317X->TC3X_Reg_WriteEx_InnerSem(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_RF_CFG3, (unsigned char*)&value, 4);
  else
    pTC317X->TC3X_Reg_WriteEx(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_RF_CFG3, (unsigned char*)&value, 4);
#endif
}

uint32_t TC317XREGGET_RFData(t_TC3X_DEMODULE * pTC317X, uint32_t innersem)
{
  uint32_t value;

#ifndef TC317X_REDUCED_IO
  if (innersem)
  {
    value = (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG3, NULL)<<24);
    value |= (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG4, NULL)<<16);
    value |= (pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG5, NULL)<<8);
    value |= pTC317X->TC3X_Reg_Read_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG6, NULL);

  }
  else
  {
    value = (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG3, NULL)<<24);
    value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG4, NULL)<<16);
    value |= (pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG5, NULL)<<8);
    value |= pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_RF_CFG6, NULL);
  }
#else //TC317X_REDUCED_IO
  if (innersem)
    pTC317X->TC3X_Reg_ReadEx_InnerSem(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_RF_CFG3, (unsigned char*)&value, 4);
  else
    pTC317X->TC3X_Reg_ReadEx(pTC317X->moduleidx, 
      (BURST_CONTINUOUS_MODE<<BURST_MODE_POS)|pTC317X->DEMOD_AddrBase, 
      TC3XREG_RF_CFG3, (unsigned char*)&value, 4);

#endif
  return SWAP32(value);
}

void TC317XREGSET_LDOCFG(t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OP_LDO_CONFIG, value);
}

void TC317XREGSET_XTAL_BIAS(t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OP_XTAL_BIAS, value);
}

void TC317XREGSET_XTAL_BIAS_KEY(t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_OP_XTAL_BIAS_KEY, value);
}

uint32_t TC3XREGGET_PC (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t v1, v2, v3;
  pTC317X->TC3X_Reg_Write (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x78, 0x55);
  v1 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x78, NULL);
  v2 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x79, NULL);
  v3 = pTC317X->TC3X_Reg_Read (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, 0x7a, NULL);
  return (v1<<16|v2<<8|v3);
}

