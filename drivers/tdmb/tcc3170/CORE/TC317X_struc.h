//----------------------------------------------------------------------------
//   FileName    : TC317X_struc.h
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_STRUC_H_
#define __TC317X_STRUC_H_

#include "../TC317X_Common.h"

#ifndef NULL
#define NULL 0
#endif

//---------------------------------------------------------------------------------------
//
//      TC3X DEVICE Structure
//

typedef struct
{
  uint32_t Protocol;
  uint32_t SlaveON;
  uint32_t FastMode;
  uint32_t DLR;
  uint32_t Size;
  uint32_t SPH;
  uint32_t SPO;
  uint32_t ByteMSB;
  uint32_t BitMSB;
  uint32_t ReadyTime;
  uint32_t WaitTime;
} TC3X_SUBIF_SPI_OPT;

typedef struct
{
  uint32_t IntelMode;
  uint32_t ByteMSB;
  uint32_t BitMSB;
} TC3X_SUBIF_HPI_OPT;

typedef struct
{
  uint32_t MSM_Tssync;
  uint32_t Parallel;
  uint32_t FastMode;
  uint32_t TSClkMask;
  uint32_t DLR;
  uint32_t TSClk_POL_Low;
  uint32_t TSSync_POL_Low;
  uint32_t TSEN_POL_Low;
  uint32_t TSStreamWaitON;
  uint32_t TSStreamWaitTime;
  uint32_t ByteMSB;
  uint32_t BitMSB;
  uint32_t MSMTSSyncHighTime;
  uint32_t MSMTSSyncLowTime;
} TC3X_SUBIF_TS_OPT;

typedef struct
{
  uint32_t SubIFDefine;
  uint32_t SubIFValue;
  uint32_t HeaderON;
  TC3X_SUBIF_SPI_OPT oSPI;
  TC3X_SUBIF_TS_OPT oTS;
  TC3X_SUBIF_HPI_OPT oHPI;
} TC3X_SUBIF_Header;

typedef struct
{
  uint32_t MainIFDefine;
} TC3X_MainIF;

typedef struct
{
  uint32_t BW_KHz;
  uint32_t FREQ_KHz;
  uint32_t Hierarchy;
} t_TC3X_NIT;

typedef struct
{
  int32_t       moduleidx;
  uint32_t DEMOD_AddrBase;                           // ??À§ 8bit Addr
  uint32_t MasterMode;
  uint32_t NumOfDemodule;
  uint32_t ChainNum;
  uint32_t MainClk_Khz;
  uint8_t PLL_WaitTime;
  uint8_t PLL_P;
  uint8_t PLL_M;
  uint8_t PLL_S;
  uint32_t OSC_Khz;
  uint32_t INT_TRIG;
  uint32_t SelRF;                                    // ?î¶² RF?? ?????
  uint32_t RF_SPECIFIC;
  TC3X_MainIF MainIF;
  TC3X_SUBIF_Header SubIF;
  t_TC3X_NIT NIT;
  uint32_t BAddressed;                               // ColdBoot?? Address?? ????.

  uint32_t OP_Status;

  //uint32_t USE_OP_IIC;    // TCC3170 : not use OP I2C!!! -> Reserved1
  uint32_t Reserved1;

  //int32_t       UseI2CRepeater;    // TCC3170 : not use I2C Repeater!!! -> Reserved0
  int32_t       Reserved0;

  uint32_t USE_PID_Filtering;
  int32_t       BoardDefined;

  int32_t       mailboxLog;
  int32_t       mailboxErrorCounter;

  uint32_t BootCodeVersion;

  // fuction pointer
  uint32_t (*TC3X_Reg_Read) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError);
  TC3X_IO_Err (*TC3X_Reg_Write) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data);
  TC3X_IO_Err (*TC3X_Reg_WriteEx) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize);
  TC3X_IO_Err (*TC3X_Reg_ReadEx) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize);

  uint32_t (*TC3X_Reg_Read_InnerSem) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError);
  TC3X_IO_Err (*TC3X_Reg_Write_InnerSem) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data);
  TC3X_IO_Err (*TC3X_Reg_WriteEx_InnerSem) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize);
  TC3X_IO_Err (*TC3X_Reg_ReadEx_InnerSem) (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize);
} t_TC3X_DEMODULE;

#endif
