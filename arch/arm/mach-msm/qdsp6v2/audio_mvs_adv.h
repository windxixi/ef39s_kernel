#ifndef SKY_SND_ADUIO_MVS_INTERFACE_H
#define SKY_SND_ADUIO_MVS_INTERFACE_H
/************************************************************************************************
**
**    MVS ADV INTERFACE
**
**    FILE
**        mvs_ADV.h
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

/************************************************************************************************
** Message Definition
*************************************************************************************************/
//#define MVS_ADV_MSG_LEVEL_LOW
#define MVS_ADV_MSG_LEVEL_MED
#define MVS_ADV_MSG_LEVEL_HIGH

#ifdef MVS_ADV_MSG_LEVEL_LOW
#define MVS_ADV_DBG_LOW(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_ADV_DBG_LOW(fmt, arg...) do {} while (0)
#endif

#ifdef MVS_ADV_MSG_LEVEL_MED
#define MVS_ADV_DBG_MED(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_ADV_DBG_MED(fmt, arg...) do {} while (0)
#endif

#ifdef MVS_ADV_MSG_LEVEL_HIGH
#define MVS_ADV_DBG_HIGH(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define MVS_ADV_DBG_HIGH(fmt, arg...) do {} while (0)
#endif

#define MVS_ADV_ERR(fmt, arg...) printk(KERN_ERR "%s: " fmt "\n", __func__, ## arg)


////////////////////////////////////////////////////////////////////////////////////
//jhpark-MVS #define MVS_ADV_SUPPORT // USE MVS interface
//jhpark-MVS #define VOLUMEKEY_NOISE_FIX // Eliminate volumekey noise
////////////////////////////////////////////////////////////////////////////////////
/*---------------------- SIZE DEFINE ------------------------*/
#define VOC_PCM_PACKET_SIZE 320
#define MAX_PCM_PKT_SIZE 160//320
#define MAX_VOIP_PCM_SIZE   160
#define MAX_MVS_PKT_NUM 10
#define MAX_MVS_START_PKT_NUM 0
#define MAX_AMR_PKT_SIZE 31
#define MAX_IF1_PACKET_SIZE  32
#define MAX_IF2_PACKET_SIZE  31
/*---------------------- TYPE DEFINE ------------------------*/
typedef enum 
{
  MVS_CLOSE,
  MVS_OPEN,
  MVS_STOP,
  MVS_START,
  MVS_MAX
}mvs_adv_state;

typedef struct
{
    unsigned char buf[MAX_MVS_PKT_NUM][MAX_PCM_PKT_SIZE];
    int buf_sz[MAX_MVS_PKT_NUM];
    int read_pos;
    int write_pos;
    mvs_adv_state state;
    int fill_buf_status;
}mvs_adv_buf_ptr_info_t;
typedef struct
{
    mvs_adv_buf_ptr_info_t rx;
    mvs_adv_buf_ptr_info_t tx;
}mvs_adv_buf_info_t;

extern void mvs_adv_buf_init(void);
extern void mvs_adv_set_state(mvs_adv_state state);
extern mvs_adv_state mvs_adv_get_state(void);

extern void audio_adv_mvs_process_pcm_uplink(uint8_t *voc_pkt,
				     uint32_t pkt_len,
				     void *private_data);
extern void audio_adv_mvs_process_pcm_downlink(uint8_t *voc_pkt,
				     uint32_t *pkt_len,
				     void *private_data);
extern void audio_adv_mvs_process_amr_uplink(uint8_t *voc_pkt,
				     uint32_t pkt_len,
				     void *private_data);
extern void audio_adv_mvs_process_amr_downlink(uint8_t *voc_pkt,
				     uint32_t *pkt_len,
				     void *private_data);				     
#ifdef VOLUMEKEY_NOISE_FIX
extern void enter_noise_critical_section(char OnOff);
#endif
#endif
