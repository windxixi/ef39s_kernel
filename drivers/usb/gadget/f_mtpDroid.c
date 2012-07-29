/* Gadget Driver for Android MTP (Media Transfer Protocol)
 *
 * $Id: f_mtpDroid.c, v1.00, 2010.04.08:15:27:22$
 *
 * Initial work by:
 *   (c) 2007 Insight-kr Lee Hyun Wook (wondo@insight-kr.com)
 *
                        EDIT HISTORY FOR MODULE
 
when            who          what, where, why
--------      ------     ----------------------------------------------------------
2007/11/27   wondo      Initial work............^^"



-----------------------------------------------------------------------------*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/err.h>
#include <linux/interrupt.h>

#include <linux/types.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#include <linux/usb/android_composite.h>

#include "f_mtpDroid.h"
//..#include "insightComp.h"

#include "gadget_chips.h"


/*-------------------------------------------------------------------------*/
#if FEATURE_MINI_DEVICE
#define DRIVER_NAME		 "mtp"
#else
#define DRIVER_NAME		 "imtpDroid"
#endif
#define DRIVER_DESC		"mtpDroid"
#define DRIVER_VERSION	"iMTPDroid v1.1"

#if FEATURE_MINI_DEVICE
static const char shortname[] = "mtp_usb";
#else
static const char shortname[] = "mtpdroid";
#endif
static const char driver_desc [] = DRIVER_DESC;
/*-------------------------------------------------------------------------*/

#if FEATURE_MINI_DEVICE
#define MTP_VENDOR_NUM	0x0525
#define MTP_PRODUCT_NUM	0x0005
#else
#define MTP_VENDOR_NUM	0x0525
#define MTP_PRODUCT_NUM	0x0004
#endif

#define STRING_MANUFACTURER		1
#define STRING_PRODUCT			2
#define STRING_SERIALNUM		3

#define DEV_CONFIG_VALUE		1

#define USB_DESC_BUFSIZE		256
#define INTERRUPT_BUFFER_SIZE	4096
#define BULK_BUFFER_SIZE		8192  //..wondo 2010/07/28  4096  -->  8192
#define BULK_BUFFER_SIZE_2		8192

/* number of rx and tx requests to allocate */
#define RX_REQ_MAX 4
#define RX_REQ_MAX_2 2  //..wondo 2010/07/27  2  -->  1
#define RX_REQ_MAX_1 1  //..wondo 2010/07/27  2  -->  1
#define TX_REQ_MAX 2  //..wondo 2010/07/28  4  -->  2


struct f_mtpdroid_dev {
	struct usb_function function;
	struct usb_composite_dev *cdev;
	spinlock_t lock;


	struct usb_ep *ep_int;
	struct usb_ep *ep_in;
	struct usb_ep *ep_out;

	int online;
	int error;
	int suspend;

	atomic_t read_excl;
	atomic_t write_excl;
	atomic_t write_interrupt_excl;
	atomic_t open_excl;

	struct list_head tx_idle;
	struct list_head tx_interrupt_idle;
	struct list_head rx_idle;
	struct list_head rx_done;

	wait_queue_head_t read_wq;
	wait_queue_head_t write_wq;
	wait_queue_head_t write_interrupt_wq;

	/* the request we're currently reading from */
	struct usb_request *read_req;
	unsigned char *read_buf;
	unsigned read_count;

	//......................................//
	USB_MTP_STATE state;
	u8 config;
	u16 interface;
	const struct usb_endpoint_descriptor *interrupt, *in, *out;

	int sendObj_Cancel;

	//......................................//
	SCRIPT_TYPE A;
	SCRIPT_TYPE B;
	
	SCRIPT_TYPE C;
	SCRIPT_TYPE D;
	SCRIPT_TYPE E;
	
};


/*-------------------------------------------------------------------------*/
static struct usb_config_descriptor mtpf_configuration_desc = {
	.bLength =		sizeof mtpf_configuration_desc,
	.bDescriptorType =	USB_DT_CONFIG,

	/* compute wTotalLength on the fly */
	.bNumInterfaces =	1,
	.bConfigurationValue =	DEV_CONFIG_VALUE,
	.iConfiguration =	0,
	.bmAttributes =		USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower	= 0xFA, /* 500ma */
};
/*-------------------------------------------------------------------------*/
static struct usb_interface_descriptor mtpdroid_interface_desc = {
	.bLength                = USB_DT_INTERFACE_SIZE,
	.bDescriptorType        = USB_DT_INTERFACE,
	.bInterfaceNumber       = 0,
	.bNumEndpoints          = 3,
	.bInterfaceClass        = USB_CLASS_STILL_IMAGE,
	.bInterfaceSubClass     = 1,
	.bInterfaceProtocol     = 1,
};

static struct usb_endpoint_descriptor hs_mtpdroid_interrupt_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_IN,
	.bmAttributes           = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize         = __constant_cpu_to_le16(16),
	.bInterval = 0x0C,
};

static struct usb_endpoint_descriptor hs_mtpdroid_in_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_IN,
	.bmAttributes           = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize         = __constant_cpu_to_le16(512),
};

static struct usb_endpoint_descriptor hs_mtpdroid_out_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_OUT,
	.bmAttributes           = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize         = __constant_cpu_to_le16(512),
};

//=============================================//

static struct usb_endpoint_descriptor fs_mtpdroid_interrupt_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_IN,
	.bmAttributes           = USB_ENDPOINT_XFER_INT,
};

static struct usb_endpoint_descriptor fs_mtpdroid_in_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_IN,
	.bmAttributes           = USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor fs_mtpdroid_out_desc = {
	.bLength                = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType        = USB_DT_ENDPOINT,
	.bEndpointAddress       = USB_DIR_OUT,
	.bmAttributes           = USB_ENDPOINT_XFER_BULK,
};

static struct usb_qualifier_descriptor dev_qualifier = {
	.bLength =		sizeof dev_qualifier,
	.bDescriptorType =	USB_DT_DEVICE_QUALIFIER,
	.bcdUSB =		__constant_cpu_to_le16(0x0200),
	.bDeviceClass =		USB_CLASS_STILL_IMAGE,
	.bNumConfigurations =	1
};

static struct usb_descriptor_header *fs_mtpdroid_descs[] = {
	(struct usb_descriptor_header *) &mtpdroid_interface_desc,
	(struct usb_descriptor_header *) &fs_mtpdroid_interrupt_desc,
	(struct usb_descriptor_header *) &fs_mtpdroid_in_desc,
	(struct usb_descriptor_header *) &fs_mtpdroid_out_desc,
	NULL,
};

static struct usb_descriptor_header *hs_mtpdroid_descs[] = {
	(struct usb_descriptor_header *) &mtpdroid_interface_desc,
	(struct usb_descriptor_header *) &hs_mtpdroid_interrupt_desc,
	(struct usb_descriptor_header *) &hs_mtpdroid_in_desc,
	(struct usb_descriptor_header *) &hs_mtpdroid_out_desc,
	NULL,
};

/* used when adb function is disabled 
static struct usb_descriptor_header *null_mtpdroid_descs[] = {
	NULL,
};
wondo...*/

/* maxpacket and other transfer characteristics vary by speed. */
#define ep_desc(g, hs, fs) (((g)->speed == USB_SPEED_HIGH)?(hs):(fs))

/* string IDs are assigned dynamically */
#define STRING_MANUFACTURER_IDX			0
#define STRING_PRODUCT_IDX					1
#define STRING_SERIAL_IDX					2

/*-------------------------------------------------------------------------*/
#if FEATURE_MINI_DEVICE
/* manufacturer */
static char manufacturer[] = "PANTECH";
/* product */
static const char product_desc[] = "PANTECH iMTPDROID";
/* serial */
static char serial_num[] = "2011-05-03.0123456789";
#else
/* manufacturer */
static char manufacturer[] = "INSIGHT";
/* product */
static const char product_desc[] = "INSIGHT MTPDROID";
/* serial */
static char serial_num[] = "2011-05-03.0123456789";
#endif

#if FEATURE_MINI_DEVICE  //..wondo.20110209
static struct usb_string f_mtpdroid_strings[] = {
	[STRING_MANUFACTURER_IDX].s = manufacturer,
	[STRING_PRODUCT_IDX].s = product_desc,
	[STRING_SERIAL_IDX].s = serial_num,
	{  },			/* end of list */
};
#else
/* static strings, in UTF-8 */
static struct usb_string	f_mtpdroid_strings [] = {
	{ STRING_MANUFACTURER,	manufacturer, },
	{ STRING_PRODUCT,	product_desc, },
	{ STRING_SERIALNUM,	serial_num, },
	{  }		/* end of list */
};
#endif

static struct usb_gadget_strings	f_mtpdroid_stringtab = {
	.language	= 0x0409,	/* en-us */
	.strings	= f_mtpdroid_strings,
};

static struct usb_gadget_strings *dev_strings[] = {
	&f_mtpdroid_stringtab,
	NULL,
};

static struct usb_gadget_strings	stringtab = {
	.language	= 0x0409,	/* en-us */
	.strings	= f_mtpdroid_strings,
};


//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================


/* temporary variable used between adb_open() and adb_gadget_bind() */
static int g_mtpDroid_isAct = 0;
struct usb_function *f_mtpdroid = NULL;
static struct f_mtpdroid_dev *__f_mtpdroid_dev;

//===============================================
// - MTPd Command Define
#define USB_MTP_NONE			0x0000
#define USB_MTP_ENABLE			0x0001  
#define USB_MTP_CANCEL			0x0002
#define USB_MTP_RESET			0x0004

//===============================================
static int g_mtpDroid_Enable = 0;
static int g_mtpDroid_Cancel = 0;
static int g_mtpDroid_Reset = 0;

static MTP_DEVICE_STATUS g_iMTP_Status = MTP_OK;
static int g_USB_MTP_CMD = USB_MTP_NONE;

//===============================================
#define ADD_USB_MTP_CMD(x) g_USB_MTP_CMD |= x;
#define DEL_USB_MTP_CMD(x) g_USB_MTP_CMD &= ~x;

//===============================================
//===============================================
//===============================================
//===============================================

/*-------------------------------------------------------------------------*/
void f_mtpdroid_set_func(struct usb_function *function)
{
	f_mtpdroid = function;

}

struct usb_function *f_mtpdroid_get_func(void)
{
	return f_mtpdroid;
	
}

int f_mtpdroid_setAct(int act)
{
	g_mtpDroid_isAct = act;
	
	return g_mtpDroid_isAct;
}

int f_mtpdroid_getAct(void)
{
	return g_mtpDroid_isAct;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

static inline struct f_mtpdroid_dev *f_mtpdroid_to_dev(struct usb_function *f)
{
	return container_of(f, struct f_mtpdroid_dev, function);
}

static inline int f_mtpdroid_lock(atomic_t *excl)
{
	if (atomic_inc_return(excl) == 1) {
		return 0;
	} else {
		atomic_dec(excl);
		return -1;
	}
}

static inline void f_mtpdroid_unlock(atomic_t *excl)
{
	atomic_dec(excl);
}

static struct usb_request *request_mtprdroid_new(struct usb_ep *ep, int buffer_size)
{
	struct usb_request *req = usb_ep_alloc_request(ep, GFP_KERNEL);
	if (!req)
		return NULL;

	/* now allocate buffers for the requests */
	req->buf = kmalloc(buffer_size, GFP_KERNEL);
	if (!req->buf) {
		usb_ep_free_request(ep, req);
		return NULL;
	}

	return req;
}

static void request_mtpdroid_free(struct usb_request *req, struct usb_ep *ep)
{
	if (req) {
		kfree(req->buf);
		usb_ep_free_request(ep, req);
	}
}


/* add a request to the tail of a list */
void req_mtpdroid_putList(struct f_mtpdroid_dev *dev, struct list_head *head,
		struct usb_request *req)
{
	unsigned long flags;

	spin_lock_irqsave(&dev->lock, flags);
	list_add_tail(&req->list, head);
	spin_unlock_irqrestore(&dev->lock, flags);
}

/* remove a request from the head of a list */
struct usb_request *req_mtpdroid_getList(struct f_mtpdroid_dev *dev, struct list_head *head)
{
	unsigned long flags;
	struct usb_request *req;

	spin_lock_irqsave(&dev->lock, flags);
	if (list_empty(head)) {
		req = 0;
	} else {
		req = list_first_entry(head, struct usb_request, list);
		list_del(&req->list);
	}
	spin_unlock_irqrestore(&dev->lock, flags);
	return req;
}

static void f_mtpdroid_command_complete(struct usb_ep *ep, struct usb_request *req)
{
	u16 cmd_req;
	u16 *pData;

	pData = (u16 *)req->buf;
	cmd_req = (u16) *(pData);

	if(cmd_req == 0x4001){
	
		g_mtpDroid_Cancel = 1;
		g_iMTP_Status = MTP_BUSY;
		
		dbglog_s("CANCEL [ok ~] CANCEL \n");

	}
	else
	{
		dbglog_s("NOT CANCEL [cmd_req = 0x%x]  \n", cmd_req);
	}

}

static void f_mtpdroid_complete_interrupt(struct usb_ep *ep, struct usb_request *req)
{
	struct f_mtpdroid_dev *dev = __f_mtpdroid_dev;

	if (req->status != 0)
		dev->error = 1;

	req_mtpdroid_putList(dev, &dev->tx_interrupt_idle, req);

	wake_up(&dev->write_interrupt_wq);

}

static void f_mtpdroid_complete_in(struct usb_ep *ep, struct usb_request *req)
{
	struct f_mtpdroid_dev *dev = __f_mtpdroid_dev;

	if (req->status != 0)
		dev->error = 1;

	req_mtpdroid_putList(dev, &dev->tx_idle, req);

	wake_up(&dev->write_wq);

}

static void f_mtpdroid_complete_out(struct usb_ep *ep, struct usb_request *req)
{
	struct f_mtpdroid_dev *dev = __f_mtpdroid_dev;

	if (req->status != 0) {
		dev->error = 1;
		dbglog_r("comp error~~\n");
		req_mtpdroid_putList(dev, &dev->rx_idle, req);
	} else {
		dbglog_r("comp done~~\n");
		req_mtpdroid_putList(dev, &dev->rx_done, req);
	}

	wake_up(&dev->read_wq);

}


static int f_mtpdroid_create_bulk_endpoints(struct f_mtpdroid_dev *dev,
				struct usb_endpoint_descriptor *int_desc,
				struct usb_endpoint_descriptor *in_desc,
				struct usb_endpoint_descriptor *out_desc)
{
	struct usb_composite_dev *cdev = dev->cdev;
	struct usb_request *req;
	struct usb_ep *ep;
	int i;

	dbglog_i("f_mtpdroid_create_bulk_endpoints dev: %p\n", dev);

	ep = usb_ep_autoconfig(cdev->gadget, int_desc);
	if (!ep) {
		dbglog_i("usb_ep_autoconfig for ep_interrupt failed\n");
		return -ENODEV;
	}
	dbglog_i("usb_ep_autoconfig for ep_interrupt got %s\n", ep->name);
	ep->driver_data = dev;
	dev->ep_int = ep;

	ep = usb_ep_autoconfig(cdev->gadget, in_desc);
	if (!ep) {
		dbglog_i("usb_ep_autoconfig for ep_in failed\n");
		return -ENODEV;
	}
	dbglog_i("usb_ep_autoconfig for ep_in got %s\n", ep->name);
	ep->driver_data = dev;
	dev->ep_in = ep;

	ep = usb_ep_autoconfig(cdev->gadget, out_desc);
	if (!ep) {
		dbglog_i("usb_ep_autoconfig for ep_out failed\n");
		return -ENODEV;
	}
	dbglog_i("usb_ep_autoconfig for mtpf ep_out got %s\n", ep->name);
	ep->driver_data = dev;
	dev->ep_out = ep;

	/* now allocate requests for our endpoints */
	for (i = 0; i < RX_REQ_MAX_1; i++) {
		req = request_mtprdroid_new(dev->ep_out, BULK_BUFFER_SIZE_2);
		if (!req)
			goto fail;
		req->complete = f_mtpdroid_complete_out;
		req_mtpdroid_putList(dev, &dev->rx_idle, req);
	}

	for (i = 0; i < TX_REQ_MAX; i++) {
		req = request_mtprdroid_new(dev->ep_in, BULK_BUFFER_SIZE);
		if (!req)
			goto fail;
		req->complete = f_mtpdroid_complete_in;
		req_mtpdroid_putList(dev, &dev->tx_idle, req);
	}
	
	for (i = 0; i < TX_REQ_MAX; i++) {
		req = request_mtprdroid_new(dev->ep_int, INTERRUPT_BUFFER_SIZE);
		if (!req)
			goto fail;
		req->complete = f_mtpdroid_complete_interrupt;
		req_mtpdroid_putList(dev, &dev->tx_interrupt_idle, req);
	}

	return 0;

fail:
	return -1;
}

//===========================================================//
//===========================================================//
static ssize_t imtpf_read(struct file *fp, char __user *buf,
				size_t count, loff_t *pos)
{
	struct f_mtpdroid_dev *dev = fp->private_data;
//..unu	struct usb_composite_dev *cdev = dev->cdev;
	struct usb_request *req;
	struct usb_request *req_save = NULL;
	int r = count, xfer = 0;
	int ret;
//..unu	int getLength = 0;

	dbglog_r("mtpf_read(%d)\n", count);

	if (f_mtpdroid_lock(&dev->read_excl))
		return -EBUSY;

	/* we will block until we're online */
	while (!(dev->online || dev->error)) 
	{
		dbglog_r("mtpf_read: waiting for online state\n");
		ret = wait_event_interruptible(dev->read_wq,
				(dev->online || dev->error));
		if (ret < 0)
		{
			f_mtpdroid_unlock(&dev->read_excl);
			return ret;
		}
	}

	while (count > 0) 
	{
		if(dev->error) 
		{
			dbglog_r("mtpf_read dev->error\n");
			r = -EIO;
			break;
		}
		else if(dev->sendObj_Cancel)
		{
			dbglog_r("mtpf_read dev->sendObj_Cancel\n");
			r = -EIO;
			break;
		}

		/* if we have idle read requests, get them queued */
		while ((req = req_mtpdroid_getList(dev, &dev->rx_idle))) 
		{
requeue_req:
			req->length = BULK_BUFFER_SIZE_2;
			ret = usb_ep_queue(dev->ep_out, req, GFP_ATOMIC);

			if (ret < 0) {
				dbglog_r("rx queue error = %d \n", ret);
				r = -EIO;
				dev->error = 1;
				req_mtpdroid_putList(dev, &dev->rx_idle, req);
				goto fail;
			} else {
				dbglog_r("rx %p queue\n", req);
				req_save = req;
			}
		}

		/* if we have data pending, give it to userspace */
		dbglog_r("rx dev->read_count = %d\n", dev->read_count);

		if (dev->read_count > 0) 
		{
			if (dev->read_count < count)
				xfer = dev->read_count;
			else
				xfer = count;

			if (copy_to_user(buf, dev->read_buf, xfer)) 
			{	
				r = -EFAULT;
				break;
			}
			dev->read_buf += xfer;
			dev->read_count -= xfer;
			buf += xfer;
			count -= xfer;
			
			if (dev->read_count == 0) 
			{
				req_mtpdroid_putList(dev, &dev->rx_idle, dev->read_req);
				dev->read_req = 0;
			}

			continue;
		}	
		else if(xfer == BULK_BUFFER_SIZE_2)
		{

		}
		else if(xfer)
		{
			r -= count;
			break;
		}

dbglog_r("ep queue done~\n");

		/* wait for a request to complete */
		req = 0;

		ret = wait_event_interruptible(dev->read_wq,
			((req = req_mtpdroid_getList(dev, &dev->rx_done)) || dev->error || dev->sendObj_Cancel));
		
		if (req != 0) {
			/* if we got a 0-len one we need to put it back into
			** service.  if we made it the current read req we'd
			** be stuck forever
			*/
			if (req->actual == 0)
				goto requeue_req;

			dev->read_req = req;
			dev->read_count = req->actual;
			dev->read_buf = req->buf;
			dbglog_r("rx %p %d\n", req, req->actual);
		}

		if (ret < 0) {
			r = ret;
			break;
		}

		
	}

fail:
	f_mtpdroid_unlock(&dev->read_excl);
	dbglog_r("mtpf_read returning %d\n", r);
	
		if(dev->sendObj_Cancel){
			r = -500;
		}
	
	return r;
}

static ssize_t imtpf_write(struct file *fp, const char __user *buf,
				 size_t count, loff_t *pos)
{
	struct f_mtpdroid_dev *dev = fp->private_data;
//..unu	struct usb_composite_dev *cdev = dev->cdev;
	struct usb_request *req = 0;
	int r = count, xfer;
	int ret;

	dbglog_w("mtpf_write(%d)\n", count);

	if (f_mtpdroid_lock(&dev->write_excl))
		return -EBUSY;

	while (count > 0) {
		if (dev->error) {
			dbglog_w("mtpf_write dev->error\n");
			r = -EIO;
			break;
		}

		/* get an idle tx request to use */
		req = 0;
		ret = wait_event_interruptible(dev->write_wq,
			((req = req_mtpdroid_getList(dev, &dev->tx_idle)) || dev->error || dev->sendObj_Cancel));

		if (ret < 0) {
			r = ret;
			break;
		}
		
		if (req != 0) {
			if (count > BULK_BUFFER_SIZE)
				xfer = BULK_BUFFER_SIZE;
			else
				xfer = count;
			if (copy_from_user(req->buf, buf, xfer)) {
				r = -EFAULT;
				break;
			}

			req->length = xfer;
			ret = usb_ep_queue(dev->ep_in, req, GFP_ATOMIC);
			if (ret < 0) {
				dbglog_w("mtpf_write: xfer error %d\n", ret);
				dev->error = 1;
				r = -EIO;
				break;
			}

			buf += xfer;
			count -= xfer;

			/* zero this so we don't try to free it on error exit */
			req = 0;
		}
	}

	if (req)
		req_mtpdroid_putList(dev, &dev->tx_idle, req);

	f_mtpdroid_unlock(&dev->write_excl);
	dbglog_w("mtpf_write returning %d\n", r);

	if(dev->sendObj_Cancel){
			r = -500;
	}
		
	return r;
}

static int imtpf_open(struct inode *ip, struct file *fp)
{
	printk("mtpf_open --->> \n");

	if (f_mtpdroid_lock(&__f_mtpdroid_dev->open_excl))
		return -EBUSY;

	fp->private_data = __f_mtpdroid_dev;

	/* clear the error latch */
	__f_mtpdroid_dev->error = 0;

	return 0;
}

static int imtpf_release(struct inode *ip, struct file *fp)
{
	printk("mtpf_release <<--- \n");
	f_mtpdroid_unlock(&__f_mtpdroid_dev->open_excl);
	return 0;
}

#if 1  //..wondo_20110502	
static long  imtpf_ioctl(struct file *fd, unsigned int code, unsigned long arg)
#else
static int imtpf_ioctl(struct inode *ip, struct file *fd, unsigned int code, unsigned long arg)
#endif
{
	struct f_mtpdroid_dev	*dev = fd->private_data;
	struct usb_composite_dev *cdev = dev->cdev;
//..unu	struct usb_request	*req = cdev->req;
//..unu	unsigned long		flags;
	int			status = 0;

	switch(code){
		case MTP_SET_PIPE_INIT :
			atomic_set(&dev->read_excl, 0);
			atomic_set(&dev->write_excl, 0);
			atomic_set(&dev->write_interrupt_excl, 0);
			break;

		case MTP_SET_LIST_INIT :
			INIT_LIST_HEAD(&dev->rx_idle);
			INIT_LIST_HEAD(&dev->rx_done);
			INIT_LIST_HEAD(&dev->tx_idle);
			INIT_LIST_HEAD(&dev->tx_interrupt_idle);
			break;
			
		case MTP_SET_STATUS :
			g_iMTP_Status = (MTP_DEVICE_STATUS)arg;

			if(g_iMTP_Status == MTP_OK)
			{
				if(dev->sendObj_Cancel)
				{
					dev->error = 0;
					dev->sendObj_Cancel = 0;
				}
			}
			dbglog_io("MTP_SET_STATUS = 0x%x \n", g_iMTP_Status);
			break;

		case MTP_GET_STATUS :
			if(g_mtpDroid_Enable)
			{
				ADD_USB_MTP_CMD(USB_MTP_ENABLE);
			}
			else
			{
				DEL_USB_MTP_CMD(USB_MTP_ENABLE);
			}
			if(g_mtpDroid_Cancel) ADD_USB_MTP_CMD(USB_MTP_CANCEL);
			if(g_mtpDroid_Reset) ADD_USB_MTP_CMD(USB_MTP_RESET);

			if(copy_to_user((void *) arg, &g_USB_MTP_CMD, sizeof(g_USB_MTP_CMD)))
			{
				return -EFAULT;
			}

			if(g_mtpDroid_Cancel)
			{
				g_mtpDroid_Cancel = 0;
				DEL_USB_MTP_CMD(USB_MTP_CANCEL);
			}
			if(g_mtpDroid_Reset)
			{
				g_mtpDroid_Reset = 0;
				DEL_USB_MTP_CMD(USB_MTP_RESET);
			}
			break;

		case MTP_END_STATUS :
			dev->online = 0;
			dev->error = 1;
			
			/* readers may be blocked waiting for us to go online */
			wake_up(&dev->read_wq);

			usb_ep_set_halt(dev->ep_out);
			msleep(10);
			usb_ep_clear_halt(dev->ep_out);

			dbglog_io("%s MTP_END_STATUS\n", dev->function.name);
			break;

		case MTP_CLEAR_HALT :
			status = usb_ep_clear_halt (dev->ep_in);
			status = usb_ep_clear_halt (dev->ep_out);
			break;
			
		case MTP_SET_SCRIPT :
			{
				SCRIPT_TYPE k_data;
				SCRIPT_TYPE *Tmp = (SCRIPT_TYPE *)arg;

				if (copy_from_user(&k_data, Tmp, sizeof(SCRIPT_TYPE))) {
					status = -EFAULT;
					break;
				}

				
				dbglog_io("MTP_SET_SCRIPT Mode = %d\n", k_data.Mode);
				switch(k_data.Mode){
					case 1 :
						memcpy(&__f_mtpdroid_dev->A, &k_data, sizeof(SCRIPT_TYPE));
						break;
					case 2 : 
						memcpy(&__f_mtpdroid_dev->B, &k_data, sizeof(SCRIPT_TYPE));
						break;
					case 3 : 
						memcpy(&__f_mtpdroid_dev->C, &k_data, sizeof(SCRIPT_TYPE));
						break;
					case 4 : 
						memcpy(&__f_mtpdroid_dev->D, &k_data, sizeof(SCRIPT_TYPE));
						break;
					case 5 : 
						memcpy(&__f_mtpdroid_dev->E, &k_data, sizeof(SCRIPT_TYPE));
						break;
				}
			}
			break;
			
		default:
			if(cdev->gadget && cdev->gadget->ops->ioctl)
			{
				status = cdev->gadget->ops->ioctl(cdev->gadget, code, arg);
			}
			else
			{
				/* could not handle ioctl */
				dbglog_io("mtpf_ioctl: ERROR cmd=0x%4.4xis not supported\n", code);
				status = -ENOTTY;
			}
			break;
	}

//..??	spin_unlock_irqrestore(&dev->lock, flags);

	return status;
}

/* file operations for ADB device /dev/android_adb */
static struct file_operations f_mtpdroid_fops = {
	.owner = THIS_MODULE,
	.read = imtpf_read,
	.write = imtpf_write,
	.open = imtpf_open,
	.release = imtpf_release,
#if 1  //..wondo_20110502	
	.unlocked_ioctl = imtpf_ioctl,
#else
	.ioctl = imtpf_ioctl,
#endif
};

static struct miscdevice f_mtpdroid_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = shortname,
	.fops = &f_mtpdroid_fops,
};

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

static int f_mtpdroid_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_composite_dev *cdev = c->cdev;
	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
	int			id;
	int			ret;

	dev->cdev = cdev;
	dbglog_i("f_mtpdroid_bind dev: %p\n", dev);

	/* allocate interface ID(s) */
	id = usb_interface_id(c, f);
	if (id < 0)
	  return id;
	
#if FEATURE_MINI_DEVICE  //..wondo_20110502 ?????
	mtpdroid_interface_desc.bInterfaceNumber = id;
#endif

	/* allocate endpoints */
	ret = f_mtpdroid_create_bulk_endpoints(dev, 
							&fs_mtpdroid_interrupt_desc,
							&fs_mtpdroid_in_desc,
							&fs_mtpdroid_out_desc);
	if (ret)
		return ret;

	/* support high speed hardware */
	if (gadget_is_dualspeed(c->cdev->gadget)) 
	{
		hs_mtpdroid_interrupt_desc.bEndpointAddress = fs_mtpdroid_interrupt_desc.bEndpointAddress;
		hs_mtpdroid_in_desc.bEndpointAddress = fs_mtpdroid_in_desc.bEndpointAddress;
		hs_mtpdroid_out_desc.bEndpointAddress = fs_mtpdroid_out_desc.bEndpointAddress;
	}


	dbglog_i("%s speed %s: INT/%s, IN/%s, OUT/%s\n",
			gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full", f->name, 
			dev->ep_int->name, dev->ep_in->name, dev->ep_out->name);
	return 0;
}

static void f_mtpdroid_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
	struct usb_request *req;

	dbglog_i("f_mtpdroid_unbind dev: %x, usb_f : %x \n", (int)dev, (int)f);
	spin_lock_irq(&dev->lock);

	while ((req = req_mtpdroid_getList(dev, &dev->rx_idle)))
		request_mtpdroid_free(req, dev->ep_out);
	while ((req = req_mtpdroid_getList(dev, &dev->tx_idle)))
		request_mtpdroid_free(req, dev->ep_in);
	while ((req = req_mtpdroid_getList(dev, &dev->tx_interrupt_idle)))
		request_mtpdroid_free(req, dev->ep_int);

	dev->online = 0;
	dev->error = 1;
	spin_unlock_irq(&dev->lock);


//..#if MTP_UNUSED_CODE
#if FEATURE_MINI_DEVICE  //..wondo_20110502 ?????
	misc_deregister(&f_mtpdroid_device);
	kfree(__f_mtpdroid_dev);
	__f_mtpdroid_dev = NULL;
#endif	
}
 
static int f_mtpdroid_set_alt(struct usb_function *f,
		unsigned intf, unsigned alt)
{
	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_ep *ep;
	int ret = -EOPNOTSUPP;


	dbglog_s("f_mtpdroid_set_alt intf: %d alt: %d\n", intf, alt);

	ret = usb_ep_enable(dev->ep_int,
			ep_choose(cdev->gadget,
				&hs_mtpdroid_interrupt_desc,
				&fs_mtpdroid_interrupt_desc));
	if (ret)
	{
		ep = dev->ep_int;
		dbglog_s("can't enable %s, result %d\n", ep->name, ret);
		return ret;
	}
	
	ret = usb_ep_enable(dev->ep_in,
			ep_choose(cdev->gadget,
				&hs_mtpdroid_in_desc,
				&fs_mtpdroid_in_desc));
	if (ret)
	{
		ep = dev->ep_in;
		dbglog_s("can't enable %s, result %d\n", ep->name, ret);
		usb_ep_disable(dev->ep_int);
		return ret;
	}
	
	ret = usb_ep_enable(dev->ep_out,
			ep_choose(cdev->gadget,
				&hs_mtpdroid_out_desc,
				&fs_mtpdroid_out_desc));
	if (ret) {
		ep = dev->ep_in;
		dbglog_s("can't enable %s, result %d\n", ep->name, ret);
		
		usb_ep_disable(dev->ep_int);
		usb_ep_disable(dev->ep_in);
		return ret;
	}
	dev->state = UM_ACTIVE;
	dev->online = 1;
	dev->error = 0;
	dev->suspend = 0;

	g_mtpDroid_Enable = 1;

	/* readers may be blocked waiting for us to go online */
	wake_up(&dev->read_wq);
	
	return ret;
}

static int f_mtpdroid_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{

	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
	struct usb_composite_dev *cdev = dev->cdev;
	struct usb_request	*req = cdev->req;

	int			value = -EOPNOTSUPP;
	u16			wIndex = le16_to_cpu(ctrl->wIndex);
	u16			wValue = le16_to_cpu(ctrl->wValue);
	u16			wLength = le16_to_cpu(ctrl->wLength);

	dbglog_s("ctrl req%02x.%02x v%04x i%04x l%d ",
		ctrl->bRequestType, ctrl->bRequest, wValue, wIndex, wLength);

	switch (ctrl->bRequestType&USB_TYPE_MASK){

	case USB_TYPE_STANDARD:
		dbglog_s("USB_TYPE_STANDARD----->\n");
		switch (ctrl->bRequest) {

		case USB_REQ_GET_DESCRIPTOR:
			if (ctrl->bRequestType != USB_DIR_IN)
				break;
			switch (wValue >> 8) {

			case USB_DT_STRING:
				{
					int id;

					id = wValue & 0xff;
					dbglog_s("String index = 0x%x", id);

					if(id == 0xEE)
					{
						value = min(wLength,
								(u16) __f_mtpdroid_dev->B.Len);
						memcpy(req->buf, __f_mtpdroid_dev->B.Buff, value);
					}
					else
					{
						value = usb_gadget_get_string(&stringtab, id, req->buf);
						if (value >= 0)
							value = min(wLength, (u16) value);
					}
				}
				break;
			}
			break;

		default:
			goto unknown;
		}
		break;

	case USB_TYPE_CLASS:
		dbglog_s("USB_TYPE_CLASS[ctrl->bRequest = 0x%x]----->\n", ctrl->bRequest);
		switch (ctrl->bRequest) {
		case 0x64 :
			if(wLength == 0x06
				&& wIndex == 0 
				&& wValue == 0)
			{

				value = wLength;
				dev->sendObj_Cancel = 1;

				req->complete = f_mtpdroid_command_complete;
				req->context = dev;

				wake_up(&dev->read_wq);  //..wondo ttt
			}
			break;
			
		case 0x66 :
			dbglog_s("RESET [ok ~] RESET \n");
			if(wLength == 0
				&& wIndex == 0 
				&& wValue == 0)
			{
				g_mtpDroid_Reset = 1;
				g_iMTP_Status = MTP_OK;
				value = 0;
			}

			break;

		case 0x67 :  // USBDC_PTP_REQUEST_GETSTATUS 0x67A1
			switch(g_iMTP_Status){
				case MTP_OK :
					value = min(wLength,
							(u16) __f_mtpdroid_dev->C.Len);
					memcpy(req->buf, __f_mtpdroid_dev->C.Buff, value);
					break;
					
				case MTP_BUSY :
					value = min(wLength,
							(u16) __f_mtpdroid_dev->D.Len);
					memcpy(req->buf, __f_mtpdroid_dev->D.Buff, value);
					break;
					
				case MTP_CANCEL :
					value = min(wLength,
							(u16) __f_mtpdroid_dev->E.Len);
					memcpy(req->buf, __f_mtpdroid_dev->E.Buff, value);
					break;
					
				default :
					dbglog_s("unknown MTP_Status");
					break;
			}
			break;
/*-------------------------------------------------------------*/			
		default:
			goto unknown;
		}
		
#if FEATURE_SETUP_RESPONSE
		/* respond with data transfer or status phase? */
		if (value >= 0) {
			int rc;
			cdev->req->zero = value < wLength;
			cdev->req->length = value;
			rc = usb_ep_queue(cdev->gadget->ep0, cdev->req, GFP_ATOMIC);
			if (rc < 0)
				ERROR(cdev, "%s setup response queue error\n", __func__);
		}
#endif
		
		break;

	case USB_TYPE_VENDOR:
		dbglog_s("USB_TYPE_VENDOR----->\n");
		switch (ctrl->bRequest) {
			case 1: 
				value = min(wLength,
						(u16) __f_mtpdroid_dev->A.Len);
				memcpy(req->buf, __f_mtpdroid_dev->A.Buff, value);
				break;
			default:
				goto unknown;
		}

#if FEATURE_SETUP_RESPONSE
		/* respond with data transfer or status phase? */
		if (value >= 0) {
			int rc;
			cdev->req->zero = value < wLength;
			cdev->req->length = value;
			rc = usb_ep_queue(cdev->gadget->ep0, cdev->req, GFP_ATOMIC);
			if (rc < 0)
				ERROR(cdev, "%s setup response queue error\n", __func__);
		}
#endif
		
		break;

	case USB_TYPE_RESERVED:
		dbglog_s("USB_TYPE_RESERVED----->\n");
		break;
		
	default:
unknown:
		dbglog_s("unknown ctrl req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest, wValue, wIndex, wLength);
		break;
	}

	return value;
}

static void f_mtpdroid_disable(struct usb_function *f)
{
	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
//..unu	struct usb_composite_dev	*cdev = dev->cdev;

	dbglog_i("f_mtpdroid_disable\n");
	dev->online = 0;
	dev->error = 1;
	dev->config = 0;

	dev->interface = 0;
	dev->state = UM_DEACTIVE;
	g_mtpDroid_Enable = 0;

	usb_ep_fifo_flush(dev->ep_int);
	usb_ep_fifo_flush(dev->ep_in);
	usb_ep_fifo_flush(dev->ep_out);

	usb_ep_disable(dev->ep_int);
	usb_ep_disable(dev->ep_in);
	usb_ep_disable(dev->ep_out);

	/* readers may be blocked waiting for us to go online */
	wake_up(&dev->read_wq);

	dbglog_i("%s disabled\n", dev->function.name);
}


static void f_mtpdroid_suspend(struct usb_function *f)
{
 
	struct f_mtpdroid_dev	*dev = f_mtpdroid_to_dev(f);
//..unu	struct usb_request *req;

	dbglog_i("f_mtpdroid_suspend\n");
	dev->online = 0;
	dev->error = 1;
//..	dev->config = 0;
	dev->interface = 0;  //..wondo.2010.12.28
	dev->suspend = 1;
	dev->state = UM_DEACTIVE;

//..Qualcomm 	mtp_set_func(NULL);  //..wondo 2010/09/27
	g_mtpDroid_Enable = 0;

	/* readers may be blocked waiting for us to go online */
	wake_up(&dev->read_wq);


	dbglog_i("%s f_mtpdroid_suspend\n", dev->function.name);
}


//===========================================================//
//===========================================================//
//..int insight_mtp_function_init(void)
static int mtpdroid_bind_config(struct usb_configuration *c)
{
	struct f_mtpdroid_dev *dev;
	int status;
	int ret;
	
	dbglog_i("mtpdroid_bind_config\n");

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

#if FEATURE_MINI_DEVICE  //..wondo.20110209
	if (f_mtpdroid_strings[STRING_MANUFACTURER_IDX].id == 0) {
		status = usb_string_id(c->cdev);
		if (status < 0) {
			kfree(dev);
			return status;
		}
		f_mtpdroid_strings[STRING_MANUFACTURER_IDX].id = status;
		mtpdroid_interface_desc.iInterface = status;
	}
#else
	mtpdroid_interface_desc.iInterface = 0;
#endif

	spin_lock_init(&dev->lock);

	init_waitqueue_head(&dev->read_wq);
	init_waitqueue_head(&dev->write_wq);
	init_waitqueue_head(&dev->write_interrupt_wq);

	atomic_set(&dev->open_excl, 0);
	atomic_set(&dev->read_excl, 0);
	atomic_set(&dev->write_excl, 0);
	atomic_set(&dev->write_interrupt_excl, 0);

	INIT_LIST_HEAD(&dev->rx_idle);
	INIT_LIST_HEAD(&dev->rx_done);
	INIT_LIST_HEAD(&dev->tx_idle);
	INIT_LIST_HEAD(&dev->tx_interrupt_idle);

	dev->cdev = c->cdev;
	dev->suspend = 0;
	dev->state = UM_NONE;
	
	dev->function.name = DRIVER_NAME;
	dev->function.strings = dev_strings;
	
	dev->function.descriptors = fs_mtpdroid_descs;
	dev->function.hs_descriptors = hs_mtpdroid_descs;

	dev->function.bind = f_mtpdroid_bind;
	dev->function.unbind = f_mtpdroid_unbind;
	dev->function.set_alt = f_mtpdroid_set_alt;
	dev->function.disable = f_mtpdroid_disable;
	dev->function.setup = f_mtpdroid_setup;
	dev->function.suspend = f_mtpdroid_suspend;

//_____________________________________________________
	dev->sendObj_Cancel = 0;

//_____________________________________________________

	/* __f_mtpdroid_dev must be set before calling usb_gadget_register_driver */
	__f_mtpdroid_dev = dev;

	ret = misc_register(&f_mtpdroid_device);
	if (ret)
		goto err1;

	ret = usb_add_function(c, &dev->function);
	if (ret)
		goto err3;

	f_mtpdroid_set_func(&__f_mtpdroid_dev->function);  // ++
	
	return 0;

err3:
	misc_deregister(&f_mtpdroid_device);
	
err1:
	kfree(dev);
	dbglog_i("mtpf gadget driver failed to initialize\n");
	return ret;
}

static struct android_usb_function mtpdroid_function = {
	.name = DRIVER_NAME,
	.bind_config = mtpdroid_bind_config,
};

static int __init init(void)
{
	dbglog_i("f_mtpDroid init\n");
	android_register_function(&mtpdroid_function);
	return 0;
}
module_init(init);

