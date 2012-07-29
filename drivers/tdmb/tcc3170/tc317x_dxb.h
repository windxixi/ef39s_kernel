/* 
 * linux/drivers/char/tc317X/tc317x_dxb.h
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TCC317x_DXB_H_
#define _TCC317x_DXB_H_

#include <linux/types.h>
#include "TC317X_API.h"
#include "TC317X_Sub_API.h"

#define TC317X_DEV_FILE		"/dev/tc317x_dxb"
#define TC317X_DEV_NAME		"tc317x_dxb"
#define TC317X_DEV_MAJOR		243
#define TC317X_DEV_MINOR		0

//#define IOCTL_TC317X_OFF     _IO(TC317X_DEV_MAJOR, 1)
//#define IOCTL_TC317X_ON      _IO(TC317X_DEV_MAJOR, 2)
#define IOCTL_TC317X_RESET      _IO(TC317X_DEV_MAJOR, 3)

#define IOCTL_TC317X_SET_FREQ      _IO(TC317X_DEV_MAJOR, 5)
#define IOCTL_TC317X_SELECT_SUBCH  _IO(TC317X_DEV_MAJOR, 6)
#define IOCTL_TC317X_RELEASE_SUBCH _IO(TC317X_DEV_MAJOR, 7)

#define IOCTL_TC317X_TEST          _IO(TC317X_DEV_MAJOR, 10)
#define IOCTL_TC317X_GET_MON_STAT  _IO(TC317X_DEV_MAJOR, 11)
#define IOCTL_TC317X_SET_INTR_THRESHOLD_SIZE _IO(TC317X_DEV_MAJOR, 12)
#define IOCTL_TC317X_RW_REG        _IO(TC317X_DEV_MAJOR, 13)

struct tc317x_dxb_ioctl_param_t {
  int32_t cmd;
  uint32_t  addr;
  int32_t           size;
  uint8_t reg_data;
};


struct tc317x_signal_quality {
  unsigned int SNR;
  unsigned int PCBER;
  unsigned int RSBER;
  unsigned int RSSI;
};

#endif
