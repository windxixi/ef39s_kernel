/* linux/arch/arm/mach-msm/rpc_nv.c
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

#include <linux/err.h>
#include <mach/rpc_nv.h>
#include <asm/mach-types.h>
#if 1 //lee.eunsuk 20110212 ; for Gingerbread;  error: implicit declaration of function 'kmalloc'
#include <linux/slab.h>
#endif 

// NV RPC is added by jwcha091103
// Basically, this RPC is not included.

#define	RPC_NV_DEBUG	0	// set 1 for debugging

static struct msm_rpc_endpoint *nv_ep;

#define	MSM_RPC_NV_PROG	0x3000000e
#define NV_VERS_9_2		0x00090002
#define NV_VERS_6_1   	0x00060001
#define	NV_VERS_2_1		0x00020001
#define	NV_VERS_1_1		0x00010001

struct msm_nv_rpc_ids {
	unsigned long	prog;
	unsigned long	vers_comp;
	unsigned long	cmd_remote;
};

static struct msm_nv_rpc_ids nv_rpc_ids;

static int msm_nv_init_rpc_ids(unsigned int vers)
{
	if (vers == NV_VERS_6_1) {
		nv_rpc_ids.prog			= 0x3000000e;
		nv_rpc_ids.vers_comp	= NV_VERS_6_1;
		nv_rpc_ids.cmd_remote	= 9;
		return 0;
	}
	else if (vers == NV_VERS_2_1) {
		nv_rpc_ids.prog			= 0x3000000e;
		nv_rpc_ids.vers_comp	= NV_VERS_2_1;
		nv_rpc_ids.cmd_remote	= 9;
		return 0;
	}
	else if (vers == NV_VERS_1_1) {
		nv_rpc_ids.prog			= 0x3000000e;
		nv_rpc_ids.vers_comp	= NV_VERS_1_1;
		nv_rpc_ids.cmd_remote	= 9;
		return 0;
	}
	else if (vers == NV_VERS_9_2) {
		nv_rpc_ids.prog			= 0x3000000e;
		nv_rpc_ids.vers_comp	= NV_VERS_9_2;
		nv_rpc_ids.cmd_remote	= 9;
		return 0;
	}
	else {
		printk(KERN_INFO "%s: no matches found for version (0x%x)\n",
			__func__, vers);
		return -ENODATA;
	}
}

/* rpc connect for nv */
int msm_nv_rpc_connect(void)
{

	if (nv_ep && !IS_ERR(nv_ep)) {
		printk(KERN_INFO "%s: nv_ep already connected\n", __func__);
		return 0;
	}

	/* Initialize rpc ids */
	if (msm_nv_init_rpc_ids(NV_VERS_9_2)) {
		printk(KERN_ERR "%s: rpc ids(0x%x) initialization failed\n",
			__func__, NV_VERS_9_2);
		return -ENODATA;
	}

	nv_ep = msm_rpc_connect_compatible(nv_rpc_ids.prog,
					nv_rpc_ids.vers_comp,
					MSM_RPC_UNINTERRUPTIBLE);

	if (IS_ERR(nv_ep)) {
		printk(KERN_ERR "%s: connect compatible failed vers = %lx\n",
			 __func__, nv_rpc_ids.vers_comp);

		/* Initialize rpc ids */
		if (msm_nv_init_rpc_ids(NV_VERS_1_1)) {
			printk(KERN_ERR "%s: rpc ids(0x%x) initialization failed\n",
				__func__, NV_VERS_1_1);
			return -ENODATA;
		}
		nv_ep = msm_rpc_connect_compatible(nv_rpc_ids.prog,
					nv_rpc_ids.vers_comp,
					MSM_RPC_UNINTERRUPTIBLE);
	}

	if (IS_ERR(nv_ep)) {
		printk(KERN_ERR "%s: connect compatible failed vers = %lx\n",
				__func__, nv_rpc_ids.vers_comp);
		return -EAGAIN;
	} else
		printk(KERN_INFO "%s: rpc connect success vers = %lx\n",
				__func__, nv_rpc_ids.vers_comp);

	return 0;
}
EXPORT_SYMBOL(msm_nv_rpc_connect);

static int msm_nv_check_validity(uint32_t item)
{
	int rc = -EINVAL;

	switch(item) {
		case	NV_SPARE_1_I:
		case	NV_SPARE_2_I:
       case	NV_WLAN_MAC_ADDRESS_I:
				rc = 0;
				break;
	}

	return rc;
}

static void msm_nv_set_req_data(int item, nv_cmd_generic_cmd *cmd, 
						nv_cmd_item_type *data, 
						int *reqsz, int *repsz)
{
	nv_cmd_item_type *ptr = (nv_cmd_item_type *)cmd->req.data; 
	int	req_hdr_sz = sizeof(nv_cmd_req_header);
	int	rep_hdr_sz = sizeof(nv_cmd_rep_header);

	switch (item) {
		case	NV_SPARE_1_I:
				ptr->spare_1 = cpu_to_be32(data->spare_1);	
				*reqsz = req_hdr_sz + sizeof(nv_cmd_spare_1_req);
				*repsz = rep_hdr_sz + sizeof(nv_cmd_spare_1_rep);
				break;
		case	NV_SPARE_2_I:	
				ptr->spare_2 = cpu_to_be32(data->spare_2);	
				*reqsz = req_hdr_sz + sizeof(nv_cmd_spare_2_req);
				*repsz = rep_hdr_sz + sizeof(nv_cmd_spare_2_rep);
				break;
		case	NV_WLAN_MAC_ADDRESS_I:	
				ptr->wlan_mac_address[0] = cpu_to_be32(data->wlan_mac_address[0]);	
				ptr->wlan_mac_address[1] = cpu_to_be32(data->wlan_mac_address[1]);	
//				ptr->wlan_mac_address[2] = cpu_to_be32(data->wlan_mac_address[2]);	
//				ptr->wlan_mac_address[3] = cpu_to_be32(data->wlan_mac_address[3]);	
//				ptr->wlan_mac_address[4] = cpu_to_be32(data->wlan_mac_address[4]);	
//				ptr->wlan_mac_address[5] = cpu_to_be32(data->wlan_mac_address[5]);	
				*reqsz = req_hdr_sz + sizeof(nv_cmd_wlan_mac_address_req);
				*repsz = rep_hdr_sz + sizeof(nv_cmd_wlan_mac_address_rep);
				break;


		// add here
	}
}

static void msm_nv_set_rep_data(int item, nv_cmd_generic_cmd *cmd, 
								nv_cmd_item_type *data)
{
	nv_cmd_item_type *ptr = (nv_cmd_item_type *)cmd->rep.data; 

	switch (item) {
		case	NV_SPARE_1_I:
				data->spare_1 = be32_to_cpu(ptr->spare_1);
				break;
		case	NV_SPARE_2_I:	
				data->spare_2 = be32_to_cpu(ptr->spare_2);
				break;
		case	NV_WLAN_MAC_ADDRESS_I:	
				data->wlan_mac_address[0] = be32_to_cpu(ptr->wlan_mac_address[0]);	
				data->wlan_mac_address[1] = be32_to_cpu(ptr->wlan_mac_address[1]);	
//				data->wlan_mac_address[2] = be32_to_cpu(ptr->wlan_mac_address[2]);	
//				data->wlan_mac_address[3] = be32_to_cpu(ptr->wlan_mac_address[3]);	
//				data->wlan_mac_address[4] = be32_to_cpu(ptr->wlan_mac_address[4]);	
//				data->wlan_mac_address[5] = be32_to_cpu(ptr->wlan_mac_address[5]);	
				break;

		// add here
	}
}

int msm_nv_cmd_remote (	uint32_t cmd, 
						uint32_t item,
						nv_cmd_item_type *data_ptr)
{
	int rc = 0;
	int reqsz = 0, repsz = 0;
	int	more_data;
	nv_cmd_generic_cmd *cmdptr;

	// validity check
	if (msm_nv_check_validity(item) < 0) {
		printk(KERN_ERR "%s : cmd(%d) is NOT supported!\n",
				__func__, item);
		return	-EINVAL;
	}

	if (!nv_ep || IS_ERR(nv_ep)) {
		printk(KERN_ERR "%s: rpc connect failed: rc = %ld\n",
			__func__, PTR_ERR(nv_ep));
		return -EAGAIN;
	}

	cmdptr = kmalloc(sizeof(nv_cmd_generic_cmd), GFP_KERNEL);
	if (!cmdptr) {
		printk(KERN_ERR "%s: malloc failed\n", __func__);
		return -EBUSY;
	}

	cmdptr->req.common.cmd 	= cpu_to_be32(cmd);
	cmdptr->req.common.item	= cpu_to_be32(item);
	more_data = (data_ptr != NULL);
	cmdptr->req.common.more_data	= cpu_to_be32(more_data);
	cmdptr->req.common.disc	= cpu_to_be32(item);

	msm_nv_set_req_data(item, cmdptr, data_ptr, &reqsz, &repsz);
#if	RPC_NV_DEBUG
	printk(KERN_INFO "%s: reqhdr(%d), rephdr(%d)\n", 
					__func__, sizeof(struct rpc_request_hdr), 
					sizeof(struct rpc_reply_hdr));
	printk(KERN_INFO "%s: reqsz(%d), repsz(%d)\n", __func__, reqsz, repsz);
#endif

	rc = msm_rpc_call_reply(nv_ep, nv_rpc_ids.cmd_remote,
				&cmdptr->req, reqsz,
				&cmdptr->rep, repsz,
				5 * HZ);

	if (rc < 0) {
		kfree(cmdptr);
		printk(KERN_ERR "%s: rpc call failed! error: %d\n",
			__func__, rc);
		return	rc;
	}
#if	RPC_NV_DEBUG
	else {
		printk(KERN_INFO "%s: rpc call success\n" ,
			__func__);
	}
#endif

	rc = be32_to_cpu(cmdptr->rep.common.result);
	if (rc == NV_DONE_S) {
		msm_nv_set_rep_data(item, cmdptr, data_ptr);
	}

	kfree(cmdptr);

	return rc;
}
EXPORT_SYMBOL(msm_nv_cmd_remote);

int msm_nv_read (uint32_t item, nv_cmd_item_type *data_ptr)
{
	return msm_nv_cmd_remote (NV_READ_F, item, data_ptr);
}
EXPORT_SYMBOL(msm_nv_read);

int msm_nv_write (uint32_t item, nv_cmd_item_type *data_ptr)
{
	return msm_nv_cmd_remote (NV_WRITE_F, item, data_ptr);
}
EXPORT_SYMBOL(msm_nv_write);

/* rpc call to close connection */
int msm_nv_rpc_close(void)
{
	int rc = 0;

	if (IS_ERR(nv_ep)) {
		printk(KERN_ERR "%s: rpc_close failed before call, rc = %ld\n",
			__func__, PTR_ERR(nv_ep));
		return -EAGAIN;
	}

	rc = msm_rpc_close(nv_ep);
	nv_ep = NULL;

	if (rc < 0) {
		printk(KERN_ERR "%s: close rpc failed! rc = %d\n",
			__func__, rc);
		return -EAGAIN;
	} else
		printk(KERN_INFO "rpc close success\n");

	return rc;
}
EXPORT_SYMBOL(msm_nv_rpc_close);

