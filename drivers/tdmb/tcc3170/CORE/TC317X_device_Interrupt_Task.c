//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_device_Interrupt_Task.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================


#include "TC317X_device_Interrupt_Task.h"
#include "TC317X_deviceAPI.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_TDMB.h"
#include "TC317X_Core_Util.h"

#define INTR_STATUS_CLEAR_ONLY

extern uint32_t tc317x_cspi_read_size;

#if defined (USE_SDIO_DATA_INT)
extern void TC317X_IO_SDIO_Func0_Write (int32_t moduleidx, uint32_t RegAddr, uint8_t Data, int32_t InnerSem);
#endif // USE_SDIO_DATA_INT

void TC317XTask_Interrupt_Create (int32_t moduleidx)
{
  TC317X_IO_InterruptTask_Create (moduleidx, TC317XTask_Interrupt);
}

void TC317XTask_Interrupt_Delete (int32_t moduleidx)
{
  TC317X_IO_InterruptTask_Delete (moduleidx);
}

#if defined(TC317X_USING_WORKQUEUE)
tc317x_taskreturn_t TC317XTask_Interrupt(ptr_tc317x_taskparam_t param)
{
  //struct tc317x_io_data_t *io_data;
  uint32_t interrupt_stat, irq_err = 0;
  int32_t moduleidx = 0;
  int32_t triger = g_pTC3X[moduleidx][0].INT_TRIG;

  //io_data = container_of(param, struct tc317x_io_data_t, task);

  /* INT Level Mode */
  if(triger != TC3X_INT_LEVEL_FALLING && triger != TC3X_INT_LEVEL_RISING) {
    return;//continue;
  }
#ifdef INTR_STATUS_CLEAR_ONLY 
  interrupt_stat = 0x8;
  irq_err = 0;
#else //INTR_STATUS_CLEAR_ONLY 
  /* Read BB Interrupt Status */
  interrupt_stat = TC317XREGGET_IRQSTAT (&g_pTC3X[moduleidx][0]);
#ifdef TC317X_REDUCED_IO 
  irq_err = interrupt_stat & 0xFF;
  interrupt_stat = (interrupt_stat>>8) & 0xFF;
#else
  irq_err = TC317XREGGET_IRQERROR(&g_pTC3X[moduleidx][0]);
#endif
#endif //INTR_STATUS_CLEAR_ONLY 

  /* BB INT Clear */
  TC317XREGSET_IRQCLR (&g_pTC3X[moduleidx][0], (uint16_t)interrupt_stat);

  /* Stream Interrupt */
  if (interrupt_stat & TC3XREG_IRQ_STAT_DATAINT){
#ifndef INTR_STATUS_CLEAR_ONLY 
    if(irq_err){
      TCDBG("FIFO Overflow!!!, %02X!\n", irq_err);
      //UF_PauseStream(moduleidx);
      //UF_RestartStream(moduleidx);
    } else 
#endif
    {
      GetTDMBStream (moduleidx, interrupt_stat);
    }
    // Interrupt Enable
    TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_ENABLE,
        g_pTC3X[moduleidx][0].INT_TRIG);
  }
}

#elif defined(TC317X_USING_KTHREAD)
tc317x_taskreturn_t TC317XTask_Interrupt(ptr_tc317x_taskparam_t param)
{
  struct tc317x_io_data_t *io_data;
  uint32_t interrupt_stat, irq_err = 0;
  int32_t moduleidx = 0;
  int32_t triger = g_pTC3X[moduleidx][0].INT_TRIG;

  /* INT Level Mode */
  if(triger != TC3X_INT_LEVEL_FALLING && triger != TC3X_INT_LEVEL_RISING) {
    return -1;
  }

  set_user_nice(current, 15);
  io_data = (struct tc317x_io_data_t*)param;
  while(io_data->task_running){
    wait_event_interruptible(io_data->wait_queue, !io_data->irq_enabled);

#ifdef INTR_STATUS_CLEAR_ONLY 
    interrupt_stat = 0x8;
#else //INTR_STATUS_CLEAR_ONLY 
    /* Read BB Interrupt Status */
    interrupt_stat = TC317XREGGET_IRQSTAT (&g_pTC3X[moduleidx][0]);
#ifdef TC317X_REDUCED_IO 
    irq_err = interrupt_stat & 0xFF;
    interrupt_stat = (interrupt_stat>>8) & 0xFF;
#else
    irq_err = TC317XREGGET_IRQERROR(&g_pTC3X[moduleidx][0]);
#endif
#endif //INTR_STATUS_CLEAR_ONLY 

    /* BB INT Clear */
    TC317XREGSET_IRQCLR (&g_pTC3X[moduleidx][0], interrupt_stat);

    /* Stream Interrupt */
    if (interrupt_stat & (TC3XREG_IRQ_STAT_DATAINT)) {
#ifndef INTR_STATUS_CLEAR_ONLY 
      if(irq_err){
        TCDBG("FIFO Overflow!!!, reset fifo!\n");
        UF_PauseStream(moduleidx);
        UF_RestartStream(moduleidx);
      } else 
#endif
      {
        GetTDMBStream (moduleidx, interrupt_stat);
      }
      // Interrupt Enable
      TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_ENABLE,
          g_pTC3X[moduleidx][0].INT_TRIG);
    }
  }
  do_exit(0);
  return 0;
}
#endif
