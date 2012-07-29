//----------------------------------------------------------------------------
//   FileName    : TC317X_IO.c
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------

#include "TC317X_IO.h"
#include "../TC317X_Common.h"


// -----------------------------------------------------------------
// Resource for TC317X Driver
//

FN_v_ii   fnLISR_SPI = (FN_v_ii) TC317X_IO_UTIL_dummy_function10;
FN_v_i    fnHISR_SPI = (FN_v_i) TC317X_IO_UTIL_dummy_function2;
FN_v_i    fnHISR_BBINT0 = (FN_v_i) TC317X_IO_UTIL_dummy_function2;
FN_v_i    fnHISR_BBINT1 = (FN_v_i) TC317X_IO_UTIL_dummy_function2;

FN_v_i    fnLISR_BBINT0 = (FN_v_i) TC317X_IO_UTIL_dummy_function2;
FN_v_i    fnLISR_BBINT1 = (FN_v_i) TC317X_IO_UTIL_dummy_function2;


// -----------------------------------------------------------------
// External Symbols
#ifdef TC_AP
#define IRQ_TC317X INT_EI3

extern volatile PGPIO RGPIO;
extern volatile PPIC   RPIC;
#else
#define IRQ_TC317X 0
#endif

static struct tc317x_io_data_t tc317x_io_data[MAX_TCC3X];

// -----------------------------------------------------------------
// Set Host Prepare Setting Stuff

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_Host_Preset
//    Description : Perform the jobs requested at Host before running Driver.
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_Host_Preset (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_GET_TIMECNT_us
//    Description : Return current time count
//    Input :
//
//    Output :
//      time count us
//    Remark :
//--------------------------------------------------------------------------
uint64_t TC317X_IO_GET_TIMECNT_ms ()
{
	uint64_t tickcount = 0;
	struct timeval tv;

	do_gettimeofday (&tv);
	tickcount = (uint64_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return tickcount;
}


//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_Set_SPIHandler
//    Description : It is required only when using SPI/STS.
//                  Connect functions for SPI/STS Interrupt Handler.
//    Input :
//      moduleidx : module index
//      BB_SPI_SLV_LISR : spi handler lisr
//      BB_SPI_SLV_HISR : spi handler hisr
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_Set_SPIHandler (
  int moduleidx, FN_v_ii BB_SPI_SLV_LISR, FN_v_i BB_SPI_SLV_HISR)
{
	fnLISR_SPI = BB_SPI_SLV_LISR;
	fnHISR_SPI = BB_SPI_SLV_HISR;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_Set_BBIntHandler
//    Description : Baseband interrupt handler.
//                  Connect functions for baseband interrupt handler.
//    Input :
//      moduleidx : module index
//      BB_INT_LISR : bb interrupt handler lisr
//      BB_INT_HISR : bb interrupt handler hisr
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_Set_BBIntHandler
  (int moduleidx, FN_v_i BB_INT_LISR, FN_v_i BB_INT_HISR)
{
	switch (moduleidx)
	{
		case 0:
			fnLISR_BBINT0 = BB_INT_LISR;
			fnHISR_BBINT0 = BB_INT_HISR;
			break;

		case 1:
			fnLISR_BBINT1 = BB_INT_LISR;
			fnHISR_BBINT1 = BB_INT_HISR;
			break;

		default:
			PRINTF_LV_0 ("[ERROR] TC3X_IO_Set_BBINTHandler - Please insert code\n");
			break;
    }
}

// -----------------------------------------------------------------
// Power Control
//
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_PW_RESET
//    Description : Power Reset to reset Baseband
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_PW_RESET (int moduleidx)
{
#ifdef FEATURE_APPLY_EF40K
  tdmb_set_gpio(DMB_RESET, 0);
  msleep (100);
  tdmb_set_gpio(DMB_RESET, 1);
  msleep (100);
#endif

#ifdef TC_AP
  BITCLR (RGPIO->GPEFN1, Hw16 - Hw12); //DXB1_RST# Set GPIO mode
  BITSET (RGPIO->GPEEN,  Hw11); //DXB1_RST# Set GPIO Output mode
  BITCLR (RGPIO->GPEDAT, Hw11); //DXB1_RST# Clear
  msleep (100);
  BITSET (RGPIO->GPEDAT, Hw11); //DXB1_RST# Set
  msleep (100);
#endif
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_PW_ON
//    Description : power on
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_PW_ON (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];
  io_data->moduleidx = moduleidx;

#ifdef FEATURE_APPLY_EF40K
  tdmb_set_gpio(DMB_PWR_EN, 0);
  msleep (20);
  tdmb_set_gpio(DMB_PWR_EN, 1);
  msleep (20);
  TC317X_IO_PW_RESET(moduleidx);
#endif

#ifdef TC_AP
  BITCLR (RGPIO->GPEFN0, Hw16 - Hw12); //DXB1_PD Set GPIO mode

  BITSET (RGPIO->GPEEN,  Hw3);  //DXB1_PD Set GPIO Output mode
  BITCLR (RGPIO->GPEDAT, Hw3);  //DXB1_PD Clear
  msleep (20);
  BITSET (RGPIO->GPEDAT, Hw3);  //DXB1_PD Set
  msleep (20);

  TC317X_IO_PW_RESET(moduleidx);

  BITCLR(RGPIO->GPAFN1, Hw16-Hw12); //DXB1_IRQ Set GPIO mode
  BITCLR(RGPIO->GPAEN, Hw11);      //DXB1_IRQ input mode
  //TCDBG("GPAFN1:0x%X, GPAEN:0x%X\n", RGPIO->GPAFN1, RGPIO->GPAEN);
#endif
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_PW_DN
//    Description : power down
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_PW_DN (int moduleidx)
{
#ifdef FEATURE_APPLY_EF40K
  tdmb_set_gpio(DMB_PWR_EN, 0);
  tdmb_set_gpio(DMB_RESET, 0);
#else

struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

#ifdef TC_AP
  BITCLR (RGPIO->GPEFN0, Hw16 - Hw12);
  BITSET (RGPIO->GPEEN,  Hw3);  //DXB1_PD Set GPIO Output mode
  BITCLR (RGPIO->GPEDAT, Hw3);  //DXB1_PD Clear
  BITCLR (RGPIO->GPEDAT, Hw11); //DXB1_RST# Clear
  //TCDBG("GPAFN1:0x%X, GPAEN:0x%X\n", RGPIO->GPAFN1, RGPIO->GPAEN);
#endif
  free_irq(IRQ_TC317X, io_data);
#endif
}

//----------------------------------------------------------------------------------------
//
//  Sleep Function Define
//
//----------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_Sleep
//    Description : sleep
//    Input :
//      ms : milli second
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_Sleep (unsigned int ms)
{
    msleep (ms);
}

//----------------------------------------------------------------------------------------
//
//  Interface Semaphore Control - used TC3X
//
//----------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_IF_SemaphoreInit
//    Description : initialize interface semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_IF_SemaphoreInit (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  sema_init(&(io_data->sem_if), 1);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_MailBox_SemaphoreInit
//    Description : initialize mailbox semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_MailBox_SemaphoreInit (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  sema_init(&(io_data->sem_mbox), 1);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC3170_IO_OP_Mailbox_SemaphoreInit
//    Description : initialize mailbox semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_OP_Mailbox_SemaphoreInit (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  sema_init(&(io_data->sem_op), 1);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_IF_SemaphoreDelete
//    Description : delete interface semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_IF_SemaphoreDelete (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_MailBox_SemaphoreDelete
//    Description : delete mailbox semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_MailBox_SemaphoreDelete (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_OP_Mailbox_SemaphoreDelete
//    Description : delete mailbox semaphore
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_OP_Mailbox_SemaphoreDelete (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_IF_LOCK
//    Description : interface lock
//    Input :
//      moduleidx : module index
//    Output :
//      don't care
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_IF_LOCK (int moduleidx)
{
#if 1
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  if(down_interruptible(&(io_data->sem_if))) {
    TCDBG("semaphore not accquired!\n");
    return 1;
  }
 #endif
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_IF_UnLOCK
//    Description : interface unlock
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_IF_UnLOCK (int moduleidx)
{
#if 1
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  up(&(io_data->sem_if));
   #endif
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_MailBox_LOCK
//    Description : mailbox lock
//    Input :
//      moduleidx : module index
//    Output :
//      don't care
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_MailBox_LOCK (int moduleidx)
{
#if 0
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  if(down_interruptible(&(io_data->sem_mbox))) {
    TCDBG("semaphore not accquired!\n");
    return 1;
  }
#endif
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_MailBox_UnLOCK
//    Description : mailbox unlock
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_MailBox_UnLOCK (int moduleidx)
{
#if 0
  up(&(tc317x_io_data[moduleidx].sem_mbox));
#endif
  return 0;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_OP_Mailbox_LOCK
//    Description : Monitoring lock
//    Input :
//      moduleidx : module index
//    Output :
//      don't care
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_OP_Mailbox_LOCK (int moduleidx)
{
#if 0
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];


  if(down_interruptible(&(io_data->sem_op))) {
    TCDBG("sleep and interrupted!\n");
    return 1;
  }
#endif
  return 0;
}


//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_OP_Mailbox_UnLOCK
//    Description : Monitoring unlock
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_OP_Mailbox_UnLOCK (int moduleidx)
{
#if 0
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  up(&(io_data->sem_op));
#endif
  return 0;
}

//----------------------------------------------------------------------------------------
//
//  Que Control
//
//----------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_QUE_INT_Create
//    Description : create interrupt message queue
//    Input :
//      moduleidx : module index
//    Output :
//      0 : fail / others : message queue pointer
//    Remark :
//--------------------------------------------------------------------------
tc317x_queue_t* TC317X_IO_QUE_INT_Create (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  return &io_data->wait_queue;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_QUE_INT_Delete
//    Description : delete interrupt message queue
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_QUE_INT_Delete (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_QUE_Reset
//    Description : reset queue
//    Input :
//      queue
//    Output :
//      0 : fail / others : success
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_QUE_Reset (tc317x_queue_t *queue)
{
  return 1;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_QUE_Send
//    Description : send queue
//    Input :
//      queue
//      message
//    Output :
//      0 : fail / others : success
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_QUE_Send (tc317x_queue_t *queue, void *message)
{
  return 1;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_QUE_Receive
//    Description : receice queue
//    Input :
//      queue
//      message
//      suspend
//    Output :
//      0 : fail / others : success
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_QUE_Receive (tc317x_queue_t *queue, void *message, unsigned int suspend)
{
  return 1;
}

//----------------------------------------------------------------------------------------
//
//  Task Control
//
//----------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_InterruptTask_Create
//    Description : create interrupt manage task
//    Input :
//      moduleidx : module index
//      pTC4XTask_Interrupt : function pointer
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_InterruptTask_Create (
  int moduleidx, tc317x_task_fn_t task_fn)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];
#if defined(TC317X_USING_WORKQUEUE)
  char workname[32];
  sprintf(workname, "tc317x_workqueue%d",moduleidx);
  io_data->wq = create_workqueue(workname);
  if(io_data->wq){
    INIT_WORK(&(io_data->task), task_fn);
    io_data->task_running = TRUE;
  }
#elif defined(TC317X_USING_KTHREAD)
  io_data->task_running = TRUE;
  init_waitqueue_head(&io_data->wait_queue);
  io_data->kt_id = kernel_thread(task_fn, io_data, 0);
#endif
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_InterruptTask_Delete
//    Description : delete interrupt manage task
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_InterruptTask_Delete (int moduleidx)
{
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

  if(io_data->task_running){
    io_data->task_running = FALSE;
    if(io_data->irq_enabled){
      io_data->irq_enabled = FALSE;
#ifndef FEATURE_APPLY_EF40K
      disable_irq(IRQ_TC317X);
#endif
    }
#if defined(TC317X_USING_WORKQUEUE)
    //flush_scheduled_work();
    destroy_workqueue(io_data->wq);
    io_data->wq = NULL;
#endif
  }
}


//----------------------------------------------------------------------------------------
//
//  HISR Control
//
//----------------------------------------------------------------------------------------

tc317x_irqreturn_t  TC317x_IO_ISR_CSPI_Handler(int irq, void *param)
{
  int ret;
  struct tc317x_io_data_t *io_data = (struct tc317x_io_data_t*)param;
#if defined(TC317X_USING_WORKQUEUE)
  if(io_data->wq){
    disable_irq_nosync(irq);
    io_data->irq_enabled = FALSE;

    ret = queue_work(io_data->wq, &(io_data->task));
  }
#elif defined(TC317X_USING_KTHREAD)
  io_data->irq_enabled = FALSE;
  disable_irq_nosync(irq);
  wake_up(&io_data->wait_queue);

#endif
  return IRQ_HANDLED;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_SPI_Create
//    Description : create spi hisr interrupt handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_SPI_Create (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_SPI_Delete
//    Description : delete spi hisr interrupt handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_SPI_Delete (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_BB_INT_Create
//    Description : create baseband hisr interrupt handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_BB_INT_Create (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_BB_INT_Delete
//    Description : delete baseband hisr interrupt handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_BB_INT_Delete (int moduleidx)
{
}

//---------------------------------------------------------------------------
//
//  BB Interrupt Setting (TCC79X use external interrupt 2)
//
//----------------------------------------------------------------------------

// TC3X case : if stream io use gpsb interrupt(SPI,STS,..), it use edge mode.
//                other case use level mode.

// Baseband Interrupt Host Control
//void TC317X_IO_BB_INT_Ctrl(int command, int EdgeMode, int ActiveHigh)
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_BB_INT_Ctrl
//    Description : baseband interrupt control function
//    Input :
//      moduleidx : module index
//      command : command
//      trigmode : trigger mode
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_BB_INT_Ctrl (int moduleidx, int command, int TrigMode)
{
  int ret;
  struct tc317x_io_data_t *io_data = &tc317x_io_data[moduleidx];

	switch (command) {
  case CMD_BB_INT_INIT:
    TCDBG("CMD_BB_INT_INIT\n");
#ifdef TC_AP
    BITCSET(RGPIO->EINTSEL0, Hw32-Hw24, 11<<24); //GPIOA[11]
    BITCLR(RPIC->POL0, 1<<IRQ_TC317X);
#endif

#ifdef FEATURE_APPLY_EF40K
    ret = 0;
    io_data->irq_enabled = FALSE;
#else
    ret = request_irq(IRQ_TC317X, TC317x_IO_ISR_CSPI_Handler,
        IRQF_TRIGGER_RISING | IRQF_TRIGGER_HIGH, "tc317x_stream", io_data);
    io_data->irq_enabled = FALSE;
    if(ret != 0)
      TCDBG("Could not allocate tc317x stream irq :%d\n", ret);
    else
      disable_irq(IRQ_TC317X);
#endif
    break;

  case CMD_BB_INT_ENABLE:
    if(!io_data->irq_enabled){
      io_data->irq_enabled = TRUE;
#ifndef FEATURE_APPLY_EF40K
      enable_irq(IRQ_TC317X);
#endif
    } else {
      //TCDBG("aready CMD_BB_INT_ENABLE?\n");
    }
    break;

  case CMD_BB_INT_DISABLE:
    if(io_data->irq_enabled){
      //TCDBG("CMD_BB_INT_DISABLE\n");
      io_data->irq_enabled = FALSE;
#ifndef FEATURE_APPLY_EF40K
      disable_irq(IRQ_TC317X);
#endif
    }
    break;

  case CMD_BB_INT_PAUSE:
    break;

  case CMD_BB_INT_RESUME:
    break;

  default:
    PRINTF_LV_0 ("# [E] TC3X_IO_BB_INT_Ctrl - Invalid Command\n");
    break;
	}
}

//-------------------------------------------------------------------------
//
//  SDIO Interrupt Control (Active Low, Leveled)
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_SDIO_INT_Ctrl
//    Description : sdio interrupt control function
//    Input :
//      moduleidx : module index
//      command : command
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_SDIO_INT_Ctrl (int moduleidx, int command)
{
}


//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_Stream_SPI_Ctrl
//    Description : spi control function
//    Input :
//      moduleidx : module index
//      command : command
//      pStreamBuff : stream buffer pointer
//      PktSize : packet size
//      PktNum : packet number
//      StreamIO : stream io
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_Stream_SPI_Ctrl (
  int moduleidx,
  int command,
  unsigned char **pStreamBuff,
  int PktSize,
  int PktNum,
  int StreamIO)
{
}

//----------------------------------------------------------------------------------------
//
//  Host Interrupt Handler - SPI
//
//----------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_ACTIVE_HISR_SPI
//    Description : activate spi interrupt hisr handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_ACTIVE_HISR_SPI (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_LISR_SPI_Handler0
//    Description : baseband #0 SPI lisr handler
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_LISR_SPI_Handler0 (void)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_LISR_SPI_Handler1
//    Description : baseband #1 SPI lisr handler
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_LISR_SPI_Handler1 (void)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_SPI_Handler0
//    Description : baseband #0 SPI hisr handler
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_SPI_Handler0 (void)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_SPI_Handler1
//    Description : baseband #1 SPI hisr handler
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_SPI_Handler1 (void)
{
}

//----------------------------------------------------------------------------------------
//
//  Host Interrupt Handler - BB Interrupt
//
//----------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_ACTIVE_HISR_BB_INT
//    Description : activate baseband interrupt hisr handler
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_ACTIVE_HISR_BB_INT (int moduleidx)
{
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_LISR_BB_INT_Handler0
//    Description : baseband #0 interrupt handler lisr
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_LISR_BB_INT_Handler0 (void)
{
	// call BB Interrupt LISR Handler Processing routine.
	fnLISR_BBINT0 (0);  // call TC3X_LISR_BBINT
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_LISR_BB_INT_Handler1
//    Description : baseband #1 interrupt handler lisr
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_LISR_BB_INT_Handler1 (void)
{
	// call BB Interrupt LISR Handler Processing routine.
	fnLISR_BBINT1 (1);  // call TC3X_LISR_BBINT
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_BB_INT_Handler0
//    Description : baseband #0 interrupt handler hisr
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_BB_INT_Handler0 (void)
{
	// call BB Interrupt HISR Handler Processing routine.
	fnHISR_BBINT0 (0);  // call TC3X_HISR_BBINT
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_HISR_BB_INT_Handler1
//    Description : baseband #1 interrupt handler hisr
//    Input :
//
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_HISR_BB_INT_Handler1 (void)
{
	// call BB Interrupt HISR Handler Processing routine.
	fnHISR_BBINT1 (1);  // call TC3X_HISR_BBINT
}

//--------------------------------------------------------------------
// Memory Control
//--------------------------------------------------------------------

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_malloc
//    Description : memory allocation
//    Input :
//      size : size
//    Output :
//      memory pointer
//    Remark :
//--------------------------------------------------------------------------
void     *TC317X_IO_malloc (unsigned int size)
{
    return kmalloc(size, GFP_KERNEL);
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_memset
//    Description : memory set
//    Input :
//      p : pointer
//      init : init value
//      size : size
//    Output :
//      memory pointer
//    Remark :
//--------------------------------------------------------------------------
void     *TC317X_IO_memset (void *p, int init, int size)
{
	return (memset (p, init, size));
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_memcpy
//    Description : memory copy
//    Input :
//      des : destination
//      src : source
//      size : size
//    Output :
//      memory pointer
//    Remark :
//--------------------------------------------------------------------------
void     *TC317X_IO_memcpy (void *des, void *src, int size)
{
	return (memcpy (des, src, size));
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_free
//    Description : memory free
//    Input :
//      p : memory pointer
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
int TC317X_IO_free (void *p)
{
    kfree (p);
    return 0;
}
