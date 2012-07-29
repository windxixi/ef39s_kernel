// jwjeon100419@DS2

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <asm/system.h>


#include <mach/rpc_nv.h>

#define PANMAC_MAJOR        60
#define PANMAC_MAC_MAX_LEN  17

#define NV_WLAN_MAC_ADDRESS_I 4678

MODULE_LICENSE("GPL");

static int panmac_open(struct inode *inode, struct file *filp)
{
  return 0;
}

static int panmac_release(struct inode *inode, struct file *filp)
{
  return 0;
}

//lee.eunsuk 20110516 static size_t panmac_read(struct file *filp, char* buf, size_t count, loff_t *f_pos)
static ssize_t panmac_read(struct file *filp, char* buf, size_t count, loff_t *f_pos)
{
  nv_cmd_item_type item;
  int rc, result;
  int copy_len;
  char temp[PANMAC_MAC_MAX_LEN + 1];

  rc = msm_nv_rpc_connect();
  if (rc != 0) {
    printk(KERN_ERR "%s : msm_nv_rpc_connect error(%d)\n", __func__, rc);
    return 0;
  }
  result = msm_nv_read(NV_WLAN_MAC_ADDRESS_I, &item);
  if (result != 0)
  {
    printk(KERN_INFO "msm_nv_read failed (%d)\n", result);
    return 0;
  }
  
  printk(KERN_INFO "msm_nv_read: %08X %08X\n", item.wlan_mac_address[0], item.wlan_mac_address[1]);
    
  sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
    (item.wlan_mac_address[0] >> 8)  & 0xFF,
    (item.wlan_mac_address[0] >> 16) & 0xFF,
    (item.wlan_mac_address[0] >> 24) & 0xFF,
    (item.wlan_mac_address[1] >> 16) & 0xFF,
    (item.wlan_mac_address[1] >> 24) & 0xFF,
    (item.wlan_mac_address[0])       & 0xFF);
    
  copy_len = PANMAC_MAC_MAX_LEN > count ? count: PANMAC_MAC_MAX_LEN;

  if(copy_to_user(buf, temp, copy_len))
  	return -EFAULT;
  
  if (*f_pos == 0)
  {
    *f_pos += copy_len;
    return copy_len;
  }

  return 0;
}

struct file_operations panmac_fops = {
  .read = panmac_read,
  .open = panmac_open,
  .release = panmac_release,
};

static int panmac_init(void)
{
  int result;
 
  printk(KERN_INFO "panmac init\n");

  result = register_chrdev(PANMAC_MAJOR, "panmac", &panmac_fops);
  if (result < 0)
  {
    printk(KERN_INFO "Failed to register chrdev (%d).\n", result);
    return result;
  }
  
  return 0;
}

static void panmac_exit(void)
{
  printk(KERN_INFO "panmac exit\n");
  unregister_chrdev(PANMAC_MAJOR, "panmac");
}

module_init(panmac_init);
module_exit(panmac_exit);
