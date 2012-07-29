/* Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.    
 *
 * No part of this work may be reproduced, modified, distributed, transmitted,    
 * transcribed, or translated into any language or computer format, in any form   
 * or by any means without written permission of: Silicon Image, Inc.,            
 * 1060 East Arques Avenue, Sunnyvale, California 94085       
 *
 */
 
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/time.h>
#include <linux/completion.h>

#include <linux/irq.h>
//#include <asm/irq.h>

#include <linux/i2c.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include <linux/syscalls.h> 
#include <linux/fcntl.h> 
#include <asm/uaccess.h> 

#include <mach/gpio.h>
#include <asm/system.h>
#include "msm_fb.h"
#include "mhl_sii9244_driver.h"


#define DEVICE_NAME "sii9244"
//#define SII9244_DEVICE_ID   0xB0

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= TP10))	 
#define F_HPD_PIN_MAP_CHANGE  
#endif

#define SII_DEV_DBG(format,...)\
    printk(KERN_ERR "[SKY_MHL] +%s\n", __FUNCTION__);
	//printk ("[ "SUBJECT " (%s,%d) ] " format "\n", __func__, __LINE__, ## __VA_ARGS__);
       // printk (("SUBJECT  (%s,%d) ] \n"), __func__, __LINE__);

#define GPIO_HIGH_VALUE 1
#define GPIO_LOW_VALUE  0

// 20110811, kkcho, gpio-pin change
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= TP10))
#define IRQ_EINT13 94
#else
#define IRQ_EINT13 172
#endif
#define MHL_INT IRQ_EINT13
/*
  Description: Setting and define the GPIO pins related to MHL transmitter.

  The below function is example function in Dempsey platform.
  
  MHL_CSDA_MSM (GPIO_AP_SDA) : CSDA pin
  MHL_CSCL_MSM (GPIO_AP_SCL) : CSCL pin
  MHL_WAKE_UP (GPIO_MHL_WAKE_UP) : Wake up pin
  MHL_RST_N (GPIO_MHL_RST) : Rset pin
  MHL_EN (GPIO_MHL_SEL):  FSA3200 USB switch 
*/
#define MHL_CSCL_MSM      27
#define MHL_CSDA_MSM     26
#define MHL_WAKE_UP       169
#define MHL_RST_N             25
#define MHL_EN                   16//154
#define MHL_SHDN              23   //MSX13047E USB switch

//MSM_GPIO_TO_INT(IRQ_EINT13)
#define MHL_INT_IRQ 	gpio_to_irq(IRQ_EINT13)
//#define MHL_WAKEUP_IRQ		gpio_to_irq(MHL_WAKE_UP)

/* kkcho, descriptor
    MHL Wake Pulses의 경우, 2가지 방법이  있으며, 현재는 I2c를 이용한 wake pulses 를 사용한다
    1. Wake_UP gpio pin을 이용 
    2. I2C를 이용 . 
*/

static uint32_t mhl_sii9244_gpio_init_table[] = {
	GPIO_CFG(MHL_CSCL_MSM, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(MHL_CSDA_MSM, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(MHL_WAKE_UP, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(MHL_RST_N, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	
	GPIO_CFG(MHL_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	
	GPIO_CFG(MHL_SHDN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	// for sleep_current
	GPIO_CFG(MHL_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),	
};

struct work_struct sii9244_int_work;
struct workqueue_struct *sii9244_wq = NULL;

struct i2c_driver sii9244_i2c_driver;
struct i2c_client *sii9244_i2c_client = NULL;

struct i2c_driver sii9244A_i2c_driver;
struct i2c_client *sii9244A_i2c_client = NULL;

struct i2c_driver sii9244B_i2c_driver;
struct i2c_client *sii9244B_i2c_client = NULL;

struct i2c_driver sii9244C_i2c_driver;
struct i2c_client *sii9244C_i2c_client = NULL;

static struct i2c_device_id sii9244_id[] = {
	{"sii9244", 0},
	{}
};

static struct i2c_device_id sii9244A_id[] = {
	{"sii9244A", 0},
	{}
};

static struct i2c_device_id sii9244B_id[] = {
	{"sii9244B", 0},
	{}
};

static struct i2c_device_id sii9244C_id[] = {
	{"sii9244C", 0},
	{}
};

int MHL_i2c_init = 0;

struct sii9244_state {
	struct i2c_client *client;
};

struct i2c_client* get_sii9244_client(u8 device_id)
{

	struct i2c_client* client_ptr;

	if(device_id == 0x72)
		client_ptr = sii9244_i2c_client;
	else if(device_id == 0x7A)
		client_ptr = sii9244A_i2c_client;
	else if(device_id == 0x92)
		client_ptr = sii9244B_i2c_client;
	else if(device_id == 0xC8)
		client_ptr = sii9244C_i2c_client;
	else
		client_ptr = NULL;

	return client_ptr;
}
EXPORT_SYMBOL(get_sii9244_client);

static ssize_t MHD_check_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	int count;
	int res = 0;
	count = sprintf(buf,"%d\n", res );
	//TVout_LDO_ctrl(false);
	return count;
}

static ssize_t MHD_check_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	printk(KERN_ERR "input data --> %s\n", buf);

	return size;
}

static DEVICE_ATTR(MHD_file, S_IRUGO , MHD_check_read, MHD_check_write);

u8 sii9244_i2c_read(struct i2c_client *client, u8 reg)
{
	u8 ret;	
//	u16 ret1;
//	u8 ret2;
//	u32 ret3;
	
	if(!MHL_i2c_init)
	{
		SII_DEV_DBG("I2C not ready");
		return 0;
	}
	
	i2c_smbus_write_byte(client, reg);	

	ret = i2c_smbus_read_byte(client);

	if (ret < 0)
	{
		SII_DEV_DBG("i2c read fail");
		return -EIO;
	}
	return ret;

}
EXPORT_SYMBOL(sii9244_i2c_read);

int sii9244_i2c_write(struct i2c_client *client, u8 reg, u8 data)
{
	if(!MHL_i2c_init)
	{
		SII_DEV_DBG("I2C not ready");
		return 0;
	}

	return i2c_smbus_write_byte_data(client, reg, data);
}
EXPORT_SYMBOL(sii9244_i2c_write);


void sii9244_interrupt_event_work(struct work_struct *p)
{

	printk("[SKY_MHL] sii9244_interrupt_event_work() is called\n");

	sii9244_interrupt_event();
}


void mhl_int_irq_handler_sched(void)
{

	printk("mhl_int_irq_handler_sched() is called\n");
	queue_work(sii9244_wq,&sii9244_int_work);		
}


irqreturn_t mhl_int_irq_handler(int irq, void *dev_id)
{

	printk("mhl_int_irq_handler() is called\n");
	
	mhl_int_irq_handler_sched();
	return IRQ_HANDLED;
}

irqreturn_t mhl_wake_up_irq_handler(int irq, void *dev_id)
{

	printk(KERN_ERR "mhl_wake_up_irq_handler() is called\n");

	//if (gpio_get_value(GPIO_MHL_SEL))	
		mhl_int_irq_handler_sched();
	
	return IRQ_HANDLED;
}

 
static int sii9244_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	//SII_DEV_DBG("");

	struct sii9244_state *state;

	struct class *mhl_class;
	struct device *mhl_dev;
	//int ret;
	
       SII_DEV_DBG("");
	   
	state = kzalloc(sizeof(struct sii9244_state), GFP_KERNEL);
	if (state == NULL) {		
		printk("failed to allocate memory \n");
		return -ENOMEM;
	}
	
	state->client = client;
	i2c_set_clientdata(client, state);


	
	/* rest of the initialisation goes here. */
	
	printk("sii9244 attach success!!!\n");

	sii9244_i2c_client = client;

	MHL_i2c_init = 1;

	mhl_class = class_create(THIS_MODULE, "mhl");
	if (IS_ERR(mhl_class))
	{
		pr_err("Failed to create class(mhl)!\n");
	}

	mhl_dev = device_create(mhl_class, NULL, 0, NULL, "mhl_dev");
	if (IS_ERR(mhl_dev))
	{
		pr_err("Failed to create device(mhl_dev)!\n");
	}

	if (device_create_file(mhl_dev, &dev_attr_MHD_file) < 0)
		printk("Failed to create device file(%s)!\n", dev_attr_MHD_file.attr.name);

	return 0;

}

static int __devexit sii9244_remove(struct i2c_client *client)
{
	struct sii9244_state *state = i2c_get_clientdata(client);
	kfree(state);

	return 0;
}

static int sii9244A_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct sii9244_state *state;
	SII_DEV_DBG("");
	state = kzalloc(sizeof(struct sii9244_state), GFP_KERNEL);
	if (state == NULL) {		
		printk("failed to allocate memory \n");
		return -ENOMEM;
	}
	
	state->client = client;
	i2c_set_clientdata(client, state);
	
	/* rest of the initialisation goes here. */
	
	printk("sii9244A attach success!!!\n");

	sii9244A_i2c_client = client;

	return 0;

}

static int __devexit sii9244A_remove(struct i2c_client *client)
{
	struct sii9244_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}

static int sii9244B_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct sii9244_state *state;
	SII_DEV_DBG("");
	state = kzalloc(sizeof(struct sii9244_state), GFP_KERNEL);
	if (state == NULL) {		
		printk("failed to allocate memory \n");
		return -ENOMEM;
	}
	
	state->client = client;
	i2c_set_clientdata(client, state);
	
	/* rest of the initialisation goes here. */
	
	printk("sii9244B attach success!!!\n");

	sii9244B_i2c_client = client;

	
	return 0;

}

static int __devexit sii9244B_remove(struct i2c_client *client)
{
	struct sii9244_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}

static int sii9244C_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{ 
	 //int irq;
	 int ret;    
        struct sii9244_state *state;
	 		
    	 SII_DEV_DBG("");

        state = kzalloc(sizeof(struct sii9244_state), GFP_KERNEL);
        if (state == NULL) {        
            printk("failed to allocate memory \n");
            return -ENOMEM;
        }
        
        state->client = client;
        i2c_set_clientdata(client, state);
        
        /* rest of the initialisation goes here. */
        
        printk("sii9244C attach success!!!\n");
    
        sii9244C_i2c_client = client;      
   
        msleep(100);    

	sii9244_wq = create_singlethread_workqueue("sii9244_wq");
	INIT_WORK(&sii9244_int_work, sii9244_interrupt_event_work);

	ret = request_threaded_irq(MHL_INT_IRQ, NULL, mhl_int_irq_handler,
				IRQF_SHARED , "mhl_int", (void *) state); 

        if (ret) 
        {
            printk("[SKY_MHL] unable to request irq mhl_int err:: %d\n", ret);
            return ret;
        }       
        printk("[SKY_MHL] MHL int reques successful %d\n", ret);
            
	//ret = request_threaded_irq(MHL_WAKEUP_IRQ, NULL,
	//	mhl_wake_up_irq_handler, IRQF_SHARED,
	//	"mhl_wake_up", (void *) state);
	//if (ret) {
	//	printk(KERN_ERR "unable to request irq mhl_wake_up"
	//				" err:: %d\n", ret);
	//	return ret;
	//}		
  
        return 0;    
    }

static int __devexit sii9244C_remove(struct i2c_client *client)
{
	struct sii9244_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}

struct i2c_driver sii9244_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9244",
	},
	.id_table	= sii9244_id,
	.probe	= sii9244_i2c_probe,
	.remove	= __devexit_p(sii9244_remove),
	.command = NULL,
};

struct i2c_driver sii9244A_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9244A",
	},
	.id_table	= sii9244A_id,
	.probe	= sii9244A_i2c_probe,
	.remove	= __devexit_p(sii9244A_remove),
	.command = NULL,
};

struct i2c_driver sii9244B_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9244B",
	},
	.id_table	= sii9244B_id,
	.probe	= sii9244B_i2c_probe,
	.remove	= __devexit_p(sii9244B_remove),
	.command = NULL,
};

struct i2c_driver sii9244C_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9244C",
	},
	.id_table	= sii9244C_id,
	.probe	= sii9244C_i2c_probe,
	.remove	= __devexit_p(sii9244C_remove),
	.command = NULL,
};

void sii9244_cfg_power_init(void)
{
	gpio_set_value(MHL_RST_N, GPIO_LOW_VALUE);
	msleep(10);	
	gpio_set_value(MHL_RST_N, GPIO_HIGH_VALUE);    

	sii9244_driver_init();
	sii9244_remote_control_init();
	gpio_set_value(MHL_RST_N, GPIO_LOW_VALUE);

  	printk(KERN_ERR "[SKY_MHL]%s \n",__func__);
}


void sii9244_cfg_power(bool on)
{
  if(on)
  {
	gpio_set_value(MHL_RST_N, GPIO_LOW_VALUE);
	msleep(10);	
	gpio_set_value(MHL_RST_N, GPIO_HIGH_VALUE);    

#if (0)
	 /* txs0102 control : HDMI-siganl level shift*/
        /* MHL_SHDW : active_low */
        gpio_set_value(MHL_SHDN, GPIO_HIGH_VALUE);
        msleep(5);
        gpio_set_value(MHL_SHDN, GPIO_LOW_VALUE);
        msleep(5);
        gpio_set_value(MHL_SHDN, GPIO_HIGH_VALUE);  
#endif
	sii9244_driver_init();
  }
  else
  {
#if (0)  
        /* txs0102 control : HDMI-siganl level shift*/
        gpio_set_value(MHL_SHDN, GPIO_LOW_VALUE);
#endif
	gpio_set_value(MHL_RST_N, GPIO_LOW_VALUE);
	msleep(10);
	gpio_set_value(MHL_RST_N, GPIO_HIGH_VALUE);
	gpio_set_value(MHL_RST_N, GPIO_LOW_VALUE);
  }
  printk(KERN_ERR "[SKY_MHL]%s : %d \n",__func__,on);
}
EXPORT_SYMBOL(sii9244_cfg_power);

void MHL_On(bool on)
{
	printk("[SKY_MHL] USB path change : %d\n", on);
	if (on == 1) {
		//if(gpio_get_value(MHL_EN))  // USB_SWITCH Check
		//	printk("[MHL] MHL_EN : already 1\n");
		//else {
		//	gpio_set_value(MHL_EN, GPIO_HIGH_VALUE); // switch-MHL			
			sii9244_cfg_power(1);
		//}	
	} else {
		//if(!gpio_get_value(MHL_EN))
		//	printk("[MHL] MHL_EN : already 0\n");
		//else {	
			sii9244_cfg_power(0);
		//	gpio_set_value(MHL_EN, GPIO_LOW_VALUE); // switch-USB	
		//}
	}
}
EXPORT_SYMBOL(MHL_On);

static void mhl_gpio_init(uint32_t *table, int len, unsigned enable)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n],
				enable ? GPIO_CFG_ENABLE : GPIO_CFG_DISABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, table[n], rc);
			break;
		}
	}
}

/*
  Description: Setting and define the GPIO pins related to MHL transmitter.

  The below function is example function in Dempsey platform.
  
  MHL_CSDA_MSM (GPIO_AP_SDA) : CSDA pin
  MHL_CSCL_MSM (GPIO_AP_SCL) : CSCL pin
  MHL_WAKE_UP (GPIO_MHL_WAKE_UP) : Wake up pin
  MHL_RST_N (GPIO_MHL_RST) : Rset pin
  MHL_EN (GPIO_MHL_SEL):  FSA3200 USB switch   
*/
static void sii9244_cfg_gpio(void)
{
	 printk(KERN_ERR "[SKY_MHL]+%s 2nd needed gpio_init\n", __FUNCTION__);
        // need to define the GPIO configuration
        mhl_gpio_init(mhl_sii9244_gpio_init_table, ARRAY_SIZE(mhl_sii9244_gpio_init_table), 1);

	 //set_irq_type(MHL_WAKEUP_IRQ, IRQ_TYPE_EDGE_RISING);
	 set_irq_type(MHL_INT_IRQ, IRQ_TYPE_EDGE_FALLING);

        /* USB switch*/
	 /* LOW : USB, HIGH : MHL */	
        //gpio_set_value(MHL_EN, GPIO_HIGH_VALUE);
        //msleep(5);
        gpio_set_value(MHL_EN, GPIO_LOW_VALUE); // USB to MSM
        //msleep(5);
        gpio_set_value(MHL_SHDN, GPIO_LOW_VALUE);  		

	 gpio_set_value(MHL_WAKE_UP, GPIO_LOW_VALUE);  // HPD pin이 제대로 동작 안되는 상황이므로.. 보완을 위해...		
}

static int __init sii9244_init(void)
{
	int ret;

	sii9244_cfg_gpio();	

	printk(KERN_ERR "[SKY_MHL]+%s 3rd i2c_add_driver\n", __FUNCTION__);
	
	ret = i2c_add_driver(&sii9244_i2c_driver);
	if (ret != 0)
		printk("[MHL sii9244] can't add i2c driver\n");	
	else
		printk("[MHL sii9244] add i2c driver\n");
	
	ret = i2c_add_driver(&sii9244A_i2c_driver);
	if (ret != 0)
		printk("[MHL sii9244A] can't add i2c driver\n");	
	else
		printk("[MHL sii9244A] add i2c driver\n");
	
	ret = i2c_add_driver(&sii9244B_i2c_driver);
	if (ret != 0)
		printk("[MHL sii9244B] can't add i2c driver\n");	
	else
		printk("[MHL sii9244B] add i2c driver\n");
	
	ret = i2c_add_driver(&sii9244C_i2c_driver);
	if (ret != 0)
		printk("[MHL sii9244C] can't add i2c driver\n");	
	else
		printk("[MHL sii9244C] add i2c driver\n");

	sii9244_cfg_power_init();	//Turn On power to sii9244 

	return ret;	
}
module_init(sii9244_init);		

static void __exit sii9244_exit(void)
{
	i2c_del_driver(&sii9244_i2c_driver);
	i2c_del_driver(&sii9244A_i2c_driver);
	i2c_del_driver(&sii9244B_i2c_driver);	
	i2c_del_driver(&sii9244C_i2c_driver);
	
};
module_exit(sii9244_exit);

MODULE_DESCRIPTION("sii9244 MHL driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Qualcomm Innovation Center, Inc.");
