/************************************************************************************************
**
**    AUDIO MVS INTERFACE FOR SKY
**
**    FILE
**        mvs_pcm.c
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
#include "mvs_pcm.h"
#include <asm/ioctls.h>
#include "audio_mvs_adv.h"
 
/************************************************************************************************
** TEST
*************************************************************************************************/
#ifdef MVS_PCM_DUMP
#include <linux/fs.h>
#include "audio_fileio.h"
#endif

//jhpark-MVS #define MVS_PCM_RX_SCALE_ENABLE     // 110520 SangwonLee Input data scaling.
/************************************************************************************************
** Defines
*************************************************************************************************/
static DEFINE_MUTEX(mvs_pcm_rx_buf_lock);
static DEFINE_MUTEX(mvs_pcm_tx_buf_lock);
static DECLARE_WAIT_QUEUE_HEAD(mvs_pcm_wait_queue);
static DEFINE_SPINLOCK(mvs_pcm_read_lock);

/************************************************************************************************
** Variables
*************************************************************************************************/
#define PCM_PACKET_SIZE     160
#define HEAD_LENGTH_VOIP_RX     2

/************************************************************************************************
** Extern 
*************************************************************************************************/
extern int audio_mvs_pcm_read_func(char * buf);
extern void audio_mvs_pcm_write_func(char * buf, int size);
extern long audio_mvs_get_elapsed_time(void);
extern mvs_adv_state mvs_adv_get_state(void);
/************************************************************************************************
** Functions
*************************************************************************************************/
static int read_avail;
void mvs_pcm_packet_is_ready(int avail_size)
{
    unsigned long flags;

    if(avail_size>0)    {
        spin_lock_irqsave(&mvs_pcm_read_lock, flags);
        if (read_avail==0) {
            read_avail = avail_size;
            wake_up_interruptible(&mvs_pcm_wait_queue);
        }
        spin_unlock_irqrestore(&mvs_pcm_read_lock, flags);
    }
}
#ifdef MVS_PCM_DUMP
typedef struct file *fs_handle_type;
#endif

static ssize_t mvs_pcm_read(struct file *fp, char __user *buf,
			 size_t count, loff_t *pos)
{
    unsigned char mvs_read_buf[PCM_PACKET_SIZE+HEAD_LENGTH_VOIP_RX];
    unsigned long flags;
    int read_length;
    int rc;

    if(mvs_adv_get_state()!=MVS_START)        return 0;

    for (;;) {
        mutex_lock(&mvs_pcm_tx_buf_lock);
        spin_lock_irqsave(&mvs_pcm_read_lock, flags);
        read_avail = 0;
        spin_unlock_irqrestore(&mvs_pcm_read_lock, flags);

        read_length=audio_mvs_pcm_read_func(&mvs_read_buf[2]);
        if(read_length) break;
        mutex_unlock(&mvs_pcm_tx_buf_lock);

        rc = wait_event_interruptible_timeout(mvs_pcm_wait_queue,
        			 read_avail!=0,HZ/10); /* 100ms timeout*/
        if (rc <= 0) {    // timeout or error
            /* qualify error message */
            if (rc != -ERESTARTSYS) {
                /* we get this anytime a signal comes in */
                MVS_PCM_DBG_LOW("wait_event_interruptible rc=%d", rc);
            }
            return rc;
        }
    }
    mvs_read_buf[0] = read_length;        mvs_read_buf[1] = 0;

#ifdef MVS_PCM_DUMP
    if(count>3)
    {
            fs_handle_type file;
            file = audiofs_fopen("data/tx_pcm_rec.bin", O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR );
		if(file != NULL) {
                    audiofs_fwrite(file, &mvs_read_buf[2], read_length);
                    audiofs_fclose(file);
		}
    }
#endif
    if(mvs_read_buf[0]>PCM_PACKET_SIZE)            mvs_read_buf[0]=PCM_PACKET_SIZE;

    rc = copy_to_user(buf, mvs_read_buf, mvs_read_buf[0]+HEAD_LENGTH_VOIP_RX);
    if (rc != 0) {
        MVS_PCM_ERR("copy_to_user failed. (rc=%d)", rc);
        mutex_unlock(&mvs_pcm_tx_buf_lock);
        return rc;
    }
    mutex_unlock(&mvs_pcm_tx_buf_lock);
    return mvs_read_buf[0]+HEAD_LENGTH_VOIP_RX;
}

#ifdef MVS_PCM_RX_SCALE_ENABLE
static void mvs_pcm_damp(unsigned char *pcm_data, int size, int shift)
{
    int x;
    int value;

    for(x=0;x<size;x+=2)
    {
        value = pcm_data[x] |(pcm_data[x+1]<<8);

        if((pcm_data[x+1]&0x80)==0x80)  {   /* negative value handlig*/
            value=(int)(((65536-value))>>shift);
            if(value>32767)  value = 32767;
            value = 65536 - value;
        }
        else        {
            value=(int)((value)>>shift);
            if(value>32767)  value = 32767;
        }

        pcm_data[x]=value & 0xff;
        pcm_data[x+1]=(value>>8)&0xff;
    }
}
#endif

static ssize_t mvs_pcm_write(struct file *file,
				const char __user *buf,
				size_t count,
				loff_t *ppos)
{
    int rc=0;
    static unsigned char mvs_write_buf[PCM_PACKET_SIZE+HEAD_LENGTH_VOIP_RX];

    if(mvs_adv_get_state()!=MVS_START)        return 0;
    
    mutex_lock(&mvs_pcm_rx_buf_lock);

    if(count>PCM_PACKET_SIZE+HEAD_LENGTH_VOIP_RX)
        count = PCM_PACKET_SIZE+HEAD_LENGTH_VOIP_RX;
    
    rc = copy_from_user(&mvs_write_buf[0], buf, count);
    if (rc != 0) {
        MVS_PCM_ERR("copy_from_user failed. (rc=%d)", rc);
        mutex_unlock(&mvs_pcm_rx_buf_lock);
        return rc;
    }

#ifdef MVS_PCM_DUMP
    if(count>3)
    {
            fs_handle_type file;
            file = audiofs_fopen("data/rx_pcm_rec.bin", O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR );
		if(file != NULL) {
                    audiofs_fwrite(file, &mvs_write_buf[2], mvs_write_buf[0]);
                    audiofs_fclose(file);
		}
    }
#endif

    if(count==3)  {
        if(mvs_write_buf[0]==1 && mvs_write_buf[1]==0)  {
            if(mvs_write_buf[2]=='A') {   // AMR START
                MVS_PCM_DBG_MED("- (mode:AMR)");
            }
            else if(mvs_write_buf[2]=='P' || mvs_write_buf[2]=='p') {   // PCM START
                MVS_PCM_DBG_MED("- (mode:PCM)");
            }
        }
    }
    else    {
        if(mvs_write_buf[0]>PCM_PACKET_SIZE)   {
            MVS_PCM_ERR("- buffer size is changed!!!(size=%d)", mvs_write_buf[0]);
        }
        else    {
            #ifdef MVS_PCM_RX_SCALE_ENABLE            
            mvs_pcm_damp(&mvs_write_buf[2], 160, 1);
            #endif
            audio_mvs_pcm_write_func(&mvs_write_buf[2], mvs_write_buf[0]);
        }
    }

    mutex_unlock(&mvs_pcm_rx_buf_lock);
    return rc;
}
static int mvs_pcm_open(struct inode *inode, struct file *file)
{
       MVS_PCM_DBG_LOW("mvs_pcm_open");
	return 0;
}

static int mvs_pcm_release(struct inode *inode, struct file *file)
{
	MVS_PCM_DBG_LOW("mvs_pcm_release ");
	return 0;	
}

static long mvs_pcm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	MVS_PCM_DBG_LOW("mvs_pcm_ioctl cmd:0x%x, arg:0x%x ", cmd, arg);
       return 0;
}

static const struct file_operations mvs_pcm_fops = {
	.owner		= THIS_MODULE,
       .read = mvs_pcm_read,
       .write = mvs_pcm_write,
	.open		= mvs_pcm_open,
	.release	= mvs_pcm_release,
	.unlocked_ioctl	= mvs_pcm_ioctl,
};

struct miscdevice mvs_pcm_miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "smd_mvs",
	.fops =     &mvs_pcm_fops
};

static int __init mvs_pcm_misc_device_init(void)
{
    int rc=0;
    MVS_PCM_DBG_HIGH("mvs_pcm_misc_device_init");
    rc = misc_register(&mvs_pcm_miscdev);
    if(rc<0)    MVS_PCM_ERR("mvs_pcm_misc_device_init ret= %d ", rc);
    return rc;
}
static void __exit mvs_pcm_misc_device_deinit(void)
{
    MVS_PCM_DBG_HIGH("mvs_pcm_misc_device_deinit");
    misc_deregister(&mvs_pcm_miscdev);
}

module_init(mvs_pcm_misc_device_init);
module_exit(mvs_pcm_misc_device_deinit);

MODULE_DESCRIPTION("MVS PCM driver");
MODULE_LICENSE("GPL v2");
