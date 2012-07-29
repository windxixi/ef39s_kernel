//----------------------------------------------------------------------------
//   FileName    : TC317X_IO.h
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#ifndef __TC317X_IO_h__
#define __TC317X_IO_h__

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
//#include <mach/bsp.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#include "TC317X_IO_UserDefine.h"
#include "TC317X_IO_UTIL.h"

#define BURST_MODE_POS        24
#define BURST_MODE_MSK        0xFF
#define BURST_FIXED_MODE      0
#define BURST_CONTINUOUS_MODE 1

typedef struct semaphore    tc317x_sema_t;
typedef struct work_struct  tc317x_task_t;
typedef wait_queue_head_t   tc317x_queue_t;
typedef irqreturn_t         tc317x_irqreturn_t;

#if defined(TC317X_USING_WORKQUEUE)
typedef void                tc317x_taskreturn_t;
typedef struct work_struct  tc317x_taskparam_t, *ptr_tc317x_taskparam_t;
#elif defined(TC317X_USING_KTHREAD)
typedef int32_t             tc317x_taskreturn_t;
typedef void                tc317x_taskparam_t, *ptr_tc317x_taskparam_t;
#endif

typedef tc317x_taskreturn_t (*tc317x_task_fn_t)(ptr_tc317x_taskparam_t);

struct tc317x_io_data_t {
  int32_t moduleidx;

  tc317x_sema_t sem_if;
  tc317x_sema_t sem_mbox;
  tc317x_sema_t sem_op;
  tc317x_task_t task;

  tc317x_queue_t wait_queue;
  tc317x_task_fn_t task_fn;

#if defined(TC317X_USING_KTHREAD)
  tc317x_taskreturn_t kt_id;
#endif
  int32_t task_running;
  int32_t irq_enabled;
  struct workqueue_struct *wq;
};

tc317x_irqreturn_t  TC317x_IO_ISR_CSPI_Handler(int32_t irq, void *param);

void      TC317X_IO_Host_Preset (int32_t moduleidx);

#ifdef USE_ANDROID
uint64_t TC317X_IO_GET_TIMECNT_ms (void);
#endif
uint32_t TC317X_IO_GET_TIMECNT_us (void);

void      TC317X_IO_PW_RESET (int32_t moduleidx);
void      TC317X_IO_PW_ON (int32_t moduleidx);
void      TC317X_IO_PW_DN (int32_t moduleidx);
void      TC317X_IO_Sleep (uint32_t ms);

void      TC317X_IO_Set_SPIHandler (int32_t moduleidx, FN_v_ii BB_SPI_SLV_LISR, FN_v_i BB_SPI_SLV_HISR);
void      TC317X_IO_Set_BBIntHandler (int32_t moduleidx, FN_v_i BB_INT_LISR, FN_v_i BB_INT_HISR);

void      TC317X_IO_ACTIVE_HISR_SPI (int32_t moduleidx);

void      TC317X_IO_IF_SemaphoreInit (int32_t moduleidx);
void      TC317X_IO_IF_SemaphoreDelete (int32_t moduleidx);

void      TC317X_IO_MailBox_SemaphoreInit (int32_t moduleidx);
void      TC317X_IO_MailBox_SemaphoreDelete (int32_t moduleidx);

void      TC317X_IO_OP_Mailbox_SemaphoreInit (int32_t moduleidx);
void      TC317X_IO_OP_Mailbox_SemaphoreDelete (int32_t moduleidx);

int32_t       TC317X_IO_IF_LOCK (int32_t moduleidx);
int32_t       TC317X_IO_IF_UnLOCK (int32_t moduleidx);
int32_t       TC317X_IO_MailBox_LOCK (int32_t moduleidx);
int32_t       TC317X_IO_MailBox_UnLOCK (int32_t moduleidx);

int32_t       TC317X_IO_OP_Mailbox_LOCK (int32_t moduleidx);
int32_t       TC317X_IO_OP_Mailbox_UnLOCK (int32_t moduleidx);

tc317x_queue_t*  TC317X_IO_QUE_INT_Create (int32_t moduleidx);
void      TC317X_IO_QUE_INT_Delete (int32_t moduleidx);

int32_t       TC317X_IO_QUE_Reset (tc317x_queue_t* queue);
int32_t       TC317X_IO_QUE_Send (tc317x_queue_t* queue, void *message);
int32_t       TC317X_IO_QUE_Receive (tc317x_queue_t* queue, void *message, uint32_t suspend);

void      TC317X_IO_InterruptTask_Create (int32_t moduleidx, tc317x_task_fn_t task_fn);
void      TC317X_IO_InterruptTask_Delete (int32_t moduleidx);

void      TC317X_IO_HISR_BB_INT_Create (int32_t moduleidx);
void      TC317X_IO_HISR_BB_INT_Delete (int32_t moduleidx);

void      TC317X_IO_BB_INT_Ctrl (int32_t moduleidx, int32_t command, int32_t TrigMode);
void      TC317X_IO_Stream_SPI_Ctrl (int32_t moduleidx, int32_t command, uint8_t **pStreamBuff, int32_t PktSize, int32_t PktNum, int32_t StreamIO);

void      TC317X_IO_LISR_BB_INT_Handler0 (void);
void      TC317X_IO_HISR_BB_INT_Handler0 (void);
void      TC317X_IO_LISR_SPI_Handler0 (void);
void      TC317X_IO_HISR_SPI_Handler0 (void);

void      TC317X_IO_LISR_BB_INT_Handler1 (void);
void      TC317X_IO_HISR_BB_INT_Handler1 (void);
void      TC317X_IO_LISR_SPI_Handler1 (void);
void      TC317X_IO_HISR_SPI_Handler1 (void);

void      TC317X_IO_SDIO_INT_Ctrl (int32_t moduleidx, int32_t command);
void     *TC317X_IO_malloc (uint32_t size);
void     *TC317X_IO_memset (void *p, int32_t init, int32_t size);
void     *TC317X_IO_memcpy (void *des, void *src, int32_t size);
int32_t       TC317X_IO_free (void *p);

#endif
