//----------------------------------------------------------------------------
//   FileName    : TC317X_API.h
//    Description : 
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#ifndef __TC317X_API_H__
#define __TC317X_API_H__
#include "TC317X_Common.h"
#include "IO/TC317X_IO_UserDefine.h"
#include "IO/TC317X_IO_UTIL.h"
#include "IO/TC317X_IO.h"

void TC317X_API_Init (int32_t moduleidx, int32_t NumOfDemodule);
void TC317X_API_IOOpen (int32_t moduleidx, int32_t MainIO, int32_t StreamIO);
void TC317X_API_BBOpen (int32_t moduleidx);
void TC317X_API_Close (int32_t moduleidx);

void TC317X_API_BBInit (int32_t moduleidx);
void TC317X_API_BBPreInit (int32_t moduleidx);
void TC317X_API_SetFifoSize(int32_t moduleidx, int32_t fifoSize);
void TC317X_API_PreSet_PLLData (
        int32_t moduleidx, 
        uint8_t 
        PLL_WaitTime, 
        uint8_t PLL_P, 
        uint8_t PLL_M, 
        uint8_t PLL_S, 
        uint32_t OSC_Khz);

int32_t TC317X_API_ColdBootTable (
        int32_t moduleidx, uint8_t *pData, int32_t size, int32_t option);

void TC317X_API_REG_Write (int32_t moduleidx, int32_t DeviceIdx, int32_t addr, int32_t data);
int32_t  TC317X_API_REG_Read (int32_t moduleidx, int32_t DeviceIdx, int32_t addr);
int32_t  TC317X_API_SetFreq (
        int32_t moduleidx, 
        uint32_t bw_khz, 
        uint32_t freq_khz, 
        tFreqSet_Option * pOption);

int32_t  TC317X_API_SET_TDMB_Service (
        int32_t moduleidx, 
        int32_t RegFlag, 
        TDMBService_t * pTDMBService);

int32_t  TC317X_API_GetTDMB_Lock (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        t_TDMBLock * pLock);

int32_t  TC317X_API_GetTDMB_Lock_Wait (
        int32_t moduleidx, 
        uint32_t DeviceIdx);

int32_t  TC317X_API_GetTDMB_SNR (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint32_t *pSNR, 
        uint32_t *pSNRdb, 
        uint32_t *pMovingAVGSNRdb);

int32_t  TC317X_API_GetTDMB_PCBER (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint32_t *pPCBER, 
        uint32_t *pMovingAVGPCBER);

int32_t  TC317X_API_GetTDMB_VITERBIBER (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint32_t *pVITERBIBER, 
        uint32_t *pMovingAVGVITERBIBER);

int32_t  TC317X_API_GetTDMB_TSPER (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint32_t *pTSPER, 
        uint32_t *pMovingAVGTSPER);

int32_t  TC317X_API_GetTDMB_RSSI (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint32_t *pRSSI, 
        uint32_t *pMovingAVGRSSI);

int32_t  TC317X_API_PushStatusData (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint8_t *pData, 
        uint32_t Size);

int32_t  TC317X_API_ParseData (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint8_t *pData, 
        uint32_t Size);

int32_t  TC317X_API_GetTDMBData (
        int32_t moduleidx, 
        uint32_t DeviceIdx, 
        uint8_t *pDMBFormatData, 
        uint32_t WantLen);

void TC317X_API_TDMB_OccurEvent (
        int32_t moduleidx, 
        TC3X_DNUM handle, 
        int32_t message, 
        void *param1, 
        void *param2, 
        void *param3, 
        void *param4);

int32_t TC3X_API_Reset_MonitorValue (int32_t moduleidx, uint32_t DeviceIdx);
#endif
