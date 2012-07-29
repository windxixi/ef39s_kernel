/* arch/arm/mach-msm/sdio_vt.c
 *    FEATURE_PANTECH_VT_SUPPORT
 *
 * SDIO sdio_vt Driver -- Provides a sdio_vt SDIO port interface.
 *
 */
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <mach/sdio_al.h>
#include "modem_notifier.h"

#define MAX_RX_BUF_SIZE (1*1024)
#define MAX_TX_BUF_SIZE (1*1024)

static DEFINE_MUTEX(sdio_vt_ch_lock);
static DEFINE_MUTEX(sdio_vt_rx_buf_lock);
static DEFINE_MUTEX(sdio_vt_tx_buf_lock);
static DEFINE_SPINLOCK(sdio_vt_read_lock);
//static DEFINE_SPINLOCK(sdio_vt_write_lock);

static DECLARE_WAIT_QUEUE_HEAD(sdio_vt_read_wait_queue);

struct sdio_vt_device_t {
    struct miscdevice misc;

    struct sdio_channel *ch;

    unsigned char tx_buf[MAX_TX_BUF_SIZE];
    unsigned char rx_buf[MAX_RX_BUF_SIZE];
    unsigned int read_avail;
    unsigned int write_avail;
    unsigned int open_count;
    unsigned int open_flag;    
};

struct sdio_vt_device_t *sdio_vt_devp;
int sdio_vt_dun_disable = 0;

static void sdio_vt_notify(void *priv, unsigned event)
{
    unsigned long flags;
    int sz;
        
    if (sdio_vt_devp->ch == NULL) {
      printk(KERN_INFO "sdio_vt_notify : notify before ch ready.\n");
      return;
    }

    switch (event) {
        case SDIO_EVENT_DATA_READ_AVAIL:
            //printk(KERN_INFO "sdio_vt_notify : SDIO_EVENT_DATA_READ_AVAIL\n");
            if( (sdio_vt_devp->open_count != 0) && (sdio_vt_devp->ch != NULL)) {    // SMD bug fix. 0811
                sz = sdio_read_avail(sdio_vt_devp->ch);
                if(( sz > 0) && (sdio_vt_devp->read_avail == 0)) {
                    spin_lock_irqsave(&sdio_vt_read_lock, flags);
                    sdio_vt_devp->read_avail = sz;
                    wake_up_interruptible(&sdio_vt_read_wait_queue);
                    spin_unlock_irqrestore(&sdio_vt_read_lock, flags);
                }
            }
            break;

        case SDIO_EVENT_DATA_WRITE_AVAIL:
            //printk(KERN_INFO "sdio_vt_notify : SDIO_EVENT_DATA_WRITE_AVAIL\n");
            #if 0
            if( (sdio_vt_devp->open_count != 0) && (sdio_vt_devp->ch != NULL)){    // SMD bug fix. 0811
                spin_lock_irqsave(&sdio_vt_write_lock, flags);
                sdio_vt_devp->write_avail = sdio_write_avail(sdio_vt_devp->ch);
                spin_unlock_irqrestore(&sdio_vt_write_lock, flags);
            }
            #endif
            break;
    }
}

static ssize_t sdio_vt_read(struct file *fp, char __user *buf,
                            size_t count, loff_t *pos)
{
    int r = 0;
    int bytes_read = 0;
    unsigned long flags;
    int sz = 0;

    //printk(KERN_INFO "sdio_vt_read\n");
    
    if(count > MAX_RX_BUF_SIZE)   count = MAX_RX_BUF_SIZE;
        
    for (;;) {
        mutex_lock(&sdio_vt_rx_buf_lock);
  
        if( (sdio_vt_devp->open_count == 0) || (sdio_vt_devp->ch == NULL)
            || (sdio_vt_devp->open_flag == 0) || (count <= 0)){
            mutex_unlock(&sdio_vt_rx_buf_lock);
            return 0;
        }

        spin_lock_irqsave(&sdio_vt_read_lock, flags);
        sdio_vt_devp->read_avail = 0;
        spin_unlock_irqrestore(&sdio_vt_read_lock, flags);

        sz = sdio_read_avail(sdio_vt_devp->ch);
        if (sz > MAX_RX_BUF_SIZE) {    // garbage  data
            sdio_read(sdio_vt_devp->ch, sdio_vt_devp->rx_buf, MAX_RX_BUF_SIZE);
            mutex_unlock(&sdio_vt_rx_buf_lock);
            continue;
        }
        
        if(sz != 0) {
            if( sz > count) sz = count; // output bffer size check. 
            r = sdio_read(sdio_vt_devp->ch, sdio_vt_devp->rx_buf, (int)sz);
            if(r) {
                printk(KERN_INFO "sdio_vt_read ERROR\n");
                break;
            }
            bytes_read = sz;
            //printk(KERN_INFO "sdio_read : bytes_read = %d\n", bytes_read);
            break;
        }

        mutex_unlock(&sdio_vt_rx_buf_lock);
        r = wait_event_interruptible_timeout(sdio_vt_read_wait_queue,
                                             sdio_vt_devp->read_avail,HZ*5);
        if (r <= 0) {    // timeout or error
            /* qualify error message */
            if (r != -ERESTARTSYS) {
                /* we get this anytime a signal comes in */
                printk(KERN_ERR "ERROR:%s:%i:%s: "
                            "wait_event_interruptible ret %i\n",
                                __FILE__,
                                __LINE__,
                                __func__,
                                r
                                );
            }
            return r;
        }
    }
    r = copy_to_user(buf, sdio_vt_devp->rx_buf, bytes_read);
    mutex_unlock(&sdio_vt_rx_buf_lock);

    if (r > 0) {
        printk(KERN_ERR "ERROR:%s:%i:%s: "
               "copy_to_user could not copy %i bytes.\n",
               __FILE__,
               __LINE__,
               __func__,
               r);
        return r;
    }
    return bytes_read;
}

static ssize_t sdio_vt_write(struct file *file,
                             const char __user *buf,
                             size_t count,
                             loff_t *ppos)
{
    int ret = 0; 
    int avail =0;
    int len = 0;
        
    //printk(KERN_INFO "sdio_vt_write\n");
    
    if(count > MAX_TX_BUF_SIZE)   count = MAX_TX_BUF_SIZE;

    mutex_lock(&sdio_vt_tx_buf_lock);

    // SMD bug fix. 0811
    if( (sdio_vt_devp->open_count == 0) || (sdio_vt_devp->ch == NULL)
        ||(sdio_vt_devp->open_flag ==0)|| (count <=0)){
        mutex_unlock(&sdio_vt_tx_buf_lock);
        return 0;
    }

    ret = copy_from_user(sdio_vt_devp->tx_buf, buf, count);
    if (ret > 0) {
        printk(KERN_ERR "ERROR:%s:%i:%s: "
               "copy_from_user could not copy %i bytes.\n",
               __FILE__,
               __LINE__,
               __func__,
               ret);
        mutex_unlock(&sdio_vt_tx_buf_lock);
        return ret;
    }
    
    avail = sdio_write_avail(sdio_vt_devp->ch);
    if(avail < count) {
        len = avail;
    } else{
        len = count;
    }
    if(len <= 0) {  
        mutex_unlock(&sdio_vt_tx_buf_lock);
        return 0;
    }
    
    ret = sdio_write(sdio_vt_devp->ch, sdio_vt_devp->tx_buf, len);
    if(ret) {
        printk(KERN_INFO "sdio_write ERROR \n");
    }
    else {
        ret = len;
    }
    //printk(KERN_INFO "sdio_write : bytes_write = %d \n", len);
    mutex_unlock(&sdio_vt_tx_buf_lock);
    return ret;
}


static int sdio_vt_open(struct inode *ip, struct file *fp)
{
    int r = 0;

    printk(KERN_INFO "sdio_vt_open\n");

    mutex_lock(&sdio_vt_ch_lock);

    if ((sdio_vt_devp->ch == NULL) || (sdio_vt_devp->open_count == 0)) {
        sdio_vt_dun_disable = 1;
        r = sdio_open("SDIO_DUN", &sdio_vt_devp->ch, NULL, sdio_vt_notify);
        //if(r == -EPERM) r = 0;
    }

    if( r ==  0){    // opened success or already opened.
        sdio_vt_devp->open_flag = 1;
        sdio_vt_devp->open_count++;
    }
    mutex_unlock(&sdio_vt_ch_lock);
    return r;
}

static int sdio_vt_release(struct inode *ip, struct file *fp)
{
    int r = 0;
    int sz = 0;
    unsigned long flags;

    printk(KERN_INFO "sdio_vt_release\n");
    
    mutex_lock(&sdio_vt_ch_lock);
    
    sdio_vt_devp->open_count--;
    
    if ((sdio_vt_devp->open_count == 0) && (sdio_vt_devp->ch != NULL)) {

        mutex_lock(&sdio_vt_rx_buf_lock);
  
        spin_lock_irqsave(&sdio_vt_read_lock, flags);
        sdio_vt_devp->read_avail = 0;
        spin_unlock_irqrestore(&sdio_vt_read_lock, flags);

        sz = sdio_read_avail(sdio_vt_devp->ch);

        while((sz != 0) && (sz <= sdio_read_avail(sdio_vt_devp->ch))) {
            if (sz > MAX_RX_BUF_SIZE) {
                sdio_read(sdio_vt_devp->ch, sdio_vt_devp->rx_buf, MAX_RX_BUF_SIZE);
                sz= sz -MAX_RX_BUF_SIZE;
            }
            else{
                sdio_read(sdio_vt_devp->ch, sdio_vt_devp->rx_buf, (int)sz);
                sz =0;
            }
        }
        mutex_unlock(&sdio_vt_rx_buf_lock);


        sdio_close(sdio_vt_devp->ch);
        sdio_vt_devp->ch = NULL;
        sdio_vt_devp->open_flag = 0;
        sdio_vt_dun_disable = 0;
    }
    mutex_unlock(&sdio_vt_ch_lock);

    return r;
}

static struct file_operations sdio_vt_fops = {
    .owner = THIS_MODULE,
    .read = sdio_vt_read,
    .write = sdio_vt_write,
    .open = sdio_vt_open,
    .release = sdio_vt_release,
};

static struct sdio_vt_device_t sdio_vt_device = {
    .misc = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "smd_vt",
        .fops = &sdio_vt_fops,
    }
};

static void __exit sdio_vt_exit(void)
{
    misc_deregister(&sdio_vt_device.misc);
}

static int __init sdio_vt_init(void)
{
  sdio_vt_devp = &sdio_vt_device;
  
  sdio_vt_devp->ch = NULL;
  sdio_vt_devp->open_count = 0;
  sdio_vt_devp->open_flag = 0;

  return misc_register(&sdio_vt_device.misc);
}

module_init(sdio_vt_init);
module_exit(sdio_vt_exit);
MODULE_DESCRIPTION("MSM SDIO VT PORT");
MODULE_LICENSE("GPL v2");
