/*
 * u_sdio.c - utilities for USB gadget serial over sdio
 *
 * This code also borrows from drivers/usb/gadget/u_serial.c, which is
 * Copyright (C) 2003 Al Borchers (alborchers@steinerpoint.com)
 * Copyright (C) 2008 David Brownell
 * Copyright (C) 2008 by Nokia Corporation
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program from the Code Aurora Forum is free software; you can
 * redistribute it and/or modify it under the GNU General Public License
 * version 2 and only version 2 as published by the Free Software Foundation.
 * The original work available from [kernel.org] is subject to the notice below.
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/termios.h>
#include <linux/debugfs.h>
#include <mach/usb_gadget_fserial.h>

#include <mach/sdio_al.h>
#include <mach/sdio_cmux.h>
#include "u_serial.h"

//20110721 choiseulkee add, reboot for PDL IDLE download
#ifdef CONFIG_PANTECH
#include <linux/reboot.h>
//20110826 choiseulkee add for PDL IDLE download of PRESTO etc.... for AT&T
#ifdef CONFIG_PANTECH_PRESTO_BOARD
#include <linux/fs.h>
#include <linux/file.h>
#include <asm/uaccess.h>

#define PDL_IMEI_SIZE 15
#define IMEI_ADDR_MAGIC_NUM 0x88776655

// read rawdata for PDL IMEI
#define SECTOR_SIZE               512
#define SKY_RAWDATA_MAX           (8192*SECTOR_SIZE) // 4MB
#define SECTOR_SIZE_DEFAULT       1

//----------------------------------------------------------------------------------

#define DLOAD_SECTOR_START        0
#define DLOAD_INFO_OFFSET         DLOAD_SECTOR_START
#define PARTITION_INFO_OFFSET     (DLOAD_SECTOR_START+SECTOR_SIZE)
#define BACKUP_DLOAD_INFO_OFFSET  (DLOAD_SECTOR_START+(SECTOR_SIZE*2))
#define DLOAD_STATUS_OFFSET       (DLOAD_SECTOR_START+(SECTOR_SIZE*3))
#define DLOAD_HISTORY_OFFSET      (DLOAD_SECTOR_START+(SECTOR_SIZE*4))
#define DLOAD_SECTOR_MAX          (SECTOR_SIZE*8)

//----------------------------------------------------------------------------------

#define BACKUP_SECTOR_START		(DLOAD_SECTOR_START+DLOAD_SECTOR_MAX)

#define SECUREESN_START			BACKUP_SECTOR_START
#define SECUREESN_LENGTH		((SECTOR_SIZE*25) * 2) // use sectors as pages // "*2" means 2 blocks

#define RFCAL_BACKUP_START		(SECUREESN_START+SECUREESN_LENGTH)
#define RFCAL_BACKUP_LENGTH		(SECTOR_SIZE*100)

#define FACTORY_EFS_INIT_START		(RFCAL_BACKUP_START+RFCAL_BACKUP_LENGTH)
#define FACTORY_EFS_INIT_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define MSEC_BACKUP_START		(FACTORY_EFS_INIT_START+FACTORY_EFS_INIT_LENGTH)
#define MSEC_BACKUP_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define FUNCTEST_RESULT_INIT_START	(MSEC_BACKUP_START+MSEC_BACKUP_LENGTH)
#define FUNCTEST_RESULT_INIT_LENGTH	(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define WIFI_DEVICE_INFO_START		(FUNCTEST_RESULT_INIT_START+FUNCTEST_RESULT_INIT_LENGTH)
#define WIFI_DEVICE_INFO_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define BT_DEVICE_INFO_START		(WIFI_DEVICE_INFO_START+WIFI_DEVICE_INFO_LENGTH)
#define BT_DEVICE_INFO_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define PWR_ON_CNT_START		(BT_DEVICE_INFO_START+BT_DEVICE_INFO_LENGTH)
#define PWR_ON_CNT_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define SDCARD_UPDATE_START		(PWR_ON_CNT_START+PWR_ON_CNT_LENGTH)
#define SDCARD_UPDATE_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define USB_CHARGING_START		(SDCARD_UPDATE_START+SDCARD_UPDATE_LENGTH)
#define USB_CHARGING_LENGTH		(SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

#define PERMANENTMEMORY_START		(USB_CHARGING_START+USB_CHARGING_LENGTH)
#define PERMANENTMEMORY_LENGTH		(SECTOR_SIZE*2)

//F_PANTECH_MEID_IMEI_ADDR_BACKUP
#define NON_SECURE_IMEI_START       (PERMANENTMEMORY_START+PERMANENTMEMORY_LENGTH)
#define NON_SECURE_IMEI_LENGTH      (SECTOR_SIZE*SECTOR_SIZE_DEFAULT)

enum {
  DLOADINFO_NONE_STATE = 0,
  DLOADINFO_AT_RESPONSE_STATE,
  DLOADINFO_PHONE_INFO_STATE,
  DLOADINFO_HASH_TABLE_STATE,
  DLOADINFO_PARTI_TABLE_STATE,
  DLOADINFO_FINISH_STATE,
  DLOADINFO_MAX_STATE
};

enum {
FINISH_CMD = 0,
PHONE_INFO_CMD,
HASH_TABLE_CMD,
PARTI_TABLE_CMD,
MAX_PHONEINFO_CMD
};

typedef struct {
  unsigned int partition_size_;
  char   partition_name_[ 8 ];
}partition_info_type;
typedef struct {
  unsigned int version_;
  char   model_name_    [ 16 ];
  char   binary_version_[ 16 ];
  unsigned int fs_version_;
  unsigned int nv_version_;
  char   build_date_    [ 16 ];
  char   build_time_    [ 16 ];

  //폰의 부트로더 정보
  unsigned int boot_loader_version_;                  // 현재 메인 바이너리에 맞는 부트로더의 버전
  unsigned int boot_section_id_[4];                 // 부트로더 버전이 바뀌는 경우 반드시 새로 받아야 하는 section id

  // 현재 폰의 EFS 정보
  unsigned int              efs_size_;                // 전체 EFS 사이즈
  unsigned int              partition_num_;           // EFS를 나눈 파티션의 개수
  partition_info_type partition_info_[ 6 ];     // EFS의 파티션의 정보

  unsigned int    fusion_id_;

  char imei[ 15 ];

  char  reserved_[ 60 ];
} phoneinfo_type;
phoneinfo_type *phoneinfo_buf;
typedef struct {
  unsigned int imei_magic_num;
  char backup_imei[PDL_IMEI_SIZE];
  char emptspace[51];
} imei_backup_info_type;

void load_phoneinfo_with_imei(phoneinfo_type *phoneinfo_tmp);
unsigned fill_writereq(int *dloadinfo_state, struct usb_request *writereq);

#endif /* CONFIG_PANTECH_PRESTO_BOARD */
#endif /* CONFIG_PANTECH */

#define RX_QUEUE_SIZE		8
#define RX_BUF_SIZE		2048

#define TX_QUEUE_SIZE		8
#define TX_BUF_SIZE		2048

/* 1 - DUN, 2-NMEA/GPS */
#define N_PORTS	2
static struct portmaster {
	struct mutex lock;
	struct gsdio_port *port;
	struct platform_driver gsdio_ch;
} ports[N_PORTS];
static unsigned n_ports;

struct sdio_port_info {
	/* data channel info */
	char *data_ch_name;
	struct sdio_channel *ch;

	/* control channel info */
	int ctrl_ch_id;
};

struct sdio_port_info sport_info[N_PORTS] = {
	{
		.data_ch_name = "SDIO_DUN",
		.ctrl_ch_id = 9,
	},
	{
		.data_ch_name = "SDIO_NMEA",
		.ctrl_ch_id = 10,
	},
};

static struct workqueue_struct *gsdio_wq;

struct gsdio_port {
	unsigned			port_num;
	spinlock_t			port_lock;

	unsigned			n_read;
	struct list_head		read_pool;
	struct list_head		read_queue;
	struct work_struct		push;
	unsigned long			rp_len;
	unsigned long			rq_len;

	struct list_head		write_pool;
	struct work_struct		pull;
	unsigned long			wp_len;

	struct work_struct		notify_modem;

	struct gserial			*port_usb;
	struct usb_cdc_line_coding	line_coding;

	int				sdio_open;
	int				sdio_probe;
	int				ctrl_ch_err;
	struct sdio_port_info		*sport_info;
	struct delayed_work		sdio_open_work;

#define ACM_CTRL_RI		(1 << 3)
#define ACM_CTRL_DSR		(1 << 1)
#define ACM_CTRL_DCD		(1 << 0)
	int				cbits_to_laptop;

#define ACM_CTRL_RTS	(1 << 1)	/* unused with full duplex */
#define ACM_CTRL_DTR	(1 << 0)	/* host is ready for data r/w */
	int				cbits_to_modem;

	/* pkt logging */
	unsigned long			nbytes_tolaptop;
	unsigned long			nbytes_tomodem;
};

#if 0 // FEATURE_PANTECH_VT_SUPPORT_QMI
extern int sdio_vt_dun_disable;
#endif

void gsdio_free_req(struct usb_ep *ep, struct usb_request *req)
{
	kfree(req->buf);
	usb_ep_free_request(ep, req);
}

struct usb_request *
gsdio_alloc_req(struct usb_ep *ep, unsigned len, gfp_t flags)
{
	struct usb_request *req;

	req = usb_ep_alloc_request(ep, flags);
	if (!req) {
		pr_err("%s: usb alloc request failed\n", __func__);
		return NULL;
	}

	req->length = len;
	req->buf = kmalloc(len, flags);
	if (!req->buf) {
		pr_err("%s: request buf allocation failed\n", __func__);
		usb_ep_free_request(ep, req);
		return NULL;
	}

	return req;
}

void gsdio_free_requests(struct usb_ep *ep, struct list_head *head)
{
	struct usb_request	*req;

	while (!list_empty(head)) {
		req = list_entry(head->next, struct usb_request, list);
		list_del(&req->list);
		gsdio_free_req(ep, req);
	}
}

int gsdio_alloc_requests(struct usb_ep *ep, struct list_head *head,
		int num, int size,
		void (*cb)(struct usb_ep *ep, struct usb_request *))
{
	int i;
	struct usb_request *req;

	pr_debug("%s: ep:%p head:%p num:%d size:%d cb:%p", __func__,
			ep, head, num, size, cb);

	for (i = 0; i < num; i++) {
		req = gsdio_alloc_req(ep, size, GFP_ATOMIC);
		if (!req) {
			pr_debug("%s: req allocated:%d\n", __func__, i);
			return list_empty(head) ? -ENOMEM : 0;
		}
		req->complete = cb;
		list_add(&req->list, head);
	}

	return 0;
}

void gsdio_start_rx(struct gsdio_port *port)
{
	struct list_head	*pool;
	struct usb_ep		*out;
	int ret;

	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return;
	}

	pr_debug("%s: port:%p port#%d\n", __func__, port, port->port_num);

	spin_lock_irq(&port->port_lock);

	if (!port->port_usb) {
		pr_debug("%s: usb is disconnected\n", __func__);
		goto start_rx_end;
	}

	if (!port->sdio_open) {
		pr_debug("%s: sdio is not open\n", __func__);
		goto start_rx_end;
	}

	pool = &port->read_pool;
	out = port->port_usb->out;

	while (!list_empty(pool)) {
		struct usb_request	*req;

		req = list_entry(pool->next, struct usb_request, list);
		list_del(&req->list);
		req->length = RX_BUF_SIZE;
		port->rp_len--;

		spin_unlock_irq(&port->port_lock);
		ret = usb_ep_queue(out, req, GFP_ATOMIC);
		spin_lock_irq(&port->port_lock);
		if (ret) {
			pr_err("%s: usb ep out queue failed"
					"port:%p, port#%d\n",
					__func__, port, port->port_num);
			list_add_tail(&req->list, pool);
			port->rp_len++;
			break;
		}

		/* usb could have disconnected while we released spin lock */
		if (!port->port_usb) {
			pr_debug("%s: usb is disconnected\n", __func__);
			goto start_rx_end;
		}
	}

start_rx_end:
	spin_unlock_irq(&port->port_lock);
}

int gsdio_write(struct gsdio_port *port, struct usb_request *req)
{
	unsigned	avail;
	char		*packet = req->buf;
	unsigned	size = req->actual;
	unsigned	n;
	int		ret = 0;


	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return -ENODEV;
	}

	if (!req) {
		pr_err("%s: usb request is null port#%d\n",
				__func__, port->port_num);
		return -ENODEV;
	}

	pr_debug("%s: port:%p port#%d req:%p actual:%d n_read:%d\n",
			__func__, port, port->port_num, req,
			req->actual, port->n_read);

	if (!port->sdio_open) {
		pr_debug("%s: SDIO IO is not supported\n", __func__);
		return -ENODEV;
	}

#if 0 // FEATURE_PANTECH_VT_SUPPORT_QMI
  if(sdio_vt_dun_disable) {
    pr_err("%s: sdio_vt port is opened\n", __func__);
    return -EBUSY;
  }
#endif

	avail = sdio_write_avail(port->sport_info->ch);

	pr_debug("%s: sdio_write_avail:%d", __func__, avail);

	if (!avail)
		return -EBUSY;

	if (!req->actual) {
		pr_debug("%s: req->actual is already zero,update bytes read\n",
				__func__);
		port->n_read = 0;
		return -ENODEV;
	}

	packet = req->buf;
	n = port->n_read;
	if (n) {
		packet += n;
		size -= n;
	}

	if (size > avail)
		size = avail;

	spin_unlock_irq(&port->port_lock);
	ret = sdio_write(port->sport_info->ch, packet, size);
	spin_lock_irq(&port->port_lock);
	if (ret) {
		pr_err("%s: port#%d sdio write failed err:%d",
				__func__, port->port_num, ret);
		/* try again later */
		return ret;
	}

	port->nbytes_tomodem += size;

	if (size + n == req->actual)
		port->n_read = 0;
	else
		port->n_read += size;

	return ret;
}

//20110826 choiseulkee add for PDL IDLE download of PRESTO etc.... for AT&T
#ifdef CONFIG_PANTECH_PRESTO_BOARD
void load_phoneinfo_with_imei(phoneinfo_type *phoneinfo_tmp)
{
        struct file *rawdata_filp;
        char read_buf[512];
        mm_segment_t oldfs;
        int rc;
        imei_backup_info_type *imei_backup_info_buf;

        // open
        rawdata_filp = filp_open("/dev/block/mmcblk0p10", O_RDONLY|O_SYNC, 0);
        if( IS_ERR(rawdata_filp) )
        {
          printk(KERN_ERR "%s: filp_open error\n",__func__);
        }
        // lseek
        rawdata_filp->f_pos = 0;
        memset( read_buf, 0x0, 512 );
        // read
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        rc = rawdata_filp->f_op->read(rawdata_filp, read_buf, 512, &rawdata_filp->f_pos);
        set_fs(oldfs);
        if (rc < 0) {
          printk(KERN_ERR "%s: read phoneinfo error = %d \n",__func__,rc);
        }
        memcpy(phoneinfo_tmp, &read_buf[32], sizeof(phoneinfo_type));

        // read IMEI
        // lseek
        rawdata_filp->f_pos = NON_SECURE_IMEI_START;
        memset( read_buf, 0x0, 512 );
        // read
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        rc = rawdata_filp->f_op->read(rawdata_filp, read_buf, 16, &rawdata_filp->f_pos);
        set_fs(oldfs);
        if (rc < 0) {
          printk(KERN_ERR "%s: read imei error = %d \n",__func__,rc);
        }
        #if 0 //test
        printk(KERN_ERR "%s : imei <%x> %x %x %x %x , %x %x %x %x, %x %x %x %x, %x %x %x %x\n",__func__,0,read_buf[0],read_buf[1],read_buf[2],read_buf[3],read_buf[4],read_buf[5],read_buf[6],read_buf[7],read_buf[8],read_buf[9],read_buf[10],read_buf[11],read_buf[12],read_buf[13],read_buf[14],read_buf[15]);
        #endif
        // close(
        fput(rawdata_filp);
        imei_backup_info_buf = (imei_backup_info_type *)&read_buf[0];
        if(imei_backup_info_buf->imei_magic_num & IMEI_ADDR_MAGIC_NUM) {
          memcpy(phoneinfo_tmp->imei, read_buf+4, PDL_IMEI_SIZE);
        }
}

unsigned fill_writereq(int *dloadinfo_state, struct usb_request *writereq)
{
        unsigned len = TX_BUF_SIZE;

        switch( *dloadinfo_state )
        {
        case DLOADINFO_AT_RESPONSE_STATE:
            {
              memcpy(writereq->buf, "AT*PHONEINFO*WAIT", sizeof("AT*PHONEINFO*WAIT")-1);
              writereq->length = sizeof("AT*PHONEINFO*WAIT")-1;
              len = writereq->length;
              printk(KERN_ERR "%s: AT*PHONEINFO*WAIT", __func__);
            }
            break;
        
        case DLOADINFO_PHONE_INFO_STATE:
            {
              unsigned char *tx_buf;
              //int i;
        
              // header
              //    command       2byte
              //    ack_nack      2byte
              //    error_code    4byte
              //    data_length   4byte
              //    reserved      4byte
        
              printk(KERN_ERR "%s: case DLOADINFO_PHONE_INFO_STATE", __func__);
              memset( writereq->buf, 0x0, 16 + sizeof(phoneinfo_type) );
              tx_buf = (unsigned char*)writereq->buf;
              tx_buf[0] = 1;
              tx_buf[9] = 1;
              phoneinfo_buf = (phoneinfo_type *)&tx_buf[16];
        
              load_phoneinfo_with_imei(phoneinfo_buf);
        
              writereq->length = 16 + sizeof(phoneinfo_type);
              len = writereq->length;

              #if 0 //test
              for( i=3; i < len/16; i++ )
              {
                printk(KERN_ERR "%s : phoneinfo <%x> %x %x %x %x, %x %x %x %x, %x %x %x %x, %x %x %x %x\n",__func__,i*16,tx_buf[i*16],tx_buf[i*16+1],tx_buf[i*16+2],tx_buf[i*16+3],tx_buf[i*16+4],tx_buf[i*16+5],tx_buf[i*16+6],tx_buf[i*16+7],tx_buf[i*16+8],tx_buf[i*16+9],tx_buf[i*16+10],tx_buf[i*16+11],tx_buf[i*16+12],tx_buf[i*16+13],tx_buf[i*16+14],tx_buf[i*16+15]);
              }
			  #endif 

              printk(KERN_ERR "%s: packet make DLOADINFO_PHONE_INFO_STATE", __func__);
            }
            break;
        
        case DLOADINFO_FINISH_STATE:
            {
              // header
              //    command       2byte
              //    ack_nack      2byte
              //    error_code    4byte
              //    data_length   4byte
              //    reserved      4byte
        
              printk(KERN_ERR "%s: case DLOADINFO_FINISH_STATE", __func__);
              memset( writereq->buf, 0x0, 16 );
              writereq->length = 16;
              len = writereq->length;
        
              *dloadinfo_state = DLOADINFO_NONE_STATE;
               printk(KERN_ERR "%s: set DLOADINFO_NONE_STATE", __func__);
            }
            break;
        }
        return len;
}
#endif /* CONFIG_PANTECH_PRESTO_BOARD */



void gsdio_rx_push(struct work_struct *w)
{
	struct gsdio_port *port = container_of(w, struct gsdio_port, push);
	struct list_head *q = &port->read_queue;
	struct usb_ep		*out;
	int ret;

//20110826 choiseulkee add for PDL IDLE download of PRESTO etc.... for AT&T
#ifdef CONFIG_PANTECH_PRESTO_BOARD
	struct list_head *writepool = &port->write_pool;
	static int dloadinfo_state = DLOADINFO_NONE_STATE;
	const unsigned short DLOADINFO_PACKET_VERSION = 0;
#endif

	pr_debug("%s: port:%p port#%d read_queue:%p", __func__,
			port, port->port_num, q);

	spin_lock_irq(&port->port_lock);

	if (!port->port_usb) {
		pr_debug("%s: usb cable is disconencted\n", __func__);
		spin_unlock_irq(&port->port_lock);
		return;
	}

	out = port->port_usb->out;

	while (!list_empty(q)) {
		struct usb_request *req;

		req = list_first_entry(q, struct usb_request, list);

		switch (req->status) {
		case -ESHUTDOWN:
			pr_debug("%s: req status shutdown portno#%d port:%p",
					__func__, port->port_num, port);
			goto rx_push_end;
		default:
			pr_warning("%s: port:%p port#%d"
					" Unexpected Rx Status:%d\n", __func__,
					port, port->port_num, req->status);
			/* FALL THROUGH */
		case 0:
			/* normal completion */
			break;
		}

//20110721 choiseulkee add, reboot for PDL IDLE download
#ifdef CONFIG_PANTECH
        if( memcmp( req->buf, "AT*PHONEINFO", sizeof("AT*PHONEINFO")-1) == 0 )
        {
#ifndef CONFIG_PANTECH_PRESTO_BOARD
          printk(KERN_ERR "%s: PDL IDLE DLOAD REBOOT", __func__);
          machine_restart("oem-33");
          return;
#else //20110826 choiseulkee add for PDL IDLE download of PRESTO etc.... for AT&T
          if( memcmp( req->buf, "AT*PHONEINFO*RESET", sizeof("AT*PHONEINFO*RESET")-1) == 0 )
          {
            printk(KERN_ERR "%s: PDL IDLE DLOAD REBOOT", __func__);
            machine_restart("oem-33");
            return;
          }
          else
          {
            printk(KERN_ERR "%s: go DLOADINFO_AT_RESPONSE_STATE", __func__);
            dloadinfo_state = DLOADINFO_AT_RESPONSE_STATE;
          }
#endif
        }
//20110826 choiseulkee add for PDL IDLE download of PRESTO etc.... for AT&T
#ifdef CONFIG_PANTECH_PRESTO_BOARD
        //else if( dloadinfo_state != DLOADINFO_NONE_STATE )
        else if( dloadinfo_state == DLOADINFO_AT_RESPONSE_STATE || dloadinfo_state == DLOADINFO_PHONE_INFO_STATE )
        {
          printk(KERN_ERR "%s: if %d", __func__, dloadinfo_state);
          if( *(unsigned int *)(req->buf) == (PHONE_INFO_CMD|(DLOADINFO_PACKET_VERSION<<16)) )
          {
            dloadinfo_state = DLOADINFO_PHONE_INFO_STATE;
            printk(KERN_ERR "%s: go DLOADINFO_PHONE_INFO_STATE", __func__);
          }
          if( *(unsigned int *)(req->buf) == (FINISH_CMD|(DLOADINFO_PACKET_VERSION<<16)) )
          {
            dloadinfo_state = DLOADINFO_FINISH_STATE;
            printk(KERN_ERR "%s: go DLOADINFO_FINISH_STATE", __func__);
          }
        }

        if( dloadinfo_state != DLOADINFO_NONE_STATE )
        {
          printk(KERN_ERR "%s: run cmd send_dload_packet", __func__);

          if (!list_empty(writepool))
          {
            struct usb_ep *in = port->port_usb->in;
            struct usb_request *writereq;
            unsigned len = TX_BUF_SIZE;
            int ret;

            printk(KERN_ERR "%s: if start, before list_entry", __func__);
            writereq = list_entry(writepool->next, struct usb_request, list);

            list_del(&writereq->list);
            port->wp_len--;

            len = fill_writereq(&dloadinfo_state, writereq);

            printk(KERN_ERR "%s: before usb_ep_queue, len= %d", __func__, len);
            spin_unlock_irq(&port->port_lock);
            ret = usb_ep_queue(in, writereq, GFP_KERNEL);
            spin_lock_irq(&port->port_lock);
            printk(KERN_ERR "%s: ret=%d", __func__,ret);
            if (ret)
            {
              pr_err("%s: usb ep out queue failed"
                     "port:%p, port#%d err:%d\n",
                     __func__, port, port->port_num, ret);

              /* could be usb disconnected */
              if (!port->port_usb)
              {
                printk(KERN_ERR "%s: before gsdio_free_req", __func__);
                gsdio_free_req(in, writereq);
              }
            }
            //printk(KERN_ERR "%s: before list_add writereq->list", __func__);
            //list_add(&writereq->list, writepool);
            //printk(KERN_ERR "%s: port->wp_len=%lu", __func__, port->wp_len);
            //port->wp_len++;
            //printk(KERN_ERR "%s: after port->wp_len++", __func__);

            port->nbytes_tolaptop += len;
  
          }

          //printk(KERN_ERR "%s: before list_del req->list", __func__);
          //list_del(&req->list);
          //port->rq_len--;

          //printk(KERN_ERR "%s: before spin_unlock_irq", __func__);
          //spin_unlock_irq(&port->port_lock);
          //printk(KERN_ERR "%s: return ok", __func__);
          //return;
        }
#endif /* CONFIG_PANTECH_PRESTO_BOARD */
#endif /* CONFIG_PANTECH */

		if (!port->sdio_open) {
			pr_err("%s: sio channel is not open\n", __func__);
			list_move(&req->list, &port->read_pool);
			port->rp_len++;
			port->rq_len--;
			goto rx_push_end;
		}


#if 0 // FEATURE_PANTECH_VT_SUPPORT_QMI
    if(sdio_vt_dun_disable) {
      pr_err("%s: sio vt channel is open\n", __func__);
		list_del(&req->list);
		port->rq_len--;
      goto rx_push_end;
    }
#endif
		list_del(&req->list);
		port->rq_len--;

		ret = gsdio_write(port, req);
		/* as gsdio_write drops spin_lock while writing data
		 * to sdio usb cable may have been disconnected
		 */
		if (!port->port_usb) {
			port->n_read = 0;
			gsdio_free_req(out, req);
			spin_unlock_irq(&port->port_lock);
			return;
		}

		if (ret || port->n_read) {
			list_add(&req->list, &port->read_queue);
			port->rq_len++;
			goto rx_push_end;
		}

		list_add(&req->list, &port->read_pool);
		port->rp_len++;
	}

	if (port->sdio_open && !list_empty(q)) {
		if (sdio_write_avail(port->sport_info->ch))
			queue_work(gsdio_wq, &port->push);
	}
rx_push_end:
	spin_unlock_irq(&port->port_lock);

	/* start queuing out requests again to host */
	gsdio_start_rx(port);
}

void gsdio_read_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gsdio_port *port = ep->driver_data;
	unsigned long flags;

	pr_debug("%s: ep:%p port:%p\n", __func__, ep, port);

	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return;
	}

#if 0 // FEATURE_PANTECH_VT_SUPPORT_QMI
  if(sdio_vt_dun_disable) {
    pr_err("%s: sdio_vt port is opened\n", __func__);
    return;
  }
#endif

	spin_lock_irqsave(&port->port_lock, flags);
	list_add_tail(&req->list, &port->read_queue);
	port->rq_len++;
	queue_work(gsdio_wq, &port->push);
	spin_unlock_irqrestore(&port->port_lock, flags);

	return;
}

void gsdio_write_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct gsdio_port *port = ep->driver_data;
	unsigned long flags;

	pr_debug("%s: ep:%p port:%p\n", __func__, ep, port);

	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return;
	}

	spin_lock_irqsave(&port->port_lock, flags);
	list_add(&req->list, &port->write_pool);
	port->wp_len++;

	switch (req->status) {
	default:
		pr_warning("%s: port:%p port#%d unexpected %s status %d\n",
				__func__, port, port->port_num,
				ep->name, req->status);
		/* FALL THROUGH */
	case 0:
		queue_work(gsdio_wq, &port->pull);
		break;

	case -ESHUTDOWN:
		/* disconnect */
		pr_debug("%s: %s shutdown\n", __func__, ep->name);
		break;
	}

	spin_unlock_irqrestore(&port->port_lock, flags);

	return;
}

void gsdio_read_pending(struct gsdio_port *port)
{
	struct sdio_channel *ch;
	char buf[1024];
	int avail;

	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return;
	}

	ch = port->sport_info->ch;

	if (!ch)
		return;

	while ((avail = sdio_read_avail(ch))) {
		if (avail > 1024)
			avail = 1024;
		sdio_read(ch, buf, avail);

		pr_debug("%s: flushed out %d bytes\n", __func__, avail);
	}
}

void gsdio_tx_pull(struct work_struct *w)
{
	struct gsdio_port *port = container_of(w, struct gsdio_port, pull);
	struct list_head *pool = &port->write_pool;

	pr_debug("%s: port:%p port#%d pool:%p\n", __func__,
			port, port->port_num, pool);

	if (!port->port_usb) {
		pr_err("%s: usb disconnected\n", __func__);

		/* take out all the pending data from sdio */
		gsdio_read_pending(port);

		return;
	}

	spin_lock_irq(&port->port_lock);

	while (!list_empty(pool)) {
		int avail;
		struct usb_ep *in = port->port_usb->in;
		struct sdio_channel *ch = port->sport_info->ch;
		struct usb_request *req;
		unsigned len = TX_BUF_SIZE;
		int ret;


		req = list_entry(pool->next, struct usb_request, list);

		if (!port->sdio_open) {
			pr_debug("%s: SDIO channel is not open\n", __func__);
			goto tx_pull_end;
		}

		avail = sdio_read_avail(ch);
		if (!avail) {
			/* REVISIT: for ZLP */
			pr_debug("%s: read_avail:%d port:%p port#%d\n",
					__func__, avail, port, port->port_num);
			goto tx_pull_end;
		}

		if (avail > len)
			avail = len;

		list_del(&req->list);
		port->wp_len--;

		spin_unlock_irq(&port->port_lock);
		ret = sdio_read(ch, req->buf, avail);
		spin_lock_irq(&port->port_lock);
		if (ret) {
			pr_err("%s: port:%p port#%d sdio read failed err:%d",
					__func__, port, port->port_num, ret);

			/* check if usb is still active */
			if (!port->port_usb) {
				gsdio_free_req(in, req);
			} else {
				list_add(&req->list, pool);
				port->wp_len++;
			}
			goto tx_pull_end;
		}

		req->length = avail;

		spin_unlock_irq(&port->port_lock);
		ret = usb_ep_queue(in, req, GFP_KERNEL);
		spin_lock_irq(&port->port_lock);
		if (ret) {
			pr_err("%s: usb ep out queue failed"
					"port:%p, port#%d err:%d\n",
					__func__, port, port->port_num, ret);

			/* could be usb disconnected */
			if (!port->port_usb) {
				gsdio_free_req(in, req);
			} else {
				list_add(&req->list, pool);
				port->wp_len++;
			}
			goto tx_pull_end;
		}

		port->nbytes_tolaptop += avail;
	}
tx_pull_end:
	spin_unlock_irq(&port->port_lock);
}

int gsdio_start_io(struct gsdio_port *port)
{
	int			ret;
	unsigned long		flags;

	pr_debug("%s:\n", __func__);

	spin_lock_irqsave(&port->port_lock, flags);

	if (!port->port_usb) {
		spin_unlock_irqrestore(&port->port_lock, flags);
		return -ENODEV;
	}

	/* start usb out queue */
	ret = gsdio_alloc_requests(port->port_usb->out,
				&port->read_pool,
				RX_QUEUE_SIZE, RX_BUF_SIZE,
				gsdio_read_complete);
	if (ret) {
		spin_unlock_irqrestore(&port->port_lock, flags);
		pr_err("%s: unable to allocate out reqs\n", __func__);
		return ret;
	}
	port->rp_len = RX_QUEUE_SIZE;

	ret = gsdio_alloc_requests(port->port_usb->in,
				&port->write_pool,
				TX_QUEUE_SIZE, TX_BUF_SIZE,
				gsdio_write_complete);
	if (ret) {
		gsdio_free_requests(port->port_usb->out, &port->read_pool);
		port->rp_len = 0;
		spin_unlock_irqrestore(&port->port_lock, flags);
		pr_err("%s: unable to allocate in reqs\n", __func__);
		return ret;
	}
	port->wp_len = TX_QUEUE_SIZE;
	spin_unlock_irqrestore(&port->port_lock, flags);

	gsdio_start_rx(port);
	queue_work(gsdio_wq, &port->pull);

	return 0;
}

void gsdio_port_free(unsigned portno)
{
	struct gsdio_port *port = ports[portno].port;
	struct platform_driver *pdriver = &ports[portno].gsdio_ch;

	if (!port) {
		pr_err("%s: invalid portno#%d\n", __func__, portno);
		return;
	}

	platform_driver_unregister(pdriver);

	kfree(port);
}

void gsdio_ctrl_wq(struct work_struct *w)
{
	struct gsdio_port *port;

	port = container_of(w, struct gsdio_port, notify_modem);

	if (!port) {
		pr_err("%s: port is null\n", __func__);
		return;
	}

	if (!port->sdio_open || port->ctrl_ch_err)
		return;

	sdio_cmux_tiocmset(port->sport_info->ctrl_ch_id,
			port->cbits_to_modem, ~(port->cbits_to_modem));
}

void gsdio_ctrl_notify_modem(struct gserial *gser, u8 portno, int ctrl_bits)
{
	struct gsdio_port *port;
	int temp;

	if (portno >= n_ports) {
		pr_err("%s: invalid portno#%d\n", __func__, portno);
		return;
	}

	if (!gser) {
		pr_err("%s: gser is null\n", __func__);
		return;
	}

	port = ports[portno].port;

	temp = ctrl_bits & ACM_CTRL_DTR ? TIOCM_DTR : 0;

	if (port->cbits_to_modem == temp)
		return;

	 port->cbits_to_modem = temp;

	/* TIOCM_DTR - 0x002 - bit(1) */
	pr_debug("%s: port:%p port#%d ctrl_bits:%08x\n", __func__,
		port, port->port_num, ctrl_bits);

	if (!port->sdio_open) {
		pr_err("%s: port:%p port#%d sdio not connected\n",
				__func__, port, port->port_num);
		return;
	}

	/* whenever DTR is high let laptop know that modem status */
	if (port->cbits_to_modem && gser->send_modem_ctrl_bits)
		gser->send_modem_ctrl_bits(gser, port->cbits_to_laptop);

	queue_work(gsdio_wq, &port->notify_modem);
}

void gsdio_ctrl_modem_status(int ctrl_bits, void *_dev)
{
	struct gsdio_port *port = _dev;

	/* TIOCM_CD - 0x040 - bit(6)
	 * TIOCM_RI - 0x080 - bit(7)
	 * TIOCM_DSR- 0x100 - bit(8)
	 */
	pr_debug("%s: port:%p port#%d event:%08x\n", __func__,
		port, port->port_num, ctrl_bits);

	port->cbits_to_laptop = 0;
	ctrl_bits &= TIOCM_RI | TIOCM_CD | TIOCM_DSR;
	if (ctrl_bits & TIOCM_RI)
		port->cbits_to_laptop |= ACM_CTRL_RI;
	if (ctrl_bits & TIOCM_CD)
		port->cbits_to_laptop |= ACM_CTRL_DCD;
	if (ctrl_bits & TIOCM_DSR)
		port->cbits_to_laptop |= ACM_CTRL_DSR;

	if (port->port_usb && port->port_usb->send_modem_ctrl_bits)
		port->port_usb->send_modem_ctrl_bits(port->port_usb,
					port->cbits_to_laptop);
}

void gsdio_ch_notify(void *_dev, unsigned event)
{
	struct gsdio_port *port = _dev;

	pr_debug("%s: port:%p port#%d event:%s\n", __func__,
		port, port->port_num,
		event == 1 ? "READ AVAIL" : "WRITE_AVAIL");

	if (event == SDIO_EVENT_DATA_WRITE_AVAIL)
		queue_work(gsdio_wq, &port->push);
	if (event == SDIO_EVENT_DATA_READ_AVAIL)
		queue_work(gsdio_wq, &port->pull);
}

static void gsdio_open_work(struct work_struct *w)
{
	struct gsdio_port *port =
			container_of(w, struct gsdio_port, sdio_open_work.work);
	struct sdio_port_info *pi = port->sport_info;
	struct gserial *gser;
	int ret;
	int ctrl_bits;
	int startio;

	ret = sdio_open(pi->data_ch_name, &pi->ch, port, gsdio_ch_notify);
	if (ret) {
		pr_err("%s: port:%p port#%d unable to open sdio ch:%s\n",
				__func__, port, port->port_num,
				pi->data_ch_name);
		return;
	}

	port->ctrl_ch_err = 0;
	ret = sdio_cmux_open(pi->ctrl_ch_id, 0, 0,
			gsdio_ctrl_modem_status, port);
	if (ret) {
		pr_err("%s: port:%p port#%d unable to open ctrl ch:%d\n",
				__func__, port, port->port_num, pi->ctrl_ch_id);
		port->ctrl_ch_err = 1;
	}

	/* check for latest status update from modem */
	if (!port->ctrl_ch_err) {
		ctrl_bits = sdio_cmux_tiocmget(pi->ctrl_ch_id);
		gsdio_ctrl_modem_status(ctrl_bits, port);
	}

	pr_debug("%s: SDIO data:%s ctrl:%d are open\n", __func__,
					pi->data_ch_name,
					pi->ctrl_ch_id);

	port->sdio_open = 1;

	/* start tx if usb is open already */
	spin_lock_irq(&port->port_lock);
	startio = port->port_usb ? 1 : 0;
	gser = port->port_usb;
	spin_unlock_irq(&port->port_lock);

	if (startio) {
		pr_debug("%s: USB is already open, start io\n", __func__);
		gsdio_start_io(port);
		 if (gser->send_modem_ctrl_bits)
			gser->send_modem_ctrl_bits(gser, port->cbits_to_laptop);
	}
}

#define SDIO_CH_NAME_MAX_LEN	9
#define SDIO_OPEN_DELAY		msecs_to_jiffies(10000)
static int gsdio_ch_remove(struct platform_device *dev)
{
	struct gsdio_port	*port;
	struct sdio_port_info	*pi;
	int i;
	unsigned long		flags;

	pr_debug("%s: name:%s\n", __func__, dev->name);

	for (i = 0; i < n_ports; i++) {
		port = ports[i].port;
		pi = port->sport_info;

		if (!strncmp(pi->data_ch_name, dev->name,
					SDIO_CH_NAME_MAX_LEN)) {
			struct gserial *gser = port->port_usb;

			port->sdio_open = 0;
			port->sdio_probe = 0;
			port->ctrl_ch_err = 1;

			/* check if usb cable is connected */
			if (!gser)
				continue;

			/* indicated call status to usb host */
			gsdio_ctrl_modem_status(0, port);

			usb_ep_fifo_flush(gser->in);
			usb_ep_fifo_flush(gser->out);

			cancel_work_sync(&port->push);
			cancel_work_sync(&port->pull);

			spin_lock_irqsave(&port->port_lock, flags);
			gsdio_free_requests(gser->out, &port->read_pool);
			gsdio_free_requests(gser->out, &port->read_queue);
			gsdio_free_requests(gser->in, &port->write_pool);

			port->rp_len = 0;
			port->rq_len = 0;
			port->wp_len = 0;
			port->n_read = 0;
			spin_unlock_irqrestore(&port->port_lock, flags);

		}
	}

	return 0;
}

static int gsdio_ch_probe(struct platform_device *dev)
{
	struct gsdio_port	*port;
	struct sdio_port_info	*pi;
	int i;

	pr_debug("%s: name:%s\n", __func__, dev->name);

	for (i = 0; i < n_ports; i++) {
		port = ports[i].port;
		pi = port->sport_info;

		pr_debug("%s: sdio_ch_name:%s dev_name:%s\n", __func__,
				pi->data_ch_name, dev->name);

		/* unfortunately cmux channle might not be ready even if
		 * sdio channel is ready. as we dont have good notification
		 * mechanism schedule a delayed work
		 */
		if (!strncmp(pi->data_ch_name, dev->name,
					SDIO_CH_NAME_MAX_LEN)) {
			port->sdio_probe = 1;
			queue_delayed_work(gsdio_wq,
				&port->sdio_open_work, SDIO_OPEN_DELAY);
			return 0;
		}
	}

	pr_info("%s: name:%s is not found\n", __func__, dev->name);

	return -ENODEV;
}

int gsdio_port_alloc(unsigned portno,
		struct usb_cdc_line_coding *coding,
		struct sdio_port_info *pi)
{
	struct gsdio_port *port;
	struct platform_driver *pdriver;

	port = kzalloc(sizeof(struct gsdio_port), GFP_KERNEL);
	if (!port) {
		pr_err("%s: port allocation failed\n", __func__);
		return -ENOMEM;
	}

	port->port_num = portno;
	spin_lock_init(&port->port_lock);
	port->line_coding = *coding;

	/* READ: read from usb and write into sdio */
	INIT_LIST_HEAD(&port->read_pool);
	INIT_LIST_HEAD(&port->read_queue);
	INIT_WORK(&port->push, gsdio_rx_push);

	INIT_LIST_HEAD(&port->write_pool);
	INIT_WORK(&port->pull, gsdio_tx_pull);

	INIT_WORK(&port->notify_modem, gsdio_ctrl_wq);

	INIT_DELAYED_WORK(&port->sdio_open_work, gsdio_open_work);

	ports[portno].port = port;

	port->sport_info = pi;
	pdriver = &ports[portno].gsdio_ch;

	pdriver->probe = gsdio_ch_probe;
	pdriver->remove = gsdio_ch_remove;
	pdriver->driver.name = pi->data_ch_name;
	pdriver->driver.owner = THIS_MODULE;

	pr_debug("%s: port:%p port#%d sdio_name: %s\n", __func__,
			port, port->port_num, pi->data_ch_name);

	platform_driver_register(pdriver);

	pr_debug("%s: port:%p port#%d\n", __func__, port, port->port_num);

	return 0;
}

int gsdio_connect(struct gserial *gser, u8 portno)
{
	struct gsdio_port *port;
	int ret = 0;
	unsigned long flags;

	if (portno >= n_ports) {
		pr_err("%s: invalid portno#%d\n", __func__, portno);
		return -EINVAL;
	}

	if (!gser) {
		pr_err("%s: gser is null\n", __func__);
		return -EINVAL;
	}

	port = ports[portno].port;

	spin_lock_irqsave(&port->port_lock, flags);
	port->port_usb = gser;
	gser->notify_modem = gsdio_ctrl_notify_modem;
	spin_unlock_irqrestore(&port->port_lock, flags);

	ret = usb_ep_enable(gser->in, gser->in_desc);
	if (ret) {
		pr_err("%s: failed to enable in ep w/ err:%d\n",
					__func__, ret);
		port->port_usb = 0;
		return ret;
	}
	gser->in->driver_data = port;

	ret = usb_ep_enable(gser->out, gser->out_desc);
	if (ret) {
		pr_err("%s: failed to enable in ep w/ err:%d\n",
					__func__, ret);
		usb_ep_disable(gser->in);
		port->port_usb = 0;
		gser->in->driver_data = 0;
		return ret;
	}
	gser->out->driver_data = port;

	if (port->sdio_open) {
		pr_debug("%s: sdio is already open, start io\n", __func__);
		gsdio_start_io(port);
		 if (gser->send_modem_ctrl_bits)
			gser->send_modem_ctrl_bits(gser, port->cbits_to_laptop);
	}

	return 0;
}

void gsdio_disconnect(struct gserial *gser, u8 portno)
{
	unsigned long flags;
	struct gsdio_port *port;

	if (portno >= n_ports) {
		pr_err("%s: invalid portno#%d\n", __func__, portno);
		return;
	}

	if (!gser) {
		pr_err("%s: gser is null\n", __func__);
		return;
	}

	port = ports[portno].port;

	/* send dtr zero to modem to notify disconnect */
	port->cbits_to_modem = 0;
	queue_work(gsdio_wq, &port->notify_modem);

	spin_lock_irqsave(&port->port_lock, flags);
	port->port_usb = 0;
	port->nbytes_tomodem = 0;
	port->nbytes_tolaptop = 0;
	spin_unlock_irqrestore(&port->port_lock, flags);

	/* disable endpoints, aborting down any active I/O */
	usb_ep_disable(gser->out);

	usb_ep_disable(gser->in);

	spin_lock_irqsave(&port->port_lock, flags);

#ifdef CONFIG_PANTECH_PRESTO_BOARD
	if (port->sdio_open)
#endif
	{
	gsdio_free_requests(gser->out, &port->read_pool);
	gsdio_free_requests(gser->out, &port->read_queue);
	gsdio_free_requests(gser->in, &port->write_pool);
	}

	port->rp_len = 0;
	port->rq_len = 0;
	port->wp_len = 0;
	port->n_read = 0;
	spin_unlock_irqrestore(&port->port_lock, flags);
}

#if defined(CONFIG_DEBUG_FS)
static ssize_t debug_read_stats(struct file *file, char __user *ubuf,
		size_t count, loff_t *ppos)
{
	struct gsdio_port *port;
	char *buf;
	unsigned long flags;
	int i = 0;
	int temp = 0;
	int ret;

	buf = kzalloc(sizeof(char) * 1024, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while (i < n_ports) {
		port = ports[i].port;
		spin_lock_irqsave(&port->port_lock, flags);
		temp += scnprintf(buf + temp, PAGE_SIZE - temp,
				"###PORT:%d port:%p###\n"
				"nbytes_tolaptop: %lu\n"
				"nbytes_tomodem:  %lu\n"
				"cbits_to_modem:  %u\n"
				"cbits_to_laptop: %u\n"
				"read_pool_len:   %lu\n"
				"read_queue_len:  %lu\n"
				"write_pool_len:  %lu\n"
				"n_read:          %u\n"
				"sdio_open:       %d\n"
				"sdio_probe:      %d\n",
				i, port,
				port->nbytes_tolaptop, port->nbytes_tomodem,
				port->cbits_to_modem, port->cbits_to_laptop,
				port->rp_len, port->rq_len, port->wp_len,
				port->n_read,
				port->sdio_open, port->sdio_probe);
		spin_unlock_irqrestore(&port->port_lock, flags);
		i++;
	}

	ret = simple_read_from_buffer(ubuf, count, ppos, buf, temp);

	kfree(buf);

	return ret;
}

static ssize_t debug_reset_stats(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	struct gsdio_port *port;
	unsigned long flags;
	int i = 0;

	while (i < n_ports) {
		port = ports[i].port;

		spin_lock_irqsave(&port->port_lock, flags);
		port->nbytes_tolaptop = 0;
		port->nbytes_tomodem = 0;
		spin_unlock_irqrestore(&port->port_lock, flags);
		i++;
	}

	return count;
}

static int debug_open(struct inode *inode, struct file *file)
{
	return 0;
}

static const struct file_operations debug_gsdio_ops = {
	.open = debug_open,
	.read = debug_read_stats,
	.write = debug_reset_stats,
};

static void gsdio_debugfs_init(void)
{
	struct dentry *dent;

	dent = debugfs_create_dir("usb_gsdio", 0);
	if (IS_ERR(dent))
		return;

	debugfs_create_file("status", 0444, dent, 0, &debug_gsdio_ops);
}
#else
static void gsdio_debugfs_init(void)
{
	return;
}
#endif

/* connect, disconnect, alloc_requests, free_requests */
int gsdio_setup(struct usb_gadget *g, unsigned count)
{
	struct usb_cdc_line_coding	coding;
	int i;
	int ret = 0;
	struct sdio_port_info *port_info;

	pr_debug("%s: gadget:(%p) count:%d\n", __func__, g, count);

	if (count == 0 || count > N_PORTS) {
		pr_err("%s: invalid number of ports count:%d max_ports:%d\n",
				__func__, count, N_PORTS);
		return -EINVAL;
	}

	coding.dwDTERate = cpu_to_le32(9600);
	coding.bCharFormat = 8;
	coding.bParityType = USB_CDC_NO_PARITY;
	coding.bDataBits = USB_CDC_1_STOP_BITS;

	gsdio_wq = create_singlethread_workqueue("k_gserial");
	if (!gsdio_wq) {
		pr_err("%s: unable to create workqueue gsdio_wq\n",
				__func__);
		return -ENOMEM;
	}

	for (i = 0; i < count; i++) {
		mutex_init(&ports[i].lock);
		ret = gsdio_port_alloc(i, &coding, sport_info + i);
		if (ret) {
			pr_err("%s: sdio logical port allocation failed\n",
					__func__);
			goto free_ports;
		}
		n_ports++;
		port_info++;

#ifdef DEBUG
		/* REVISIT: create one file per port
		 * or do not create any file
		 */
		if (i == 0) {
			ret = device_create_file(&g->dev, &dev_attr_input);
			if (ret)
				pr_err("%s: unable to create device file\n",
						__func__);
		}
#endif

	}

	gsdio_debugfs_init();

	return 0;

free_ports:
	for (i = 0; i < n_ports; i++)
		gsdio_port_free(i);
	destroy_workqueue(gsdio_wq);

	return ret;
}

/* TODO: Add gserial_cleanup */
