/************************************************************************************************
**
**    AUDIO MVS INTERFACE FOR SKY
**
**    FILE
**        mvs_amr.c
**
**    DESCRIPTION
**        This file contains mvs service for SKY.
**          
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/msm_audio_mvs.h>
#include <mach/gpio.h>
#include "mvs_amr.h"

#include <asm/ioctls.h>
#include "audio_mvs_adv.h"

/************************************************************************************************
** TEST
*************************************************************************************************/
#ifdef MVS_AMR_DUMP
#include <linux/fs.h>
#include "audio_fileio.h"

static int enable_dump=0;
void set_enable_dump(int value)
{
     enable_dump=value;
}
#endif

/************************************************************************************************
** Defines
*************************************************************************************************/
static DEFINE_MUTEX(mvs_amr_rx_buf_lock);
static DEFINE_MUTEX(mvs_amr_tx_buf_lock);
static DECLARE_WAIT_QUEUE_HEAD(mvs_amr_wait_queue);
static DEFINE_SPINLOCK(mvs_amr_read_lock);

/************************************************************************************************
** Variables
*************************************************************************************************/
#define AMR_PACKET_SIZE     31
#define HEAD_LENGTH_VT_RX     2

/************************************************************************************************
** Extern 
*************************************************************************************************/
extern int audio_mvs_amr_read_func(char * buf);
extern void audio_mvs_amr_write_func(char * buf, int size);
extern long audio_mvs_get_elapsed_time(void);
extern mvs_adv_state mvs_adv_get_state(void);
/************************************************************************************************
** Functions
*************************************************************************************************/
static int read_avail;
void mvs_amr_packet_is_ready(int avail_size)
{
    unsigned long flags;

    if(avail_size>0)    {
        spin_lock_irqsave(&mvs_amr_read_lock, flags);
        if (read_avail==0) {
            read_avail = avail_size;
            wake_up_interruptible(&mvs_amr_wait_queue);
        }
        spin_unlock_irqrestore(&mvs_amr_read_lock, flags);
    }
}
#ifdef MVS_AMR_DUMP
typedef struct file *fs_handle_type;
#endif

static ssize_t mvs_amr_read(struct file *fp, char __user *buf,
			 size_t count, loff_t *pos)
{
    unsigned char mvs_read_buf[AMR_PACKET_SIZE+HEAD_LENGTH_VT_RX];
    unsigned long flags;
    int read_length;
    int rc;

    if(mvs_adv_get_state()!=MVS_START)        return 0;

    for (;;) {
        mutex_lock(&mvs_amr_tx_buf_lock);
            spin_lock_irqsave(&mvs_amr_read_lock, flags);
            read_avail = 0;
            spin_unlock_irqrestore(&mvs_amr_read_lock, flags);

            read_length=audio_mvs_amr_read_func(&mvs_read_buf[2]);
            if(read_length) break;
            mutex_unlock(&mvs_amr_tx_buf_lock);

            rc = wait_event_interruptible_timeout(mvs_amr_wait_queue,
    						 read_avail!=0,HZ/10); /* 100ms timeout*/
            if (rc <= 0) {    // timeout or error
                /* qualify error message */
                if (rc != -ERESTARTSYS) {
                    /* we get this anytime a signal comes in */
                    MVS_AMR_DBG_LOW("wait_event_interruptible rc=%d", rc);
                }
                return rc;
            }
        }

        mvs_read_buf[0] = read_length;        mvs_read_buf[1] = 0;
#ifdef MVS_AMR_DUMP
    if(count>3 && enable_dump)
    {
        fs_handle_type file;
        file = audiofs_fopen("data/tx_amr_rec.bin", O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR );
        if(file != NULL) {
            audiofs_fwrite(file, &mvs_read_buf[2], read_length);
            audiofs_fclose(file);
        }
    }
#endif
        if(mvs_read_buf[0]>AMR_PACKET_SIZE)            mvs_read_buf[0]=AMR_PACKET_SIZE;

        rc = copy_to_user(buf, mvs_read_buf, mvs_read_buf[0]+HEAD_LENGTH_VT_RX);
        if (rc != 0) {
            MVS_AMR_ERR("copy_to_user failed. (rc=%d)", rc);
            mutex_unlock(&mvs_amr_tx_buf_lock);
            return rc;
        }
        mutex_unlock(&mvs_amr_tx_buf_lock);
        return mvs_read_buf[0]+HEAD_LENGTH_VT_RX;
}

static ssize_t mvs_amr_write(struct file *file,
				const char __user *buf,
				size_t count,
				loff_t *ppos)
{
    int rc=0;
    static unsigned char mvs_write_buf[AMR_PACKET_SIZE+HEAD_LENGTH_VT_RX];

    if(mvs_adv_get_state()!=MVS_START)        return 0;
    
    mutex_lock(&mvs_amr_rx_buf_lock);

    if(count>AMR_PACKET_SIZE+HEAD_LENGTH_VT_RX)
        count = AMR_PACKET_SIZE+HEAD_LENGTH_VT_RX;
    rc = copy_from_user(&mvs_write_buf[0], buf, count);
    if (rc != 0) {
        MVS_AMR_ERR("copy_to_user failed. (rc=%d)", rc);
        mutex_unlock(&mvs_amr_rx_buf_lock);
        return rc;
    }

#ifdef MVS_AMR_DUMP
    if(count>3 && enable_dump)
    {
        fs_handle_type file;
        file = audiofs_fopen("data/rx_amr_rec.bin", O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR );
        if(file != NULL) {
            audiofs_fwrite(file, &mvs_write_buf[2], mvs_write_buf[0]);
            audiofs_fclose(file);
        }
    }
#endif

    if(count==3)  {
        if(mvs_write_buf[0]==1 && mvs_write_buf[1]==0)  {
            if(mvs_write_buf[2]=='A') {   // AMR START
                MVS_AMR_DBG_MED("- (mode:AMR)");
                #ifdef VOLUMEKEY_NOISE_FIX
                enter_noise_critical_section(0);
                #endif
            }
            else if(mvs_write_buf[2]=='P' || mvs_write_buf[2]=='p') {   // PCM START
                MVS_AMR_DBG_MED("- (mode:PCM)");
            }
        }
    }
    else    {
        if(mvs_write_buf[0]>AMR_PACKET_SIZE)   {
            MVS_AMR_ERR("- buffer size is changed!!!(size=%d)", mvs_write_buf[0]);
        }
        else	audio_mvs_amr_write_func(&mvs_write_buf[2], mvs_write_buf[0]);
    }

    mutex_unlock(&mvs_amr_rx_buf_lock);
    return rc;
}


static int mvs_amr_open(struct inode *inode, struct file *file)
{
       MVS_AMR_DBG_LOW("mvs_amr_open");
	return 0;
}

static int mvs_amr_release(struct inode *inode, struct file *file)
{
	MVS_AMR_DBG_LOW("mvs_amr_release ");
#ifdef MVS_AMR_DUMP     
       enable_dump=0;
#endif
	return 0;	
}

static long mvs_amr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	MVS_AMR_DBG_LOW("mvs_amr_ioctl cmd:0x%x, arg:0x%x ", cmd, arg);
       return 0;
}

static const struct file_operations mvs_amr_fops = {
	.owner		= THIS_MODULE,
       .read = mvs_amr_read,
       .write = mvs_amr_write,
	.open		= mvs_amr_open,
	.release	= mvs_amr_release,
	.unlocked_ioctl	= mvs_amr_ioctl,
};

struct miscdevice mvs_amr_miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "mvs_amr",
	.fops =     &mvs_amr_fops
};

 
static int __init mvs_amr_misc_device_init(void)
{
    int rc=0;
    MVS_AMR_DBG_HIGH("mvs_amr_misc_device_init");
    rc = misc_register(&mvs_amr_miscdev);
    if(rc<0)    MVS_AMR_ERR("mvs_amr_misc_device_init rc= %d ", rc);
    return rc;
}
static void __exit mvs_amr_misc_device_deinit(void)
{
    MVS_AMR_DBG_HIGH("mvs_amr_misc_device_deinit");
    misc_deregister(&mvs_amr_miscdev);
}

module_init(mvs_amr_misc_device_init);
module_exit(mvs_amr_misc_device_deinit);

MODULE_DESCRIPTION("MVS AMR driver");
MODULE_LICENSE("GPL v2");
