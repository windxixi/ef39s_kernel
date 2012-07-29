//----------------------------------------------------------------------------
//    FileName    : TC317X_API.c
//    Description : API Function
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#include "TC317X_API.h"
#include "TC317X_Sub_API.h"
#include "tcc3170_bb.h"

//  Sequence : IO Open - BB Open - BB PreInit(standard) - ColdBoot(standard) - BBInit(standard) - RF Open - RF Init

//  SetFreq Sequence : Stream Stop - Stream Start - SetFreq
//  Change Freq (same standard) : Same as SetFreq Sequence
//  Change Freq (diff standard) : CMD_BB_STREAMOUT_STOP - BB PreInit(standard) - ColdBoot(standard) - BBInit(standard) - SetFreq Sequence

//--------------------------------------------------------------------------
// common
//

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_Init
//    Description : Application initialize code
//    Input :
//      moduleidx : module index
//      NumOfDemodule : Number of Demodulator
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_Init (int32_t moduleidx, int32_t NumOfDemodule)
{
#if 0
  PRINTF_LV_2 ("[TC317X]-----------------\n");
  PRINTF_LV_2 ("# APIInit \n");
  PRINTF_LV_2 ("# NumberOfDemodule [%d]\n", NumOfDemodule);
#endif
  TC317X_SubAPI_Init (moduleidx, NumOfDemodule);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_IOOpen
//    Description : IO Open API
//    Input :
//      moduleidx : module index
//      MainIO : main IO
//      StreamIO : stream IO
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_IOOpen (int32_t moduleidx, int32_t MainIO, int32_t StreamIO)
{
  char *ifLabel[5] = { "SRAMLIKE", "I2C", "CSPI", "SDIO1BIT", "SDIO4BIT" };
  char *streamioLabel[7] = { "MAINIO", "PTS", "STS", "SPIMS", "SPISLV",
    "HPI_HEADERON", "HPI_HEADEROFF" };

  PRINTF_LV_0 ("# CommandIO : %s\n", ifLabel[MainIO]);
  PRINTF_LV_0 ("# StreamIO : %s\n", streamioLabel[StreamIO]);

  TC317X_SubAPI_IOOpen (moduleidx, MainIO, StreamIO);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_BBOpen
//    Description : Find BaseBand & Attach
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_BBOpen (int32_t moduleidx)
{
  //	PRINTF_LV_2 ("# BB Open\n");
  TC317X_SubAPI_BBOpen (moduleidx);     // find TC3X
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_Close
//    Description : Close BaseBand
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_Close (int32_t moduleidx)
{

  TC317X_SubAPI_Close (moduleidx);
  TCDBG("\n");
  //	PRINTF_LV_2 ("# BB Close\n");
  //	PRINTF_LV_2 ("[TC3X End]------------\n");
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_BBPreInit
//    Description : BaseBand Pre-Initialize & Register Callback Function
//    Input :
//      moduleidx : module index
//      Standard : Standard
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_BBPreInit (int32_t moduleidx)
{
  PRINTF_LV_0 ("# BBPreInit V38\n");

  TC317X_SubAPI_BBPreInit (moduleidx, TC317X_API_TDMB_OccurEvent);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_PreSet_PLLData
//    Description : Optional Function. PLL Control Value Setting. Apply before ColdBoot.
//    Input :
//      moduleidx : module index
//      PLL_WaitTime
//      PLL_P
//      PLL_M
//      PLL_S
//      OSC_Khz : Oscillator clk
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_PreSet_PLLData (
    int32_t moduleidx,
    uint8_t
    PLL_WaitTime,
    uint8_t PLL_P,
    uint8_t PLL_M,
    uint8_t PLL_S,
    uint32_t OSC_Khz)
{
  uint8_t PLL_WPMS[4];

  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  PLL_WPMS[0] = PLL_WaitTime;
  PLL_WPMS[1] = PLL_P;
  PLL_WPMS[2] = PLL_M;
  PLL_WPMS[3] = PLL_S;

  g_pSTDCtrl[moduleidx][0].BB_UserFunction (
      moduleidx, 0, CMD_BB_PLL_SET, PLL_WPMS, &OSC_Khz, NULL, NULL);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_ColdBootTable
//    Description : Write Boot Code Binary File to BB
//    Input :
//      moduleidx : module index
//      pData : Boot Binary Data Pointer
//      size : size of boot binary
//      option : broadcast / normal / non write
//    Output :
//      Success: 0x1ACCE551 Fail : others
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_ColdBootTable (
    int32_t moduleidx, uint8_t *pData, int32_t size, int32_t option)
{
  int32_t       ret;
  //PRINTF_LV_2 ("# ColdBootTable [%d]\n", size);

  if (option == CMD_BB_COLDBOOT_BROADCAST)
    ret = TC317X_SubAPI_BBColdBoot_FILE (
        moduleidx, pData, size, CMD_BB_COLDBOOT_BROADCAST);
  else if (option == CMD_BB_COLDBOOT_NON_WRITE)
    ret = TC317X_SubAPI_BBColdBoot_NONWrite (moduleidx);
  else        // (option==CMD_BB_COLDBOOT_NORMAL)
    ret = TC317X_SubAPI_BBColdBoot_FILE (
        moduleidx, pData, size, CMD_BB_COLDBOOT_NORMAL);

  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_BBInit
//    Description : BB Initialization
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_BBInit (int32_t moduleidx)
{
  TC317X_SubAPI_BBInit (moduleidx, 0);
}

void TC317X_API_SetFifoSize(int32_t moduleidx, int32_t fifoSize)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  g_pSTDCtrl[moduleidx][0].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
      CMD_BB_SET_FIFO_SIZE, &fifoSize, NULL, NULL, NULL);
}
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_REG_Write
//    Description : Register write to BB
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      addr : Address
//      data : data
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_REG_Write (int32_t moduleidx, int32_t DeviceIdx, int32_t addr, int32_t data)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_BB_REG_W, &addr, &data, NULL, NULL);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_REG_Read
//    Description : Register read from BB
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      addr : Address
//    Output :
//      Read Value
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_REG_Read (int32_t moduleidx, int32_t DeviceIdx, int32_t addr)
{
  int32_t       ret;

  if (!g_pSTDCtrl)
    return 0;

  if (!g_pSTDCtrl[moduleidx])
    return 0;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return 0;

  ret = g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_BB_REG_R, &addr, NULL, NULL, NULL);
  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_SetFreq
//    Description : BB/RF frequency setting
//    Input :
//      moduleidx : module index
//      bw_khz : bandwidth
//      freq_khz : frequency
//      pOption : option
//    Output :
//      int32_t (0:Fail, 1:Success)
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_SetFreq (
    int32_t moduleidx,
    uint32_t bw_khz,
    uint32_t freq_khz,
    tFreqSet_Option * pOption)
{
  if (!g_pSTDCtrl)
    return 0;

  if (!g_pSTDCtrl[moduleidx])
    return 0;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return 0;

  return (TC317X_SubAPI_SetFrequency (moduleidx, freq_khz, bw_khz, pOption));
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_RF_REG_R
//    Description : Read RF Register
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      addr : Address
//    Output :
//      read value
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_RF_REG_R (
    int32_t moduleidx, uint32_t DeviceIdx, uint32_t addr)
{
  int32_t ret = 0;

  if (!g_pSTDCtrl)
    return 0;

  if (!g_pSTDCtrl[moduleidx])
    return 0;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return 0;

  ret = g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_RF_READ_REG_SINGLE, &addr, NULL, NULL, NULL);
  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_RF_REG_W
//    Description : Write to RF
//    Input :
//      moduleidx : module index
//      moduleidx : module index
//      DeviceIdx : device index
//      addr : Address
//      data : Data
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_RF_REG_W (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t addr,
    uint32_t data)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_RF_WRITE_REG_SINGLE, &addr, &data, NULL, NULL);
}

// stream pause
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_StreamPause
//    Description : Stream Pause
//    Input :
//      moduleidx : module index
//   
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_StreamPause (int32_t moduleidx)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  g_pSTDCtrl[moduleidx][0].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
      CMD_BB_STREAMOUT_PAUSE, NULL, NULL, NULL, NULL);
}

// stream restart
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_StreamRestart
//    Description : Stream Restart
//    Input :
//      moduleidx : module index
//   
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_API_StreamRestart (int32_t moduleidx)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return;

  g_pSTDCtrl[moduleidx][0].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
      CMD_BB_STREAMOUT_RESTART, NULL, NULL, NULL, NULL);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetBootCodeVersion
//    Description : Get Boot Code (ASM) Version
//    Input :
//      moduleidx : module index
//   
//    Output :
//      version data
//    Remark :
//--------------------------------------------------------------------------
uint32_t TC317X_API_GetBootCodeVersion (
    int32_t moduleidx, uint32_t *pVersion)
{
  //uint32_t ret;

  if (!g_pSTDCtrl)
    return 0x00;

  if (!g_pSTDCtrl[moduleidx])
    return 0x00;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return 0x00;

  pVersion[0] = 0x00;
  g_pSTDCtrl[moduleidx][0].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
      CMD_BB_GET_BOOTCODE_VERSION, pVersion, NULL, NULL, NULL);

  return 0;
}

// Stream Will be Come Into This Function
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_TDMB_OccurEvent
//    Description : Event fuction (call fuction)
//    Input :
//      moduleidx : module index
//      handle : device index
//      message
//      param1, param2, param3, param4 : parameters
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
//
#ifndef FEATURE_APPLY_EF40K
extern unsigned int tc317x_dxb_fifo_put(
  void *buffer, unsigned int size, unsigned int crc, int type);
#endif

void TC317X_API_TDMB_OccurEvent (
    int32_t moduleidx,
    TC3X_DNUM handle,
    int32_t message,
    void *param1,
    void *param2,
    void *param3,
    void *param4)
{
  switch (message)
  {
    case CMD_STREAM_TDMB:
      {
        uint8_t *src;
        uint32_t StreamSize;
        int32_t       Temp;
        int32_t       Type;
        int32_t       SubChID;
        int32_t       ServiceMailBoxIdx;
        uint32_t CRCErrorValue;

        src = (uint8_t *) param1;
        StreamSize = *(uint32_t *) param2;
        Temp = *(int32_t *) param3;
        Type = ((Temp >> 8) & 0xFF);
        SubChID = (Temp & 0xFF);
        ServiceMailBoxIdx = *(int32_t *) param4;
        CRCErrorValue = *(uint32_t *) param4;

        switch (Type)
        {
          case SRVTYPE_STATUS:
            TC317X_API_PushStatusData (moduleidx, handle, src, StreamSize);
            //TCDBG("STATUS type:%d, size:%d\n", Type, StreamSize);
            break;

          case SRVTYPE_FIC:
          case SRVTYPE_FIC_WITH_ERR:
          case EWS_ANNOUNCE_FLAG:
          case RECONFIGURATION_FLAG:
          case EWS_ANNOUNCE_RECONFIGURATION_FLAG:
            //TCDBG("STATUS type:%d, size:%d\n", Type, StreamSize);
#ifdef FEATURE_APPLY_EF40K
          tcc3170_put_fic_data(src, StreamSize, CRCErrorValue, Type);
#else
          tc317x_dxb_fifo_put(src, StreamSize, CRCErrorValue, Type);
#endif
            break;

          case SRVTYPE_DMB:
          //TCDBG("TS -----> [%02X][%02X][%02X][%02X]\n", src[0],src[1],src[2],src[3]);
#ifdef FEATURE_APPLY_EF40K
          tcc3170_put_msc_data(src, StreamSize, CRCErrorValue, Type);
#else
          tc317x_dxb_fifo_put(src, StreamSize, CRCErrorValue, Type);
#endif
            break;

          case SRVTYPE_DAB:
          case SRVTYPE_DABPLUS:
          //TCDBG("DAB  -----> [%02X][%02X][%02X][%02X]\n", src[0],src[1],src[2],src[3]);
#ifdef FEATURE_APPLY_EF40K
            tcc3170_put_msc_data(src, StreamSize, CRCErrorValue, Type);
#else
            tc317x_dxb_fifo_put(src, StreamSize, CRCErrorValue, Type);
#endif
            break;

          case SRVTYPE_DATA:
            //TCDBG("DATA type%d\n", Type);
#ifdef FEATURE_APPLY_EF40K
            //tcc3170_put_msc_data(src, StreamSize, CRCErrorValue, Type);
#else
            tc317x_dxb_fifo_put(src, StreamSize, CRCErrorValue, Type);
#endif
            break;

          default:
            TCDBG("unknown type!! %d\n", Type);
            break;
        }
      }
      break;

    case CMD_STREAM_STOPPED:
      break;

    case TC3X_ERROR_UNKNOWN:
      PRINTF_LV_0 ("# CRITICAL ERROR [UNKNOWN] \n");
      PRINTF_LV_0 ("# Please close TC3X Driver \n");
      break;

    case TC3X_ERROR_CRITICAL_MAILBOX:
      PRINTF_LV_0 ("# CRITICAL ERROR [MAILBOX] \n");
      PRINTF_LV_0 ("# Please close TC3X Driver \n");
      break;
  }
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_SET_TDMB_Service
//    Description : Set TDMB Service information
//    Input :
//      moduleidx : module index
//      RegFlag : 1 Reg / 0 UnReg
//      pTDMBService : Service information
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_SET_TDMB_Service (
    int32_t moduleidx, int32_t RegFlag, TDMBService_t * pTDMBService)
{
  int32_t       retv;

  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (RegFlag == 0) {
    retv = g_pSTDCtrl[moduleidx][0].BB_UserFunction (
        moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
        CMD_BB_TDMB_UNREG, pTDMBService, NULL, NULL, NULL);
  } else {
    retv = g_pSTDCtrl[moduleidx][0].BB_UserFunction (
        moduleidx, g_pSTDCtrl[moduleidx][0].hBB,
        CMD_BB_TDMB_REG, pTDMBService, NULL, NULL, NULL);
  }

  return retv;
}

//--------------------------------------------------------------------------
// TDMB Status Monitoring
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_Lock
//    Description : Get Lock Status
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      pLock : Lock Status
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_Lock (
    int32_t moduleidx, uint32_t DeviceIdx, t_TDMBLock * pLock)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_LOCK, pLock, NULL, NULL, NULL);

  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_Lock_Wait
//    Description : Get Lock Status until Thr Time.
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//    Output :
//      1 : success, others : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_Lock_Wait (int32_t moduleidx, uint32_t DeviceIdx)
{
  t_TDMBLock Lock;
  int32_t TickCnt;
  int32_t SubLockOK;
  int64_t StartTick;

  extern int64_t TC317X_GetTickCnt (void);
  extern int64_t TC317X_GetInterval (int64_t startTick);

  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;


  // OFDM Detect
  SubLockOK = 0;
  TickCnt = TDMB_OFDMDETECT_LOCK * TDMB_OFDMDETECT_RETRY;

  StartTick = TC317X_GetTickCnt();
  do
  {
    TC317X_API_GetTDMB_Lock (moduleidx, DeviceIdx, &Lock);

    if(Lock.CTO && Lock.CFO)
    {
      return 1; // Lock Success
    }

    if(Lock.OFDM_DETECT)
    {
      SubLockOK = 1;
      break;
    }
  }while((TC317X_GetInterval(StartTick)/1000) < TickCnt);

  if(SubLockOK==0)   // Lock Fail
    return 0;

  // CTO & CFO Detect
  SubLockOK = 0;
  TickCnt = ((TDMB_CTO_LOCK * TDMB_CTO_RETRY) +
      (TDMB_CTO_LOCK+TDMB_CFO_LOCK) * TDMB_CFO_RETRY);

  StartTick = TC317X_GetTickCnt();
  do
  {
    TC317X_API_GetTDMB_Lock (moduleidx, DeviceIdx, &Lock);
    if(Lock.CTO && Lock.CFO)
    {
      SubLockOK = 1;
      return 1; // Lock Success
    }
  }while(TC317X_GetInterval(StartTick)/1000 < TickCnt);

  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_SNR
//    Description : Get snr
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      psnr : snr
//      pSNRdb : snr db
//      pMovingAVGSNRdb : Average snr db
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_SNR (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t *pSNR,
    uint32_t *pSNRdb,
    uint32_t *pMovingAVGSNRdb)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_SNR_SET, pSNR, pSNRdb, pMovingAVGSNRdb, NULL);

  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_PCBER
//    Description : Get pcber
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      pPCBER : pcber
//      pMovingAVGPCBER : Average pcber
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_PCBER (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t *pPCBER,
    uint32_t *pMovingAVGPCBER)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_PCBER_SET, pPCBER, pMovingAVGPCBER, NULL, NULL);
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_VITERBIBER
//    Description : Get VITERBI Ber
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      pVITERBIBER : Viterbi ber
//      pMovingAVGVITERBIBER : Average Viterbi ber
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_VITERBIBER (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t *pVITERBIBER,
    uint32_t *pMovingAVGVITERBIBER)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_VITERBIBER_SET, pVITERBIBER, pMovingAVGVITERBIBER,
      NULL, NULL);
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_TSPER
//    Description : Get TSPER
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      pTSPER : TSPER
//      pMovingAVGTSPER : Average TSPER
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_TSPER (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t *pTSPER,
    uint32_t *pMovingAVGTSPER)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_TSPER_SET, pTSPER, pMovingAVGTSPER, NULL, NULL);
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_GetTDMB_RSSI
//    Description : Get RSSI
//    Input :
//      moduleidx : module index
//      DeviceIdx : device index
//      pRSSI : RSSI
//      pMovingAVGRSSI : Average RSSI
//    Output :
//      -1 : fail
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_GetTDMB_RSSI (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint32_t *pRSSI,
    uint32_t *pMovingAVGRSSI)
{
  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_GET_TDMB_RSSI_SET, pRSSI, pMovingAVGRSSI, NULL, NULL);
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_PushStatusData
//    Description : Manual Push Status Data
//    Input :
//      moduleidx : module index
//      DeviceIdx : Device index
//      pData : Data pointer
//      Size : Size of Data
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_API_PushStatusData (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint8_t *pData,
    uint32_t Size)
{
  int32_t       ret = 0;

  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  ret = g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_PUSH_TDMB_STAT_DATA, pData, &Size, NULL, NULL);
  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_API_ParseData
//    Description : Manual Push Status Data
//    Input :
//      moduleidx : module index
//      DeviceIdx : Device index
//      pData : Data pointer
//      Size : Size of Data
//    Output :
//   
//    Remark : It will be call event function
//--------------------------------------------------------------------------
int32_t TC317X_API_ParseData (
    int32_t moduleidx,
    uint32_t DeviceIdx,
    uint8_t *pData, uint32_t Size)
{
  int32_t       ret = 0;

  if (!g_pSTDCtrl)
    return -1;

  if (!g_pSTDCtrl[moduleidx])
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;

  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  ret = g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (
      moduleidx, g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
      CMD_REQUEST_PARSE_TDMB_DATA, pData, &Size, NULL, NULL);
  return ret;
}

int32_t TC3X_API_Reset_MonitorValue (int32_t moduleidx, uint32_t DeviceIdx)
{
  int32_t       ret = 0;

  if (!g_pSTDCtrl)
    return -1;
  if (!g_pSTDCtrl[moduleidx])
    return -1;
  if (!g_pSTDCtrl[moduleidx][0].BSetted)
    return -1;
  if (!g_pSTDCtrl[moduleidx][0].BInited)
    return -1;

  ret = g_pSTDCtrl[moduleidx][DeviceIdx].BB_UserFunction (moduleidx, 
        g_pSTDCtrl[moduleidx][DeviceIdx].hBB,
        CMD_REQUEST_TDMB_RESET_MONITOR_VALUE, 
        NULL, NULL, NULL, NULL);
  return ret;
}

/*
#if defined (STDDEF_DMB)
//-----------------------------------------------------------------------------------------------
// TDMB example code
//
void DMB_Main ()
{
uint8_t Data[4];                                 // it might be 34kbytes
uint32_t Size;
t_TDMBLock LockStat;

#ifdef __WINDOWS__
FILE     *fp;
fp = fopen ("DMB_BOOT.BIN");
fseek (fp, 0, SEEK_END);
Size = ftell (fp);
fread (fp, &Data[0], 1, Size);
#endif

// Prepare Process
TC3X_API_Init (1);
TC3X_API_IOOpen (TC3X_IF_I2C, TC3X_STREAM_IO_SPIMS);
TC3X_API_BBOpen ();
TC3X_API_BBPreInit (TC3X_STD_DMB);
TC3X_API_PreSet_PLLData (0x60, 0x06, 0x60, 0x03, 19200);
TC3X_API_ColdBootTable (&Data[0], Size);
TC3X_API_BBInit ();

// Start Service

TC3X_API_SetFreq (1500, 181280, NULL);

// Get lock status
TC3X_API_GetTDMB_Lock (0, &LockStat);
if (!(LockStat.CFO && LockStat.CTO))
{
// No Channel
return;
}

// Service select

// TDMB/DAB Stream will come TC3X_API_TDMB_OccurEvent()
// First, FIC Stream will come.
// User must parse FIC, know other service informations.

{
// There are 5 Services in one ensemble.
TDMBService_t TDMBService[5];

// Set Services Information
// TDMBService[0]??.[4]??

// Start Service#0, Service#4
TC3X_API_SET_TDMB_Service (1, &TDMBService[0]);
TC3X_API_SET_TDMB_Service (1, &TDMBService[4]);

// Stop Service#4
TC3X_API_SET_TDMB_Service (0, &TDMBService[4]);
}

// Get Status monitoring
{
double    SNR;
double    AVGSNR;
double    PCBER;
double    AVGPCBER;
double    RSSI;
double    AVGRSSI;

//TC3X_API_GetTDMB_SNR (0, &SNR, &AVGSNR);
TC3X_API_GetTDMB_PCBER (0, &PCBER, &AVGPCBER);
TC3X_API_GetTDMB_RSSI (0, &RSSI, &AVGRSSI);
// You can also get following status.
//TC3X_API_GetTDMB_VITERBIBER(uint32_t DeviceIdx, double *pVITERBIBER, double *pMovingAVGVITERBIBER)
//TC3X_API_GetTDMB_TSPER(uint32_t DeviceIdx, double *pTSPER, double *pMovingAVGTSPER)
}

// if user change freq
TC3X_API_SetFreq (1500, 183008, NULL);

// end service.
TC3X_API_StreamPause ();
TC3X_API_Close ();
}
#endif // STDDEF_DMB
*/
