//----------------------------------------------------------------------------
//   FileName    : TC317X_IO_I2C.h
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_IO_I2C_H__
#define __TC317X_IO_I2C_H__

#include "../TC317X_Common.h"

#if defined (USE_IF_I2C)
void      TC317X_IO_I2C_Init (int moduleidx);
TC3X_IO_Err TC317X_IO_I2C_Close (int moduleidx);
void      TC317X_IO_I2C_Variables_Init (int moduleidx);
TC3X_IO_Err TC317X_IO_I2C_Reg_Write (int moduleidx, int ChipAddr, int RegAddr, unsigned int data);
TC3X_IO_Err TC317X_IO_I2C_Reg_Write_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned int data);
unsigned int TC317X_IO_I2C_Reg_Read (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError);
unsigned int TC317X_IO_I2C_Reg_Read_InnerSem (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError);
TC3X_IO_Err TC317X_IO_I2C_Reg_WriteEx (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize);
TC3X_IO_Err TC317X_IO_I2C_Reg_WriteEx_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize);
TC3X_IO_Err TC317X_IO_I2C_Reg_ReadEx (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize);
TC3X_IO_Err TC317X_IO_I2C_Reg_ReadEx_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize);
void TC317X_IO_I2C_Reg_Read_Wrap(int iHandleNum, unsigned int Addr, unsigned char *data, unsigned char bytesize);
void TC317X_IO_I2C_Reg_Write_Wrap(int iHandleNum, unsigned int Addr, unsigned char data);
#endif // USE_IF_I2C

#endif
