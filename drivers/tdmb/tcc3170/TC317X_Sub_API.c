//----------------------------------------------------------------------------
//   FileName    : TC317X_Sub_API.c
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#include "TC317X_Sub_API.h"
#include "IO/TC317X_IO.h"

#if defined (USE_IF_SRAMLIKE)
#include "IO/TC317X_IO_SRAMLIKE.h"
#endif // USE_IF_SRAMLIKE
#if defined (USE_IF_I2C)
#include "IO/TC317X_IO_I2C.h"
#endif // USE_IF_I2C
#if defined (USE_IF_CSPI)
#include "IO/TC317X_IO_CSPI.h"
#endif // USE_IF_CSPI
#if defined (USE_IF_SDIO)
#include "IO/TC317X_IO_SDIO.h"
#endif // USE_IF_SDIO
#if defined (USE_IF_HPI)
#include "IO/TC317X_IO_HPI.h"
#endif // USE_IF_HPI

#include "IO/TC317X_IO_UTIL.h"

// extern
extern TC3X_DNUM TC317X_Entry (int32_t moduleidx,
    TC3X_DNUM hSTD,
    FN_v_ihv * BB_PreInit,
    FN_i_ivi * BB_ColdBoot,
    FN_v_ihi * BB_Init,
    FN_v_ih * BB_Close,
    FN_i_ihiiv * BB_SetFrequency,
    FN_v_ihiPci * BB_StartService,
    FN_v_ihi * BB_StopService, FN_v_ihi * BB_CtrlPower, FN_i_ihivvvv * BB_UserFunction, FN_v_ihivvvv eventFn,
    int32_t option);
extern void TC317X_AttachDevice (int32_t moduleidx, int32_t num);

tSTDInfo  g_tSTDInfo[MAX_TCC3X];
void     *pg_tSTDInfo0 = NULL;
void     *pg_tSTDInfo1 = NULL;
tSTDCtrl **g_pSTDCtrl = NULL;

typedef struct
{
  uint32_t open_count;
  uint32_t Standard;
} tTCC3X_Driver;

tTCC3X_Driver TCC3X_Driver[MAX_TCC3X];
void     *pTCC3X_Driver0 = NULL;
void     *pTCC3X_Driver1 = NULL;

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_Init
//    Description : Application initialize code
//    Input :
//      moduleidx : module index
//      NumOfDemodule : Number of Demodulator
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_Init (int32_t moduleidx, int32_t NumDemodule)
{
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (moduleidx >= maxbb)
  {
    PRINTF_LV_0 ("[Error] TC3X to much devices\n");
    return;
  }

  switch (moduleidx)
  {
    case 0:
      if (pTCC3X_Driver0)
      {
        PRINTF_LV_0 ("[Error] TC3X aleady opend\n");
        return;
      }
      else
      {
        pTCC3X_Driver0 = &TCC3X_Driver[0];
      }
      break;

    case 1:
      if (pTCC3X_Driver1)
      {
        PRINTF_LV_0 ("[Error] TC3X aleady opend\n");
        return;
      }
      else
      {
        pTCC3X_Driver1 = &TCC3X_Driver[1];
      }
      break;

    default:
      break;
  }

  TCC3X_Driver[moduleidx].open_count++;

  TC317X_IO_memset (&g_tSTDInfo[moduleidx], 0, sizeof (tSTDInfo));
  g_tSTDInfo[moduleidx].NumOfDemodule = NumDemodule;

  if (!g_pSTDCtrl)
  {
    g_pSTDCtrl = (tSTDCtrl **) TC317X_IO_malloc (sizeof (tSTDCtrl) * maxbb);
    TC317X_IO_memset (g_pSTDCtrl, 0, sizeof (g_pSTDCtrl) * maxbb);
  }

  if (g_pSTDCtrl[moduleidx])
  {
    TC317X_IO_free (g_pSTDCtrl[moduleidx]);
    g_pSTDCtrl[moduleidx] = NULL;
  }

  g_pSTDCtrl[moduleidx] = (tSTDCtrl *) TC317X_IO_malloc (sizeof (tSTDCtrl) * g_tSTDInfo[moduleidx].NumOfDemodule);
  TC317X_IO_memset (g_pSTDCtrl[moduleidx], 0, sizeof (tSTDCtrl) * g_tSTDInfo[moduleidx].NumOfDemodule);

  TC317X_IO_Host_Preset (moduleidx);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_IOOpen
//    Description : IO Open API
//    Input :
//      moduleidx : module index
//      MainIO : main IO
//      StreamIO : stream IO
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_IOOpen (int32_t moduleidx, int32_t MainIO, int32_t StreamIO)
{
  g_tSTDInfo[moduleidx].MainIO = MainIO;
  g_tSTDInfo[moduleidx].StreamIO = StreamIO;

 // io select & setting
  TC317X_SubAPI_MainIOSel (moduleidx, MainIO);
  TC317X_SubAPI_StreamIOSel (moduleidx, StreamIO);

  switch (moduleidx)
  {
    case 0:
      pg_tSTDInfo0 = &g_tSTDInfo[moduleidx];
      break;

    case 1:
      pg_tSTDInfo1 = &g_tSTDInfo[moduleidx];
      break;

    default:
      break;
  }

  // BB & RF PWON
  TC317X_SubAPI_PWON (moduleidx);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBOpen
//    Description : Find BaseBand & Attach
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_BBOpen (int32_t moduleidx)
{
  g_tSTDInfo[moduleidx].BBType = TC317X_SubAPI_BBDetect (moduleidx);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_GetPacketInfo
//    Description : Get packet information
//    Input :
//      moduleidx : module index
//      Standard : Standard
//      pPktSize : packet size
//      pPktThrNum : packet threshold number
//    Output :
//      0
//    Remark :
//--------------------------------------------------------------------------
static int32_t TC317X_SubAPI_GetPacketInfo (int32_t moduleidx, int32_t *pPktSize, int32_t *pPktThrNum)
{
  uint32_t table_pktThrNum[TC3X_STREAM_IO_MAX + 1] = {
    TC3X_TDMB_PKTNUM_CMDIO, TC3X_TDMB_PKTNUM,
    TC3X_TDMB_PKTNUM, TC3X_TDMB_PKTNUM,
    TC3X_TDMB_PKTNUM, TC3X_TDMB_PKTNUM_CMDIO,
    TC3X_TDMB_PKTNUM_CMDIO, TC3X_TDMB_PKTNUM
  };

  *pPktSize = TC3X_TDMB_PKTSIZE;

  if (g_tSTDInfo[moduleidx].StreamIO < TC3X_STREAM_IO_MAX)
    *pPktThrNum = table_pktThrNum[g_tSTDInfo[moduleidx].StreamIO];
  else
    *pPktThrNum = table_pktThrNum[TC3X_STREAM_IO_MAX];

  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBPreInit
//    Description : BaseBand Pre-Initialize & Register Callback Function
//    Input :
//      moduleidx : module index
//      Standard : Standard
//      EVENTFunction : Event function (call func)
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_BBPreInit (int32_t moduleidx, FN_v_ihivvvv EVENTFunction)
{
  int32_t       i;
  tSTDIOSet STDIOSet;
  int32_t       pktSize, pktThrNum;

  TC317X_SubAPI_GetPacketInfo (moduleidx, &pktSize, &pktThrNum);

  for (i = 0; i < g_tSTDInfo[moduleidx].NumOfDemodule; i++)
  {
    switch (g_tSTDInfo[moduleidx].BBType)
    {
      case DEVICE_BB_TC3X:
        g_pSTDCtrl[moduleidx][i].bbType = DEVICE_BB_TC3X;

        g_pSTDCtrl[moduleidx][i].hBB = TC317X_Entry (moduleidx, i,
            &g_pSTDCtrl[moduleidx][i].BB_PreInit, &g_pSTDCtrl[moduleidx][i].BB_ColdBoot,
            &g_pSTDCtrl[moduleidx][i].BB_Init, &g_pSTDCtrl[moduleidx][i].BB_Close,
            &g_pSTDCtrl[moduleidx][i].BB_SetFrequency, &g_pSTDCtrl[moduleidx][i].BB_StartService,
            &g_pSTDCtrl[moduleidx][i].BB_StopService, &g_pSTDCtrl[moduleidx][i].BB_CtrlPower,
            &g_pSTDCtrl[moduleidx][i].BB_UserFunction, EVENTFunction, 0);

        STDIOSet.MainIO = g_tSTDInfo[moduleidx].MainIO;
        STDIOSet.StreamIO = g_tSTDInfo[moduleidx].StreamIO;
        STDIOSet.PktSize = pktSize;
        STDIOSet.PktThrNum = pktThrNum;

        g_pSTDCtrl[moduleidx][i].BB_PreInit (moduleidx, g_pSTDCtrl[moduleidx][i].hBB, (void *) &STDIOSet);
        g_pSTDCtrl[moduleidx][i].BSetted = 1;       // Setted
        break;
    }
  }
}

// -------------------------------------------------------
// BB ColdBoot
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBColdBoot_NONWrite
//    Description : Cold boot without write to memory
//    Input :
//      moduleidx : module index
//    Output :
//      Success: 0x1ACCE551 Fail : others
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_SubAPI_BBColdBoot_NONWrite (int32_t moduleidx)
{
  int32_t       ret;
  ret = TC317X_SubAPI_COLDBOOT (moduleidx, NULL, CMD_BB_COLDBOOT_NON_WRITE);
  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBColdBoot_FILE
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
int32_t TC317X_SubAPI_BBColdBoot_FILE (int32_t moduleidx, uint8_t *pData, uint32_t Size, int32_t option)
{
  int32_t       ret;
  TC3X_BOOTBIN BootBin;

  ret = TC317X_IO_UTIL_ColdBootParser (pData, Size, &BootBin);
  if (!ret)
  {
    PRINTF_LV_0 ("# coldboot parser error\n");
  }
  ret = TC317X_SubAPI_COLDBOOT (moduleidx, &BootBin, option);

  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBColdBoot_MEM
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
int32_t TC317X_SubAPI_BBColdBoot_MEM (int32_t moduleidx, uint8_t *pData, uint32_t Size, int32_t option)
{
  int32_t       ret;
  TC3X_BOOTBIN BootBin;

  ret = TC317X_IO_UTIL_ColdBootParser (pData, Size, &BootBin);
  if (!ret)
  {
    PRINTF_LV_0 ("# coldboot parser error\n");
  }
  ret = TC317X_SubAPI_COLDBOOT (moduleidx, &BootBin, option);

  return ret;
}

//uint8_t *pNANDDAta;
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBColdBoot_NANDFLASH
//    Description : Write Boot Code Binary File to BB from nandflash
//    Input :
//      moduleidx : module index
//      pData : Boot Binary Data Pointer
//      size : size of boot binary
//      option : broadcast / normal / non write
//    Output :
//      Success: 0x1ACCE551 Fail : others
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_SubAPI_BBColdBoot_NANDFLASH (int32_t moduleidx, char *FileName, int32_t option)
{
#if defined(__NUCLEUS__)
  TC3X_BOOTBIN BootBin;
#if defined (TCC78X) || defined (TCC79X)
  signed int32_t pFile;
#else
  void     *pFile;
#endif

  uint32_t size;
  int32_t       ret;

  pFile = TC317X_IO_UTIL_fopen (FileName, "rb");
  size = TC317X_IO_UTIL_fseek (pFile, 0, TC_SEEK_END);
  TC317X_IO_UTIL_fseek (pFile, 0, TC_SEEK_SET);

  pNANDDAta = TC317X_IO_malloc (size);

  TC317X_IO_UTIL_fread (&pNANDDAta[0], 1, size, pFile);

  ret = TC317X_IO_UTIL_ColdBootParser (&pNANDDAta[0], size, &BootBin);
  if (!ret)
  {
    PRINTF_LV_0 ("# coldboot parser error\n");
  }

  TC317X_IO_UTIL_fclose (pFile);

  ret = TC317X_SubAPI_COLDBOOT (moduleidx, &BootBin, option);

  TC317X_IO_free (pNANDDAta);
  return ret;
#endif
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_COLDBOOT
//    Description : cold boot
//    Input :
//      moduleidx : module index
//      BootBin : boot binary
//      option : option
//    Output :
//      Success: 0x1ACCE551 Fail : others
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_SubAPI_COLDBOOT (int32_t moduleidx, TC3X_BOOTBIN * BootBin, int32_t option)
{       // access at once
  int32_t       ret;
  ret = g_pSTDCtrl[moduleidx][0].BB_ColdBoot (moduleidx, (void *) BootBin, option);
  return ret;
}

// First Setup Slave and then Setup Master.
// TC3X GPIO Setup & RF Search & RF Init
//void TC3X_SubAPI_BBInit (TC3X_DNUM handle, int32_t Reserved0)
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBInit
//    Description : BB Initialization
//    Input :
//      moduleidx : module index
//      Reserved0 : None
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_BBInit (int32_t moduleidx, int32_t Reserved0)
{
  g_pSTDCtrl[moduleidx][0].BB_Init (moduleidx, g_pSTDCtrl[moduleidx][0].hBB, 0);
  g_pSTDCtrl[moduleidx][0].BInited = 1;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_Close
//    Description : close sub api
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_Close (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;

  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;

  // Detach
  TC317X_SubAPI_BBDetach (moduleidx);   // include rfdetach

  // BB & RF PWDN
  TC317X_SubAPI_PWDN (moduleidx);

  // IO Close
  switch (g_tSTDInfo[moduleidx].MainIO)
  {
    case TC3X_IF_SRAMLIKE:
#if defined (USE_IF_SRAMLIKE)
      TC317X_IO_SRAMLIKE_Close (moduleidx);
#endif // USE_IF_SRAMLIKE
      break;

    case TC3X_IF_I2C:
#if defined (USE_IF_I2C)
      TC317X_IO_I2C_Close (moduleidx);
#endif // USE_IF_I2C
      break;

    case TC3X_IF_CSPI:
#if defined (USE_IF_CSPI)

      TC317X_IO_CSPI_Close (moduleidx);
#endif // USE_IF_CSPI
      break;

    case TC3X_IF_SDIO1BIT:
#if defined (USE_IF_SDIO)
      TC317X_IO_SDIO_Close (moduleidx);
#endif // USE_IF_SDIO
      break;

    case TC3X_IF_SDIO4BIT:
#if defined (USE_IF_SDIO)
      TC317X_IO_SDIO_Close (moduleidx);
#endif // USE_IF_SDIO
      break;
  }

  if (g_pSTDCtrl[moduleidx])
  {
    TC317X_IO_free (g_pSTDCtrl[moduleidx]);
    g_pSTDCtrl[moduleidx] = NULL;
  }

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (g_pSTDCtrl[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (g_pSTDCtrl);
    g_pSTDCtrl = NULL;
  }

  TCC3X_Driver[moduleidx].open_count = 0;

  switch (moduleidx)
  {
    case 0:
      pTCC3X_Driver0 = NULL;
      break;

    case 1:
      pTCC3X_Driver1 = NULL;
      break;

    default:
      break;
  }
}

// -------------------------------------------------------
// PW Ctrl
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_PWON
//    Description : power on
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_PWON (int32_t moduleidx)
{
  TC317X_IO_PW_ON (moduleidx);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_PWDN
//    Description : power down
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_PWDN (int32_t moduleidx)
{
  TC317X_IO_PW_DN (moduleidx);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_PWRESET
//    Description : power reset
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_PWRESET (int32_t moduleidx)
{
  TC317X_IO_PW_RESET (moduleidx);
}

// -------------------------------------------------------
// Select IO
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_MainIOSel
//    Description : select main io and initialize
//    Input :
//      moduleidx : module index
//      IOSeries : io series
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_MainIOSel (int32_t moduleidx, int32_t IOSeries)
{
  switch (IOSeries)
  {
    case TC3X_IF_SRAMLIKE:
#if defined (USE_IF_SRAMLIKE)
      TC317X_IO_SRAMLIKE_Init (moduleidx);
#endif // USE_IF_SRAMLIKE
      break;

    case TC3X_IF_I2C:
#if defined (USE_IF_I2C)
      TC317X_IO_I2C_Init (moduleidx);
#endif // USE_IF_I2C
      break;

    case TC3X_IF_CSPI:
#if defined (USE_IF_CSPI)
      TC317X_IO_CSPI_Init (moduleidx);
#endif // USE_IF_CSPI
      break;

    case TC3X_IF_SDIO1BIT:
#if defined (USE_IF_SDIO)
      TC317X_IO_SDIO_Init (moduleidx, 1);
#endif // USE_IF_SDIO
      break;

    case TC3X_IF_SDIO4BIT:
#if defined (USE_IF_SDIO)
      TC317X_IO_SDIO_Init (moduleidx, 4);
#endif // USE_IF_SDIO
      break;
  }
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_StreamIOSel
//    Description : select stream io
//    Input :
//      moduleidx : module index
//      IOSeries : io series
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_StreamIOSel (int32_t moduleidx, int32_t IOSeries)
{

}

// -------------------------------------------------------
// BB Detect
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBDetect
//    Description : detect baseband
//    Input :
//      moduleidx : module index
//    Output :
//      baseband
//    Remark :
//--------------------------------------------------------------------------
int32_t TC317X_SubAPI_BBDetect (int32_t moduleidx)
{
  TC317X_SubAPI_BBAttach (moduleidx, DEVICE_BB_TC3X);
  return DEVICE_BB_TC3X;
}

// -------------------------------------------------------
// BB Attach
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBAttach
//    Description : attach baseband
//    Input :
//      moduleidx : module index
//      DeviceBB : device baseband
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_BBAttach (int32_t moduleidx, int32_t DeviceBB)
{
  switch (DeviceBB)
  {
    case DEVICE_BB_TC3X:
      TC317X_AttachDevice (moduleidx, g_tSTDInfo[moduleidx].NumOfDemodule);
      break;
  }
}

// =======================================================
// Service Control
//

//  SetFreq Sequence : Stream Stop - Stream Start - SetFreq
//  Standard
//  Change Freq (same standard) : Same as SetFreq Sequence
//  Change Freq (diff standard) : BB PreInit(standard) - ColdBoot(standard) - BBInit(standard) - SetFreq Sequence

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_SetFrequency
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
int32_t TC317X_SubAPI_SetFrequency (int32_t moduleidx, int32_t freq_khz, int32_t bw_khz, tFreqSet_Option * pOption)
{
  return (g_pSTDCtrl[moduleidx][0].BB_SetFrequency (moduleidx, g_pSTDCtrl[moduleidx][0].hBB, freq_khz, bw_khz, pOption));
}

// =======================================================
// Driver Detach
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_SubAPI_BBDetach
//    Description : detach baseband
//    Input :
//      moduleidx : module index
//    Output :
//   
//    Remark :
//--------------------------------------------------------------------------
void TC317X_SubAPI_BBDetach (int32_t moduleidx)
{
  if (!g_pSTDCtrl)
    return;

  if (!g_pSTDCtrl[moduleidx])
    return;

  if (!g_pSTDCtrl[moduleidx][0].BB_Close)
    return;

  g_pSTDCtrl[moduleidx][0].BB_Close (moduleidx, g_pSTDCtrl[moduleidx][0].hBB);
}
