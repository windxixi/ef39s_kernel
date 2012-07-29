/*
* Customer code to add GPIO control during WLAN start/stop
* Copyright (C) 1999-2010, Broadcom Corporation
* 
*      Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2 (the "GPL"),
* available at http://www.broadcom.com/licenses/GPLv2.php, with the
* following added to such license:
* 
*      As a special exception, the copyright holders of this software give you
* permission to link this software with independent modules, and to copy and
* distribute the resulting executable under terms of your choice, provided that
* you also meet, for each linked independent module, the terms and conditions of
* the license of that module.  An independent module is a module which is not
* derived from this software.  The special exception does not apply to any
* modifications of the software.
* 
*      Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*
* $Id: dhd_custom_gpio.c,v 1.1.4.2.20.4.2.2 2010/11/09 02:06:54 Exp $
*/


#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <bcmutils.h>

#include <dngl_stats.h>
#include <dhd.h>

#include <wlioctl.h>
#include <wl_iw.h>

#ifdef CUSTOMER_HW_PT
#include <asm/gpio.h>
#include <linux/random.h>
#endif

#define WL_ERROR(x) printf x
#define WL_TRACE(x)

#if defined(CUSTOMER_HW_PT) && !defined(CONFIG_MACH_MAHIMAHI)
static int dhd_wl_reset_gpio = CONFIG_BCM4329_WL_RESET_GPIO;
#endif
#ifdef CUSTOMER_HW
extern  void bcm_wlan_power_off(int);
extern  void bcm_wlan_power_on(int);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_MACH_MAHIMAHI
int wifi_set_carddetect(int on);
int wifi_set_power(int on, unsigned long msec);
int wifi_get_irq_number(unsigned long *irq_flags_ptr);
int wifi_get_mac_addr(unsigned char *buf);
#endif

#if defined(OOB_INTR_ONLY)

#if defined(BCMLXSDMMC)
extern int sdioh_mmc_irq(int irq);
#endif /* (BCMLXSDMMC)  */
#ifdef CUSTOMER_HW3
#include <mach/gpio.h>
#endif

/* Customer specific Host GPIO defintion  */
static int dhd_oob_gpio_num = -1;

module_param(dhd_oob_gpio_num, int, 0644);
MODULE_PARM_DESC(dhd_oob_gpio_num, "DHD oob gpio number");

/* that function will returns :
    1) return :  Host gpio interrupt number per customer platform
    2) irq_flags_ptr : Type of Host interrupt as Level or Edge

    NOTE :
    Customer should check his platform definitions
    and hist Host Interrupt  spec
    to figure out the proper setting for his platform.
    BRCM provides just reference settings as example.

*/
int dhd_customer_oob_irq_map(unsigned long *irq_flags_ptr)
{
	int  host_oob_irq = 0;

#ifdef CONFIG_MACH_MAHIMAHI
	host_oob_irq = wifi_get_irq_number(irq_flags_ptr);

#else /* for NOT  CONFIG_MACH_MAHIMAHI */
#if defined(CUSTOM_OOB_GPIO_NUM)
	if (dhd_oob_gpio_num < 0) {
		dhd_oob_gpio_num = CUSTOM_OOB_GPIO_NUM;
	}
#endif

	if (dhd_oob_gpio_num < 0) {
		WL_ERROR(("%s: ERROR customer specific Host GPIO is NOT defined \n",
		__FUNCTION__));
		return (dhd_oob_gpio_num);
	}

	WL_ERROR(("%s: customer specific Host GPIO number is (%d)\n",
	         __FUNCTION__, dhd_oob_gpio_num));

#if defined (CUSTOMER_HW) || defined(CUSTOMER_HW_PT)
	host_oob_irq = MSM_GPIO_TO_INT(dhd_oob_gpio_num);
#elif defined CUSTOMER_HW3
	gpio_request(dhd_oob_gpio_num, "oob irq");
	host_oob_irq = gpio_to_irq(dhd_oob_gpio_num);
	gpio_direction_input(dhd_oob_gpio_num);
#endif /* CUSTOMER_HW */
#endif /* CONFIG_MACH_MAHIMAHI */

	return (host_oob_irq);
}
#endif /* defined(OOB_INTR_ONLY) */

/* Customer function to control hw specific wlan gpios */
void
dhd_customer_gpio_wlan_ctrl(int onoff)
{
	switch (onoff) {
		case WLAN_RESET_OFF:
			WL_TRACE(("%s: call customer specific GPIO to insert WLAN RESET\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_off(2);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_MACH_MAHIMAHI
			wifi_set_power(0, 0);
#elif defined(CUSTOMER_HW_PT)
			gpio_set_value(dhd_wl_reset_gpio, 0);
			printk("WLAN_RESET_OFF[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
			WL_ERROR(("=========== WLAN placed in RESET ========\n"));
		break;

		case WLAN_RESET_ON:
			WL_TRACE(("%s: callc customer specific GPIO to remove WLAN RESET\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_on(2);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_MACH_MAHIMAHI
			wifi_set_power(1, 0);
#elif defined(CUSTOMER_HW_PT)
			gpio_set_value(dhd_wl_reset_gpio, 1);
			printk("WLAN_RESET_ON[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
			WL_ERROR(("=========== WLAN going back to live  ========\n"));
		break;

		case WLAN_POWER_OFF:
			WL_TRACE(("%s: call customer specific GPIO to turn off WL_REG_ON\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_off(1);
#endif /* CUSTOMER_HW */
#if defined(CUSTOMER_HW_PT) && !defined(CONFIG_MACH_MAHIMAHI)
			gpio_set_value(dhd_wl_reset_gpio, 0);
			printk("WLAN_POWER_OFF[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
		break;

		case WLAN_POWER_ON:
			WL_TRACE(("%s: call customer specific GPIO to turn on WL_REG_ON\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_on(1);
#endif /* CUSTOMER_HW */
#if defined(CUSTOMER_HW_PT) && !defined(CONFIG_MACH_MAHIMAHI)
			gpio_set_value(dhd_wl_reset_gpio, 1);
			printk("WLAN_POWER_ON[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
			/* Lets customer power to get stable */
			OSL_DELAY(200);
		break;
	}
}

#ifdef GET_CUSTOM_MAC_ENABLE
#ifdef CUSTOMER_HW_PT
static int
dhd_read_mac_from_file(char *fpath, struct ether_addr *addr)
{
	void *fp = NULL;
	int len;
	int ret = -1;
	char ethbuf[32] = {0, };
	struct ether_addr tmp;

	if ( !fpath || !addr )
		return -1;

	if ((fp = dhd_os_open_image(fpath)) == NULL)
		return -1;

	len = dhd_os_get_image_block(ethbuf, 32, fp);
	if ( len < 17 ) /* mac address format xx:xx:xx:xx:xx:xx */
		goto err;

	ethbuf[17] = '\0';
	if (sscanf(ethbuf, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&tmp.octet[0], &tmp.octet[1], &tmp.octet[2],
			&tmp.octet[3], &tmp.octet[4], &tmp.octet[5]) != 6)
		goto err;

	memcpy(addr, &tmp, sizeof(struct ether_addr));

	ret = 0;
err:
	dhd_os_close_image(fp);
	return ret;
}

static int
dhd_write_mac_to_file(char *fpath, struct ether_addr *addr)
{
	struct file *fp = NULL;
	char ethbuf[32] = {0, };
	mm_segment_t old_fs;
	loff_t pos;

	if ( !fpath || !addr )
		return -1;

	fp = filp_open(fpath, O_CREAT | O_RDWR, 0666);
	if (IS_ERR(fp))
		return -1;

	sprintf(ethbuf, "%02x:%02x:%02x:%02x:%02x:%02x",
			addr->octet[0], addr->octet[1], addr->octet[2],
			addr->octet[3], addr->octet[4], addr->octet[5]);

	old_fs = get_fs();
	set_fs(get_ds());
	pos = fp->f_pos;
	vfs_write(fp, ethbuf, 18, &pos);
	set_fs(old_fs);

	filp_close(fp, NULL);

	return 0;
}
#endif
/* Function to get custom MAC address */
int
dhd_custom_get_mac_address(unsigned char *buf)
{
	int ret = 0;

	WL_TRACE(("%s Enter\n", __FUNCTION__));
	if (!buf)
		return -EINVAL;

	/* Customer access to MAC address stored outside of DHD driver */
#if defined(CONFIG_MACH_MAHIMAHI) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	/* Lin - this will call wifi_control_data->get_mac_addr (dhd_linux.c) */
	ret = wifi_get_mac_addr(buf);
#endif

#ifdef EXAMPLE_GET_MAC
	/* EXAMPLE code */
	{
		struct ether_addr ea_example = {{0x00, 0x11, 0x22, 0x33, 0x44, 0xFF}};
		bcopy((char *)&ea_example, buf, sizeof(struct ether_addr));
	}
#endif /* EXAMPLE_GET_MAC */

#ifdef CUSTOMER_HW_PT
	{
		char panmac_path[] = "/dev/panmac";
		static struct ether_addr mac;
		static int custom_mac_applied = 0;

		if ( custom_mac_applied )
		{
			memcpy(buf, &mac, sizeof(struct ether_addr));
			return 0;
		}

		ret = dhd_read_mac_from_file(panmac_path, &mac);

		if ( ret == 0 )
		{
			memcpy(buf, &mac, sizeof(struct ether_addr));
			custom_mac_applied = 1;
		}
		else
		{
			int i;
			char rand_panmac_path[] = "/data/misc/wifi/panmac";

			ret = dhd_read_mac_from_file(rand_panmac_path, &mac);
			if ( ret != 0 )
			{
				mac.octet[0] = 0x00;
				mac.octet[1] = 0x0f;
				mac.octet[2] = 0xe4;
				for ( i = 3; i < ETHER_ADDR_LEN; i++ )
					get_random_bytes(&mac.octet[i], 1);

				dhd_write_mac_to_file(rand_panmac_path, &mac);
			}
			memcpy(buf, &mac, sizeof(struct ether_addr));
			custom_mac_applied = 1;
			ret = 0;
		}
	}
#endif

	return ret;
}
#endif /* GET_CUSTOM_MAC_ENABLE */
