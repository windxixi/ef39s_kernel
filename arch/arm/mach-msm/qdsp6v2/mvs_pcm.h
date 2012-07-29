#ifndef SKY_SND_MVS_PCM_INTERFACE_H
#define SKY_SND_MVS_PCM_INTERFACE_H
/************************************************************************************************
**
**    MVS PCM INTERFACE
**
**    FILE
**        mvs_pcm.h
**
**    DESCRIPTION
**        This file contains audio external amp api
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/
/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>
#include "sky_snd_audio.h"
/************************************************************************************************
** Message Definition
*************************************************************************************************/
//#define MVS_PCM_MSG_LEVEL_LOW
//jhpark#define MVS_PCM_MSG_LEVEL_MED
//jhpark#define MVS_PCM_MSG_LEVEL_HIGH

#ifdef MVS_PCM_MSG_LEVEL_LOW
#define MVS_PCM_DBG_LOW(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_PCM_DBG_LOW(fmt, arg...) do {} while (0)
#endif

#ifdef MVS_PCM_MSG_LEVEL_MED
#define MVS_PCM_DBG_MED(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_PCM_DBG_MED(fmt, arg...) do {} while (0)
#endif

#ifdef MVS_PCM_MSG_LEVEL_HIGH
#define MVS_PCM_DBG_HIGH(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_PCM_DBG_HIGH(fmt, arg...) do {} while (0)
#endif

#define MVS_PCM_ERR(fmt, arg...) printk(KERN_ERR "%s: " fmt "\n", __func__, ## arg)

extern void mvs_pcm_packet_is_ready(int avail_size);
#endif