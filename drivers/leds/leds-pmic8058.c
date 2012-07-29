/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/mfd/pmic8058.h>
#include <linux/leds-pmic8058.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
// ----------------------------------------------------------------
// DEBUG
// ----------------------------------------------------------------
//#define DBG_ENABLE

#ifdef DBG_ENABLE
#define dbg(fmt, args...)   printk("[LED] " fmt, ##args)
#else
#define dbg(fmt, args...)
#endif
#define dbg_func_in()       dbg("[+] %s\n", __func__)
#define dbg_func_out()      dbg("[-] %s\n", __func__)
#define dbg_line()          dbg("[LINE] %d(%s)\n", __LINE__, __func__)
// ----------------------------------------------------------------

#define SSBI_REG_ADDR_DRV_KEYPAD	0x48
#define PM8058_DRV_KEYPAD_BL_MASK	0xf0
#define PM8058_DRV_KEYPAD_BL_SHIFT	0x04

#define SSBI_REG_ADDR_FLASH_DRV0        0x49
#define PM8058_DRV_FLASH_MASK           0xf0
#define PM8058_DRV_FLASH_SHIFT          0x04

#define SSBI_REG_ADDR_FLASH_DRV1        0xFB

#define SSBI_REG_ADDR_LED_CTRL_BASE	0x131
#define SSBI_REG_ADDR_LED_CTRL(n)	(SSBI_REG_ADDR_LED_CTRL_BASE + (n))
#define PM8058_DRV_LED_CTRL_MASK	0xf8
#define PM8058_DRV_LED_CTRL_SHIFT	0x03

#define MAX_FLASH_CURRENT	300
#define MAX_KEYPAD_CURRENT 300
#define MAX_KEYPAD_BL_LEVEL	(1 << 4)
#define MAX_LED_DRV_LEVEL	20 /* 2 * 20 mA */

#define PMIC8058_LED_OFFSET(id) ((id) - PMIC8058_ID_LED_0)

struct pmic8058_led_data {
	struct led_classdev	cdev;
	int			id;
	enum led_brightness	brightness;
	u8			flags;
	struct pm8058_chip	*pm_chip;
	struct work_struct	work;
	struct mutex		lock;
	spinlock_t		value_lock;
	u8			reg_kp;
	u8			reg_led_ctrl[3];
	u8			reg_flash_led0;
	u8			reg_flash_led1;
};

#define PM8058_MAX_LEDS		7
static struct pmic8058_led_data led_data[PM8058_MAX_LEDS];

static int leds_fops_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static void led_lc_set(struct pmic8058_led_data *led, enum led_brightness value);
static void kp_bl_set(struct pmic8058_led_data *led, enum led_brightness value);
static void pmic8058_led_set(struct led_classdev *led_cdev,enum led_brightness value);
#ifdef CONFIG_USER_GPIO_CONTROL
#define USER_SET_SLEEP_GPIO	3000
#define USER_GET_SLEEP_GPIO	3001
#define USER_GET_CURR_GPIO		3002
static int user_gpio_fops_ioctl(struct inode *inode, struct file *filp,
	       unsigned int cmd, unsigned long arg)
{
	int gpio_num, gpio_conf;
	if(cmd == USER_GET_CURR_GPIO) {
		if (copy_from_user(&gpio_num, (void *) arg, sizeof(gpio_num))) {
			pr_err("%s: copy from user failed\n", __func__);
			return -EFAULT;
		}
		else {
			gpio_conf = sky_user_get_curr_gpio(gpio_num);
			if (copy_to_user((void *) arg, &gpio_conf, sizeof(gpio_conf))) {
				pr_err("%s: copy to user failed\n", __func__);
				return -EFAULT;
			}
		}	
	}
	else if(cmd == USER_GET_SLEEP_GPIO) {
		if (copy_from_user(&gpio_num, (void *) arg, sizeof(gpio_num))) {
			pr_err("%s: copy from user failed\n", __func__);
			return -EFAULT;
		}
		else {
			gpio_conf = sky_user_get_sleep_gpio(gpio_num);
			if (copy_to_user((void *) arg, &gpio_conf, sizeof(gpio_conf))) {
				pr_err("%s: copy to user failed\n", __func__);
				return -EFAULT;
			}
		}	
	}
	else if(cmd == USER_SET_SLEEP_GPIO){
		if (copy_from_user(&gpio_conf, (void *) arg, sizeof(gpio_conf))) {
			pr_err("%s: copy from user failed\n", __func__);
			return -EFAULT;
		}
		else {
			sky_user_set_sleep_gpio(gpio_conf);
		}
	}
	else {
		pr_err("Device does not support CMD\n");
	}
	return 0;
}
static struct file_operations user_gpio_fops = {
	.owner = THIS_MODULE,
	.ioctl = user_gpio_fops_ioctl,
};
static struct miscdevice user_gpio_event = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gpio_ctrl_fops",
	.fops = &user_gpio_fops,
};
#endif

static struct file_operations leds_fops = {
	.owner = THIS_MODULE,
	.ioctl = leds_fops_ioctl,
};

static struct miscdevice led_event = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "led_fops",
	.fops = &leds_fops,
};

static int leds_fops_ioctl(struct inode *inode, struct file *filp,
	       unsigned int cmd, unsigned long arg)
{
	//void __user *argp = (void __user *)arg;
	dbg("leds_fops_ioctl : %d\n",cmd);

//	if(cmd)
//		cmd = 102;
	
	pmic8058_led_set(&led_data[PMIC8058_ID_LED_KB_LIGHT].cdev, cmd);
#if defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
	led_lc_set(&led_data[PMIC8058_ID_LED_KB_LIGHT], (int)cmd);
#else  // PRESTO
	kp_bl_set(&led_data[PMIC8058_ID_LED_KB_LIGHT], (int)cmd);
#endif
	return true;
}

static void kp_bl_set(struct pmic8058_led_data *led, enum led_brightness value)
{
	int rc;
	u8 level;
	unsigned long flags;

	dbg_func_in();
	dbg("[LED] value = %d \n", value);
	
	spin_lock_irqsave(&led->value_lock, flags);
	level = (value << PM8058_DRV_KEYPAD_BL_SHIFT) &
				 PM8058_DRV_KEYPAD_BL_MASK;

	led->reg_kp &= ~PM8058_DRV_KEYPAD_BL_MASK;
	led->reg_kp |= level;
	spin_unlock_irqrestore(&led->value_lock, flags);

	rc = pm8058_write(led->pm_chip, SSBI_REG_ADDR_DRV_KEYPAD,
				 &led->reg_kp, 1);
	if (rc)
		pr_err("%s: can't set keypad backlight level\n", __func__);

	dbg_func_out();
}

#if defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
#else  // PRESTO
static enum led_brightness kp_bl_get(struct pmic8058_led_data *led)
{
	if ((led->reg_kp & PM8058_DRV_KEYPAD_BL_MASK) >>
			 PM8058_DRV_KEYPAD_BL_SHIFT)
		return LED_FULL;
	else
		return LED_OFF;
}

#endif
static void led_lc_set(struct pmic8058_led_data *led, enum led_brightness value)
{
	unsigned long flags;
	int rc, offset;
	u8 level, tmp;

	dbg_func_in();

	spin_lock_irqsave(&led->value_lock, flags);

	level = (led->brightness << PM8058_DRV_LED_CTRL_SHIFT) &
		PM8058_DRV_LED_CTRL_MASK;

#if defined(CONFIG_MACH_MSM8X60_EF39S) 
	offset = PMIC8058_LED_OFFSET(PMIC8058_ID_LED_2);
#elif defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
	offset = PMIC8058_LED_OFFSET(PMIC8058_ID_LED_1);
#else
	offset = PMIC8058_LED_OFFSET(led->id);
#endif
	tmp = led->reg_led_ctrl[offset];

	tmp &= ~PM8058_DRV_LED_CTRL_MASK;
	tmp |= level;
	spin_unlock_irqrestore(&led->value_lock, flags);

	rc = pm8058_write(led->pm_chip,	SSBI_REG_ADDR_LED_CTRL(offset),
			&tmp, 1);
	if (rc) {
		dev_err(led->cdev.dev, "can't set (%d) led value\n",
				led->id);
		return;
	}
#if defined(CONFIG_MACH_MSM8X60_EF39S)
  if(offset ==2) {
    offset=1;
    dbg(" defined(CONFIG_MACH_MSM8X60_EF39S) SECOND LEDS START \n");
    rc = pm8058_write(led->pm_chip,	SSBI_REG_ADDR_LED_CTRL(offset),
			&tmp, 1);
		if (rc) {
			dev_err(led->cdev.dev, "can't set (%d) led value\n",
					led->id);
			return;
		}
	}
#endif
	spin_lock_irqsave(&led->value_lock, flags);
	led->reg_led_ctrl[offset] = tmp;
	spin_unlock_irqrestore(&led->value_lock, flags);

	dbg_func_out();
}

static enum led_brightness led_lc_get(struct pmic8058_led_data *led)
{
	int offset;
	u8 value;

	dbg("led_lc_get : %d\n", led->id);

#if defined(CONFIG_MACH_MSM8X60_EF39S) 
	offset = PMIC8058_LED_OFFSET(PMIC8058_ID_LED_2);
#elif defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
	offset = PMIC8058_LED_OFFSET(PMIC8058_ID_LED_1);
#else
	offset = PMIC8058_LED_OFFSET(led->id);
#endif
	value = led->reg_led_ctrl[offset];

	if ((value & PM8058_DRV_LED_CTRL_MASK) >>
			PM8058_DRV_LED_CTRL_SHIFT)
		return LED_FULL;
	else
		return LED_OFF;
}

static void
led_flash_set(struct pmic8058_led_data *led, enum led_brightness value)
{
	int rc;
	u8 level;
	unsigned long flags;
	u8 reg_flash_led;
	u16 reg_addr;

	spin_lock_irqsave(&led->value_lock, flags);
	level = (value << PM8058_DRV_FLASH_SHIFT) &
				 PM8058_DRV_FLASH_MASK;

	if (led->id == PMIC8058_ID_FLASH_LED_0) {
		led->reg_flash_led0 &= ~PM8058_DRV_FLASH_MASK;
		led->reg_flash_led0 |= level;
		reg_flash_led	    = led->reg_flash_led0;
		reg_addr	    = SSBI_REG_ADDR_FLASH_DRV0;
	} else {
		led->reg_flash_led1 &= ~PM8058_DRV_FLASH_MASK;
		led->reg_flash_led1 |= level;
		reg_flash_led	    = led->reg_flash_led1;
		reg_addr	    = SSBI_REG_ADDR_FLASH_DRV1;
	}
	spin_unlock_irqrestore(&led->value_lock, flags);

	rc = pm8058_write(led->pm_chip, reg_addr, &reg_flash_led, 1);
	if (rc)
		pr_err("%s: can't set flash led%d level %d\n", __func__,
			led->id, rc);
}

int pm8058_set_flash_led_current(enum pmic8058_leds id, unsigned mA)
{
	struct pmic8058_led_data *led;

	if ((id < PMIC8058_ID_FLASH_LED_0) || (id > PMIC8058_ID_FLASH_LED_1)) {
		pr_err("%s: invalid LED ID (%d) specified\n", __func__, id);
		return -EINVAL;
	}

	led = &led_data[id];
	if (!led) {
		pr_err("%s: flash led not available\n", __func__);
		return -EINVAL;
	}

	if (mA > MAX_FLASH_CURRENT)
		return -EINVAL;

	led_flash_set(led, mA / 20);

	return 0;
}
EXPORT_SYMBOL(pm8058_set_flash_led_current);

int pm8058_set_led_current(enum pmic8058_leds id, unsigned mA)
{
	struct pmic8058_led_data *led;
	int brightness = 0;

	if ((id < PMIC8058_ID_LED_KB_LIGHT) || (id > PMIC8058_ID_FLASH_LED_1)) {
		pr_err("%s: invalid LED ID (%d) specified\n", __func__, id);
		return -EINVAL;
	}

	led = &led_data[id];
	if (!led) {
		pr_err("%s: flash led not available\n", __func__);
		return -EINVAL;
	}

	switch (id) {
	case PMIC8058_ID_LED_0:
	case PMIC8058_ID_LED_1:
	case PMIC8058_ID_LED_2:
		brightness = mA / 2;
		if (brightness  > led->cdev.max_brightness)
			return -EINVAL;
		led_lc_set(led, brightness);
		break;

	case PMIC8058_ID_LED_KB_LIGHT:
	case PMIC8058_ID_FLASH_LED_0:
	case PMIC8058_ID_FLASH_LED_1:
		brightness = mA / 20;
		if (brightness  > led->cdev.max_brightness)
			return -EINVAL;
		if (id == PMIC8058_ID_LED_KB_LIGHT)
			kp_bl_set(led, brightness);
		else
			led_flash_set(led, brightness);
		break;
	}

	return 0;
}
EXPORT_SYMBOL(pm8058_set_led_current);

static void pmic8058_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	struct pmic8058_led_data *led;
	unsigned long flags;

	dbg_func_in();

	//dbg("value = %d ", value);
	dbg("[LED] value = %d \n", value);
//#if defined(CONFIG_MACH_MSM8X60_EF39S)
	value = (value & 0xF0) >> 4;
//#endif
	dbg("--> value = %d\n", value);

	led = container_of(led_cdev, struct pmic8058_led_data, cdev);

	spin_lock_irqsave(&led->value_lock, flags);
	led->brightness = value;
	schedule_work(&led->work);
	spin_unlock_irqrestore(&led->value_lock, flags);
}

static void pmic8058_led_work(struct work_struct *work)
{
	struct pmic8058_led_data *led = container_of(work,
					 struct pmic8058_led_data, work);

	mutex_lock(&led->lock);

	dbg("pmic8058_led_work : %d\n", led->id);

	switch (led->id) {
	case PMIC8058_ID_LED_KB_LIGHT:
#if defined(CONFIG_MACH_MSM8X60_EF39S)||defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
		led_lc_set(led, led->brightness);
#else // PRESTO
		kp_bl_set(led, led->brightness);
#endif
		break;
	case PMIC8058_ID_LED_0:
		led_lc_set(led, led->brightness);
	case PMIC8058_ID_LED_1:
		led_lc_set(led, led->brightness);
	case PMIC8058_ID_LED_2:
		led_lc_set(led, led->brightness);
		break;
	case PMIC8058_ID_FLASH_LED_0:
	case PMIC8058_ID_FLASH_LED_1:
		led_flash_set(led, led->brightness);
		break;
	}

	mutex_unlock(&led->lock);
}

static enum led_brightness pmic8058_led_get(struct led_classdev *led_cdev)
{
	struct pmic8058_led_data *led;

	led = container_of(led_cdev, struct pmic8058_led_data, cdev);

	dbg("pmic8058_led_get : %d\n", led->id);

	switch (led->id) {
	case PMIC8058_ID_LED_KB_LIGHT:
#if defined(CONFIG_MACH_MSM8X60_EF39S)||defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)
		return led_lc_get(led);
#else // PRESTO
		return kp_bl_get(led);
#endif
	case PMIC8058_ID_LED_0:
	case PMIC8058_ID_LED_1:
//#if defined(CONFIG_MACH_MSM8X60_EF39S)
		return led_lc_get(led);
//#else
//		return kp_bl_get(led);
//#endif
	case PMIC8058_ID_LED_2:
		return led_lc_get(led);
	}
	return LED_OFF;
}

static int pmic8058_led_probe(struct platform_device *pdev)
{
	struct pmic8058_leds_platform_data *pdata = pdev->dev.platform_data;
	struct pmic8058_led_data *led_dat;
	struct pmic8058_led *curr_led;
	int rc, i = 0;
	struct pm8058_chip	*pm_chip;
	u8			reg_kp;
	u8			reg_led_ctrl[3];
	u8			reg_flash_led0;
	u8			reg_flash_led1;

	dbg_func_in();

	pm_chip = platform_get_drvdata(pdev);
	if (pm_chip == NULL) {
		dev_err(&pdev->dev, "no parent data passed in\n");
		return -EFAULT;
	}

	if (pdata == NULL) {
		dev_err(&pdev->dev, "platform data not supplied\n");
		return -EINVAL;
	}

	rc = pm8058_read(pm_chip, SSBI_REG_ADDR_DRV_KEYPAD, &reg_kp,
				1);
	if (rc) {
		dev_err(&pdev->dev, "can't get keypad backlight level\n");
		goto err_reg_read;
	}

	rc = pm8058_read(pm_chip, SSBI_REG_ADDR_LED_CTRL_BASE,
			reg_led_ctrl, 3);
	if (rc) {
		dev_err(&pdev->dev, "can't get led levels\n");
		goto err_reg_read;
	}

	rc = pm8058_read(pm_chip, SSBI_REG_ADDR_FLASH_DRV0,
			&reg_flash_led0, 1);
	if (rc) {
		dev_err(&pdev->dev, "can't read flash led0\n");
		goto err_reg_read;
	}

	rc = pm8058_read(pm_chip, SSBI_REG_ADDR_FLASH_DRV1,
			&reg_flash_led1, 1);
	if (rc) {
		dev_err(&pdev->dev, "can't get flash led1\n");
		goto err_reg_read;
	}

	for (i = 0; i < pdata->num_leds; i++) {
		curr_led	= &pdata->leds[i];
		led_dat		= &led_data[curr_led->id];

		led_dat->cdev.name		= curr_led->name;
		led_dat->cdev.default_trigger   = curr_led->default_trigger;
		led_dat->cdev.brightness_set    = pmic8058_led_set;
		led_dat->cdev.brightness_get    = pmic8058_led_get;
		led_dat->cdev.brightness	= LED_OFF;
//#if defined(CONFIG_MACH_MSM8X60_EF39S)
		led_dat->cdev.max_brightness	= 255;
//#else
//		led_dat->cdev.max_brightness	= curr_led->max_brightness;
//#endif
		led_dat->cdev.flags		= LED_CORE_SUSPENDRESUME;

		led_dat->id		        = curr_led->id;
		led_dat->reg_kp			= reg_kp;
		memcpy(led_data->reg_led_ctrl, reg_led_ctrl,
					 sizeof(reg_led_ctrl));
		led_dat->reg_flash_led0		= reg_flash_led0;
		led_dat->reg_flash_led1		= reg_flash_led1;

		if (!((led_dat->id >= PMIC8058_ID_LED_KB_LIGHT) &&
				(led_dat->id <= PMIC8058_ID_FLASH_LED_1))) {
			dev_err(&pdev->dev, "warning: invalid LED ID (%d) specified\n",
						 led_dat->id);
			rc = -EINVAL;
			goto fail_id_check;
		}

		led_dat->pm_chip		= pm_chip;

		mutex_init(&led_dat->lock);
		spin_lock_init(&led_dat->value_lock);
		INIT_WORK(&led_dat->work, pmic8058_led_work);

		rc = led_classdev_register(&pdev->dev, &led_dat->cdev);
		if (rc) {
			dev_err(&pdev->dev, "unable to register led %d\n",
						 led_dat->id);
			goto fail_id_check;
		}
	}

	platform_set_drvdata(pdev, led_data);
	// p12279 Added TestMenu
	misc_register(&led_event);
#ifdef CONFIG_USER_GPIO_CONTROL
	misc_register(&user_gpio_event);
#endif
	dbg_func_out();

	return 0;

err_reg_read:
fail_id_check:
	if (i > 0) {
		for (i = i - 1; i >= 0; i--)
			led_classdev_unregister(&led_data[i].cdev);
	}
	return rc;
}

static int __devexit pmic8058_led_remove(struct platform_device *pdev)
{
	int i;
	struct pmic8058_leds_platform_data *pdata = pdev->dev.platform_data;
	struct pmic8058_led_data *led = platform_get_drvdata(pdev);

	for (i = 0; i < pdata->num_leds; i++) {
		led_classdev_unregister(&led[led->id].cdev);
		cancel_work_sync(&led[led->id].work);
	}

	return 0;
}

static struct platform_driver pmic8058_led_driver = {
	.probe		= pmic8058_led_probe,
	.remove		= __devexit_p(pmic8058_led_remove),
	.driver		= {
		.name	= "pm8058-led",
		.owner	= THIS_MODULE,
	},
};

static int __init pmic8058_led_init(void)
{
	return platform_driver_register(&pmic8058_led_driver);
}
module_init(pmic8058_led_init);

static void __exit pmic8058_led_exit(void)
{
	platform_driver_unregister(&pmic8058_led_driver);
}
module_exit(pmic8058_led_exit);

MODULE_DESCRIPTION("PMIC8058 LEDs driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:pmic8058-led");
