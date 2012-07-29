//----------------------------------------------------------------------------
//   FileName    : TC317X_Sub_API.h
//    Description : 
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_Sub_API_H__
#define __TC317X_Sub_API_H__

#include "IO/TC317X_IO_UTIL.h"
#include "TC317X_Common.h"

void TC317X_SubAPI_Init (
        int32_t moduleidx, 
        int32_t NumDemodule);

void TC317X_SubAPI_IOOpen (
        int32_t moduleidx, 
        int32_t MainIO, 
        int32_t StreamIO);

void TC317X_SubAPI_BBOpen (
        int32_t moduleidx);

void TC317X_SubAPI_BBPreInit (
        int32_t moduleidx, 
        FN_v_ihivvvv EVENTFunction);

int32_t TC317X_SubAPI_BBColdBoot_FILE (
        int32_t moduleidx, 
        uint8_t *pData, 
        uint32_t Size, 
        int32_t option);

int32_t TC317X_SubAPI_BBColdBoot_MEM (
        int32_t moduleidx, 
        uint8_t *pData, 
        uint32_t Size, 
        int32_t option);

int32_t TC317X_SubAPI_BBColdBoot_NANDFLASH (
        int32_t moduleidx, 
        char *FileName, 
        int32_t option);

int32_t TC317X_SubAPI_BBColdBoot_NONWrite (int32_t moduleidx);

int32_t TC317X_SubAPI_COLDBOOT (
        int32_t moduleidx, 
        TC3X_BOOTBIN * BootBin, 
        int32_t option);

void TC317X_SubAPI_BBInit ( int32_t moduleidx, int32_t Reserved0);
void TC317X_SubAPI_Close (int32_t moduleidx);
void TC317X_SubAPI_PWON (int32_t moduleidx);
void TC317X_SubAPI_PWDN (int32_t moduleidx);
void TC317X_SubAPI_PWRESET (int32_t moduleidx);
void TC317X_SubAPI_MainIOSel (int32_t moduleidx, int32_t IOSeries);
void TC317X_SubAPI_StreamIOSel (int32_t moduleidx, int32_t IOSeries);
int32_t  TC317X_SubAPI_BBDetect (int32_t moduleidx);
void TC317X_SubAPI_BBAttach (int32_t moduleidx, int32_t DeviceBB);

int32_t  TC317X_SubAPI_SetFrequency (
        int32_t moduleidx, 
        int32_t freq_khz, 
        int32_t bw_khz, 
        tFreqSet_Option * pOption);

void TC317X_SubAPI_BBDetach (int32_t moduleidx);

extern tSTDInfo g_tSTDInfo[MAX_TCC3X];
extern tSTDCtrl **g_pSTDCtrl;

#endif
