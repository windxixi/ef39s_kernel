/* linux/include/mach/rpc_nv.h
 *
 * Copyright (c) 2008-2009, PANTECH, Co. Ltd. All rights reserved.
 *
 * All source code in this file is licensed under the following license except
 * where indicated.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can find it at http://www.fsf.org
 */

// NV RPC is added by jwcha091103
// Basically, this RPC is not included in QCT release.

#ifndef __ASM_ARCH_MSM_RPC_NV_H
#define __ASM_ARCH_MSM_RPC_NV_H

#include <mach/msm_rpcrouter.h>


typedef enum {
  NV_READ_F,          /* Read item */
  NV_WRITE_F,         /* Write item */
  NV_PEEK_F,          /* Peek at a location */
  NV_POKE_F,          /* Poke into a location */
  NV_FREE_F,          /* Free an nv item's memory allocation */
  NV_CHKPNT_DIS_F,    /* Disable cache checkpointing for glitch recovery */
  NV_CHKPNT_ENA_F,    /* Enable cache checkpointing for glitch recovery */
  NV_OTASP_COMMIT_F,  /* Commit (write) OTASP parameters to nv */
  NV_REPLACE_F,       /* Replace (overwrite) a dynamic pool item */
  NV_INCREMENT_F,     /* Increment the rental timer item */
  NV_RTRE_OP_CONFIG_F,/* Set the operational configuration of RTRE */
  NV_FUNC_ENUM_PAD = 0x7FFF     /* Pad to 16 bits on ARM */
} nv_func_enum_type;

typedef enum {
  NV_DONE_S,
  NV_BUSY_S,
  NV_BADCMD_S,
  NV_FULL_S,
  NV_FAIL_S,
  NV_NOTACTIVE_S,
  NV_BADPARM_S,
  NV_READONLY_S,
  NV_BADTG_S,
  NV_NOMEM_S,
  NV_NOTALLOC_S,
  NV_STAT_ENUM_PAD = 32767
} nv_stat_enum_type;

typedef enum {
	// add here 
	
	NV_SPARE_1_I = 107,
	NV_SPARE_2_I = 108,
	NV_WLAN_MAC_ADDRESS_I = 4678
	// add here 
} nv_items_enum_type;

// generic header
typedef struct {
    struct rpc_request_hdr hdr;
    uint32_t	cmd;
    uint32_t	item;
		uint32_t	more_data;
		uint32_t	disc;	
} nv_cmd_req_header;

typedef struct {
    struct rpc_reply_hdr hdr;
    uint32_t result;
		uint32_t	more_data;
		uint32_t	disc;	
} nv_cmd_rep_header;

// generic data
typedef struct {
	nv_cmd_req_header common;
    uint8_t data[1024];
} nv_cmd_generic_req;

typedef struct {
	nv_cmd_rep_header common;
    uint8_t data[1024];
} nv_cmd_generic_rep;

// generic command
typedef struct {
 	nv_cmd_generic_req req;
 	nv_cmd_generic_rep rep;
} nv_cmd_generic_cmd;

// NV_SPARE_1_I (107)
typedef struct {
    uint32_t spare_1;
} nv_cmd_spare_1_req;

typedef struct {
    uint32_t spare_1;
} nv_cmd_spare_1_rep;

// NV_SPARE_2_I (108)
typedef struct {
    uint32_t spare_2;
} nv_cmd_spare_2_req;

typedef struct {
    uint32_t spare_2;
} nv_cmd_spare_2_rep;

// NV_WLAN_MAC_ADDRESS_I (4678)
typedef struct {
    uint32_t wlan_mac_address[2];
} nv_cmd_wlan_mac_address_req;

typedef struct {
    uint32_t wlan_mac_address[2];
} nv_cmd_wlan_mac_address_rep;

// add here

typedef  union {
	// add here

	uint32_t	spare_1;
	uint32_t	spare_2;
	uint32_t	wlan_mac_address[6];
	// add here
} nv_cmd_item_type;


extern int msm_nv_rpc_connect(void);
extern int msm_nv_read(uint32_t item, nv_cmd_item_type *data);
extern int msm_nv_write(uint32_t item, nv_cmd_item_type *data);
extern int msm_nv_cmd_remote(	uint32_t cmd, 
						uint32_t item, 	
						nv_cmd_item_type *data_ptr);
extern int msm_nv_rpc_close(void);

#endif
