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




#include <linux/usb/ch9.h>
#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>
#include <linux/usb/android_composite.h>

#if defined(CONFIG_HSUSB_PANTECH_USB_TEST)
#include "pantech_loopback.h"
#endif

#define BULK_BUFFER_SIZE 4096

/* number of rx and tx requests to allocate */
#define RX_REQ_MAX 4
#define TX_REQ_MAX 4

//#define DEBUG_OBEX
#ifdef DEBUG_OBEX
#define PRINTK_OBEX(fmt, args...) \
	printk(KERN_ERR "OBEX: "fmt, ## args);
#else
#define PRINTK_OBEX(fmt, args...)
#endif

struct obex_dev
{
  struct usb_function function;
	struct usb_composite_dev *cdev;

	spinlock_t lock;

	int online;
	int error;

	atomic_t read_excl;
	atomic_t write_excl;
	atomic_t open_excl;

	struct usb_ep *ep_out;
	struct usb_ep *ep_in;

	struct list_head tx_idle;
	struct list_head rx_idle;
	struct list_head rx_done;

	wait_queue_head_t read_wq;
	wait_queue_head_t write_wq;

	/* the request we're currently reading from */
	struct usb_request *read_req;
	unsigned char *read_buf;
	unsigned read_count;
	unsigned bound;
};


static struct usb_interface_descriptor obex_cdc_intf_desc = {
	.bLength =		sizeof obex_cdc_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bNumEndpoints =	0,
	.bInterfaceClass =	0x02,
	.bInterfaceSubClass =	0x0B,
	.bInterfaceProtocol =	0x00,
};

static struct usb_interface_descriptor obex_data_intf_desc = {
	.bLength =		sizeof obex_data_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bNumEndpoints =	2,
	.bInterfaceClass =	0x0A,
	.bInterfaceSubClass =	0x00,
	.bInterfaceProtocol =	0x01,
};

static struct usb_endpoint_descriptor hs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
	.bInterval =		0,
};
static struct usb_endpoint_descriptor fs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(64),
	.bInterval =		0,
};

static struct usb_endpoint_descriptor hs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
	.bInterval =		0,
};

static struct usb_endpoint_descriptor fs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(64),
	.bInterval =		0,
};

static struct usb_descriptor_header *fs_obex_descs[] = {
  (struct usb_descriptor_header *) &obex_cdc_intf_desc,
  (struct usb_descriptor_header *) &obex_data_intf_desc,
  (struct usb_descriptor_header *) &fs_bulk_in_desc,
  (struct usb_descriptor_header *) &fs_bulk_out_desc,
  NULL,
};

static struct usb_descriptor_header *hs_obex_descs[] = {
  (struct usb_descriptor_header *) &obex_cdc_intf_desc,
  (struct usb_descriptor_header *) &obex_data_intf_desc,
  (struct usb_descriptor_header *) &hs_bulk_in_desc,
  (struct usb_descriptor_header *) &hs_bulk_out_desc,
  NULL,
};

static struct obex_dev *_obex_dev;

static inline struct obex_dev *func_to_dev(struct usb_function *f)
{
  return container_of(f, struct obex_dev, function);
}

static struct usb_request *obex_request_new(struct usb_ep *ep, int buffer_size)
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

static void obex_request_free(struct usb_request *req, struct usb_ep *ep)
{
  if (req) {
    kfree(req->buf);
    usb_ep_free_request(ep, req);
  }
}

static inline int _lock(atomic_t *excl)
{
	if (atomic_inc_return(excl) == 1) {
		return 0;
	} else {
		atomic_dec(excl);
		return -1;
	}
}

static inline void _unlock(atomic_t *excl)
{
	atomic_dec(excl);
}

/* add a request to the tail of a list */
static void req_put(struct obex_dev *dev, struct list_head *head, struct usb_request *req)
{
	unsigned long flags;

	spin_lock_irqsave(&dev->lock, flags);
	list_add_tail(&req->list, head);
	spin_unlock_irqrestore(&dev->lock, flags);
}

/* remove a request from the head of a list */
static struct usb_request *req_get(struct obex_dev *dev, struct list_head *head)
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

static void obex_complete_in(struct usb_ep *ept, struct usb_request *req)
{
	struct obex_dev *dev = _obex_dev;

	if (req->status != 0)
		dev->error = 1;

	req_put(dev, &dev->tx_idle, req);

	wake_up(&dev->write_wq);
}

static void obex_complete_out(struct usb_ep *ept, struct usb_request *req)
{
	struct obex_dev *dev = _obex_dev;

#if defined(CONFIG_HSUSB_PANTECH_USB_TEST)
	if((pantech_usb_test_get_run() == 1) 
	  &&	(pantech_usb_test_get_open() == 1) 
	  && (pantech_usb_test_get_port_type() == 2))
	{
		//printk("obex_loopback_read size[%d] = count[%d]\n", req->length, req->actual);

		if(req->status == 0) { 
			if(req->actual == 0) { 
			  pantech_usb_test_send_loopback((void*)dev, dev->ep_in);
			} else if(req->actual < req->length) {
				pantech_usb_test_copy_buffer(req->buf, req->actual);
			  pantech_usb_test_send_loopback((void*)dev, dev->ep_in);
			} else {
				pantech_usb_test_copy_buffer(req->buf, req->actual);
			}
		}
		usb_ep_queue(dev->ep_out, req, GFP_ATOMIC);

		return;	
	}
#endif

	if (req->status != 0) {
		dev->error = 1;
		req_put(dev, &dev->rx_idle, req);
	} else {
		req_put(dev, &dev->rx_done, req);
	}

	wake_up(&dev->read_wq);
}

static int create_bulk_endpoints(struct obex_dev *dev,
        struct usb_endpoint_descriptor *in_desc,
        struct usb_endpoint_descriptor *out_desc)
{
  struct usb_composite_dev *cdev = dev->cdev;
  struct usb_request *req;
  struct usb_ep *ep;
  int i;
  ep = usb_ep_autoconfig(cdev->gadget, in_desc);
  if (!ep) {
    //DBG(cdev, "usb_ep_autoconfig for ep_in failed\n");
    return -ENODEV;
  }
  //DBG(cdev, "usb_ep_autoconfig for ep_in got %s\n", ep->name);
  dev->ep_in = ep;

  ep = usb_ep_autoconfig(cdev->gadget, out_desc);
  if (!ep) {
    //DBG(cdev, "usb_ep_autoconfig for ep_out failed\n");
    return -ENODEV;
  }
  //DBG(cdev, "usb_ep_autoconfig for obex ep_out got %s\n", ep->name);
  dev->ep_out = ep;

  /* now allocate requests for our endpoints */
  for (i = 0; i < RX_REQ_MAX; i++) {
    req = obex_request_new(dev->ep_out, BULK_BUFFER_SIZE);
    if (!req)
      goto fail;
    req->complete = obex_complete_out;
    req_put(dev, &dev->rx_idle, req);
  }

  for (i = 0; i < TX_REQ_MAX; i++) {
    req = obex_request_new(dev->ep_in, BULK_BUFFER_SIZE);
    if (!req)
      goto fail;
    req->complete = obex_complete_in;
    req_put(dev, &dev->tx_idle, req);
  }

  return 0;

fail:
  PRINTK_OBEX("obex_bind() could not allocate requests\n");
  return -1;
}


static ssize_t obex_read(struct file *fp, char __user *buf,
			size_t count, loff_t *pos)
{
	struct obex_dev *dev =  fp->private_data;
	struct usb_request *req;
	int r = count, xfer;
	int ret;
	int sended_count;

  PRINTK_OBEX("obex_read (%x)(%d)\n", (uint32_t)dev, atomic_read(&dev->read_excl));

	if (!dev)
		return -EIO;

	if (_lock(&dev->read_excl))
		return -EBUSY;

	/* we will block until we're online */
	while (!(dev->online || dev->error)) {
		ret = wait_event_interruptible(dev->read_wq, (dev->online || dev->error));
		if (ret < 0) {
			_unlock(&dev->read_excl);
			return ret;
		}
	}

	sended_count = 0;

	while (count > 0) {
		if (dev->error) {
			r = -EIO;
			break;
		}

		/* if we have idle read requests, get them queued */
		while ((req = req_get(dev, &dev->rx_idle))) {
			req->length = BULK_BUFFER_SIZE;
			ret = usb_ep_queue(dev->ep_out, req, GFP_ATOMIC); 

			if (ret < 0) {
				//DBG("obex_read: failed to queue req %p (%d)\n", req, ret);
				r = -EIO;
				dev->error = 1;
				req_put(dev, &dev->rx_idle, req);
				goto fail;
			} else {
				//DBG("rx %p queue\n", req);
			}
		}


		/* if we have data pending, give it to userspace */
		if (dev->read_count > 0) {
			xfer = (dev->read_count < count) ? dev->read_count : count;

			if (copy_to_user(buf, dev->read_buf, xfer)) {
				r = -EFAULT;
				break;
			}
			dev->read_buf += xfer;
			dev->read_count -= xfer;
			buf += xfer;
			count -= xfer;

			sended_count += xfer;

			/* if we've emptied the buffer, release the request */
			if (dev->read_count == 0) {
				req_put(dev, &dev->rx_idle, dev->read_req);
				dev->read_req = 0;
			}
			continue;
		}

		/* wait for a request to complete */
		req = 0;
		ret = wait_event_interruptible(dev->read_wq,
					       ((req = req_get(dev, &dev->rx_done)) || dev->error || (fp->f_flags & O_NONBLOCK) ));

		if ((req == 0) && (fp->f_flags & O_NONBLOCK))
		{
			r = (sended_count > 0)? sended_count : -EAGAIN;
			break;
		}

		if (req != 0) {
			/* if we got a 0-len one we need to put it back into
			** service.  if we made it the current read req we'd
			** be return to userspace
			*/
			if (req->actual == 0){ //End Of Packet
				req->length = BULK_BUFFER_SIZE;
				ret = usb_ep_queue(dev->ep_out, req, GFP_ATOMIC);
				if (ret < 0) {
					//DBG("obex_read: failed to queue req %p (%d)\n", req, ret);
					r = -EIO;
					dev->error = 1;
					req_put(dev, &dev->rx_idle, req);
					goto fail;
				} else {
					//DBG("rx %p queue\n", req);
					r = (sended_count > 0)? sended_count : 0;
					break;
				}
			}

			dev->read_req = req;
			dev->read_count = req->actual;
			dev->read_buf = req->buf;
		}

		if (ret < 0) {
			r = ret;
			break;
		}
	}

fail:
	_unlock(&dev->read_excl);
	return r;
}

static ssize_t obex_write(struct file *fp, const char __user *buf,
			 size_t count, loff_t *pos)
{
	struct obex_dev *dev = fp->private_data;
	struct usb_request *req = 0;
	int r = count, xfer;
	int all_count = count;
	int ret;

  PRINTK_OBEX("obex_write (%x)(%d)(%d)\n", (uint32_t)dev, dev->online, atomic_read(&dev->write_excl));

	if(!dev || !dev->online)
		return -EIO;

	if (_lock(&dev->write_excl))
		return -EBUSY;

	while (count > 0) {
		if (dev->error) {
			r = -EIO;
			break;
		}

		/* get an idle tx request to use */
		req = 0;
		ret = wait_event_interruptible(dev->write_wq,
					       ((req = req_get(dev, &dev->tx_idle)) || dev->error));

		if (ret < 0) {
			r = ret;
			break;
		}

		if (req != 0) {
			xfer = count > BULK_BUFFER_SIZE ? BULK_BUFFER_SIZE : count;
			if (copy_from_user(req->buf, buf, xfer)) {
				r = -EFAULT;
				break;
			}

			req->length = xfer;
			ret = usb_ep_queue(dev->ep_in, req, GFP_ATOMIC);
			if (ret < 0) {
				//DBG("obex_write: xfer error %d\n", ret);
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
		req_put(dev, &dev->tx_idle, req);

	/* send 0-length packet */
	if(all_count > dev->ep_in->maxpacket && (all_count % dev->ep_in->maxpacket) == 0) {
		req = 0;
		ret = wait_event_interruptible(dev->write_wq,
					       ((req = req_get(dev, &dev->tx_idle)) || dev->error));

		if (ret >= 0) {
			if (req != 0) {
				req->length = 0;
			  usb_ep_queue(dev->ep_in, req, GFP_ATOMIC);
				req = 0;
			}
		}
	}

	_unlock(&dev->write_excl);
	return r;
}

static unsigned int obex_poll (struct file *fp, struct poll_table_struct *wait)
{
	struct obex_dev *dev = fp->private_data;
	struct usb_request *req;
	int ret;

	//DBG("obex_poll\n");

	/* read_exec is shared with poll&read */
  PRINTK_OBEX("obex_poll (%x)(%d)(%d)(%d)\n", (uint32_t)dev, dev->online, dev->error, atomic_read(&dev->read_excl));

	if (_lock(&dev->read_excl))
		return -EBUSY;

	if(!dev->online || dev->error) {
		PRINTK_OBEX("obex_poll (%d) (%d)\n", dev->online, dev->error);
		//DBG("obex_poll: It's not online state\n");
		_unlock(&dev->read_excl);
		return POLLERR | POLLHUP;
	}


	if(dev->read_count > 0) {
		PRINTK_OBEX("obex_poll read(%d)\n", dev->read_count);
		_unlock(&dev->read_excl);
		return POLLIN | POLLRDNORM;
	}

	if(!list_empty(&dev->rx_idle) && (req = req_get(dev, &dev->rx_idle))) 
	{
		req->length = BULK_BUFFER_SIZE;
		ret = usb_ep_queue(dev->ep_out, req, GFP_ATOMIC);
		if (ret < 0) {
			//DBG("obex_poll:xfer error %d\n", ret);
			PRINTK_OBEX("obex_poll xfer error(%d)\n", ret);
			dev->error = 1;
			req_put(dev, &dev->rx_idle, req);
			_unlock(&dev->read_excl);
		  return -EIO;
		}
	}

	poll_wait(fp, &dev->read_wq, wait);

	if(!list_empty(&dev->rx_done)) {
		PRINTK_OBEX("obex_poll POLLIN\n");
		_unlock(&dev->read_excl);
		return POLLIN | POLLRDNORM;
	}

	_unlock(&dev->read_excl);
	return 0;
}

static int obex_open(struct inode *ip, struct file *fp)
{
	struct obex_dev *dev = _obex_dev;
	struct usb_request *req;

	//DBG("obex_open\n");
  PRINTK_OBEX("obex_open (%x)(%d)\n", (uint32_t)dev, atomic_read(&dev->open_excl));

	if (!dev)
		return -EIO;

	if (_lock(&dev->open_excl))
		return -EBUSY;

	fp->private_data = dev;
	
	/* clear the error latch */
	dev->error = 0;

	/* if we have a stale request being read, recycle it */
	dev->read_buf = 0;
	dev->read_count = 0;
	if (dev->read_req) {
		req_put(dev, &dev->rx_idle, dev->read_req);
		dev->read_req = 0;
	}

	/* retire any completed rx requests from previous session */
	while ((req = req_get(dev, &dev->rx_done)))
		req_put(dev, &dev->rx_idle, req);

	return 0;
}

static int obex_release(struct inode *ip, struct file *fp)
{
	struct obex_dev *dev = fp->private_data;
//	struct obex_dev *dev = _obex_dev;
	struct usb_request *req;

  PRINTK_OBEX("obex_release (%x)\n", (uint32_t)dev);

	if (!dev) {
		return -EIO;
	}

	//DBG("obex_release\n");

	/* if we have a stale request being read, recycle it */
	dev->read_buf = 0;
	dev->read_count = 0;
	if (dev->read_req) {
		req_put(dev, &dev->rx_idle, dev->read_req);
		dev->read_req = 0;
	}

	/* retire any completed rx requests from previous session */
	while ((req = req_get(dev, &dev->rx_done)))
		req_put(dev, &dev->rx_idle, req);
	
	dev->error = 1;
	wake_up(&dev->read_wq);

	_unlock(&dev->open_excl);
	return 0;
}

static struct file_operations obex_fops = {
	.owner =   THIS_MODULE,
	.read =    obex_read,
	.write =   obex_write,
	.poll =    obex_poll,
	.open =    obex_open,
	.release = obex_release,
};

static struct miscdevice obex_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "obex",
	.fops = &obex_fops,
};

static int obex_function_bind(struct usb_configuration *c, struct usb_function *f)
{
  struct usb_composite_dev *cdev = c->cdev;
  struct obex_dev  *dev = func_to_dev(f);
	int id;
	int ret;

  dev->cdev = cdev;
  //DBG(cdev, "obex_function_bind dev: %p\n", dev);

  /* allocate interface ID(s) */
  id = usb_interface_id(c, f);
  if (id < 0)
    return id;
  obex_cdc_intf_desc.bInterfaceNumber = id;

  id = usb_interface_id(c, f);
  if (id < 0)
    return id;
  obex_data_intf_desc.bInterfaceNumber = id;

  /* allocate endpoints */
  ret = create_bulk_endpoints(dev, &fs_bulk_in_desc,
      &fs_bulk_out_desc);
  if (ret)
    return ret;

  dev->ep_in->driver_data = cdev;
  dev->ep_out->driver_data = cdev;

  /* support high speed hardware */
  if (gadget_is_dualspeed(c->cdev->gadget)) {
    hs_bulk_in_desc.bEndpointAddress =
      fs_bulk_in_desc.bEndpointAddress;
    hs_bulk_out_desc.bEndpointAddress =
      fs_bulk_out_desc.bEndpointAddress;
  }

  //DBG(cdev, "%s speed %s: IN/%s, OUT/%s\n", gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full", f->name, dev->ep_in->name, dev->ep_out->name);

	dev->bound = 1;
	return 0;
}

static void obex_function_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct obex_dev *dev = func_to_dev(f);
	struct usb_request *req;

	if (!dev->bound)
		return;

	while ((req = req_get(dev, &dev->rx_idle))) {
		obex_request_free(req, dev->ep_out);
	}
	while ((req = req_get(dev, &dev->tx_idle))) {
		obex_request_free(req, dev->ep_in);
	}

	spin_lock_irq(&dev->lock);
	dev->online = 0;
	dev->error = 1;
	spin_unlock_irq(&dev->lock);

	dev->bound = 0;

#ifdef xsemiyas_mutex_lock_problem
	misc_deregister(&obex_device);
	kfree(_obex_dev);
	_obex_dev = NULL;
#endif

}


static int obex_function_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
  struct obex_dev  *dev = func_to_dev(f);
  struct usb_composite_dev *cdev = f->config->cdev;
  int ret;

  //DBG(cdev, "obex_function_set_alt intf: %d alt: %d\n", intf, alt);
  ret = usb_ep_enable(dev->ep_in,
      ep_choose(cdev->gadget,
        &hs_bulk_in_desc,
        &fs_bulk_in_desc));
  if (ret)
    return ret;
  ret = usb_ep_enable(dev->ep_out,
      ep_choose(cdev->gadget,
        &hs_bulk_out_desc,
        &fs_bulk_out_desc));
  if (ret) {
    usb_ep_disable(dev->ep_in);
    return ret;
  }
  dev->online = 1;

  /* readers may be blocked waiting for us to go online */
  wake_up(&dev->read_wq);
  return 0;
}

static void obex_function_disable(struct usb_function *f)
{
  struct obex_dev  *dev = func_to_dev(f);

  //DBG(cdev, "obex_function_disable\n");
  dev->online = 0;
  dev->error = 1;
  usb_ep_fifo_flush(dev->ep_in);
  usb_ep_fifo_flush(dev->ep_out);
  usb_ep_disable(dev->ep_in);
  usb_ep_disable(dev->ep_out);

  /* readers may be blocked waiting for us to go online */
  wake_up(&dev->read_wq);

  //DBG(cdev, "%s disabled\n", dev->function.name);
}


#if defined(CONFIG_HSUSB_PANTECH_USB_TEST)
void obex_loopback_open(void)
{
	struct obex_dev *dev = _obex_dev;
	struct usb_request *req;
	int ret;

	if (atomic_read(&dev->open_excl) != 0)
		return ;

	/* clear the error latch */
	dev->error = 0;

	/* if we have a stale request being read, recycle it */
	dev->read_buf = 0;
	dev->read_count = 0;
	if (dev->read_req) {
		req_put(dev, &dev->rx_idle, dev->read_req);
		dev->read_req = 0;
	}

	/* retire any completed rx requests from previous session */
	while ((req = req_get(dev, &dev->rx_done)))
		req_put(dev, &dev->rx_idle, req);

	while ((req = req_get(dev, &dev->rx_idle))) {
		req->length = BULK_BUFFER_SIZE;
		ret = usb_ep_queue(dev->ep_out, req, GFP_ATOMIC);
		if(ret < 0) {
			req_put(dev, &dev->rx_idle, req);
		}
	}

}

void obex_loopback_close(void)
{
	struct obex_dev *dev = _obex_dev;
	struct usb_request *req;

	/* if we have a stale request being read, recycle it */
	dev->read_buf = 0;
	dev->read_count = 0;
	if (dev->read_req) {
		req_put(dev, &dev->rx_idle, dev->read_req);
		dev->read_req = 0;
	}

	/* retire any completed rx requests from previous session */
	while ((req = req_get(dev, &dev->rx_done)))
		req_put(dev, &dev->rx_idle, req);
	
	dev->error = 1;
}
#endif

int pantech_obex_function_add(struct usb_composite_dev *cdev,
  struct usb_configuration *c)
{
  printk(KERN_INFO "obex_function_add\n");
	
	if(_obex_dev)
		return usb_add_function(c, &_obex_dev->function);
	else
		return 0;
}


static int pantech_obex_bind_config(struct usb_configuration *c)
{
  struct obex_dev *dev;
  int ret;

  printk(KERN_ERR "pantech_obex_function_init\n");

  dev = kzalloc(sizeof(*dev), GFP_KERNEL);
  if (!dev)
    return -ENOMEM;

  spin_lock_init(&dev->lock);

  init_waitqueue_head(&dev->read_wq);
  init_waitqueue_head(&dev->write_wq);

  atomic_set(&dev->open_excl, 0);
  atomic_set(&dev->read_excl, 0);
  atomic_set(&dev->write_excl, 0);

  INIT_LIST_HEAD(&dev->rx_idle);
  INIT_LIST_HEAD(&dev->rx_done);
  INIT_LIST_HEAD(&dev->tx_idle);

  dev->function.name = "obex";
  dev->function.descriptors = fs_obex_descs;
  dev->function.hs_descriptors = hs_obex_descs;
  dev->function.bind = obex_function_bind;
  dev->function.unbind = obex_function_unbind;
  dev->function.set_alt = obex_function_set_alt;
  dev->function.disable = obex_function_disable;

  /* _obex_dev must be set before calling usb_gadget_register_driver */
  _obex_dev = dev;

  ret = misc_register(&obex_device);
  if (ret){
		printk(KERN_ERR "obex gadget driver failed to register misc\n");
    goto err1;
	}

  //msy_usb 
	ret = usb_add_function(c, &dev->function);
	if (ret)
		goto err2;

  return 0;

err2:
	misc_deregister(&obex_device);
err1:
  misc_deregister(&obex_device);
  kfree(dev);
  PRINTK_OBEX("obex gadget driver failed to initialize\n");
  return ret;
}


static struct android_usb_function pantech_obex_function = {
	.name = "obex",
  .bind_config = pantech_obex_bind_config,
};


static int __init init(void)
{
	printk(KERN_INFO "f_obex init\n");
  android_register_function(&pantech_obex_function);
	return 0;
}
module_init(init);
