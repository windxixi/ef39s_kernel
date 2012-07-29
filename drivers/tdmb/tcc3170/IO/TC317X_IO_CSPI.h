//----------------------------------------------------------------------------
//   FileName    : TC317X_IO_CSPI.h
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_IO_CSPI_H__
#define __TC317X_IO_CSPI_H__

#include "../TC317X_Common.h"
#include "TC317X_IO.h"

TC3X_IO_Err TC317X_IO_CSPI_Init (int moduleidx);
TC3X_IO_Err TC317X_IO_CSPI_Close (int moduleidx);
void      TC317X_IO_CKC_SetSPISClock2 (int iCh, int iFreq);
void      TC317X_IO_CSPI_GPSB_Init (int moduleidx, int channel);
void      TC317X_IO_CSPI_SetGPSBCh (int moduleidx, int ch);
int       TC317X_IO_CSPI_GetGPSBCh (int moduleidx);
unsigned int TC317X_IO_CSPI_Reg_Read (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError);    // use 52cmd
TC3X_IO_Err TC317X_IO_CSPI_Reg_Write (int moduleidx, int ChipAddr, int RegAddr, unsigned int data);       // use 52cmd
TC3X_IO_Err TC317X_IO_CSPI_Reg_ReadEx (int moduleidx, int ChipAddr, int RegAddr, unsigned char *data, int iSize); // use 53cmd
TC3X_IO_Err TC317X_IO_CSPI_Reg_WriteEx (int moduleidx, int ChipAddr, int RegAddr, unsigned char *pCh, int iSize); // use 53cmd
unsigned int TC317X_IO_CSPI_Reg_Read_InnerSem (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError);
TC3X_IO_Err TC317X_IO_CSPI_Reg_Write_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned int data);
TC3X_IO_Err TC317X_IO_CSPI_Reg_ReadEx_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned char *data, int iSize);
TC3X_IO_Err TC317X_IO_CSPI_Reg_WriteEx_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned char *pCh, int iSize);
int       TC317X_IO_CSPI_CMD0 (int moduleidx);               // R1
int       TC317X_IO_CSPI_CMD3 (int moduleidx, unsigned char *outbuff);
int       TC317X_IO_CSPI_CMD5 (int moduleidx, unsigned char *outbuff);    // Return R4
int       TC317X_IO_CSPI_CMD52 (int moduleidx, unsigned char write_flag,    // write flag  0: read   1: write
    unsigned short reg_addr,     // SDIO register address
    unsigned char *buf);         // data buffer to be read and write

int       TC317X_IO_CSPI_CMD53 (int moduleidx, unsigned char write_flag, unsigned int reg_addr, unsigned char *buf,
    unsigned short len, int fixedmode);

void      TC317X_IO_CSPI_SINGLE_ERR_INIT (int moduleidx);
void      TC317X_IO_CSPI_MULTI_ERR_INIT (int moduleidx);
void      TC317X_IO_CSPI_Reg_Read_Wrap(int iHandleNum, unsigned int Addr, unsigned char *data, unsigned char bytesize);
void      TC317X_IO_CSPI_Reg_Write_Wrap(int iHandleNum, unsigned int Addr, unsigned char data);

#endif
