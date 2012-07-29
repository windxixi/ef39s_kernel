#ifndef SKY_SND_AMR_TRANS_H
#define SKY_SND_AMR_TRANS_H
/************************************************************************************************
**
**    AMR TRANSCODE
**
**    FILE
**        amr_trans.h
**
**    DESCRIPTION
**        This file contains AMR transcoder
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/
/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>
#include <linux/msm_audio_mvs.h>

typedef  unsigned char     uint8;
typedef  unsigned short     uint16;
typedef  unsigned long int     uint32;
typedef  unsigned char     boolean;

uint16 qvp_if1_to_if2
(
  const void  *if1,
  uint16      octets_in,
  void        *if2,
  uint16      max_octets_out,
  uint16      *octet_count,
  uint32      *packet_count
);

uint16 qvp_if2_to_if1
(
  const void  *if2,
  uint16      octets_in,
  void        *if1,
  uint16      max_octets_out,
  uint16      *octet_count,
  uint32      *packet_count
);
#endif
