//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_device_Interrupt_Task.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_DEVICE_INTERRUPT_TASK_H__
#define __TC317X_DEVICE_INTERRUPT_TASK_H__

#include "TC317X_Core.h"

#ifndef YES
#define YES                     1
#endif

#ifndef NO
#define NO                      0
#endif

typedef struct
{
  uint16_t usCommand;
} TC3X_InterruptCOMMAND_t;

void      TC317XTask_Interrupt_Create (int32_t moduleidx);
void      TC317XTask_Interrupt_Delete (int32_t moduleidx);
tc317x_taskreturn_t TC317XTask_Interrupt (ptr_tc317x_taskparam_t);

#endif
