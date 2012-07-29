//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Mailbox.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#include "../IO/TC317X_IO.h"
#include "TC317X_Core.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_Mailbox.h"
#include "TC317X_Core_Util.h"
#include "TC317X_Core_TDMB.h"

#define MAXWAITMAILBOX 1000000
//#define PRINT_MAILBOX_PC

void TC317X_MailBoxDebugCMD (int32_t Message)
{
}

#if defined (PRINT_MAILBOX_PC)
void TC317X_MailBoxPC (t_TC3X_DEMODULE * pTC317X)
{
  int32_t       i;
  PRINTF_LV_0 ("---------------------------\n");

  for (i = 0; i < 10; i++)
  {
    uint32_t mailstat0;
    uint32_t mailstat1;
    uint32_t mailstat2;
    uint32_t valued;
    valued = 0x00;

    TC317XREGSET_Manual (pTC317X, 0x1a, 0xFF, 0);
    mailstat0 = TC317XREGGET_Manual (pTC317X, 0x1a, 0);
    mailstat1 = TC317XREGGET_Manual (pTC317X, 0x1b, 0);
    mailstat2 = TC317XREGGET_Manual (pTC317X, 0x1c, 0);

    valued = (((mailstat0 & 0x7f) << 8 | (mailstat1 & 0xff)) << 2);

    if (mailstat0 & Bit7)
      PRINTF_LV_0 ("OP Hang-up\n");
    else
      PRINTF_LV_0 ("OP OK\n");

    PRINTF_LV_0 ("PC [0x%x]\n", valued);
    PRINTF_LV_0 ("DebugRegister [0x%x]\n", mailstat2);
  }

  PRINTF_LV_0 ("---------------------------\n");
}
#endif // PRINT_MAILBOX_PC

int32_t TC317X_MB_MailStat (uint32_t input, uint32_t WFlag)
{
  static uint32_t stat;

  if (WFlag)
    stat = input;
  else
    return stat;

  return stat;
}

void TC317X_MB_TxMail (t_TC3X_DEMODULE * pTC317X, int32_t rw_flag, int32_t cmd, uint32_t *data_array, int32_t word_cnt, int32_t *pmailboxok)
{
  uint32_t temp;
  uint32_t i;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID

  pmailboxok[0] = 1;
  TC317XREGSET_MailBox_CTRL (pTC317X, TC3XREG_MAIL_INIT, 0);

  temp = (MB_HOSTMAIL << 24) | (word_cnt << 20) | (rw_flag << 19) | (MB_ERR_OK << 16) | cmd;

  // LSB First
  TC317XREGSET_MailBox_FIFOWindow (pTC317X, temp & 0xff);
  TC317XREGSET_MailBox_FIFOWindow (pTC317X, (temp >> 8) & 0xff);
  TC317XREGSET_MailBox_FIFOWindow (pTC317X, (temp >> 16) & 0xff);
  TC317XREGSET_MailBox_FIFOWindow (pTC317X, (temp >> 24) & 0xff);

  for (i = 0; i < (uint32_t) word_cnt; i++)
  {
    TC317XREGSET_MailBox_FIFOWindow (pTC317X, data_array[i] & 0xff);
    TC317XREGSET_MailBox_FIFOWindow (pTC317X, (data_array[i] >> 8) & 0xff);
    TC317XREGSET_MailBox_FIFOWindow (pTC317X, (data_array[i] >> 16) & 0xff);
    TC317XREGSET_MailBox_FIFOWindow (pTC317X, (data_array[i] >> 24) & 0xff);
  }

  TC317X_MB_MailStat (cmd, 1);
  TC317XREGSET_MailBox_CTRL (pTC317X, TC3XREG_MAIL_HOSTMAILPOST, 0);

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    if (!(pTC317X->OP_Status & TC3XREG_SYS_EN_DSP))    // Exceptional Error
    {
      PRINTF_LV_0 ("# [Error] MailBox - OP Disabled!!! \n");
      return;
    }

    if (TC317X_GetInterval (CurrTime) > MAXWAITMAILBOX)
    {
      pmailboxok[0] = -1;
#if defined (PRINT_MAILBOX_PC)
      TC317X_MailBoxPC (pTC317X);
#endif // PRINT_MAILBOX_PC
      return;
    }

    TC317X_IO_Sleep (0);
  }
  while (!(TC317XREGGET_MailBox_FIFO_WriteStat (pTC317X) & 0x01));
}

int32_t TC317X_MB_RxMail (t_TC3X_DEMODULE * pTC317X, tMAILBOX * p_mailbox, uint32_t warmboot, int32_t *pmailboxok)
{
  int32_t       temp;
  int32_t       i;
  int32_t       total_word_num;
  int32_t       total_byte_num;
  uint32_t cmd;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID

  pmailboxok[0] = 1;

  i = 0;
  CurrTime = TC317X_GetTickCnt ();

  do
  {
    if (!(pTC317X->OP_Status & TC3XREG_SYS_EN_DSP))    // Exceptional Error
    {
      PRINTF_LV_0 ("# [Error] MailBox - OP Disabled!!! \n");
      return -1;
    }

    temp = TC317XREGGET_MailBox_FIFO_ReadStat (pTC317X);

    if (TC317X_GetInterval (CurrTime) > MAXWAITMAILBOX)
    {
      pmailboxok[0] = -1;
#if defined (PRINT_MAILBOX_PC)
      TC317X_MailBoxPC (pTC317X);
#endif // PRINT_MAILBOX_PC
      return -1;
    }
    TC317X_IO_Sleep (0);
  }
  while ((temp & 0xfc) < 3);

  if (warmboot)
  {
    TC317XREGGET_MailBox_FIFO_WordData (pTC317X, &p_mailbox->data_array[0], 1);
    return 0;
  }

  total_byte_num = (temp >> 2) & 0x3f;
  total_word_num = (total_byte_num >> 2);

  // LSB First
  TC317XREGGET_MailBox_FIFO_WordData (pTC317X, &cmd, 1);
  TC317XREGGET_MailBox_FIFO_WordData (pTC317X, &p_mailbox->data_array[0], total_word_num - 1);

  //mark check
  if ((cmd >> 24) != MB_SLAVEMAIL)
  {
    uint32_t mailstat;

    PRINTF_LV_0 ("# Mailbox Error Cmd[0x%x] Total Byte Num[0x%x]\n", cmd, total_byte_num);

    for (i = 0; i < total_word_num - 1; i++)
    {
      PRINTF_LV_0 ("# data_array[%d][0x%x]\n", i, p_mailbox->data_array[i]);
    }

    mailstat = TC317X_MB_MailStat (cmd, 0);
    PRINTF_LV_0 ("# [MBERR]Mark Error[0x%x][ori:0x%x]\n", cmd, mailstat);
    PRINTF_LV_0 ("# [MBERR]please check the header.\n");
    pmailboxok[0] = -1;
    return -1;
  }

  p_mailbox->cmd = cmd & 0xffff;
  p_mailbox->word_cnt = total_word_num - 1;
  p_mailbox->status = ((cmd >> 16) & 0x07);

  if (p_mailbox->status != 0)
  {
    PRINTF_LV_0 ("# [MBERR] Error Message : 0x%0x[cmd 0x%x][totalnum 0x%x]\n", p_mailbox->status, cmd, total_word_num);
    pmailboxok[0] = -1;
    return -1;
  }

  return total_word_num - 1;
}

int32_t TC317X_MB_TxOnly_Sub (t_TC3X_DEMODULE * pTC317X, int32_t cmd, uint32_t *data_array, int32_t word_cnt)
{
  int32_t       mailboxok;

  TC317X_IO_MailBox_LOCK (pTC317X->moduleidx);
  TC317X_MB_TxMail (pTC317X, MB_CMD_WRITE, cmd, data_array, word_cnt, &mailboxok);
  TC317X_IO_MailBox_UnLOCK (pTC317X->moduleidx);

  if (mailboxok == -1)
  {
    PRINTF_LV_0 ("# [ERR] MailboxTX Error.\n");
    TC317X_Mailbox_Error_Handler (pTC317X);
    return -1;
  }

  return 1;
}

int32_t TC317X_MB_TxOnly (t_TC3X_DEMODULE * pTC317X, int32_t cmd, uint32_t *data_array, int32_t word_cnt)
{
  int32_t       ret;

  TC317X_IO_OP_Mailbox_LOCK(pTC317X->moduleidx);
  if(!(pTC317X->OP_Status&TC3XREG_SYS_EN_DSP))
  {
    TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
    return -1;
  }
  if (pTC317X->mailboxErrorCounter > MAX_MAILBOX_RETRY)
  {
    g_pTC3XCTRL[pTC317X->moduleidx][0].BBEventFn (pTC317X->moduleidx, 0, TC3X_ERROR_CRITICAL_MAILBOX, NULL, NULL, NULL, NULL);
    TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
    return -1;
  }

  ret = TC317X_MB_TxOnly_Sub (pTC317X, cmd, data_array, word_cnt);

  if (ret == -1)
  {
    // one more time set and give up
    PRINTF_LV_0 ("# [M] Mailbox Retry\n");
    ret = TC317X_MB_TxOnly_Sub (pTC317X, cmd, data_array, word_cnt);
    if (ret == -1)
    {
      PRINTF_LV_0 ("# [ERR] Can't recover mailbox. Please Retune!\n");
      pTC317X->mailboxErrorCounter++;
      TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
      return -1;
    }
  }

  pTC317X->mailboxErrorCounter = 0;
  TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
  return 1;
}

int32_t TC317X_MB_TxnRead_Sub (t_TC3X_DEMODULE * pTC317X, tMAILBOX * p_mailbox, int32_t cmd, uint32_t *data_array, int32_t word_cnt, int32_t warmboot)
{
  int32_t       ret = 0;
  int32_t       mailboxok;

  TC317X_IO_MailBox_LOCK (pTC317X->moduleidx);

  if (cmd == MBPARA_SYS_WARMBOOT)
  {
    TC317X_MB_TxMail (pTC317X, MB_CMD_WRITE, cmd, data_array, word_cnt, &mailboxok);
  }
  else
  {
    TC317X_MB_TxMail (pTC317X, MB_CMD_READ, cmd, data_array, word_cnt, &mailboxok);
  }

  if (mailboxok == -1)
  {
    TC317X_IO_MailBox_UnLOCK (pTC317X->moduleidx);
    PRINTF_LV_0 ("# [ERR] MailboxTXRX Error. cmd[%x]\n", cmd);
    TC317X_Mailbox_Error_Handler (pTC317X);
    return -1;
  }
  else
  {
    ret = TC317X_MB_RxMail (pTC317X, p_mailbox, warmboot, &mailboxok);

    if (mailboxok == -1)
    {
      TC317X_IO_MailBox_UnLOCK (pTC317X->moduleidx);
      PRINTF_LV_0 ("# [ERR] MailboxTXRX Error. cmd[%x]\n", cmd);
      TC317X_Mailbox_Error_Handler (pTC317X);
      return -1;
    }
  }
  TC317X_IO_MailBox_UnLOCK (pTC317X->moduleidx);
  return ret;
}

int32_t TC317X_MB_TxnRead (t_TC3X_DEMODULE * pTC317X, tMAILBOX * p_mailbox, int32_t cmd, uint32_t *data_array, int32_t word_cnt, int32_t warmboot)
{
  int32_t ret;

  TC317X_IO_OP_Mailbox_LOCK(pTC317X->moduleidx);
  if(!(pTC317X->OP_Status&TC3XREG_SYS_EN_DSP))
  {
    TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
    return -1;
  }
  if (pTC317X->mailboxErrorCounter > MAX_MAILBOX_RETRY)
  {
    g_pTC3XCTRL[pTC317X->moduleidx][0].BBEventFn (pTC317X->moduleidx, 0, TC3X_ERROR_CRITICAL_MAILBOX, NULL, NULL, NULL, NULL);
    TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
    return -1;
  }

  ret = TC317X_MB_TxnRead_Sub (pTC317X, p_mailbox, cmd, data_array, word_cnt, warmboot);

  if (ret == -1)
  {
    // one more time set and give up
    PRINTF_LV_0 ("# [M] Mailbox Retry\n");
    ret = TC317X_MB_TxnRead_Sub (pTC317X, p_mailbox, cmd, data_array, word_cnt, warmboot);
    if (ret == -1)
    {
      PRINTF_LV_0 ("# [ERR] Can't recover mailbox. Please Retune!\n");
      pTC317X->mailboxErrorCounter++;
      TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
      return -1;
    }
  }

  pTC317X->mailboxErrorCounter = 0;
  TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
  return ret;
}

uint32_t TC317X_GetAccessMail (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t access_mail;

  TC317XREGGET_MailBox_FIFO_WordData (pTC317X, &access_mail, 1);
  //PRINTF_LV_1 ("# [%d] AccessMail[0x%08x]\n", pTC317X->ChainNum, access_mail);
  return access_mail;
}

//------------------------------------------------------------------
// Error routine for mailbox

void TC317X_Mailbox_Error_Handler (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t NumData[7];
  uint32_t FreqData[7];

  // basically
  TC317X_IO_OP_Mailbox_UnLOCK(pTC317X->moduleidx);
  TC317X_OP_Disable (pTC317X, 0);
  TC317XREGSET_MailBox_CTRL (pTC317X, TC3XREG_MAIL_INIT, 0);

  switch (pTC317X->MainIF.MainIFDefine)
  {
    case TC3X_IF_I2C:
      TC317X_OP_Enable (pTC317X, 0);
      break;

    case TC3X_IF_SRAMLIKE:
    case TC3X_IF_CSPI:
    case TC3X_IF_SDIO1BIT:
    case TC3X_IF_SDIO4BIT:
      TC317X_OP_Enable (pTC317X, 0);
      break;

    default:
      TC317X_OP_Enable (pTC317X, 0);
      break;
  }

  switch (pTC317X->mailboxLog)
  {
    case MAILBOX_LOG_FREQSET_PREPARE:
      break;
    case MAILBOX_LOG_FREQSET_START:
      break;
    case MAILBOX_LOG_FREQSET_STOP:
      break;

    case MAILBOX_LOG_FREQSET_START_STEP1:
      NumData[0] = 1;
      TC317X_MB_TxOnly (pTC317X, MBPARA_SYS_NUM_FREQ, &NumData[0], 1);
      break;

    case MAILBOX_LOG_FREQSET_START_STEP2:
    case MAILBOX_LOG_SERVICE:
      TC317X_IO_memset (&FreqData[0], 0, sizeof (uint32_t) * 7);
      TC317X_IO_memset (&NumData[0], 0, sizeof (uint32_t) * 7);

      NumData[0] = 1;
      FreqData[0] = pTC317X->NIT.FREQ_KHz;
      TC317X_MB_TxOnly (pTC317X, MBPARA_SYS_NUM_FREQ, &NumData[0], 1);
      TC317X_MB_TxOnly (pTC317X, MBPARA_SYS_FREQ_0_6, &FreqData[0], 7);
      TC317X_WarmBoot (pTC317X, 1);
      break;

    default:
      break;
  }

  if (pTC317X->mailboxLog == MAILBOX_LOG_SERVICE)
  {
    SetTDMBStreamOn (pTC317X, 1, 1, 1);
  }
  TC317X_IO_OP_Mailbox_LOCK(pTC317X->moduleidx);
}
