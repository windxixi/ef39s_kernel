/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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
 *
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/mfd/pmic8058.h>

#include <linux/input/pmic8058-keypad.h>
#include <linux/pmic8058-batt-alarm.h>
#include <linux/pmic8058-pwrkey.h>
#include <linux/rtc/rtc-pm8058.h>
#include <linux/pmic8058-vibrator.h>
#include <linux/leds.h>
#include <linux/pmic8058-othc.h>
#include <linux/mfd/pmic8901.h>
#include <linux/regulator/pmic8058-regulator.h>
#include <linux/regulator/pmic8901-regulator.h>
#include <linux/bootmem.h>
#include <linux/pwm.h>
#include <linux/pmic8058-pwm.h>
#include <linux/leds-pmic8058.h>
#include <linux/pmic8058-xoadc.h>
#include <linux/msm_adc.h>
#include <linux/m_adcproc.h>
#include <linux/mfd/marimba.h>
#include <linux/msm-charger.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/smsc911x.h>
#include <linux/spi/spi.h>
#include <linux/input/tdisc_shinetsu.h>
#include <linux/input/cy8c_ts.h>
#include <linux/cyttsp.h>
#include <linux/i2c/isa1200.h>
#include <linux/dma-mapping.h>
#include <linux/i2c/bq27520.h>

#include <linux/spi/spi.h>
#ifdef CONFIG_SPI_GPIO
#include <linux/spi/spi_gpio.h>
#endif

#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif

#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
#include <linux/i2c/smb137b.h>
#endif
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>

#include <mach/dma.h>
#include <mach/mpp.h>
#include <mach/board.h>
#include <mach/irqs.h>
#include <mach/msm_spi.h>
#include <mach/msm_serial_hs.h>
#include <mach/msm_serial_hs_lite.h>
#include <mach/msm_iomap.h>
#include <asm/mach/mmc.h>
#include <mach/msm_battery.h>
#include <mach/msm_hsusb.h>
#ifdef CONFIG_MSM_DSPS
#include <mach/msm_dsps.h>
#endif
#include <mach/msm_xo.h>
#include <mach/msm_bus_board.h>
#include <mach/socinfo.h>
#include <linux/i2c/isl9519.h>
#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android_composite.h>
#endif
#include <mach/usb_gadget_fserial.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <mach/sdio_al.h>
#include <mach/rpm.h>
#include <mach/rpm-regulator.h>

#include "devices.h"
#include "devices-msm8x60.h"
#include "cpuidle.h"
#include "pm.h"
#include "mpm.h"
#include "spm.h"
#include "rpm_log.h"
#include "timer.h"
#include "saw-regulator.h"
#include "gpiomux.h"
#include "gpiomux-8x60.h"
#include "rpm_stats.h"
#include <linux/i2c-gpio.h>

#ifdef CONFIG_SKY_SND_EXTAMP /* elecjang 20110421 for max97001 subsystem */
#include <linux/i2c-gpio.h>
#include "./qdsp6v2/sky_snd_ext_amp_max97001.h"
#endif

#ifdef CONFIG_WIFI_CONTROL_FUNC //platform device
#include <linux/wlan_plat.h>
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
#include <linux/i2c-gpio.h>
#include "./qdsp6v2/snd_presto_audience_a2020.h"
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200  // jmlee 20110505 add
#include <linux/i2c-gpio.h>
#include "./qdsp6v2/snd_presto_sub_fab2200.h"
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
#define I2C_SCL_AUDIENCE      73  
#define I2C_SDA_AUDIENCE     72  
static struct i2c_gpio_platform_data i2c_gpio_audience_data = {
	.scl_pin = I2C_SCL_AUDIENCE,
	.sda_pin = I2C_SDA_AUDIENCE,
	//.udelay = 5,
	.udelay = 2,  // 20110908 jmlee 100KHz(udelay 5) --> 400KHz(udelay 2) boot time issue
};
struct platform_device msm_i2c_gpio_device_audience = {
	.name = "i2c-gpio",
	.id = MSM_AUDIENCE_A2020_I2C_BUS_ID,
	.dev	= {
		.platform_data = &i2c_gpio_audience_data,
	},
};

#endif
#include "peripheral-loader.h"
#define MSM_SHARED_RAM_PHYS 0x40000000

#ifdef CONFIG_MACH_MSM8X60_EF39S  // 20110429, FEATURE-WORK
#define LCD_BL_I2C_CONTROL  // 20110405, kkcho, for EF39S Backlight-control
#endif

#ifdef	CONFIG_INPUT_SENSOR
#include "msm8x60-sky-sensor.h"
#endif
#ifdef CONFIG_FB_MSM_MHL_SII9244  // MHL_KKCHO
#define PANTECH_MHL_SII9244_POWER_CTRL // 20110614, kkcho
#define MHL_I2C_CONTROL // 20110405, kkcho, for EF39S MHL-control
#ifndef CONFIG_F_SKYDISP_HDMI_OPTION
#define CONFIG_F_SKYDISP_HDMI_OPTION
#endif /*CONFIG_F_SKYDISP_HDMI_OPTION*/
#endif /*CONFIG_FB_MSM_MHL_SII9244*/

#if defined(CONFIG_PN544)
#include <linux/pn544.h>

#define NFC_I2C_SDA             72
#define NFC_I2C_SCL             73
#define NFC_WAKEUP_GPIO         105
#define NFC_ENABLE_GPIO         153
#define NFC_FW_DL_GPIO          142
#define NFC_SLAVE_ADDR		    0x28	
#endif
//pz1946 20110907 interrupt pin change
#ifdef CONFIG_SKY_SMB_CHARGER
#define SC_STAT_IRQ_GPIO	63
#endif

/* Macros assume PMIC GPIOs start at 0 */
#define PM8058_GPIO_BASE			NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_GPIO_BASE)
#define PM8058_MPP_BASE			(PM8058_GPIO_BASE + PM8058_GPIOS)
#define PM8058_MPP_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_MPP_BASE)
#define PM8058_MPP_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_MPP_BASE)
#define PM8058_IRQ_BASE				(NR_MSM_IRQS + NR_GPIO_IRQS)

#define PM8901_GPIO_BASE			(PM8058_GPIO_BASE + \
						PM8058_GPIOS + PM8058_MPPS)
#define PM8901_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8901_GPIO_BASE)
#define PM8901_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM901_GPIO_BASE)
#define PM8901_IRQ_BASE				(PM8058_IRQ_BASE + \
						NR_PMIC8058_IRQS)

#define MDM2AP_SYNC 129

#ifdef CONFIG_MACH_MSM8X60_PRESTO  // kkcho_temp_presto  // p13777 kej 110623
#define LCDC_SPI_GPIO_CLK				40//73
#define LCDC_SPI_GPIO_CS				33//72
#define LCDC_SPI_GPIO_MOSI				39//70
#else
#define LCDC_SPI_GPIO_CLK				73
#define LCDC_SPI_GPIO_CS				72
#define LCDC_SPI_GPIO_MOSI				70
#endif

#define LCDC_AUO_PANEL_NAME				"lcdc_auo_wvga"
#define LCDC_SAMSUNG_OLED_PANEL_NAME	"lcdc_samsung_oled"
#define LCDC_SAMSUNG_WSVGA_PANEL_NAME	"lcdc_samsung_wsvga"
#define LCDC_SAMSUNG_SPI_DEVICE_NAME	"lcdc_samsung_ams367pe02"
#define LCDC_AUO_SPI_DEVICE_NAME		"lcdc_auo_nt35582"

#define DSPS_PIL_GENERIC_NAME		"dsps"
#define DSPS_PIL_FLUID_NAME		"dsps_fluid"


#define CONFIG_PANTECH_BT 1 // lsi@ps2.20110408 bluez by KSJ_device 2011_05_12
enum {
	GPIO_EXPANDER_IRQ_BASE  = PM8901_IRQ_BASE + NR_PMIC8901_IRQS,
	GPIO_EXPANDER_GPIO_BASE = PM8901_GPIO_BASE + PM8901_MPPS,
	/* CORE expander */
	GPIO_CORE_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE,
	GPIO_CLASS_D1_EN        = GPIO_CORE_EXPANDER_BASE,
	GPIO_WLAN_DEEP_SLEEP_N,
	GPIO_LVDS_SHUTDOWN_N,
	GPIO_DISP_RESX_N        = GPIO_LVDS_SHUTDOWN_N,
	GPIO_MS_SYS_RESET_N,
	GPIO_CAP_TS_RESOUT_N,
	GPIO_CAP_GAUGE_BI_TOUT,
	GPIO_ETHERNET_PME,
	GPIO_EXT_GPS_LNA_EN,
	GPIO_MSM_WAKES_BT,
	GPIO_ETHERNET_RESET_N,
	GPIO_HEADSET_DET_N,
	GPIO_USB_UICC_EN,
	GPIO_BACKLIGHT_EN,
	GPIO_EXT_CAMIF_PWR_EN,
	GPIO_BATT_GAUGE_INT_N,
	GPIO_BATT_GAUGE_EN,
	/* DOCKING expander */
	GPIO_DOCKING_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + 16,
	GPIO_MIPI_DSI_RST_N        = GPIO_DOCKING_EXPANDER_BASE,
	GPIO_AUX_JTAG_DET_N,
	GPIO_DONGLE_DET_N,
	GPIO_SVIDEO_LOAD_DET,
	GPIO_SVID_AMP_SHUTDOWN1_N,
	GPIO_SVID_AMP_SHUTDOWN0_N,
	GPIO_SDC_WP,
	GPIO_IRDA_PWDN,
	GPIO_IRDA_RESET_N,
	GPIO_DONGLE_GPIO0,
	GPIO_DONGLE_GPIO1,
	GPIO_DONGLE_GPIO2,
	GPIO_DONGLE_GPIO3,
	GPIO_DONGLE_PWR_EN,
	GPIO_EMMC_RESET_N,
	GPIO_TP_EXP2_IO15,
	/* SURF expander */
	GPIO_SURF_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 2),
	GPIO_SD_CARD_DET_1      = GPIO_SURF_EXPANDER_BASE,
	GPIO_SD_CARD_DET_2,
	GPIO_SD_CARD_DET_4,
	GPIO_SD_CARD_DET_5,
	GPIO_UIM3_RST,
	GPIO_SURF_EXPANDER_IO5,
	GPIO_SURF_EXPANDER_IO6,
	GPIO_ADC_I2C_EN,
	GPIO_SURF_EXPANDER_IO8,
	GPIO_SURF_EXPANDER_IO9,
	GPIO_SURF_EXPANDER_IO10,
	GPIO_SURF_EXPANDER_IO11,
	GPIO_SURF_EXPANDER_IO12,
	GPIO_SURF_EXPANDER_IO13,
	GPIO_SURF_EXPANDER_IO14,
	GPIO_SURF_EXPANDER_IO15,
	/* LEFT KB IO expander */
	GPIO_LEFT_KB_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 3),
	GPIO_LEFT_LED_1            = GPIO_LEFT_KB_EXPANDER_BASE,
	GPIO_LEFT_LED_2,
	GPIO_LEFT_LED_3,
	GPIO_LEFT_LED_WLAN,
	GPIO_JOYSTICK_EN,
	GPIO_CAP_TS_SLEEP,
	GPIO_LEFT_KB_IO6,
	GPIO_LEFT_LED_5,
	/* RIGHT KB IO expander */
	GPIO_RIGHT_KB_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 3) + 8,
	GPIO_RIGHT_LED_1            = GPIO_RIGHT_KB_EXPANDER_BASE,
	GPIO_RIGHT_LED_2,
	GPIO_RIGHT_LED_3,
	GPIO_RIGHT_LED_BT,
	GPIO_WEB_CAMIF_STANDBY,
	GPIO_COMPASS_RST_N,
	GPIO_WEB_CAMIF_RESET_N,
	GPIO_RIGHT_LED_5,
	GPIO_R_ALTIMETER_RESET_N,
	/* FLUID S IO expander */
	GPIO_SOUTH_EXPANDER_BASE,
	GPIO_MIC2_ANCR_SEL = GPIO_SOUTH_EXPANDER_BASE,
	GPIO_MIC1_ANCL_SEL,
	GPIO_HS_MIC4_SEL,
	GPIO_FML_MIC3_SEL,
	GPIO_FMR_MIC5_SEL,
	GPIO_TS_SLEEP,
	GPIO_HAP_SHIFT_LVL_OE,
	GPIO_HS_SW_DIR,
	/* FLUID N IO expander */
	GPIO_NORTH_EXPANDER_BASE,
	GPIO_EPM_3_3V_EN = GPIO_NORTH_EXPANDER_BASE,
	GPIO_EPM_5V_BOOST_EN,
	GPIO_AUX_CAM_2P7_EN,
	GPIO_LED_FLASH_EN,
	GPIO_LED1_GREEN_N,
	GPIO_LED2_RED_N,
	GPIO_FRONT_CAM_RESET_N,
	GPIO_EPM_LVLSFT_EN,
	GPIO_N_ALTIMETER_RESET_N,
	/* EPM expander */
	GPIO_EPM_EXPANDER_BASE,
	GPIO_PWR_MON_START = GPIO_EPM_EXPANDER_BASE,
	GPIO_PWR_MON_RESET_N,
	GPIO_ADC1_PWDN_N,
	GPIO_ADC2_PWDN_N,
	GPIO_EPM_EXPANDER_IO4,
	GPIO_ADC1_MUX_SPI_INT_N_3_3V,
	GPIO_ADC2_MUX_SPI_INT_N,
	GPIO_EPM_EXPANDER_IO7,
	GPIO_PWR_MON_ENABLE,
	GPIO_EPM_SPI_ADC1_CS_N,
	GPIO_EPM_SPI_ADC2_CS_N,
	GPIO_EPM_EXPANDER_IO11,
	GPIO_EPM_EXPANDER_IO12,
	GPIO_EPM_EXPANDER_IO13,
	GPIO_EPM_EXPANDER_IO14,
	GPIO_EPM_EXPANDER_IO15,
};

#ifdef LCD_BL_I2C_CONTROL
// 201110421, kkcho,  PT����, BL-pin control ����.
#if defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV == EV10)
#define LCD_BL_SCL	73
#define LCD_BL_SDA	72
#else
#define LCD_BL_SCL	21
#define LCD_BL_SDA	22
#endif
//	.udelay = 2,	/* 250 KHz */
static struct i2c_gpio_platform_data i2c_gpio_led_data = {
	.scl_pin = LCD_BL_SCL,
	.sda_pin = LCD_BL_SDA,
	.udelay = 5,	/* 100 KHz */
};

struct platform_device msm_device_i2c_gpio_led = {
	.name = "i2c-gpio",
	.id = MSM_LED_I2C_BUS_ID,
	.dev = {
		.platform_data = &i2c_gpio_led_data,
	}
};

static struct i2c_board_info i2c_led_devices[] __initdata = {
	{
		I2C_BOARD_INFO("MAX8831", 0x9A >> 1),  // 10011010
	},
};
#endif /* LCD_BL_I2C_CONTROL */

#ifdef CONFIG_TOUCHSCREEN_MELFAS_TKI
#define GPIO_TKI_SCL      52
#define GPIO_TKI_SDA      51
static struct i2c_gpio_platform_data i2c_gpio_tki_data = {
        .scl_pin = GPIO_TKI_SCL,
        .sda_pin = GPIO_TKI_SDA,
        .udelay = 5,    /* 100 KHz */
};

struct platform_device msm_device_i2c_gpio_tki = {
        .name = "i2c-gpio",
        .id = MSM_TKI_I2C_BUS_ID,
        .dev = {
                .platform_data = &i2c_gpio_tki_data,
        }
};

static struct i2c_board_info i2c_tki_devices[] __initdata = {
        {
//                I2C_BOARD_INFO("tki-i2c", 0x20 >> 1),
                I2C_BOARD_INFO("tki-i2c", 0x20),
        },
};
#endif /* CONFIG_TOUCHSCREEN_MELFAS_TKI */

#ifdef MHL_I2C_CONTROL
#define MHL_CSCL	27
#define MHL_CSDA	26

static struct i2c_gpio_platform_data i2c_gpio_mhl_data = {
	.scl_pin = MHL_CSCL,
	.sda_pin = MHL_CSDA,
	.udelay = 5,	/* 100 KHz */
	//	.udelay = 1,	/* 500 KHz */ // kkcho_temp
};

struct platform_device msm_device_i2c_gpio_mhl = {
	.name = "i2c-gpio",
	.id = MSM_MHL_I2C_BUS_ID,
	.dev = {
		.platform_data = &i2c_gpio_mhl_data,
	}
};

static struct i2c_board_info i2c_mhl_devices[] __initdata = {
	{
		I2C_BOARD_INFO("sii9244", 0x72 >> 1),
	},
	{
		I2C_BOARD_INFO("sii9244A", 0x7A >> 1),
	},
	{
		I2C_BOARD_INFO("sii9244B", 0x92 >> 1),
	},
	{
		I2C_BOARD_INFO("sii9244C", 0xC8 >> 1),
	},
};
#endif

/*
 * The UI_INTx_N lines are pmic gpio lines which connect i2c
 * gpio expanders to the pm8058.
 */
#define UI_INT1_N 25
#define UI_INT2_N 34
#define UI_INT3_N 14
/*
FM GPIO is GPIO 18 on PMIC 8058.
As the index starts from 0 in the PMIC driver, and hence 17
corresponds to GPIO 18 on PMIC 8058.
*/
#define FM_GPIO 17

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static void (*sdc2_status_notify_cb)(int card_present, void *dev_id);
static void *sdc2_status_notify_cb_devid;
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static void (*sdc5_status_notify_cb)(int card_present, void *dev_id);
static void *sdc5_status_notify_cb_devid;
#endif

static struct msm_spm_platform_data msm_spm_data_v1[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x0F,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0xFFFFFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFFFFFFFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0x94,
		.retention_vlevel = 0x81,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x94,
		.collapse_mid_vlevel = 0x8C,

		.vctl_timeout_us = 50,
	},

	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x0F,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0xFFFFFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFFFFFFFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x13,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0x94,
		.retention_vlevel = 0x81,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x94,
		.collapse_mid_vlevel = 0x8C,

		.vctl_timeout_us = 50,
	},
};

static struct msm_spm_platform_data msm_spm_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x1C,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x0C0CFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0x78780FFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0xA0,
		.retention_vlevel = 0x89,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x89,
		.collapse_mid_vlevel = 0x89,

		.vctl_timeout_us = 50,
	},

	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x1C,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x0C0CFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0x78780FFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x13,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0xA0,
		.retention_vlevel = 0x89,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x89,
		.collapse_mid_vlevel = 0x89,

		.vctl_timeout_us = 50,
	},
};
#ifdef CONFIG_INPUT_PANTECH_EARJACK
static struct platform_device pantech_earjack_device = {
    .name   = "pantech_earjack",
    .id     = -1,
};
#endif

static struct msm_acpu_clock_platform_data msm8x60_acpu_clock_data = {
};

static struct regulator_consumer_supply saw_s0_supply =
	REGULATOR_SUPPLY("8901_s0", NULL);
static struct regulator_consumer_supply saw_s1_supply =
	REGULATOR_SUPPLY("8901_s1", NULL);

static struct regulator_init_data saw_s0_init_data = {
		.constraints = {
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
			.min_uV = 840000,
			.max_uV = 1250000,
		},
		.num_consumer_supplies = 1,
		.consumer_supplies = &saw_s0_supply,
};

static struct regulator_init_data saw_s1_init_data = {
		.constraints = {
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
			.min_uV = 840000,
			.max_uV = 1250000,
		},
		.num_consumer_supplies = 1,
		.consumer_supplies = &saw_s1_supply,
};

static struct platform_device msm_device_saw_s0 = {
	.name          = "saw-regulator",
	.id            = SAW_VREG_ID_S0,
	.dev           = {
		.platform_data = &saw_s0_init_data,
	},
};

static struct platform_device msm_device_saw_s1 = {
	.name          = "saw-regulator",
	.id            = SAW_VREG_ID_S1,
	.dev           = {
		.platform_data = &saw_s1_init_data,
	},
};

/*
 * The smc91x configuration varies depending on platform.
 * The resources data structure is filled in at runtime.
 */
static struct resource smc91x_resources[] = {
	[0] = {
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device smc91x_device = {
	.name          = "smc91x",
	.id            = 0,
	.num_resources = ARRAY_SIZE(smc91x_resources),
	.resource      = smc91x_resources,
};

static struct resource smsc911x_resources[] = {
	[0] = {
		.flags = IORESOURCE_MEM,
		.start = 0x1b800000,
		.end   = 0x1b8000ff
	},
	[1] = {
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWLEVEL,
	},
};

static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type	= SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags		= SMSC911X_USE_16BIT,
	.has_reset_gpio	= 1,
	.reset_gpio	= GPIO_ETHERNET_RESET_N
};

static struct platform_device smsc911x_device = {
	.name          = "smsc911x",
	.id            = 0,
	.num_resources = ARRAY_SIZE(smsc911x_resources),
	.resource      = smsc911x_resources,
	.dev           = {
		.platform_data = &smsc911x_config
	}
};

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

#define QCE_SIZE		0x10000
#define QCE_0_BASE		0x18500000

#define QCE_HW_KEY_SUPPORT	0
#define QCE_SHA_HMAC_SUPPORT	0
#define QCE_SHARE_CE_RESOURCE	2
#define QCE_CE_SHARED		1

static struct resource qcrypto_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.name = "crypto_crci_hash",
		.start = DMOV_CE_HASH_CRCI,
		.end = DMOV_CE_HASH_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource qcedev_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.name = "crypto_crci_hash",
		.start = DMOV_CE_HASH_CRCI,
		.end = DMOV_CE_HASH_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)

static struct msm_ce_hw_support qcrypto_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
};

static struct platform_device qcrypto_device = {
	.name		= "qcrypto",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcrypto_resources),
	.resource	= qcrypto_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcrypto_ce_hw_suppport,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

static struct msm_ce_hw_support qcedev_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
};

static struct platform_device qcedev_device = {
	.name		= "qce",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcedev_resources),
	.resource	= qcedev_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcedev_ce_hw_suppport,
	},
};
#endif

#ifdef CONFIG_SKY_SMB_CHARGER // i2c
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
#define GPIO_SC_SCL      17
#define GPIO_SC_SDA      18
static struct i2c_gpio_platform_data i2c_gpio_smb_data = {
        .scl_pin = GPIO_SC_SCL,
        .sda_pin = GPIO_SC_SDA,
        .udelay = 5,    /* 100 KHz */
};

struct platform_device msm_device_i2c_gpio_smb = {
        .name = "i2c-gpio",
        .id = MSM_SMB_I2C_BUS_ID,
        .dev = {
                .platform_data = &i2c_gpio_smb_data,
        }
};
#endif
#endif

#if defined(CONFIG_HAPTIC_ISA1200) || \
		defined(CONFIG_HAPTIC_ISA1200_MODULE)

static const char *vregs_isa1200_name[] = {
	"8058_s3",
	"8901_l4",
};

static const int vregs_isa1200_val[] = {
	1800000,/* uV */
	2600000,
};
static struct regulator *vregs_isa1200[ARRAY_SIZE(vregs_isa1200_name)];
static struct msm_xo_voter *xo_handle_a1;

static int isa1200_power(int vreg_on)
{
	int i, rc = 0;

	for (i = 0; i < ARRAY_SIZE(vregs_isa1200_name); i++) {
		rc = vreg_on ? regulator_enable(vregs_isa1200[i]) :
			regulator_disable(vregs_isa1200[i]);
		if (rc < 0) {
			pr_err("%s: vreg %s %s failed (%d)\n",
				__func__, vregs_isa1200_name[i],
				vreg_on ? "enable" : "disable", rc);
			goto vreg_fail;
		}
	}

	rc = vreg_on ? msm_xo_mode_vote(xo_handle_a1, MSM_XO_MODE_ON) :
			msm_xo_mode_vote(xo_handle_a1, MSM_XO_MODE_OFF);
	if (rc < 0) {
		pr_err("%s: failed to %svote for TCXO A1 buffer%d\n",
				__func__, vreg_on ? "" : "de-", rc);
		goto vreg_fail;
	}
	return 0;

vreg_fail:
	while (i--)
		!vreg_on ? regulator_enable(vregs_isa1200[i]) :
			regulator_disable(vregs_isa1200[i]);
	return rc;
}

static int isa1200_dev_setup(bool enable)
{
	int i, rc;

	if (enable == true) {
		for (i = 0; i < ARRAY_SIZE(vregs_isa1200_name); i++) {
			vregs_isa1200[i] = regulator_get(NULL,
						vregs_isa1200_name[i]);
			if (IS_ERR(vregs_isa1200[i])) {
				pr_err("%s: regulator get of %s failed (%ld)\n",
					__func__, vregs_isa1200_name[i],
					PTR_ERR(vregs_isa1200[i]));
				rc = PTR_ERR(vregs_isa1200[i]);
				goto vreg_get_fail;
			}
			rc = regulator_set_voltage(vregs_isa1200[i],
				vregs_isa1200_val[i], vregs_isa1200_val[i]);
			if (rc) {
				pr_err("%s: regulator_set_voltage(%s) failed\n",
					__func__, vregs_isa1200_name[i]);
				goto vreg_get_fail;
			}
		}

		rc = gpio_request(GPIO_HAP_SHIFT_LVL_OE, "haptics_shft_lvl_oe");
		if (rc) {
			pr_err("%s: unable to request gpio %d (%d)\n",
					__func__, GPIO_HAP_SHIFT_LVL_OE, rc);
			goto vreg_get_fail;
		}

		rc = gpio_direction_output(GPIO_HAP_SHIFT_LVL_OE, 1);
		if (rc) {
			pr_err("%s: Unable to set direction\n", __func__);;
			goto free_gpio;
		}

		xo_handle_a1 = msm_xo_get(MSM_XO_TCXO_A1, "isa1200");
		if (IS_ERR(xo_handle_a1)) {
			rc = PTR_ERR(xo_handle_a1);
			pr_err("%s: failed to get the handle for A1(%d)\n",
							__func__, rc);
			goto gpio_set_dir;
		}
	} else {
		gpio_set_value(GPIO_HAP_SHIFT_LVL_OE, 0);
		gpio_free(GPIO_HAP_SHIFT_LVL_OE);

		for (i = 0; i < ARRAY_SIZE(vregs_isa1200_name); i++)
			regulator_put(vregs_isa1200[i]);

		msm_xo_put(xo_handle_a1);
	}

	return 0;
gpio_set_dir:
	gpio_set_value(GPIO_HAP_SHIFT_LVL_OE, 0);
free_gpio:
	gpio_free(GPIO_HAP_SHIFT_LVL_OE);
vreg_get_fail:
	while (i)
		regulator_put(vregs_isa1200[--i]);
	return rc;
}

#define PMIC_GPIO_HAP_ENABLE   18  /* PMIC GPIO Number 19 */
static struct isa1200_platform_data isa1200_1_pdata = {
	.name = "vibrator",
	.power_on = isa1200_power,
	.dev_setup = isa1200_dev_setup,
	/*gpio to enable haptic*/
	.hap_en_gpio = PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_HAP_ENABLE),
	.max_timeout = 15000,
	.mode_ctrl = PWM_GEN_MODE,
	.pwm_fd = {
		.pwm_div = 256,
	},
	.is_erm = false,
	.smart_en = true,
	.ext_clk_en = true,
	.chip_en = 1,
};

static struct i2c_board_info msm_isa1200_board_info[] = {
	{
		I2C_BOARD_INFO("isa1200_1", 0x90>>1),
		.platform_data = &isa1200_1_pdata,
	},
};
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200  // jmlee 20110505 add
static struct i2c_board_info msm_audio_i2c_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("fab2200-amp", 	FAB2200_SLAVE_ADDR),
		//I2C_BOARD_INFO("fab2200-amp", 	0x70),  // jmlee for test code temp for TI
	}
};
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
static struct i2c_board_info msm_audience_i2c_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("audience-a2020", 	A2020_SLAVE_ADDR),
		//I2C_BOARD_INFO("audience-a2020", 	0x70),  // jmlee for test code
	}
};
#endif


#ifdef CONFIG_SKY_SND_EXTAMP /* elecjang 20110421 for max97001 subsystem */
static struct i2c_board_info i2c_subsystem_devices[] __initdata = {
	{
		I2C_BOARD_INFO("max97001-amp", MAX97001_SLAVE_ADDR),
	},
};
#endif

#if defined(CONFIG_BATTERY_BQ27520) || \
		defined(CONFIG_BATTERY_BQ27520_MODULE)
static struct bq27520_platform_data bq27520_pdata = {
	.name		= "fuel-gauge",
	.vreg_name	= "8058_s3",
	.vreg_value	= 1800000,
	.soc_int	= GPIO_BATT_GAUGE_INT_N,
	.bi_tout	= GPIO_CAP_GAUGE_BI_TOUT,
	.chip_en	= GPIO_BATT_GAUGE_EN,
	.enable_dlog	= 0, /* if enable coulomb counter logger */
};

static struct i2c_board_info msm_bq27520_board_info[] = {
	{
		I2C_BOARD_INFO("bq27520", 0xaa>>1),
		.platform_data = &bq27520_pdata,
	},
};
#endif

static struct msm_pm_platform_data msm_pm_data[MSM_PM_SLEEP_MODE_NR * 2] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.supported = 1,
		.suspend_enabled = 0,
		.idle_enabled = 0,
		.latency = 4000,
		.residency = 13000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.supported = 1,
		.suspend_enabled = 0,
		.idle_enabled = 0,
		.latency = 500,
		.residency = 6000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.supported = 1,
		.suspend_enabled = 1,
		.idle_enabled = 1,
		.latency = 2,
		.residency = 0,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.supported = 1,
		.suspend_enabled = 0,
		.idle_enabled = 0,
		.latency = 600,
		.residency = 7200,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.supported = 1,
		.suspend_enabled = 0,
		.idle_enabled = 0,
		.latency = 500,
		.residency = 6000,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.supported = 1,
		.suspend_enabled = 1,
		.idle_enabled = 1,
		.latency = 2,
		.residency = 0,
	},
};

static struct msm_cpuidle_state msm_cstates[] __initdata = {
	{0, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{0, 1, "C1", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},

	{0, 2, "C2", "POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE},

	{1, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{1, 1, "C1", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},
};
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)

#define ISP1763_INT_GPIO		117
#define ISP1763_RST_GPIO		152
static struct resource isp1763_resources[] = {
	[0] = {
		.flags	= IORESOURCE_MEM,
		.start	= 0x1D000000,
		.end	= 0x1D005FFF,		/* 24KB */
	},
	[1] = {
		.flags	= IORESOURCE_IRQ,
	},
};
static void __init msm8x60_cfg_isp1763(void)
{
	isp1763_resources[1].start = gpio_to_irq(ISP1763_INT_GPIO);
	isp1763_resources[1].end = gpio_to_irq(ISP1763_INT_GPIO);
}

static int isp1763_setup_gpio(int enable)
{
	int status = 0;

	if (enable) {
		status = gpio_request(ISP1763_INT_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_INT_GPIO);
			return status;
		}
		status = gpio_direction_input(ISP1763_INT_GPIO);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_INT_GPIO);
			goto gpio_free_int;
		}
		status = gpio_request(ISP1763_RST_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_RST_GPIO);
			goto gpio_free_int;
		}
		status = gpio_direction_output(ISP1763_RST_GPIO, 1);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_RST_GPIO);
			goto gpio_free_rst;
		}
		pr_debug("\nISP GPIO configuration done\n");
		return status;
	}

gpio_free_rst:
	gpio_free(ISP1763_RST_GPIO);
gpio_free_int:
	gpio_free(ISP1763_INT_GPIO);

	return status;
}
static struct isp1763_platform_data isp1763_pdata = {
	.reset_gpio	= ISP1763_RST_GPIO,
	.setup_gpio	= isp1763_setup_gpio
};

static struct platform_device isp1763_device = {
	.name          = "isp1763_usb",
	.num_resources = ARRAY_SIZE(isp1763_resources),
	.resource      = isp1763_resources,
	.dev           = {
		.platform_data = &isp1763_pdata
	}
};
#endif
// added by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
static void mpu3050_power_up(void)
{
	struct regulator *vdd = regulator_get(NULL, "8058_l10");
	struct regulator *vldgc = regulator_get(NULL, "8058_lvs1");
	int rc, retry, retrylevel;

	//      retrylevel 0 : failed both vdd and vldgc
	//      retrylevel 1 : vdd is success but vldgc is fail
	//      retrylevel 2 : success both vdd and vldgc

	retry = 3;
	retrylevel = 0;
	do {
		retry--;
		printk(":::::: retry = %d / retrylevel = %d\n", retry, retrylevel);
		if(retrylevel == 1) {
			if(IS_ERR(vdd)) {
				rc = PTR_ERR(vdd);
				printk(KERN_ERR "<1-1> %s: regulator get of failed (%d)\n", __func__, rc);
			}
			else {
				rc = regulator_disable(vdd);
				if (rc) {
					printk(KERN_ERR "<1-2> %s: vreg disable failed (%d)\n",__func__, rc);
				}
				else {
					retrylevel = 0;
				}
			}
		}

		if(retrylevel == 0) {
			vdd = regulator_get(NULL, "8058_l10");
			if(IS_ERR(vdd)) {
				rc = PTR_ERR(vdd);
				printk(KERN_ERR "<2-1> %s: regulator get of failed (%d)\n", __func__, rc);
			}
			else {
				//---	'11/06/15 edited by hkkwon@PS2
				//rc = regulator_set_voltage(vdd, 3000000, 3000000);
				rc = regulator_set_voltage(vdd, 2600000, 2600000);
				if (rc) {
					printk(KERN_ERR "<2-2> %s: vreg set level failed (%d)\n", __func__, rc);
				}
				else {
					rc = regulator_enable(vdd);
					if (rc) {
						printk(KERN_ERR "<2-3> %s: vreg enable failed (%d)\n",__func__, rc);
					}
					else {
						retrylevel = 1;
					}
				}
			}
		}

		if(retrylevel == 1) {
			mdelay(10);
			vldgc = regulator_get(NULL, "8058_lvs1");
			if(IS_ERR(vldgc)) {
				rc = PTR_ERR(vldgc);
				printk(KERN_ERR "<3-1> %s: regulator get of failed (%d)\n", __func__, rc);
			}
			rc = regulator_enable(vldgc);
			if (rc) {
				printk(KERN_ERR "<3-2> %s: vreg enable failed (%d)\n",__func__, rc);
			}
			else {
				regulator_put(vldgc);
				retrylevel = 2;
				retry = 0;
			}
		}
		mdelay(100);
	} while(retry);

	if(retrylevel == 1) {
		vdd = regulator_get(NULL, "8058_l10");
		if(IS_ERR(vdd)) {
			rc = PTR_ERR(vdd);
			printk(KERN_ERR "<4> %s: regulator get of failed (%d)\n", __func__, rc);
		}
		rc = regulator_disable(vdd);
		if (rc) {
			printk(KERN_ERR "<4> %s: vreg disable failed (%d)\n",__func__, rc);
		}
		else {
			retrylevel = 0;
		}
	}

	if(retrylevel == 2) {
		printk("<5> %s OK\n", __func__);
	}
}
#endif
//---	edited by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_APDS9900_HW) || defined(CONFIG_EF39S_SENSORS_APDS9900_SW) ||	\
	defined(CONFIG_EF40K_SENSORS_APDS9900_HW) || defined(CONFIG_EF40K_SENSORS_APDS9900_SW) ||   \
	defined(CONFIG_EF40S_SENSORS_APDS9900_HW) || defined(CONFIG_EF40S_SENSORS_APDS9900_SW)

static void proximity_sensor_power_up(void)
{
	struct regulator *vdd = regulator_get(NULL, "8058_l17");
	int rc;

	if(IS_ERR(vdd)) {
		rc = PTR_ERR(vdd);
		printk(KERN_ERR "<prox> %s: regulator get of failed (%d)\n", __func__, rc);
	}
	else {
		rc = regulator_set_voltage(vdd, 2600000, 2600000);
		if (rc) {
			printk(KERN_ERR "<prox> %s: vreg set level failed (%d)\n", __func__, rc);
		}
		else {
			rc = regulator_enable(vdd);
			if (rc) {
				printk(KERN_ERR "<prox> %s: vreg enable failed (%d)\n",__func__, rc);
			}
			else{
				printk("<5> %s OK\n", __func__);
			}
		}
	}
}
#endif

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
static void sensors_power_up(void)
{
	struct regulator *vdd;
#if (BOARD_REV >= TP10)	
	struct regulator *vreg_l15;
#endif	
	struct regulator *vldgc;
	int rc;
	
	vdd = regulator_get(NULL, "8058_l10");
	if(IS_ERR(vdd)) {
		rc = PTR_ERR(vdd);
		printk(KERN_ERR "<L10> %s: regulator get of failed (%d)\n", __func__, rc);
	}
	else {
		rc = regulator_set_voltage(vdd, 3000000, 3000000);
		if (rc) {
			printk(KERN_ERR "<L10> %s: vreg set level failed (%d)\n", __func__, rc);
		}
		else {
			rc = regulator_enable(vdd);
			if (rc) {
				printk(KERN_ERR "<L10> %s: vreg enable failed (%d)\n",__func__, rc);
			}
			else{
				printk("<L10> %s OK\n", __func__);
			}
		}
	}

	vldgc = regulator_get(NULL, "8058_lvs1");
	if(IS_ERR(vldgc)) {
		rc = PTR_ERR(vldgc);
		printk(KERN_ERR "<LVS1> %s: regulator get of failed (%d)\n", __func__, rc);
	}
	rc = regulator_enable(vldgc);
	if (rc) {
		printk(KERN_ERR "<LVS1> %s: vreg enable failed (%d)\n",__func__, rc);
	}
	else {
		regulator_put(vldgc);
		printk(KERN_ERR "<LVS1> Power up OK\n");
	}

#if (BOARD_REV >= TP10)
	// APDS9190 Power up
	vreg_l15 = regulator_get(NULL, "8058_l15");
	if(IS_ERR(vreg_l15)) {
		rc = PTR_ERR(vreg_l15);
		printk(KERN_ERR "<L15> %s: regulator get of failed (%d)\n", __func__, rc);
	}
	else {
		rc = regulator_set_voltage(vreg_l15, 3000000, 3000000);
		if (rc) {
			printk(KERN_ERR "<L15> %s: vreg set level failed (%d)\n", __func__, rc);
		}
		else {
			rc = regulator_enable(vreg_l15);
			if (rc) {
				printk(KERN_ERR "<L15> %s: vreg enable failed (%d)\n",__func__, rc);
			}
			else{
				printk("<L15> %s OK\n", __func__);
			}
		}
	}
#endif
}
#endif

#if defined(CONFIG_PN544)

static struct pn544_i2c_platform_data pn544 = {
	.irq_gpio = NFC_WAKEUP_GPIO,
	.ven_gpio = NFC_ENABLE_GPIO,
	.firm_gpio = NFC_FW_DL_GPIO,
};

static struct i2c_board_info nfc_i2c_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO("pn544", NFC_SLAVE_ADDR),
        .irq = MSM_GPIO_TO_INT(NFC_WAKEUP_GPIO),
        .platform_data = &pn544,
	},
};
#endif	

#if defined(CONFIG_USB_GADGET_MSM_72K) || defined(CONFIG_USB_EHCI_MSM_72K)
static struct regulator *ldo6_3p3;
static struct regulator *ldo7_1p8;
static struct regulator *vdd_cx;
#define PMICID_INT		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 36)
notify_vbus_state notify_vbus_state_func_ptr;
static int usb_phy_susp_dig_vol = 750000;
static int pmic_id_notif_supported;

#ifdef CONFIG_USB_EHCI_MSM_72K
#define USB_PMIC_ID_DET_DELAY	msecs_to_jiffies(100)
struct delayed_work pmic_id_det;

static int __init usb_id_pin_rework_setup(char *support)
{
	if (strncmp(support, "true", 4) == 0)
		pmic_id_notif_supported = 1;

	return 1;
}
__setup("usb_id_pin_rework=", usb_id_pin_rework_setup);

static void pmic_id_detect(struct work_struct *w)
{
	int val = gpio_get_value_cansleep(PM8058_GPIO_PM_TO_SYS(36));
	pr_debug("%s(): gpio_read_value = %d\n", __func__, val);

	if (notify_vbus_state_func_ptr)
		(*notify_vbus_state_func_ptr) (val);
}

static irqreturn_t pmic_id_on_irq(int irq, void *data)
{
	/*
	 * Spurious interrupts are observed on pmic gpio line
	 * even though there is no state change on USB ID. Schedule the
	 * work to to allow debounce on gpio
	 */
	schedule_delayed_work(&pmic_id_det, USB_PMIC_ID_DET_DELAY);

	return IRQ_HANDLED;
}

static int msm_hsusb_pmic_id_notif_init(void (*callback)(int online), int init)
{
	unsigned ret = -ENODEV;

	if (!callback)
		return -EINVAL;

	if (machine_is_msm8x60_fluid())
		return -ENOTSUPP;

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 2) {
		pr_debug("%s: USB_ID pin is not routed to PMIC"
					"on V1 surf/ffa\n", __func__);
		return -ENOTSUPP;
	}

	if ((machine_is_msm8x60_ffa() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) &&
			!pmic_id_notif_supported) {
		pr_debug("%s: USB_ID is not routed to PMIC"
			"on V2 ffa\n", __func__);
		return -ENOTSUPP;
	}

	usb_phy_susp_dig_vol = 500000;

	if (init) {
		notify_vbus_state_func_ptr = callback;
		ret = pm8901_mpp_config_digital_out(1,
			PM8901_MPP_DIG_LEVEL_L5, 1);
		if (ret) {
			pr_err("%s: MPP2 configuration failed\n", __func__);
			return -ENODEV;
		}
		INIT_DELAYED_WORK(&pmic_id_det, pmic_id_detect);
		ret = request_threaded_irq(PMICID_INT, NULL, pmic_id_on_irq,
			(IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING),
						"msm_otg_id", NULL);
		if (ret) {
			pm8901_mpp_config_digital_out(1,
					PM8901_MPP_DIG_LEVEL_L5, 0);
			pr_err("%s:pmic_usb_id interrupt registration failed",
					__func__);
			return ret;
		}
		/* Notify the initial Id status */
		pmic_id_detect(&pmic_id_det.work);
	} else {
		free_irq(PMICID_INT, 0);
		cancel_delayed_work_sync(&pmic_id_det);
		notify_vbus_state_func_ptr = NULL;
		ret = pm8901_mpp_config_digital_out(1,
			PM8901_MPP_DIG_LEVEL_L5, 0);
		if (ret) {
			pr_err("%s:MPP2 configuration failed\n", __func__);
			return -ENODEV;
		}
	}
	return 0;
}
#endif

#define USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL	1000000
#define USB_PHY_MAX_VDD_DIG_VOL			1320000
static int msm_hsusb_init_vddcx(int init)
{
	int ret = 0;

	if (init) {
		vdd_cx = regulator_get(NULL, "8058_s1");
		if (IS_ERR(vdd_cx)) {
			return PTR_ERR(vdd_cx);
		}

		ret = regulator_set_voltage(vdd_cx,
				USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL,
				USB_PHY_MAX_VDD_DIG_VOL);
		if (ret) {
			pr_err("%s: unable to set the voltage for regulator"
				"vdd_cx\n", __func__);
			regulator_put(vdd_cx);
			return ret;
		}

		ret = regulator_enable(vdd_cx);
		if (ret) {
			pr_err("%s: unable to enable regulator"
				"vdd_cx\n", __func__);
			regulator_put(vdd_cx);
		}
	} else {
		ret = regulator_disable(vdd_cx);
		if (ret) {
			pr_err("%s: Unable to disable the regulator:"
				"vdd_cx\n", __func__);
			return ret;
		}

		regulator_put(vdd_cx);
	}

	return ret;
}

static int msm_hsusb_config_vddcx(int high)
{
	int max_vol = USB_PHY_MAX_VDD_DIG_VOL;
	int min_vol;
	int ret;

	if (high)
		min_vol = USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL;
	else
		min_vol = usb_phy_susp_dig_vol;

	ret = regulator_set_voltage(vdd_cx, min_vol, max_vol);
	if (ret) {
		pr_err("%s: unable to set the voltage for regulator"
			"vdd_cx\n", __func__);
		return ret;
	}

	pr_debug("%s: min_vol:%d max_vol:%d\n", __func__, min_vol, max_vol);

	return ret;
}

#define USB_PHY_3P3_VOL_MIN	3050000 /* uV */
#define USB_PHY_3P3_VOL_MAX	3050000 /* uV */
#define USB_PHY_3P3_HPM_LOAD	50000	/* uA */
#define USB_PHY_3P3_LPM_LOAD	4000	/* uA */

#define USB_PHY_1P8_VOL_MIN	1800000 /* uV */
#define USB_PHY_1P8_VOL_MAX	1800000 /* uV */
#define USB_PHY_1P8_HPM_LOAD	50000	/* uA */
#define USB_PHY_1P8_LPM_LOAD	4000	/* uA */
static int msm_hsusb_ldo_init(int init)
{
	int rc = 0;

	if (init) {
		ldo6_3p3 = regulator_get(NULL, "8058_l6");
		if (IS_ERR(ldo6_3p3))
			return PTR_ERR(ldo6_3p3);

		ldo7_1p8 = regulator_get(NULL, "8058_l7");
		if (IS_ERR(ldo7_1p8)) {
			rc = PTR_ERR(ldo7_1p8);
			goto put_3p3;
		}

		rc = regulator_set_voltage(ldo6_3p3, USB_PHY_3P3_VOL_MIN,
				USB_PHY_3P3_VOL_MAX);
		if (rc) {
			pr_err("%s: Unable to set voltage level for"
				"ldo6_3p3 regulator\n", __func__);
			goto put_1p8;
		}
		rc = regulator_enable(ldo6_3p3);
		if (rc) {
			pr_err("%s: Unable to enable the regulator:"
				"ldo6_3p3\n", __func__);
			goto put_1p8;
		}
		rc = regulator_set_voltage(ldo7_1p8, USB_PHY_1P8_VOL_MIN,
				USB_PHY_1P8_VOL_MAX);
		if (rc) {
			pr_err("%s: Unable to set voltage level for"
				"ldo7_1p8 regulator\n", __func__);
			goto disable_3p3;
		}
		rc = regulator_enable(ldo7_1p8);
		if (rc) {
			pr_err("%s: Unable to enable the regulator:"
				"ldo7_1p8\n", __func__);
			goto disable_3p3;
		}

		return 0;
	}

	regulator_disable(ldo7_1p8);
disable_3p3:
	regulator_disable(ldo6_3p3);
put_1p8:
	regulator_put(ldo7_1p8);
put_3p3:
	regulator_put(ldo6_3p3);
	return rc;
}

static int msm_hsusb_ldo_enable(int on)
{
	int ret = 0;

	if (!ldo7_1p8 || IS_ERR(ldo7_1p8)) {
		pr_err("%s: ldo7_1p8 is not initialized\n", __func__);
		return -ENODEV;
	}

	if (!ldo6_3p3 || IS_ERR(ldo6_3p3)) {
		pr_err("%s: ldo6_3p3 is not initialized\n", __func__);
		return -ENODEV;
	}

	if (on) {
		ret = regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_HPM_LOAD);
		if (ret < 0) {
			pr_err("%s: Unable to set HPM of the regulator:"
				"ldo7_1p8\n", __func__);
			return ret;
		}
		ret = regulator_set_optimum_mode(ldo6_3p3,
				USB_PHY_3P3_HPM_LOAD);
		if (ret < 0) {
			pr_err("%s: Unable to set HPM of the regulator:"
				"ldo6_3p3\n", __func__);
			regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_LPM_LOAD);
			return ret;
		}
	} else {
		ret = regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_LPM_LOAD);
		if (ret < 0)
			pr_err("%s: Unable to set LPM of the regulator:"
				"ldo7_1p8\n", __func__);
		ret = regulator_set_optimum_mode(ldo6_3p3,
				USB_PHY_3P3_LPM_LOAD);
		if (ret < 0)
			pr_err("%s: Unable to set LPM of the regulator:"
				"ldo6_3p3\n", __func__);
	}

	pr_debug("reg (%s)\n", on ? "HPM" : "LPM");
	return ret < 0 ? ret : 0;
 }
#endif
#ifdef CONFIG_USB_EHCI_MSM_72K
#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
static void msm_hsusb_smb137b_vbus_power(unsigned phy_info, int on)
{
	static int vbus_is_on;

	/* If VBUS is already on (or off), do nothing. */
	if (on == vbus_is_on)
		return;
	smb137b_otg_power(on);
	vbus_is_on = on;
}
#endif
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	static struct regulator *votg_5v_switch;
	static struct regulator *ext_5v_reg;
	static int vbus_is_on;

	/* If VBUS is already on (or off), do nothing. */
	if (on == vbus_is_on)
		return;

	if (!votg_5v_switch) {
		votg_5v_switch = regulator_get(NULL, "8901_usb_otg");
		if (IS_ERR(votg_5v_switch)) {
			pr_err("%s: unable to get votg_5v_switch\n", __func__);
			return;
		}
	}
	if (!ext_5v_reg) {
		ext_5v_reg = regulator_get(NULL, "8901_mpp0");
		if (IS_ERR(ext_5v_reg)) {
			pr_err("%s: unable to get ext_5v_reg\n", __func__);
			return;
		}
	}
	if (on) {
		if (regulator_enable(ext_5v_reg)) {
			pr_err("%s: Unable to enable the regulator:"
					" ext_5v_reg\n", __func__);
			return;
		}
		if (regulator_enable(votg_5v_switch)) {
			pr_err("%s: Unable to enable the regulator:"
					" votg_5v_switch\n", __func__);
			return;
		}
	} else {
		if (regulator_disable(votg_5v_switch))
			pr_err("%s: Unable to enable the regulator:"
				" votg_5v_switch\n", __func__);
		if (regulator_disable(ext_5v_reg))
			pr_err("%s: Unable to enable the regulator:"
				" ext_5v_reg\n", __func__);
	}

	vbus_is_on = on;
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info	= (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
	.power_budget	= 390,
};
#endif

#ifdef CONFIG_BATTERY_MSM8X60
static int msm_hsusb_pmic_vbus_notif_init(void (*callback)(int online),
								int init)
{
	int ret = -ENOTSUPP;

#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
	if (machine_is_msm8x60_fluid()
#ifdef CONFIG_SKY_SMB_CHARGER
                || machine_is_msm8x60_ef39s() || machine_is_msm8x60_ef40k() || machine_is_msm8x60_ef40s()
#endif
        ) {
		if (init)
			msm_charger_register_vbus_sn(callback);
		else
			msm_charger_unregister_vbus_sn(callback);
		return  0;
	}
#endif
	/* ID and VBUS lines are connected to pmic on 8660.V2.SURF,
	 * hence, irrespective of either peripheral only mode or
	 * OTG (host and peripheral) modes, can depend on pmic for
	 * vbus notifications
	 */
	if ((SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2)
			&& (machine_is_msm8x60_surf() ||
				pmic_id_notif_supported)) {
		if (init)
			ret = msm_charger_register_vbus_sn(callback);
		else {
			msm_charger_unregister_vbus_sn(callback);
			ret = 0;
		}
	} else {
#if !defined(CONFIG_USB_EHCI_MSM_72K)
	if (init)
		ret = msm_charger_register_vbus_sn(callback);
	else {
		msm_charger_unregister_vbus_sn(callback);
		ret = 0;
	}
#endif
	}
	return ret;
}
#endif

#if defined(CONFIG_USB_GADGET_MSM_72K) || defined(CONFIG_USB_EHCI_MSM_72K)
static struct msm_otg_platform_data msm_otg_pdata = {
	/* if usb link is in sps there is no need for
	 * usb pclk as dayatona fabric clock will be
	 * used instead
	 */
	.pclk_src_name		 = "dfab_usb_hs_clk",
	.pemp_level		 = PRE_EMPHASIS_WITH_20_PERCENT,
	.cdr_autoreset		 = CDR_AUTO_RESET_DISABLE,
	.se1_gating		 = SE1_GATING_DISABLE,
#ifdef CONFIG_USB_EHCI_MSM_72K
	.pmic_id_notif_init = msm_hsusb_pmic_id_notif_init,
#endif
#ifdef CONFIG_USB_EHCI_MSM_72K
	.vbus_power = msm_hsusb_vbus_power,
#endif
#ifdef CONFIG_BATTERY_MSM8X60
	.pmic_vbus_notif_init	= msm_hsusb_pmic_vbus_notif_init,
#endif
	.ldo_init		 = msm_hsusb_ldo_init,
	.ldo_enable		 = msm_hsusb_ldo_enable,
	.config_vddcx            = msm_hsusb_config_vddcx,
	.init_vddcx              = msm_hsusb_init_vddcx,
#ifdef CONFIG_BATTERY_MSM8X60
	.chg_vbus_draw = msm_charger_vbus_draw,
#endif
};
#endif

#ifdef CONFIG_USB_GADGET_MSM_72K
static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.is_phy_status_timer_on = 1,
};
#endif


#ifdef CONFIG_USB_ANDROID
#ifndef CONFIG_USB_ANDROID_PANTECH
static char *usb_functions_default[] = {
	"diag",
	"modem",
	"nmea",
	"rmnet",
	"usb_mass_storage",
};

static char *usb_functions_default_adb[] = {
	"diag",
	"adb",
	"modem",
	"nmea",
	"rmnet",
	"usb_mass_storage",
};
#endif

#ifdef CONFIG_USB_ANDROID_PANTECH
static char *pantech_charm_usb_functions_default[] = {
	"diag",
	"diag_mdm",
	"modem",
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	"rmnet_sdio",
#endif
	"usb_mass_storage",
#ifndef CONFIG_MACH_MSM8X60_PRESTO
	"obex",
#endif //p13120 2011.09.09 Disable obex port.
};

static char *pantech_charm_usb_functions_default_adb[] = {
	"diag",
	"diag_mdm",
	"adb",
	"modem",
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	"rmnet_sdio",
#endif
	"usb_mass_storage",
#ifndef CONFIG_MACH_MSM8X60_PRESTO
	"obex",
#endif //p13120 2011.09.09 Disable obex port.
};
static char *pantech_usb_functions_rndis[] = {
	"rndis",
	"diag",
	"diag_mdm",
};

static char *pantech_usb_functions_rndis_adb[] = {
	"rndis",
	"adb",
	"diag",
	"diag_mdm",
};
#endif

#ifdef CONFIG_PANTECH_MTP
static char *usb_functions_mtp[] = {
	"mtp",
};
#endif

#ifndef CONFIG_USB_ANDROID_PANTECH
static char *svlte2_usb_functions_default[] = {
	"diag",
	"diag_mdm",
	"modem",
	"nmea",
	"rmnet_smd_sdio",
	"usb_mass_storage",
};

static char *svlte2_usb_functions_default_adb[] = {
	"diag",
	"diag_mdm",
	"adb",
	"modem",
	"nmea",
	"rmnet_smd_sdio",
	"usb_mass_storage",
};

static char *charm_usb_acm_functions[] = {
	"diag",
	"diag_mdm",
	"acm1",
	"acm2",
	"rmnet_sdio",
	"usb_mass_storage",
};

static char *charm_usb_acm_functions_adb[] = {
	"diag",
	"diag_mdm",
	"adb",
	"acm1",
	"acm2",
	"rmnet_sdio",
	"usb_mass_storage",
};

static char *charm_usb_functions_default[] = {
	"diag",
	"diag_mdm",
	"modem",
	"nmea",
	"rmnet_sdio",
	"usb_mass_storage",
};

static char *charm_usb_functions_default_adb[] = {
	"diag",
	"diag_mdm",
	"adb",
	"modem",
	"nmea",
	"rmnet_sdio",
	"usb_mass_storage",
};

static char *usb_functions_rndis[] = {
	"rndis",
	"diag",
};

static char *usb_functions_rndis_adb[] = {
	"rndis",
	"diag",
	"adb",
};

static char *charm_usb_functions_rndis[] = {
	"rndis",
	"diag",
	"diag_mdm",
};

static char *charm_usb_functions_rndis_adb[] = {
	"rndis",
	"diag",
	"diag_mdm",
	"adb",
};

static char *usb_functions_mtp[] = {
	"mtp",
};

static char *usb_functions_mtp_adb[] = {
	"mtp",
	"adb",
};

static char *usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
#ifdef CONFIG_USB_ANDROID_DIAG
	"diag",
#endif
#ifdef CONFIG_USB_ANDROID_MTP
	"mtp",
#endif
	"adb",
#ifdef CONFIG_USB_F_SERIAL
	"modem",
	"nmea",
#endif
#ifdef CONFIG_USB_ANDROID_RMNET
	"rmnet",
#endif
	"usb_mass_storage",
#ifdef CONFIG_USB_ANDROID_ACM
	"acm",
#endif
};

static char *svlte2_usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
#ifdef CONFIG_USB_ANDROID_DIAG
	"diag",
	"diag_mdm",
#endif
#ifdef CONFIG_USB_ANDROID_MTP
	"mtp",
#endif
	"adb",
#ifdef CONFIG_USB_F_SERIAL
	"modem",
	"nmea",
#endif
#ifdef CONFIG_USB_ANDROID_ACM
	"acm1",
	"acm2",
#endif
#ifdef CONFIG_USB_ANDROID_RMNET_SMD_SDIO
	"rmnet_smd_sdio",
#endif
	"usb_mass_storage",
};
#endif /*CONFIG_USB_ANDROID_PANTECH*/

static char *charm_usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
	"usb_mass_storage",//YGP 20110610 for mtp
#ifdef CONFIG_USB_ANDROID_DIAG
	"diag",
	"diag_mdm",
#endif
#ifdef CONFIG_USB_ANDROID_MTP
    #ifndef CONFIG_PANTECH_MTP
	"mtp",
	#endif
#endif
	"adb",
#ifdef CONFIG_USB_F_SERIAL
	"modem",
	"nmea",
#endif
#ifdef CONFIG_USB_ANDROID_ACM
	"acm1",
	"acm2",
#endif
#ifdef CONFIG_USB_ANDROID_RMNET_SDIO
	"rmnet_sdio",
#endif
	//"usb_mass_storage",//YGP 20110610 for mtp
#ifdef CONFIG_PANTECH_MTP
	"mtp",
#endif
#ifdef CONFIG_USB_ANDROID_PANTECH
#ifndef CONFIG_MACH_MSM8X60_PRESTO
	"obex"
#endif //p13120 2011.09.09 Disable obex port.
#endif
};
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
#if 1  // jmlee =====> 20110505 now dummy value and function.... todo modify for presto
//#define GPIO_MIC_SEL_SEL 49 //  pull down 0 :NR(A2020),  1 : QTR,
//#define GPIO_NR_AMP_EN 46 // pull down  0 : amp off, 1 : amp on
//#define GPIO_NR_OUT_SEL 57 // pull down   0 : QTR, 1 : NR(A2020)

#if 0  // jmlee for test code i2c device check for AUDIENCE DEBUG TOOL
static uint32_t audience_gpio_table[] = {
   GPIO_CFG(GPIO_MIC_SEL_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
   GPIO_CFG(GPIO_NR_AMP_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
   GPIO_CFG(GPIO_NR_OUT_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),   
    
   GPIO_CFG(GPIO_NR_WAKEUP_N, 0, GPIO_CFG_INPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA),  // GPIO_NR_WAKEUP_N
   GPIO_CFG(GPIO_NR_RESET_N, 0, GPIO_CFG_INPUT,	GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  // GPIO_NR_RESET_N
#if (BOARD_REV > WS10)   // Presto NR CLK_IN TP Test ws20 remove boot err
   GPIO_CFG(GPIO_XO_OUT_D0_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), // pull down , eS310 CLK_IN enable pin
#endif   
#else
static uint32_t audience_gpio_table[] = {
   GPIO_CFG(GPIO_MIC_SEL_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
   GPIO_CFG(GPIO_NR_AMP_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
   GPIO_CFG(GPIO_NR_OUT_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),   
    
   GPIO_CFG(GPIO_NR_WAKEUP_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_NO_PULL, GPIO_CFG_2MA),  // GPIO_NR_WAKEUP_N
   GPIO_CFG(GPIO_NR_RESET_N, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  // GPIO_NR_RESET_N
#if (BOARD_REV > WS10) // Presto NR CLK_IN TP Test ws20 remove boot err
   GPIO_CFG(GPIO_XO_OUT_D0_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), // pull down , eS310 CLK_IN enable pin
#endif   
#endif
};

#if (BOARD_REV > WS10)   // Presto NR CLK_IN TP Test ws20 remove boot err
static struct regulator *snddev_reg_l2_audience;
static struct msm_xo_voter *audience_clock;

// presto 8058_l2 always on
int audience_pmic_clk_vreg_on(void)
{
	int rc = 0;
	printk("[Snd_audience_a2020] audience_pmic_clk_vreg_on ==> START  \n");
	
	snddev_reg_l2_audience = regulator_get(NULL, "8058_l2");
	if (IS_ERR(snddev_reg_l2_audience)) {
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l2", rc);
	}

	rc = regulator_set_voltage(snddev_reg_l2_audience, 1800000, 1800000);
	if (rc < 0)
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n", "8058_l2", rc);

	rc = regulator_enable(snddev_reg_l2_audience);
	if (rc < 0)
		printk("[Snd_audience_a2020] vreg_enable failed(%s) = %d \n",	 "8058_l2", rc);

	audience_clock = msm_xo_get(MSM_XO_TCXO_D0, "audience_clk");

	rc = msm_xo_mode_vote(audience_clock, MSM_XO_MODE_ON);

	if (rc < 0) {
	printk("[Snd_audience_a2020] Failed to vote for TCXO_DO ON\n");
		pr_err("Failed to vote for TCXO_DO ON\n");
	}

	msleep(10);

	rc = msm_xo_mode_vote(audience_clock, MSM_XO_MODE_PIN_CTRL);

	if (rc < 0) {
		pr_err("Failed to vote for TCXO_DO pin control\n");
	}

	printk("[Snd_audience_a2020] audience_pmic_clk_vreg_on ==> END rc : %d  \n",rc);

	return rc;
}
#endif

static int __init snddev_PAN_audio_gpio_init(void)
{
      int rc = 0;
      int i;
	for (i = 0; i < ARRAY_SIZE(audience_gpio_table); ++i)
	{
		 rc = gpio_tlmm_config(audience_gpio_table[i], GPIO_CFG_ENABLE);
		 if ( rc ) {
			  printk(KERN_ERR "%s: Failed toAudience i2c gpio_tlmm_config(%d)=%d\r\n", __func__, i, rc);
		 }
	}

/////////  initalizing gpio config ==>	
#if (BOARD_REV > WS10)    // Presto NR CLK_IN TP Test ws20 remove boot err

    audience_pmic_clk_vreg_on();
    gpio_request(GPIO_XO_OUT_D0_EN, "GPIO_XO_OUT_D0_EN");
    gpio_direction_output(GPIO_XO_OUT_D0_EN,1);  //high
#endif

    gpio_request(GPIO_NR_RESET_N, "GPIO_NR_RESET_N");
    gpio_direction_output(GPIO_NR_RESET_N,1);  //high
    config_GPIO_NR_RESET_N(1);
	
    gpio_request(GPIO_NR_WAKEUP_N, "GPIO_NR_WAKEUP_N");
    gpio_direction_output(GPIO_NR_WAKEUP_N,0);  // low
    config_GPIO_NR_WAKEUP_N(0);

    gpio_request(GPIO_MIC_SEL_SEL, "GPIO_MIC_SEL_SEL");
    gpio_direction_output(GPIO_MIC_SEL_SEL,0);  // low
    config_GPIO_MIC_SEL_SEL(0);

    gpio_request(GPIO_NR_AMP_EN, "GPIO_NR_AMP_EN");
    gpio_direction_output(GPIO_NR_AMP_EN,0);  // low
    config_GPIO_NR_AMP_EN(0);

    gpio_request(GPIO_NR_OUT_SEL, "GPIO_NR_OUT_SEL");
    gpio_direction_output(GPIO_NR_OUT_SEL,0);  // low
    config_GPIO_NR_OUT_SEL(0);
/////////  initalizing gpio config <==

	return rc;
}

void msm_snddev_mic_sel_a2020(void)
{
	config_GPIO_MIC_SEL_SEL(0);	
	pr_info(" #@#@#@#@#@#@#@#@# mic selection A2020\n");
}
int msm_snddev_mic_sel_QTR(void)
{
	config_GPIO_MIC_SEL_SEL(1);	
	pr_info(" #@#@#@#@#@#@#@#@# mic selection QTR\n");
	return 0;
}
void msm_snddev_nr_receiver_amp_on(void)
{
	config_GPIO_NR_AMP_EN(1);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_receiver_amp_on\n");
}
void msm_snddev_nr_receiver_amp_off(void)
{
	config_GPIO_NR_AMP_EN(0);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_receiver_amp_off\n");
}
int msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker(void)
{
	config_GPIO_NR_OUT_SEL(0);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_out_sel_qtrReceiver_or_A2020Speaker\n");
	return 0;
}
void msm_snddev_nr_out_sel_A2020Reciever(void)
{
	config_GPIO_NR_OUT_SEL(1);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_out_sel_A2020Reciever\n");
}
int msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_on(void)
{
	config_GPIO_NR_OUT_SEL(1);
	pr_info(" #@#@#@#@#@#@#@#@# nr_out_sel_A2020Reciever\n");
	config_GPIO_NR_AMP_EN(1);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_receiver_amp_on\n");
	return 0;
}
void msm_snddev_nr_out_sel_A2020Reciever_nr_receiver_amp_off(void)
{
	config_GPIO_NR_OUT_SEL(1);
	pr_info(" #@#@#@#@#@#@#@#@# nr_out_sel_A2020Reciever\n");
	config_GPIO_NR_AMP_EN(0);	
	pr_info(" #@#@#@#@#@#@#@#@# nr_receiver_amp_on\n");
}
#endif  // jmlee <===== 20110505 now dummy value and function.... todo modify for presto
#endif


#ifndef CONFIG_USB_ANDROID_PANTECH
static struct android_usb_product usb_products[] = {
	{
		.product_id	= 0x9026,
		.num_functions	= ARRAY_SIZE(usb_functions_default),
		.functions	= usb_functions_default,
	},
	{
		.product_id	= 0x9025,
		.num_functions	= ARRAY_SIZE(usb_functions_default_adb),
		.functions	= usb_functions_default_adb,
	},
	{
		.product_id	= 0x902c,
		.num_functions	= ARRAY_SIZE(usb_functions_rndis),
		.functions	= usb_functions_rndis,
	},
	{
		.product_id	= 0x902d,
		.num_functions	= ARRAY_SIZE(usb_functions_rndis_adb),
		.functions	= usb_functions_rndis_adb,
	},
	{
		.product_id	= 0xF003,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp),
		.functions	= usb_functions_mtp,
	},
	{
		.product_id	= 0x9039,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp_adb),
		.functions	= usb_functions_mtp_adb,
	},
};

static struct android_usb_product svlte2_usb_products[] = {
	{
		.product_id	= 0x9038,
		.num_functions	= ARRAY_SIZE(svlte2_usb_functions_default),
		.functions	= svlte2_usb_functions_default,
	},
	{
		.product_id	= 0x9037,
		.num_functions	= ARRAY_SIZE(svlte2_usb_functions_default_adb),
		.functions	= svlte2_usb_functions_default_adb,
	},
	{
		.product_id	= 0x9041,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_rndis),
		.functions	= charm_usb_functions_rndis,
	},
	{
		.product_id	= 0x9042,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_rndis_adb),
		.functions	= charm_usb_functions_rndis_adb,
	},
	{
		.product_id	= 0xF003,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp),
		.functions	= usb_functions_mtp,
	},
	{
		.product_id	= 0x9039,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp_adb),
		.functions	= usb_functions_mtp_adb,
	},
	{
		.product_id	= 0x903C,
		.num_functions	= ARRAY_SIZE(charm_usb_acm_functions),
		.functions	= charm_usb_acm_functions,
	},
	{
		.product_id	= 0x903B,
		.num_functions	= ARRAY_SIZE(charm_usb_acm_functions_adb),
		.functions	= charm_usb_acm_functions_adb,
	},
};
#endif /*CONFIG_USB_ANDROID_PANTECH*/

#ifdef CONFIG_MACH_MSM8X60_PRESTO
#define PANTECH_PID 0x6050
#else
#define PANTECH_PID 0x1104
#endif

static struct android_usb_product charm_usb_products[] = {
#ifdef CONFIG_USB_ANDROID_PANTECH
	{
		.product_id	= PANTECH_PID,
		.num_functions	= ARRAY_SIZE(pantech_charm_usb_functions_default),
		.functions	= pantech_charm_usb_functions_default,
	},
	{
		.product_id	= PANTECH_PID,
		.num_functions	= ARRAY_SIZE(pantech_charm_usb_functions_default_adb),
		.functions	= pantech_charm_usb_functions_default_adb,
	},
	{
		.product_id	= PANTECH_PID,
		.num_functions	= ARRAY_SIZE(pantech_usb_functions_rndis),
		.functions	= pantech_usb_functions_rndis,
	},
	{
		.product_id	= PANTECH_PID,
		.num_functions	= ARRAY_SIZE(pantech_usb_functions_rndis_adb),
		.functions	= pantech_usb_functions_rndis_adb,
	},

#else
	{
		.product_id	= 0x9032,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_default),
		.functions	= charm_usb_functions_default,
	},
	{
		.product_id	= 0x9031,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_default_adb),
		.functions	= charm_usb_functions_default_adb,
	},
	{
		.product_id	= 0x9041,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_rndis),
		.functions	= charm_usb_functions_rndis,
	},
	{
		.product_id	= 0x9042,
		.num_functions	= ARRAY_SIZE(charm_usb_functions_rndis_adb),
		.functions	= charm_usb_functions_rndis_adb,
	},
	{
		.product_id	= 0xF003,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp),
		.functions	= usb_functions_mtp,
	},
	{
		.product_id	= 0x9039,
		.num_functions	= ARRAY_SIZE(usb_functions_mtp_adb),
		.functions	= usb_functions_mtp_adb,
	},
	/* below acm functions are same for charm or svlte2 */
	{
		.product_id	= 0x903C,
		.num_functions	= ARRAY_SIZE(charm_usb_acm_functions),
		.functions	= charm_usb_acm_functions,
	},
	{
		.product_id	= 0x903B,
		.num_functions	= ARRAY_SIZE(charm_usb_acm_functions_adb),
		.functions	= charm_usb_acm_functions_adb,
	},
#endif

#ifdef CONFIG_PANTECH_MTP
	{
	  /* PANTECH MTP USB Mode: MTP only*/
		.product_id 	= 0x6043,
		.num_functions  = ARRAY_SIZE(usb_functions_mtp),
		.functions      = usb_functions_mtp,
	},
#endif
};

static struct usb_mass_storage_platform_data mass_storage_pdata = {
	.nluns		= 2,
#ifdef CONFIG_USB_ANDROID_PANTECH
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	.vendor		= "Pantech",
#else
	.vendor		= "SKY",
#endif // p13120, 2011.09.15
#else
	.vendor		= "Qualcomm Incorporated",
#endif
	.product        = "Mass storage",
	.can_stall	= 1,
};

static struct platform_device usb_mass_storage_device = {
	.name	= "usb_mass_storage",
	.id	= -1,
	.dev	= {
		.platform_data = &mass_storage_pdata,
	},
};

static struct usb_ether_platform_data rndis_pdata = {
	/* ethaddr is filled by board_serialno_setup */
	.vendorID	= 0x05C6,
	.vendorDescr	= "Qualcomm Incorporated",
};

static struct platform_device rndis_device = {
	.name	= "rndis",
	.id	= -1,
	.dev	= {
		.platform_data = &rndis_pdata,
	},
};

static struct android_usb_platform_data android_usb_pdata = {
#ifdef CONFIG_USB_ANDROID_PANTECH
	.vendor_id	= 0x10A9,
	.product_id	= PANTECH_PID,
	.version	= 0x0100,
	.product_name	= "Pantech Android USB Composite Device",
	.manufacturer_name = "Pantech",
	.num_products = ARRAY_SIZE(charm_usb_products),
	.products = charm_usb_products,
	.num_functions = ARRAY_SIZE(charm_usb_functions_all),
	.functions = charm_usb_functions_all,
	.serial_number = "1234567890ABCDEF",
#else
	.vendor_id	= 0x05C6,
	.product_id	= 0x9026,
	.version	= 0x0100,
	.product_name		= "Qualcomm HSUSB Device",
	.manufacturer_name	= "Qualcomm Incorporated",
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_all),
	.functions = usb_functions_all,
	.serial_number = "1234567890ABCDEF",
#endif
};
static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};

static int __init board_serialno_setup(char *serialno)
{
	int i;
	char *src = serialno;

	/* create a fake MAC address from our serial number.
	 * first byte is 0x02 to signify locally administered.
	 */
	rndis_pdata.ethaddr[0] = 0x02;
	for (i = 0; *src; i++) {
		/* XOR the USB serial across the remaining bytes */
		rndis_pdata.ethaddr[i % (ETH_ALEN - 1) + 1] ^= *src++;
	}

	android_usb_pdata.serial_number = serialno;
	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);
#endif

#ifdef CONFIG_MSM_VPE
static struct resource msm_vpe_resources[] = {
	{
		.start	= 0x05300000,
		.end	= 0x05300000 + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= INT_VPE,
		.end	= INT_VPE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_vpe_device = {
	.name = "msm_vpe",
	.id   = 0,
	.num_resources = ARRAY_SIZE(msm_vpe_resources),
	.resource = msm_vpe_resources,
};
#endif

#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_MSM_CAMERA_FLASH
#define VFE_CAMIF_TIMER1_GPIO 29
#define VFE_CAMIF_TIMER2_GPIO 30
#define VFE_CAMIF_TIMER3_GPIO_INT 31
#define FUSION_VFE_CAMIF_TIMER1_GPIO 42
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_PMIC,
	._fsrc.pmic_src.num_of_src = 2,
	._fsrc.pmic_src.low_current  = 100,
	._fsrc.pmic_src.high_current = 300,
	._fsrc.pmic_src.led_src_1 = PMIC8058_ID_FLASH_LED_0,
	._fsrc.pmic_src.led_src_2 = PMIC8058_ID_FLASH_LED_1,
	._fsrc.pmic_src.pmic_set_current = pm8058_set_flash_led_current,
};
#ifdef CONFIG_IMX074
static struct msm_camera_sensor_strobe_flash_data strobe_flash_xenon = {
	.flash_trigger = VFE_CAMIF_TIMER2_GPIO,
	.flash_charge = VFE_CAMIF_TIMER1_GPIO,
	.flash_charge_done = VFE_CAMIF_TIMER3_GPIO_INT,
	.flash_recharge_duration = 50000,
	.irq = MSM_GPIO_TO_INT(VFE_CAMIF_TIMER3_GPIO_INT),
};
#endif
#endif

int msm_cam_gpio_tbl[] = {
	32,/*CAMIF_MCLK*/
	47,/*CAMIF_I2C_DATA*/
	48,/*CAMIF_I2C_CLK*/
#ifndef CONFIG_PANTECH_CAMERA_HW
	/* You have to configure common gpios here, and should configure other
	 * gpios in sensor drivers. */
	105,/*STANDBY*/
#endif
};

enum msm_cam_stat{
	MSM_CAM_OFF,
	MSM_CAM_ON,
};

/*PANTECH_CAMERA_TODO*/
#if defined(CONFIG_MACH_MSM8X60_PRESTO) && defined(CONFIG_PANTECH_CAMERA)
static uint32_t cam_gpio_config_data[] =
{
	GPIO_CFG(31, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"FLASH ENABLE" },
	GPIO_CFG(62, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"FLASH MODE" },
#ifdef CONFIG_PANTECH_CAMERA_MT9P111
	GPIO_CFG(86, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"5M STANDBY" },
#endif
	GPIO_CFG(106, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"5M RESET" },
	GPIO_CFG(137, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"VGA RESET" },
	GPIO_CFG(139, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"VGA STANDBY" },
};
#endif

static int config_gpio_table(enum msm_cam_stat stat)
{
	int rc = 0, i = 0;

//PANTECH_CAMERA_TODO
#if defined(CONFIG_MACH_MSM8X60_PRESTO) && defined(CONFIG_PANTECH_CAMERA)
	if(stat == MSM_CAM_ON)
	{
		for (i = 0; i< ARRAY_SIZE(cam_gpio_config_data); ++i)
			gpio_tlmm_config(cam_gpio_config_data[i], 0);
	}
#endif

	if (stat == MSM_CAM_ON) {
		for (i = 0; i < ARRAY_SIZE(msm_cam_gpio_tbl); i++) {
			rc = gpio_request(msm_cam_gpio_tbl[i], "CAM_GPIO");
			if (unlikely(rc < 0)) {
				pr_err("%s not able to get gpio\n", __func__);
				for (i--; i >= 0; i--)
					gpio_free(msm_cam_gpio_tbl[i]);
				break;
			}
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(msm_cam_gpio_tbl); i++)
			gpio_free(msm_cam_gpio_tbl[i]);
	}
	return rc;
}

#ifdef CONFIG_IMX074		// add for CONFIG_PANTECH_CAMERA
static struct msm_camera_sensor_platform_info sensor_board_info = {
	.mount_angle = 0
};
#endif

/*external regulator VREG_5V*/
static struct regulator *reg_flash_5V;

static int config_camera_on_gpios_fluid(void)
{
	int rc = 0;

	reg_flash_5V = regulator_get(NULL, "8901_mpp0");
	if (IS_ERR(reg_flash_5V)) {
		pr_err("'%s' regulator not found, rc=%ld\n",
				"8901_mpp0", IS_ERR(reg_flash_5V));
		return -ENODEV;
	}

	rc = regulator_enable(reg_flash_5V);
	if (rc) {
		pr_err("'%s' regulator enable failed, rc=%d\n",
			"8901_mpp0", rc);
		regulator_put(reg_flash_5V);
		return rc;
	}

#ifdef CONFIG_IMX074
	sensor_board_info.mount_angle = 90;
#endif
	rc = config_gpio_table(MSM_CAM_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	rc = gpio_request(GPIO_EXT_CAMIF_PWR_EN, "CAM_EN");
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio %d request"
			"failed\n", __func__, GPIO_EXT_CAMIF_PWR_EN);
		regulator_disable(reg_flash_5V);
		regulator_put(reg_flash_5V);
		return rc;
	}
	gpio_direction_output(GPIO_EXT_CAMIF_PWR_EN, 0);
	msleep(20);
	gpio_set_value_cansleep(GPIO_EXT_CAMIF_PWR_EN, 1);


	/*Enable LED_FLASH_EN*/
	rc = gpio_request(GPIO_LED_FLASH_EN, "LED_FLASH_EN");
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio %d request"
			"failed\n", __func__, GPIO_LED_FLASH_EN);

		regulator_disable(reg_flash_5V);
		regulator_put(reg_flash_5V);
		config_gpio_table(MSM_CAM_OFF);
		gpio_set_value_cansleep(GPIO_EXT_CAMIF_PWR_EN, 0);
		gpio_free(GPIO_EXT_CAMIF_PWR_EN);
		return rc;
	}
	gpio_direction_output(GPIO_LED_FLASH_EN, 1);
	msleep(20);
	return rc;
}


static void config_camera_off_gpios_fluid(void)
{
	regulator_disable(reg_flash_5V);
	regulator_put(reg_flash_5V);

	gpio_direction_output(GPIO_LED_FLASH_EN, 0);
	gpio_free(GPIO_LED_FLASH_EN);

	config_gpio_table(MSM_CAM_OFF);

	gpio_set_value_cansleep(GPIO_EXT_CAMIF_PWR_EN, 0);
	gpio_free(GPIO_EXT_CAMIF_PWR_EN);
}

#ifdef CONFIG_PANTECH_CAMERA_HW
static int config_camera_on_gpios_main_cam(void)
{
	int rc = 0;

	rc = config_gpio_table(MSM_CAM_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio table request"
				"failed\n", __func__);
		return rc;
	}
	return rc;
}

static void config_camera_off_gpios_main_cam(void)
{
	config_gpio_table(MSM_CAM_OFF);
}

static int config_camera_on_gpios_sub_cam(void)
{
	int rc = 0;

	rc = config_gpio_table(MSM_CAM_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio table request"
				"failed\n", __func__);
		return rc;
	}
	return rc;
}

static void config_camera_off_gpios_sub_cam(void)
{
	config_gpio_table(MSM_CAM_OFF);
}
#endif

static int config_camera_on_gpios(void)
{
	int rc = 0;

	if (machine_is_msm8x60_fluid())
		return config_camera_on_gpios_fluid();

	rc = config_gpio_table(MSM_CAM_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	rc = gpio_request(GPIO_EXT_CAMIF_PWR_EN, "CAM_EN");
	if (rc < 0) {
		config_gpio_table(MSM_CAM_OFF);
		printk(KERN_ERR "%s: CAMSENSOR gpio %d request"
			"failed\n", __func__, GPIO_EXT_CAMIF_PWR_EN);
		return rc;
	}
	gpio_direction_output(GPIO_EXT_CAMIF_PWR_EN, 0);
	mdelay(20);
	gpio_set_value_cansleep(GPIO_EXT_CAMIF_PWR_EN, 1);

#ifdef CONFIG_MSM_CAMERA_FLASH
#ifdef CONFIG_IMX074
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			)
		strobe_flash_xenon.flash_charge = FUSION_VFE_CAMIF_TIMER1_GPIO;
#endif
#endif
	return rc;
}

static void config_camera_off_gpios(void)
{
	if (machine_is_msm8x60_fluid())
		return config_camera_off_gpios_fluid();


	config_gpio_table(MSM_CAM_OFF);

	gpio_set_value_cansleep(GPIO_EXT_CAMIF_PWR_EN, 0);
	gpio_free(GPIO_EXT_CAMIF_PWR_EN);
}

#ifdef CONFIG_QS_S5K4E1

#define QS_CAM_HC37_CAM_PD PM8058_GPIO_PM_TO_SYS(26)

static int config_camera_on_gpios_qs_cam_fluid(void)
{
	int rc = 0;

	/* request QS_CAM_HC37_CAM_PD as an output to HC37 ASIC pin CAM_PD */
	rc = gpio_request(QS_CAM_HC37_CAM_PD, "QS_CAM_HC37_CAM_PD");
	if (rc < 0) {
		printk(KERN_ERR "%s: QS_CAM_HC37_CAM_PD gpio %d request"
			" failed\n", __func__, QS_CAM_HC37_CAM_PD);
		return rc;
	}
	gpio_direction_output(QS_CAM_HC37_CAM_PD, 0);
	msleep(20);
	gpio_set_value_cansleep(QS_CAM_HC37_CAM_PD, 1);
	msleep(20);

	/*
	 * Set GPIO_AUX_CAM_2P7_EN to 1 on North Expander IO2
	 * to enable 2.7V power to Camera
	 */
	rc = gpio_request(GPIO_AUX_CAM_2P7_EN, "CAM_2P7_EN");
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio %d request"
			" failed\n", __func__, GPIO_AUX_CAM_2P7_EN);
		gpio_set_value_cansleep(QS_CAM_HC37_CAM_PD, 0);
		gpio_free(QS_CAM_HC37_CAM_PD);
		return rc;
	}
	gpio_direction_output(GPIO_AUX_CAM_2P7_EN, 0);
	msleep(20);
	gpio_set_value_cansleep(GPIO_AUX_CAM_2P7_EN, 1);
	msleep(20);

	rc = config_camera_on_gpios_fluid();
	if (rc < 0) {
		printk(KERN_ERR "%s: config_camera_on_gpios_fluid"
		" failed\n", __func__);
		gpio_set_value_cansleep(QS_CAM_HC37_CAM_PD, 0);
		gpio_free(QS_CAM_HC37_CAM_PD);
		gpio_set_value_cansleep(GPIO_AUX_CAM_2P7_EN, 0);
		gpio_free(GPIO_AUX_CAM_2P7_EN);
		return rc;
	}
	return rc;
}

static void config_camera_off_gpios_qs_cam_fluid(void)
{
	/*
	 * Set GPIO_AUX_CAM_2P7_EN to 0 on North Expander IO2
	 * to disable 2.7V power to Camera
	 */
	gpio_set_value_cansleep(GPIO_AUX_CAM_2P7_EN, 0);
	gpio_free(GPIO_AUX_CAM_2P7_EN);

	/* set QS_CAM_HC37_CAM_PD to 0 to power off HC37 ASIC*/
	gpio_set_value_cansleep(QS_CAM_HC37_CAM_PD, 0);
	gpio_free(QS_CAM_HC37_CAM_PD);

	config_camera_off_gpios_fluid();
	return;
}

static int config_camera_on_gpios_qs_cam(void)
{
	int rc = 0;

	if (machine_is_msm8x60_fluid())
		return config_camera_on_gpios_qs_cam_fluid();

	rc = config_camera_on_gpios();
	return rc;
}

static void config_camera_off_gpios_qs_cam(void)
{
	if (machine_is_msm8x60_fluid())
		return config_camera_off_gpios_qs_cam_fluid();

	config_camera_off_gpios();
	return;
}
#endif

static int config_camera_on_gpios_web_cam(void)
{
	int rc = 0;
	rc = config_gpio_table(MSM_CAM_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	if (!machine_is_msm8x60_fluid()) {
		rc = gpio_request(GPIO_WEB_CAMIF_STANDBY, "CAM_EN");
		if (rc < 0) {
			config_gpio_table(MSM_CAM_OFF);
			pr_err(KERN_ERR "%s: CAMSENSOR gpio %d request"
				"failed\n", __func__, GPIO_WEB_CAMIF_STANDBY);
			return rc;
		}
		gpio_direction_output(GPIO_WEB_CAMIF_STANDBY, 0);
	}
	return rc;
}

static void config_camera_off_gpios_web_cam(void)
{
	config_gpio_table(MSM_CAM_OFF);
	if (!machine_is_msm8x60_fluid()) {
		gpio_set_value_cansleep(GPIO_WEB_CAMIF_STANDBY, 1);
		gpio_free(GPIO_WEB_CAMIF_STANDBY);
	}
	return;
}

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 283115520,
		.ib  = 452984832,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 283115520,
		.ib  = 452984832,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 283115520,
		.ib  = 452984832,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 319610880,
		.ib  = 511377408,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 566231040,
		.ib  = 905969664,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 69984000,
		.ib  = 111974400,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 320864256,
		.ib  = 513382810,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 320864256,
		.ib  = 513382810,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 566231040,
		.ib  = 905969664,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 706199040,
		.ib  = 1129918464,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 320864256,
		.ib  = 513382810,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 320864256,
		.ib  = 513382810,
	},
};

static struct msm_bus_vectors cam_stereo_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 212336640,
		.ib  = 339738624,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 25090560,
		.ib  = 40144896,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 239708160,
		.ib  = 383533056,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 79902720,
		.ib  = 127844352,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_stereo_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 300902400,
		.ib  = 481443840,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 230307840,
		.ib  = 368492544,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 245113344,
		.ib  = 392181351,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 106536960,
		.ib  = 170459136,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 106536960,
		.ib  = 170459136,
	},
};

static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_stereo_video_vectors),
		cam_stereo_video_vectors,
	},
	{
		ARRAY_SIZE(cam_stereo_snapshot_vectors),
		cam_stereo_snapshot_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_HW
struct msm_camera_device_platform_data msm_camera_device_data_main_cam = {
	.camera_gpio_on  = config_camera_on_gpios_main_cam,
	.camera_gpio_off = config_camera_off_gpios_main_cam,
	.ioext.csiphy = 0x04800000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_0_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
#ifdef CONFIG_MSM_BUS_SCALING
	.cam_bus_scale_table = &cam_bus_client_pdata,
#endif
};

struct msm_camera_device_platform_data msm_camera_device_data_sub_cam = {
	.camera_gpio_on  = config_camera_on_gpios_sub_cam,
	.camera_gpio_off = config_camera_off_gpios_sub_cam,
	.ioext.csiphy = 0x04900000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_1_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
#ifdef CONFIG_MSM_BUS_SCALING
	.cam_bus_scale_table = &cam_bus_client_pdata,
#endif
};
#endif


struct msm_camera_device_platform_data msm_camera_device_data = {
	.camera_gpio_on  = config_camera_on_gpios,
	.camera_gpio_off = config_camera_off_gpios,
	.ioext.csiphy = 0x04800000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_0_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
#ifdef CONFIG_MSM_BUS_SCALING
	.cam_bus_scale_table = &cam_bus_client_pdata,
#endif
};

#ifdef CONFIG_QS_S5K4E1
struct msm_camera_device_platform_data msm_camera_device_data_qs_cam = {
	.camera_gpio_on  = config_camera_on_gpios_qs_cam,
	.camera_gpio_off = config_camera_off_gpios_qs_cam,
	.ioext.csiphy = 0x04800000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_0_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
#ifdef CONFIG_MSM_BUS_SCALING
	.cam_bus_scale_table = &cam_bus_client_pdata,
#endif
};
#endif

struct msm_camera_device_platform_data msm_camera_device_data_web_cam = {
	.camera_gpio_on  = config_camera_on_gpios_web_cam,
	.camera_gpio_off = config_camera_off_gpios_web_cam,
	.ioext.csiphy = 0x04900000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_1_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
#ifdef CONFIG_MSM_BUS_SCALING
	.cam_bus_scale_table = &cam_bus_client_pdata,
#endif
};

struct resource msm_camera_resources[] = {
	{
		.start	= 0x04500000,
		.end	= 0x04500000 + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= VFE_IRQ,
		.end	= VFE_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};
#ifdef CONFIG_MT9E013
static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type			= MSM_CAMERA_FLASH_LED,
	.flash_src			= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
		.sensor_name	= "mt9e013",
		.sensor_reset	= 106,
		.sensor_pwd		= 85,
		.vcm_pwd		= 1,
		.vcm_enable		= 0,
		.pdata			= &msm_camera_device_data,
		.resource		= msm_camera_resources,
		.num_resources	= ARRAY_SIZE(msm_camera_resources),
		.flash_data		= &flash_mt9e013,
		.strobe_flash_data	= &strobe_flash_xenon,
		.csi_if			= 1
};
struct platform_device msm_camera_sensor_mt9e013 = {
		.name	= "msm_camera_mt9e013",
		.dev	= {
			.platform_data = &msm_camera_sensor_mt9e013_data,
		},
};
#endif

#ifdef CONFIG_IMX074
static struct msm_camera_sensor_flash_data flash_imx074 = {
	.flash_type		= MSM_CAMERA_FLASH_LED,
	.flash_src		= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_imx074_data = {
	.sensor_name		= "imx074",
	.sensor_reset		= 106,
	.sensor_pwd		= 85,
	.vcm_pwd		= GPIO_AUX_CAM_2P7_EN,
	.vcm_enable		= 1,
	.pdata			= &msm_camera_device_data,
	.resource		= msm_camera_resources,
	.num_resources		= ARRAY_SIZE(msm_camera_resources),
	.flash_data		= &flash_imx074,
	.strobe_flash_data	= &strobe_flash_xenon,
	.sensor_platform_info = &sensor_board_info,
	.csi_if			= 1
};
struct platform_device msm_camera_sensor_imx074 = {
	.name	= "msm_camera_imx074",
	.dev	= {
		.platform_data = &msm_camera_sensor_imx074_data,
	},
};
#endif
#ifdef CONFIG_WEBCAM_OV9726

static struct msm_camera_sensor_platform_info ov9726_sensor_8660_info = {
	.mount_angle = 0
};

static struct msm_camera_sensor_flash_data flash_ov9726 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
	.flash_src	= &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data = {
	.sensor_name	= "ov9726",
	.sensor_reset	= GPIO_FRONT_CAM_RESET_N,
	.sensor_pwd	= 85,
	.vcm_pwd	= 1,
	.vcm_enable	= 0,
	.pdata		= &msm_camera_device_data_web_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_ov9726,
	.sensor_platform_info = &ov9726_sensor_8660_info,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_webcam_ov9726 = {
	.name	= "msm_camera_ov9726",
	.dev	= {
		.platform_data = &msm_camera_sensor_ov9726_data,
	},
};
#endif
#ifdef CONFIG_WEBCAM_OV7692
static struct msm_camera_sensor_flash_data flash_ov7692 = {
	.flash_type		= MSM_CAMERA_FLASH_LED,
	.flash_src		= &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_ov7692_data = {
	.sensor_name	= "ov7692",
	.sensor_reset	= GPIO_WEB_CAMIF_RESET_N,
	.sensor_pwd	= 85,
	.vcm_pwd	= 1,
	.vcm_enable	= 0,
	.pdata		= &msm_camera_device_data_web_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_ov7692,
	.csi_if		= 1
};

static struct platform_device msm_camera_sensor_webcam_ov7692 = {
	.name	= "msm_camera_ov7692",
	.dev	= {
		.platform_data = &msm_camera_sensor_ov7692_data,
	},
};
#endif
#ifdef CONFIG_QS_S5K4E1

static char eeprom_data[864];
static struct msm_camera_sensor_flash_data flash_qs_s5k4e1 = {
	.flash_type		= MSM_CAMERA_FLASH_LED,
	.flash_src		= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_qs_s5k4e1_data = {
	.sensor_name	= "qs_s5k4e1",
	.sensor_reset	= 106,
	.sensor_pwd		= 85,
	.vcm_pwd		= 1,
	.vcm_enable		= 0,
	.pdata			= &msm_camera_device_data_qs_cam,
	.resource		= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data		= &flash_qs_s5k4e1,
	.strobe_flash_data	= &strobe_flash_xenon,
	.csi_if			= 1,
	.eeprom_data	= eeprom_data,
};
struct platform_device msm_camera_sensor_qs_s5k4e1 = {
	.name	= "msm_camera_qs_s5k4e1",
	.dev	= {
		.platform_data = &msm_camera_sensor_qs_s5k4e1_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_CE1612
static struct msm_camera_sensor_flash_data flash_ce1612 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_ce1612_data = {
	.sensor_name	= "ce1612",
	.pdata		= &msm_camera_device_data_main_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_ce1612,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_ce1612 = {
	.name	= "msm_camera_ce1612",
	.dev	= {
		.platform_data = &msm_camera_sensor_ce1612_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_ICP_HD
static struct msm_camera_sensor_flash_data flash_icp_hd = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_icp_hd_data = {
	.sensor_name	= "icp_hd",
	.pdata		= &msm_camera_device_data_main_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_icp_hd,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_icp_hd = {
	.name	= "msm_camera_icp_hd",
	.dev	= {
		.platform_data = &msm_camera_sensor_icp_hd_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_MT9P111
static struct msm_camera_sensor_flash_data flash_mt9p111 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
	.flash_src	= &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_mt9p111_data = {
	.sensor_name	= "mt9p111",
	.pdata		= &msm_camera_device_data_main_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_mt9p111,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_mt9p111 = {
	.name	= "msm_camera_mt9p111",
	.dev	= {
		.platform_data = &msm_camera_sensor_mt9p111_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_S5K4ECGX
static struct msm_camera_sensor_flash_data flash_s5k4ecgx = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4ecgx_data = {
	.sensor_name	= "s5k4ecgx",
	.pdata		= &msm_camera_device_data_main_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_s5k4ecgx,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_s5k4ecgx = {
	.name	= "msm_camera_s5k4ecgx",
	.dev	= {
		.platform_data = &msm_camera_sensor_s5k4ecgx_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_MT9D113
static struct msm_camera_sensor_flash_data flash_mt9d113 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9d113_data = {
	.sensor_name	= "mt9d113",
	.pdata		= &msm_camera_device_data_sub_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_mt9d113,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_mt9d113 = {
	.name	= "msm_camera_mt9d113",
	.dev	= {
		.platform_data = &msm_camera_sensor_mt9d113_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_S5K6AAFX13
static struct msm_camera_sensor_flash_data flash_s5k6aafx13 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k6aafx13_data = {
	.sensor_name	= "s5k6aafx13",
	.pdata		= &msm_camera_device_data_sub_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_s5k6aafx13,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_s5k6aafx13 = {
	.name	= "msm_camera_s5k6aafx13",
	.dev	= {
		.platform_data = &msm_camera_sensor_s5k6aafx13_data,
	},
};
#endif

#ifdef CONFIG_PANTECH_CAMERA_MT9V113
static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
	.flash_src	= &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
	.sensor_name	= "mt9v113",
	.pdata		= &msm_camera_device_data_sub_cam,
	.resource	= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data	= &flash_mt9v113,
	.csi_if		= 1
};
struct platform_device msm_camera_sensor_mt9v113 = {
	.name	= "msm_camera_mt9v113",
	.dev	= {
		.platform_data = &msm_camera_sensor_mt9v113_data,
	},
};
#endif

static struct i2c_board_info msm_camera_boardinfo[] __initdata = {
	#ifdef CONFIG_MT9E013
	{
		I2C_BOARD_INFO("mt9e013", 0x6C >> 2),
	},
	#endif
	#ifdef CONFIG_IMX074
	{
		I2C_BOARD_INFO("imx074", 0x1A),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV7692
	{
		I2C_BOARD_INFO("ov7692", 0x78),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV9726
	{
		I2C_BOARD_INFO("ov9726", 0x10),
	},
	#endif
	#ifdef CONFIG_QS_S5K4E1
	{
		I2C_BOARD_INFO("qs_s5k4e1", 0x20),
	},
	#endif
#ifdef CONFIG_PANTECH_CAMERA_CE1612
	{
		I2C_BOARD_INFO("ce1612", 0x7C >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_ICP_HD
	{
		I2C_BOARD_INFO("icp_hd", 0x7A >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9P111
	{
		I2C_BOARD_INFO("mt9p111", 0x7A >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_S5K4ECGX
	{
		I2C_BOARD_INFO("s5k4ecgx", 0x5A >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9D113
	{
		I2C_BOARD_INFO("mt9d113", 0x78 >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_S5K6AAFX13
	{
		I2C_BOARD_INFO("s5k6aafx13", 0x5A >> 1),
	},
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9V113
	{
		I2C_BOARD_INFO("mt9v113", 0x78 >> 1),
	},
#endif
};
#endif
#ifdef CONFIG_SKY_BATTERY_MAX17040 //p14682 kobj 110607ps2 team shs : fuel gauge porting

static struct i2c_board_info max17040_i2c_boardinfo[] ={
	{  
		I2C_BOARD_INFO("max17040", 0x36),
	},
};
#endif //CONFIG_SKY_BATTERY_MAX17040
#ifdef CONFIG_MSM_GEMINI
static struct resource msm_gemini_resources[] = {
	{
		.start  = 0x04600000,
		.end    = 0x04600000 + SZ_1M - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = INT_JPEG,
		.end    = INT_JPEG,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device msm_gemini_device = {
	.name           = "msm_gemini",
	.resource       = msm_gemini_resources,
	.num_resources  = ARRAY_SIZE(msm_gemini_resources),
};
#endif

#ifdef CONFIG_I2C_QUP
#define GSBI7_SDA 59
#define GSBI7_SCL 60

static uint32_t gsbi7_gpio_table[] = {
	GPIO_CFG(GSBI7_SDA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(GSBI7_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi7_i2c_table[] = {
	GPIO_CFG(GSBI7_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(GSBI7_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
}

static void gsbi7_qup_i2c_gpio_config(int adap_id, int config_type)
{
	if (config_type == 0) {
		gpio_tlmm_config(gsbi7_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi7_gpio_table[1], GPIO_CFG_ENABLE);
	} else {
		gpio_tlmm_config(gsbi7_i2c_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi7_i2c_table[1], GPIO_CFG_ENABLE);
	}
}

static struct msm_i2c_platform_data msm_gsbi3_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi4_qup_i2c_pdata = {
#if defined(CONFIG_PANTECH_CAMERA_HW)
	.clk_freq = 384000,
#else
	.clk_freq = 100000,
#endif
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
static struct msm_i2c_platform_data msm_gsbi5_qup_i2c_pdata = {
	.clk_freq = 400000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif
#else
#ifdef CONFIG_SKY_SMB_CHARGER
static struct msm_i2c_platform_data msm_gsbi5_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif
#endif

static struct msm_i2c_platform_data msm_gsbi7_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.pri_clk = 60,
	.pri_dat = 59,
	.msm_i2c_config_gpio = gsbi7_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi8_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi9_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

#if defined(CONFIG_PN544)
static struct msm_i2c_platform_data msm_gsbi10_qup_i2c_pdata = {
	.clk_freq = 400000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif
#ifdef CONFIG_SKY_BATTERY_MAX17040 //p14682 kobj 110607ps2 team shs : fuel gauge porting
static struct msm_i2c_platform_data msm_gsbi11_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif //CONFIG_SKY_BATTERY_MAX17040
static struct msm_i2c_platform_data msm_gsbi12_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.clk = "gsbi_qup_clk",
	.pclk = "gsbi_pclk",
	.use_gsbi_shared_mode = 1,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
static struct msm_spi_platform_data msm_gsbi1_qup_spi_pdata = {
	.max_clock_speed = 24000000,
};

#ifndef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020  // jmlee 20110505 add
static struct msm_spi_platform_data msm_gsbi10_qup_spi_pdata = {
	.max_clock_speed = 24000000,
};
#endif
#endif

#ifdef CONFIG_SKY_TDMB_SPI_QUP
static struct msm_spi_platform_data msm_gsbi2_qup_spi_pdata = {
	.max_clock_speed = 10800000,
};
#endif /* CONFIG_SKY_TDMB_SPI_QUP */

#ifdef CONFIG_I2C_SSBI
/* PMIC SSBI */
static struct msm_i2c_ssbi_platform_data msm_ssbi1_pdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
};

/* PMIC SSBI */
static struct msm_i2c_ssbi_platform_data msm_ssbi2_pdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
};

/* CODEC/TSSC SSBI */
static struct msm_i2c_ssbi_platform_data msm_ssbi3_pdata = {
	.controller_type = MSM_SBI_CTRL_SSBI,
};
#endif

#ifdef CONFIG_BATTERY_MSM
/* Use basic value for fake MSM battery */
static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.avail_chg_sources = AC_CHG,
};

static struct platform_device msm_batt_device = {
	.name              = "msm-battery",
	.id                = -1,
	.dev.platform_data = &msm_psy_batt_data,
};
#endif

#ifdef CONFIG_FB_MSM_LCDC_DSUB
/* VGA = 1440 x 900 x 4(bpp) x 2(pages)
   prim = 1024 x 600 x 4(bpp) x 2(pages)
   This is the difference. */
#define MSM_FB_DSUB_PMEM_ADDER (0x9E3400-0x4B0000)
#else
#define MSM_FB_DSUB_PMEM_ADDER (0)
#endif

/* Sensors DSPS platform data */
#ifdef CONFIG_MSM_DSPS

static struct dsps_gpio_info dsps_surf_gpios[] = {
	{
		.name = "compass_rst_n",
		.num = GPIO_COMPASS_RST_N,
		.on_val = 1,	/* device not in reset */
		.off_val = 0,	/* device in reset */
	},
	{
		.name = "gpio_r_altimeter_reset_n",
		.num = GPIO_R_ALTIMETER_RESET_N,
		.on_val = 1,	/* device not in reset */
		.off_val = 0,	/* device in reset */
	}
};

static struct dsps_gpio_info dsps_fluid_gpios[] = {
	{
		.name = "gpio_n_altimeter_reset_n",
		.num = GPIO_N_ALTIMETER_RESET_N,
		.on_val = 1,	/* device not in reset */
		.off_val = 0,	/* device in reset */
	}
};

static void __init msm8x60_init_dsps(void)
{
	struct msm_dsps_platform_data *pdata =
		msm_dsps_device.dev.platform_data;
	/*
	 * On Fluid the Compass sensor Chip-Select (CS) is directly connected
	 * to the power supply and not controled via GPIOs. Fluid uses a
	 * different IO-Expender (north) than used on surf/ffa.
	 */
	if (machine_is_msm8x60_fluid()) {
		/* fluid has different firmware, gpios */
		peripheral_dsps.name = DSPS_PIL_FLUID_NAME;
		pdata->pil_name = DSPS_PIL_FLUID_NAME;
		pdata->gpios = dsps_fluid_gpios;
		pdata->gpios_num = ARRAY_SIZE(dsps_fluid_gpios);
	} else {
		peripheral_dsps.name = DSPS_PIL_GENERIC_NAME;
		pdata->pil_name = DSPS_PIL_GENERIC_NAME;
		pdata->gpios = dsps_surf_gpios;
		pdata->gpios_num = ARRAY_SIZE(dsps_surf_gpios);
	}

	msm_pil_add_device(&peripheral_dsps);

	platform_device_register(&msm_dsps_device);
}
#endif /* CONFIG_MSM_DSPS */

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined (CONFIG_MACH_MSM8X60_EF40K) ||defined (CONFIG_MACH_MSM8X60_EF40S)
/* prim = 1280 x 800 x 4(bpp) x 3(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0xBB8000
#elif defined CONFIG_MACH_MSM8X60_PRESTO
/* prim = 800 x 480 x 4(bpp) x 3(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0x465000
#else
/* prim = 1024 x 600 x 4(bpp) x 3(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0x708000
#endif
#else
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined (CONFIG_MACH_MSM8X60_EF40K) \
	|| defined (CONFIG_MACH_MSM8X60_EF40S)
/* prim = 1280 x 800 x 4(bpp) x 2(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0x7D0000
#elif defined CONFIG_MACH_MSM8X60_PRESTO
/* prim = 800 x 480 x 4(bpp) x 2(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0x2EE000
#else
/* prim = 1024 x 600 x 4(bpp) x 2(pages) */
#define MSM_FB_PRIM_BUF_SIZE 0x4B0000
#endif
#endif


#ifdef CONFIG_FB_MSM_OVERLAY_WRITEBACK
/* width x height x 3 bpp x 2 frame buffer */
//#define MSM_FB_WRITEBACK_SIZE (1024 * 600 * 3 * 2)
#ifdef CONFIG_MACH_MSM8X60_PRESTO
#define MSM_FB_WRITEBACK_SIZE (480 * 800 * 3 * 2)
#else
#define MSM_FB_WRITEBACK_SIZE (800 * 1280 * 3 * 2)
#endif
#else
#define MSM_FB_WRITEBACK_SIZE 0
#endif

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
#ifdef CONFIG_F_SKYDISP_HDMI_OPTION
/* prim = 800 x 1280 x 4(bpp) x 2(pages)
 * hdmi = 1920 x 1080 x 4(bpp) x 2(page)
 * Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + 0xFD2000 + \
			MSM_FB_WRITEBACK_SIZE + MSM_FB_DSUB_PMEM_ADDER, 4096)
#else
/* prim = 1024 x 600 x 4(bpp) x 2(pages)
 * hdmi = 1920 x 1080 x 2(bpp) x 1(page)
 * Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + 0x3F4800 + \
			MSM_FB_WRITEBACK_SIZE + MSM_FB_DSUB_PMEM_ADDER, 4096)
#endif
#elif defined(CONFIG_FB_MSM_TVOUT)
/* prim = 1024 x 600 x 4(bpp) x 2(pages)
 * tvout = 720 x 576 x 2(bpp) x 2(pages)
 * Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + 0x195000 + \
			MSM_FB_WRITEBACK_SIZE + MSM_FB_DSUB_PMEM_ADDER, 4096)
#else /* CONFIG_FB_MSM_HDMI_MSM_PANEL */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE +  \
			MSM_FB_WRITEBACK_SIZE + MSM_FB_DSUB_PMEM_ADDER, 4096)
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */

#ifndef CONFIG_F_SKYDISP_USE_ASHMEM
#ifdef CONFIG_MACH_MSM8X60_PRESTO
#define MSM_PMEM_SF_SIZE 0x2000000 /* 32 Mbytes */
#else
#define MSM_PMEM_SF_SIZE 0x4000000 /* 64 Mbytes */
#endif
#endif

#define MSM_PMEM_KERNEL_EBI1_SIZE  0x600000
#define MSM_PMEM_ADSP_SIZE         0x2000000
#define MSM_PMEM_AUDIO_SIZE        0x279000

#define MSM_SMI_BASE          0x38000000
/* Kernel SMI PMEM Region for video core, used for Firmware */
/* and encoder,decoder scratch buffers */
/* Kernel SMI PMEM Region Should always precede the user space */
/* SMI PMEM Region, as the video core will use offset address */
/* from the Firmware base */
#define PMEM_KERNEL_SMI_BASE  (MSM_SMI_BASE)
#define PMEM_KERNEL_SMI_SIZE  0x600000
/* User space SMI PMEM Region for video core*/
/* used for encoder, decoder input & output buffers  */
#define MSM_PMEM_SMIPOOL_BASE (PMEM_KERNEL_SMI_BASE + PMEM_KERNEL_SMI_SIZE)
#define MSM_PMEM_SMIPOOL_SIZE 0x3A00000

static unsigned fb_size = MSM_FB_SIZE;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}
early_param("fb_size", fb_size_setup);

#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
static unsigned pmem_kernel_ebi1_size = MSM_PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_F_SKYDISP_USE_ASHMEM
static unsigned pmem_sf_size = MSM_PMEM_SF_SIZE;
static int __init pmem_sf_size_setup(char *p)
{
	pmem_sf_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_sf_size", pmem_sf_size_setup);
#endif

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;

static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);
#endif

static struct resource msm_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

#ifdef CONFIG_FB_MSM_LCDC_AUTO_DETECT
static int msm_fb_detect_panel(const char *name)
{
	if (machine_is_msm8x60_fluid()) {
		uint32_t soc_platform_version = socinfo_get_platform_version();
		if (SOCINFO_VERSION_MAJOR(soc_platform_version) < 3) {
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
			if (!strncmp(name, LCDC_SAMSUNG_OLED_PANEL_NAME,
					strlen(LCDC_SAMSUNG_OLED_PANEL_NAME)))
				return 0;
#endif
		} else { /*P3 and up use AUO panel */
#ifdef CONFIG_FB_MSM_LCDC_AUO_WVGA
			if (!strncmp(name, LCDC_AUO_PANEL_NAME,
					strlen(LCDC_AUO_PANEL_NAME)))
				return 0;
#endif
		}
		if (!strncmp(name, LCDC_SAMSUNG_WSVGA_PANEL_NAME,
				strlen(LCDC_SAMSUNG_WSVGA_PANEL_NAME)))
			return -ENODEV;
	} else {
		if (!strncmp(name, LCDC_SAMSUNG_WSVGA_PANEL_NAME,
				strlen(LCDC_SAMSUNG_WSVGA_PANEL_NAME)))
			return 0;
		if (!strncmp(name, LCDC_SAMSUNG_OLED_PANEL_NAME,
				strlen(LCDC_SAMSUNG_OLED_PANEL_NAME)))
			return -ENODEV;
	}
	pr_warning("%s: not supported '%s'", __func__, name);
	return -ENODEV;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
};
#endif /* CONFIG_FB_MSM_LCDC_AUTO_DETECT */

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
#ifdef CONFIG_FB_MSM_LCDC_AUTO_DETECT
	.dev.platform_data = &msm_fb_pdata,
#endif /* CONFIG_FB_MSM_LCDC_AUTO_DETECT */
};

#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
static struct android_pmem_platform_data android_pmem_kernel_ebi1_pdata = {
	.name = PMEM_KERNEL_EBI1_DATA_NAME,
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_kernel_ebi1_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_kernel_ebi1_pdata },
};
#endif

#ifdef CONFIG_KERNEL_PMEM_SMI_REGION
static struct android_pmem_platform_data android_pmem_kernel_smi_pdata = {
	.name = PMEM_KERNEL_SMI_DATA_NAME,
	/* defaults to bitmap don't edit */
	.cached = 0,
};

static struct platform_device android_pmem_kernel_smi_device = {
	.name = "android_pmem",
	.id = 6,
	.dev = { .platform_data = &android_pmem_kernel_smi_pdata },
};
#endif

#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_ALLORNOTHING,
	.cached = 1,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};

#define PMEM_BUS_WIDTH(_bw) \
	{ \
		.vectors = &(struct msm_bus_vectors){ \
			.src = MSM_BUS_MASTER_AMPSS_M0, \
			.dst = MSM_BUS_SLAVE_SMI, \
			.ib = (_bw), \
			.ab = 0, \
		}, \
	.num_paths = 1, \
	}
static struct msm_bus_paths pmem_smi_table[] = {
	[0] = PMEM_BUS_WIDTH(0), /* Off */
	[1] = PMEM_BUS_WIDTH(1), /* On */
};

static struct msm_bus_scale_pdata smi_client_pdata = {
	.usecase = pmem_smi_table,
	.num_usecases = ARRAY_SIZE(pmem_smi_table),
	.name = "pmem_smi",
};

int pmem_request_smi_region(void *data)
{
	int bus_id = (int) data;

	msm_bus_scale_client_update_request(bus_id, 1);
	return 0;
}

int pmem_release_smi_region(void *data)
{
	int bus_id = (int) data;

	msm_bus_scale_client_update_request(bus_id, 0);
	return 0;
}

void *pmem_setup_smi_region(void)
{
	return (void *)msm_bus_scale_register_client(&smi_client_pdata);
}
static struct android_pmem_platform_data android_pmem_smipool_pdata = {
	.name = "pmem_smipool",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.request_region = pmem_request_smi_region,
	.release_region = pmem_release_smi_region,
	.setup_region = pmem_setup_smi_region,
	.map_on_demand = 1,
};
static struct platform_device android_pmem_smipool_device = {
	.name = "android_pmem",
	.id = 7,
	.dev = { .platform_data = &android_pmem_smipool_pdata },
};

#endif

#define GPIO_DONGLE_PWR_EN 258
static void setup_display_power(void);
static int lcdc_vga_enabled;
static int vga_enable_request(int enable)
{
	if (enable)
		lcdc_vga_enabled = 1;
	else
		lcdc_vga_enabled = 0;
	setup_display_power();

	return 0;
}

#define GPIO_BACKLIGHT_PWM0 0
#define GPIO_BACKLIGHT_PWM1 1

static int pmic_backlight_gpio[2]
	= { GPIO_BACKLIGHT_PWM0, GPIO_BACKLIGHT_PWM1 };
static struct msm_panel_common_pdata lcdc_samsung_panel_data = {
	.gpio_num = pmic_backlight_gpio, /* two LPG CHANNELS for backlight */
	.vga_switch = vga_enable_request,
};

static struct platform_device lcdc_samsung_panel_device = {
	.name = LCDC_SAMSUNG_WSVGA_PANEL_NAME,
	.id = 0,
	.dev = {
		.platform_data = &lcdc_samsung_panel_data,
	}
};
#if (!defined(CONFIG_SPI_QUP)) && \
	(defined(CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT) || \
	defined(CONFIG_FB_MSM_LCDC_AUO_WVGA))

static int lcdc_spi_gpio_array_num[] = {
	LCDC_SPI_GPIO_CLK,
	LCDC_SPI_GPIO_CS,
	LCDC_SPI_GPIO_MOSI,
};

static uint32_t lcdc_spi_gpio_config_data[] = {
	GPIO_CFG(LCDC_SPI_GPIO_CLK, 0,
			GPIO_CFG_OUTPUT,GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(LCDC_SPI_GPIO_CS, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(LCDC_SPI_GPIO_MOSI, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};


#ifdef CONFIG_MACH_MSM8X60_PRESTO  // p13777 kej 110623
static uint32_t lcdc_gpio_config_data[] = {
	GPIO_CFG(0, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_pclk" },
	GPIO_CFG(1, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_hsync" },
	GPIO_CFG(2, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_vsync" },
	GPIO_CFG(3, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_en" },

	GPIO_CFG(4, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red7" },
	GPIO_CFG(5, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red6" },
	GPIO_CFG(6, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red5" },
	GPIO_CFG(7, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red4" },
	GPIO_CFG(8, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red3" },
	GPIO_CFG(9, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red2" },
	GPIO_CFG(10, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red1" },
	GPIO_CFG(11, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_red0" },

	GPIO_CFG(12, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn7" },
	GPIO_CFG(13, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn6" },
	GPIO_CFG(14, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn5" },
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn4" },
	GPIO_CFG(16, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn3" },
	GPIO_CFG(17, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn2" },
	GPIO_CFG(18, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn1" },
	GPIO_CFG(19, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_grn0" },

	GPIO_CFG(20, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu7" },
	GPIO_CFG(21, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu6" },
	GPIO_CFG(22, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu5" },
	GPIO_CFG(23, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu4" },
	GPIO_CFG(24, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu3" },
	GPIO_CFG(25, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu2" },
	GPIO_CFG(26, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu1" },
	GPIO_CFG(27, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), //"lcdc_blu0" },
};
#endif


#ifdef CONFIG_MACH_MSM8X60_PRESTO   // p13777 kej 110623
static void lcdc_config_spi_gpios(int enable)
{
	int n;
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	int m;
#endif
	//printk(KERN_ERR "+%s: kkcho for preston spi_init \n", __func__);
	for (n = 0; n < ARRAY_SIZE(lcdc_spi_gpio_config_data); ++n)
		gpio_tlmm_config(lcdc_spi_gpio_config_data[n], 0);
	//printk(KERN_ERR "-%s: kkcho for preston spi_init \n", __func__);

#ifdef CONFIG_MACH_MSM8X60_PRESTO  // kkcho_temp_presto
	//printk(KERN_ERR "+%s: kkcho for preston LCDC_init \n", __func__);
	for (m = 0; m < ARRAY_SIZE(lcdc_gpio_config_data); ++m)
		gpio_tlmm_config(lcdc_gpio_config_data[m], 0);
	//printk(KERN_ERR "-%s: kkcho for preston LCDC_init \n", __func__);
#endif

}

#else
static void lcdc_config_spi_gpios(int enable)
{
	int n;
	for (n = 0; n < ARRAY_SIZE(lcdc_spi_gpio_config_data); ++n)
		gpio_tlmm_config(lcdc_spi_gpio_config_data[n], 0);
}

#endif 
#endif

#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
#ifdef CONFIG_SPI_QUP
static struct spi_board_info lcdc_samsung_spi_board_info[] __initdata = {
	{
		.modalias       = LCDC_SAMSUNG_SPI_DEVICE_NAME,
		.mode           = SPI_MODE_3,
		.bus_num        = 1,
		.chip_select    = 0,
		.max_speed_hz   = 10800000,
	}
};
#endif /* CONFIG_SPI_QUP */

static struct msm_panel_common_pdata lcdc_samsung_oled_panel_data = {
#ifndef CONFIG_SPI_QUP
	.panel_config_gpio = lcdc_config_spi_gpios,
	.gpio_num          = lcdc_spi_gpio_array_num,
#endif
};

static struct platform_device lcdc_samsung_oled_panel_device = {
	.name   = LCDC_SAMSUNG_OLED_PANEL_NAME,
	.id     = 0,
	.dev.platform_data = &lcdc_samsung_oled_panel_data,
};
#endif /*CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT */

#ifdef CONFIG_FB_MSM_LCDC_AUO_WVGA
#ifdef CONFIG_SPI_QUP
static struct spi_board_info lcdc_auo_spi_board_info[] __initdata = {
	{
		.modalias       = LCDC_AUO_SPI_DEVICE_NAME,
		.mode           = SPI_MODE_3,
		.bus_num        = 1,
		.chip_select    = 0,
		.max_speed_hz   = 10800000,
	}
};
#endif

static struct msm_panel_common_pdata lcdc_auo_wvga_panel_data = {
#ifndef CONFIG_SPI_QUP
	.panel_config_gpio = lcdc_config_spi_gpios,
	.gpio_num          = lcdc_spi_gpio_array_num,
#endif
};

static struct platform_device lcdc_auo_wvga_panel_device = {
	.name   = LCDC_AUO_PANEL_NAME,
	.id     = 0,
	.dev.platform_data = &lcdc_auo_wvga_panel_data,
};
#endif /*CONFIG_FB_MSM_LCDC_AUO_WVGA*/

#ifdef CONFIG_SKY_TDMB_SPI_IF
static struct spi_board_info tdmb_spi_board_info[] __initdata = {
	{
		.modalias       = "tdmb_spi",
#ifdef CONFIG_MACH_MSM8X60_EF39S
		.mode           = SPI_MODE_0,
#else
		.mode           = SPI_MODE_2,
#endif
		.bus_num        = 5,
		.chip_select    = 0,
		.max_speed_hz   = 10800000,
		//.max_speed_hz   = 24000000,
#ifdef CONFIG_SKY_TDMB_SPI_GPIO
		.controller_data = (void *) 39, // chip select
#endif
	}
};
#endif /* CONFIG_SKY_TDMB_SPI_IF */

#ifdef CONFIG_SKY_TDMB_SPI_GPIO
static struct spi_gpio_platform_data gpio_spi_tdmb_data = {
	.sck = 40,
	.mosi = 37,
	.miso = 38,
	.num_chipselect = 1,
};
struct platform_device gpio_spi_tdmb_device = {
	.name = "spi_gpio",
	.id = 5,
	.dev = {
		.platform_data = &gpio_spi_tdmb_data,
	}
};
#endif /* CONFIG_SKY_TDMB_SPI_GPIO */

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct resource hdmi_msm_resources[] = {
	{
		.name  = "hdmi_msm_qfprom_addr",
		.start = 0x00700000,
		.end   = 0x007060FF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_hdmi_addr",
		.start = 0x04A00000,
		.end   = 0x04A00FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_irq",
		.start = HDMI_IRQ,
		.end   = HDMI_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static int hdmi_enable_5v(int on);
static int hdmi_core_power(int on, int show);
static int hdmi_cec_power(int on);

static struct msm_hdmi_platform_data hdmi_msm_data = {
	.irq = HDMI_IRQ,
	.enable_5v = hdmi_enable_5v,
	.core_power = hdmi_core_power,
	.cec_power = hdmi_cec_power,
};

static struct platform_device hdmi_msm_device = {
	.name = "hdmi_msm",
	.id = 0,
	.num_resources = ARRAY_SIZE(hdmi_msm_resources),
	.resource = hdmi_msm_resources,
	.dev.platform_data = &hdmi_msm_data,
};
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */

#ifdef CONFIG_FB_MSM_MIPI_DSI_SAMSUNG
static struct platform_device mipi_dsi_samsung_panel_device = {
	.name = "mipi_samsung",
	.id = 0,
};
#endif

#ifdef CONFIG_FB_MSM_MIPI_DSI_SONY
static struct platform_device mipi_dsi_sony_panel_device = {
	.name = "mipi_sony",
	.id = 0,
};
#endif

#ifdef CONFIG_FB_MSM_MIPI_DSI_TOSHIBA
static struct platform_device mipi_dsi_toshiba_panel_device = {
	.name = "mipi_toshiba",
	.id = 0,
};
#endif

#ifdef CONFIG_FB_MSM_MIPI_DSI_NOVATEK
#define FPGA_3D_GPIO_CONFIG_ADDR	0x1D00017A

static struct mipi_dsi_novatek_platform_data novatek_pdata = {
	.fpga_3d_config_addr  = FPGA_3D_GPIO_CONFIG_ADDR,
};

static struct platform_device mipi_dsi_novatek_panel_device = {
	.name = "mipi_novatek",
	.id = 0,
	.dev = {
		.platform_data = &novatek_pdata,
	}
};
#endif

static void __init msm8x60_allocate_memory_regions(void)
{
	void *addr;
	unsigned long size;

	size = MSM_FB_SIZE;
	addr = alloc_bootmem(size);
	msm_fb_resources[0].start = __pa(addr);
	msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
		size, addr, __pa(addr));

#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
	size = pmem_kernel_ebi1_size;
	if (size) {
		addr = alloc_bootmem_aligned(size, 0x100000);
		android_pmem_kernel_ebi1_pdata.start = __pa(addr);
		android_pmem_kernel_ebi1_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for kernel"
			" ebi1 pmem arena\n", size, addr, __pa(addr));
	}
#endif

#ifdef CONFIG_KERNEL_PMEM_SMI_REGION
	size = PMEM_KERNEL_SMI_SIZE;
	if (size) {
		android_pmem_kernel_smi_pdata.start = PMEM_KERNEL_SMI_BASE;
		android_pmem_kernel_smi_pdata.size = size;
		pr_info("allocating %lu bytes at %lx physical for kernel"
			" smi pmem arena\n", size,
			(unsigned long) PMEM_KERNEL_SMI_BASE);
	}
#endif

#ifdef CONFIG_ANDROID_PMEM
	size = pmem_adsp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_adsp_pdata.start = __pa(addr);
		android_pmem_adsp_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for adsp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = MSM_PMEM_SMIPOOL_SIZE;
	if (size) {
		android_pmem_smipool_pdata.start = MSM_PMEM_SMIPOOL_BASE;
		android_pmem_smipool_pdata.size = size;
		pr_info("allocating %lu bytes at %lx physical for user"
			" smi  pmem arena\n", size,
			(unsigned long) MSM_PMEM_SMIPOOL_BASE);
	}

	size = MSM_PMEM_AUDIO_SIZE;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_audio_pdata.start = __pa(addr);
		android_pmem_audio_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for audio "
			"pmem arena\n", size, addr, __pa(addr));
	}

#ifndef CONFIG_F_SKYDISP_USE_ASHMEM
	size = pmem_sf_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_pdata.start = __pa(addr);
		android_pmem_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for sf "
			"pmem arena\n", size, addr, __pa(addr));
	}
#endif
#endif
}

#if defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C) || \
		defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C_MODULE)
/*virtual key support */
static ssize_t tma300_vkeys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,
	__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":60:900:90:120"
	":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":180:900:90:120"
	":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":300:900:90:120"
	":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":420:900:90:120"
	"\n");
}

static struct kobj_attribute tma300_vkeys_attr = {
	.attr = {
		.mode = S_IRUGO,
	},
	.show = &tma300_vkeys_show,
};

static struct attribute *tma300_properties_attrs[] = {
	&tma300_vkeys_attr.attr,
	NULL
};

static struct attribute_group tma300_properties_attr_group = {
	.attrs = tma300_properties_attrs,
};

static struct kobject *properties_kobj;



#define CYTTSP_TS_GPIO_IRQ	61
static int cyttsp_platform_init(struct i2c_client *client)
{
	int rc = -EINVAL;
	struct regulator *pm8058_l5 = NULL, *pm8058_s3;

	if (machine_is_msm8x60_fluid()) {
		pm8058_l5 = regulator_get(NULL, "8058_l5");
		if (IS_ERR(pm8058_l5)) {
			pr_err("%s: regulator get of 8058_l5 failed (%ld)\n",
				__func__, PTR_ERR(pm8058_l5));
			rc = PTR_ERR(pm8058_l5);
			return rc;
		}
		rc = regulator_set_voltage(pm8058_l5, 2850000, 2850000);
		if (rc) {
			pr_err("%s: regulator_set_voltage of 8058_l5 failed(%d)\n",
				__func__, rc);
			goto reg_l5_put;
		}

		rc = regulator_enable(pm8058_l5);
		if (rc) {
			pr_err("%s: regulator_enable of 8058_l5 failed(%d)\n",
				__func__, rc);
			goto reg_l5_put;
		}
	}
	/* vote for s3 to enable i2c communication lines */
	pm8058_s3 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(pm8058_s3)) {
		pr_err("%s: regulator get of 8058_s3 failed (%ld)\n",
			__func__, PTR_ERR(pm8058_s3));
		rc = PTR_ERR(pm8058_s3);
		goto reg_l5_disable;
	}

	rc = regulator_set_voltage(pm8058_s3, 1800000, 1800000);
	if (rc) {
		pr_err("%s: regulator_set_voltage() = %d\n",
			__func__, rc);
		goto reg_s3_put;
	}

	rc = regulator_enable(pm8058_s3);
	if (rc) {
		pr_err("%s: regulator_enable of 8058_l5 failed(%d)\n",
			__func__, rc);
		goto reg_s3_put;
	}

	/* wait for vregs to stabilize */
	usleep_range(10000, 10000);

	/* check this device active by reading first byte/register */
	rc = i2c_smbus_read_byte_data(client, 0x01);
	if (rc < 0) {
		pr_err("%s: i2c sanity check failed\n", __func__);
		goto reg_s3_disable;
	}

	/* configure touchscreen interrupt gpio */
	rc = gpio_request(CYTTSP_TS_GPIO_IRQ, "cyttsp_irq_gpio");
	if (rc) {
		pr_err("%s: unable to request gpio %d\n",
			__func__, CYTTSP_TS_GPIO_IRQ);
		goto reg_s3_disable;
	}

	/* virtual keys */
	if (machine_is_msm8x60_fluid()) {
		tma300_vkeys_attr.attr.name = "virtualkeys.cyttsp-i2c";
		properties_kobj = kobject_create_and_add("board_properties",
					NULL);
		if (properties_kobj)
			rc = sysfs_create_group(properties_kobj,
				&tma300_properties_attr_group);
		if (!properties_kobj || rc)
			pr_err("%s: failed to create board_properties\n",
					__func__);
	}
	return CY_OK;

reg_s3_disable:
	regulator_disable(pm8058_s3);
reg_s3_put:
	regulator_put(pm8058_s3);
reg_l5_disable:
	if (machine_is_msm8x60_fluid())
		regulator_disable(pm8058_l5);
reg_l5_put:
	if (machine_is_msm8x60_fluid())
		regulator_put(pm8058_l5);
	return rc;
}

static int cyttsp_platform_resume(struct i2c_client *client)
{
	/* add any special code to strobe a wakeup pin or chip reset */
	msleep(10);

	return CY_OK;
}

static struct cyttsp_platform_data cyttsp_fluid_pdata = {
	.flags = 0x04,
	.gen = CY_GEN3,	/* or */
	.use_st = CY_USE_ST,
	.use_mt = CY_USE_MT,
	.use_hndshk = CY_SEND_HNDSHK,
	.use_trk_id = CY_USE_TRACKING_ID,
	.use_sleep = CY_USE_DEEP_SLEEP_SEL | CY_USE_LOW_POWER_SEL,
	.use_gestures = CY_USE_GESTURES,
	/* activate up to 4 groups
	 * and set active distance
	 */
	.gest_set = CY_GEST_GRP1 | CY_GEST_GRP2 |
				CY_GEST_GRP3 | CY_GEST_GRP4 |
				CY_ACT_DIST,
	/* change act_intrvl to customize the Active power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.act_intrvl = CY_ACT_INTRVL_DFLT,
	/* change tch_tmout to customize the touch timeout for the
	 * Active power state for Operating mode
	 */
	.tch_tmout = CY_TCH_TMOUT_DFLT,
	/* change lp_intrvl to customize the Low Power power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.lp_intrvl = CY_LP_INTRVL_DFLT,
	.resume = cyttsp_platform_resume,
	.init = cyttsp_platform_init,
};

static struct cyttsp_platform_data cyttsp_tmg240_pdata = {
	.panel_maxx = 1083,
	.panel_maxy = 659,
	.disp_minx = 30,
	.disp_maxx = 1053,
	.disp_miny = 30,
	.disp_maxy = 629,
	.correct_fw_ver = 8,
	.fw_fname = "cyttsp_8660_ffa.hex",
	.flags = 0x00,
	.gen = CY_GEN2,	/* or */
	.use_st = CY_USE_ST,
	.use_mt = CY_USE_MT,
	.use_hndshk = CY_SEND_HNDSHK,
	.use_trk_id = CY_USE_TRACKING_ID,
	.use_sleep = CY_USE_DEEP_SLEEP_SEL | CY_USE_LOW_POWER_SEL,
	.use_gestures = CY_USE_GESTURES,
	/* activate up to 4 groups
	 * and set active distance
	 */
	.gest_set = CY_GEST_GRP1 | CY_GEST_GRP2 |
				CY_GEST_GRP3 | CY_GEST_GRP4 |
				CY_ACT_DIST,
	/* change act_intrvl to customize the Active power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.act_intrvl = CY_ACT_INTRVL_DFLT,
	/* change tch_tmout to customize the touch timeout for the
	 * Active power state for Operating mode
	 */
	.tch_tmout = CY_TCH_TMOUT_DFLT,
	/* change lp_intrvl to customize the Low Power power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.lp_intrvl = CY_LP_INTRVL_DFLT,
	.resume = cyttsp_platform_resume,
	.init = cyttsp_platform_init,
	.disable_ghost_det = true,
};
static void cyttsp_set_params(void)
{
	if (SOCINFO_VERSION_MAJOR(socinfo_get_platform_version()) < 3) {
		cyttsp_fluid_pdata.fw_fname = "cyttsp_8660_fluid_p2.hex";
		cyttsp_fluid_pdata.panel_maxx = 539;
		cyttsp_fluid_pdata.panel_maxy = 994;
		cyttsp_fluid_pdata.disp_minx = 30;
		cyttsp_fluid_pdata.disp_maxx = 509;
		cyttsp_fluid_pdata.disp_miny = 60;
		cyttsp_fluid_pdata.disp_maxy = 859;
		cyttsp_fluid_pdata.correct_fw_ver = 4;
	} else {
		cyttsp_fluid_pdata.fw_fname = "cyttsp_8660_fluid_p3.hex";
		cyttsp_fluid_pdata.panel_maxx = 550;
		cyttsp_fluid_pdata.panel_maxy = 1013;
		cyttsp_fluid_pdata.disp_minx = 35;
		cyttsp_fluid_pdata.disp_maxx = 515;
		cyttsp_fluid_pdata.disp_miny = 69;
		cyttsp_fluid_pdata.disp_maxy = 869;
		cyttsp_fluid_pdata.correct_fw_ver = 5;
	}

}

static struct i2c_board_info cyttsp_fluid_info[] __initdata = {
	{
		I2C_BOARD_INFO(CY_I2C_NAME, 0x24),
		.platform_data = &cyttsp_fluid_pdata,
#ifndef CY_USE_TIMER
		.irq = MSM_GPIO_TO_INT(CYTTSP_TS_GPIO_IRQ),
#endif /* CY_USE_TIMER */
	},
};

static struct i2c_board_info cyttsp_ffa_info[] __initdata = {
	{
		I2C_BOARD_INFO(CY_I2C_NAME, 0x3b),
		.platform_data = &cyttsp_tmg240_pdata,
#ifndef CY_USE_TIMER
		.irq = MSM_GPIO_TO_INT(CYTTSP_TS_GPIO_IRQ),		
#endif /* CY_USE_TIMER */
	},
};
#endif

#if defined(CONFIG_TOUCHSCREEN_QT602240) || defined(CONFIG_TOUCHSCREEN_PRESTO_WS)
static struct i2c_board_info __initdata qt602240_i2c_boardinfo[] ={
	{
		I2C_BOARD_INFO("qt602240-i2c", 0x4A),
	},
};
#endif
static struct regulator *vreg_tmg200;

#define TS_PEN_IRQ_GPIO 61
static int tmg200_power(int vreg_on)
{
	int rc = -EINVAL;

	if (!vreg_tmg200) {
		printk(KERN_ERR "%s: regulator 8058_s3 not found (%d)\n",
			__func__, rc);
		return rc;
	}

	rc = vreg_on ? regulator_enable(vreg_tmg200) :
		  regulator_disable(vreg_tmg200);
	if (rc < 0)
		printk(KERN_ERR "%s: vreg 8058_s3 %s failed (%d)\n",
				__func__, vreg_on ? "enable" : "disable", rc);

	/* wait for vregs to stabilize */
	msleep(100);

	return rc;
}

static int tmg200_dev_setup(bool enable)
{
	int rc;

	if (enable) {
		vreg_tmg200 = regulator_get(NULL, "8058_s3");
		if (IS_ERR(vreg_tmg200)) {
			pr_err("%s: regulator get of 8058_s3 failed (%ld)\n",
				__func__, PTR_ERR(vreg_tmg200));
			rc = PTR_ERR(vreg_tmg200);
			return rc;
		}

		rc = regulator_set_voltage(vreg_tmg200, 1800000, 1800000);
		if (rc) {
			pr_err("%s: regulator_set_voltage() = %d\n",
				__func__, rc);
			goto reg_put;
		}
	} else {
		/* put voltage sources */
		regulator_put(vreg_tmg200);
	}
	return 0;
reg_put:
	regulator_put(vreg_tmg200);
	return rc;
}

static struct cy8c_ts_platform_data cy8ctmg200_pdata = {
	.ts_name = "msm_tmg200_ts",
	.dis_min_x = 0,
	.dis_max_x = 1023,
	.dis_min_y = 0,
	.dis_max_y = 599,
	.min_tid = 0,
	.max_tid = 255,
	.min_touch = 0,
	.max_touch = 255,
	.min_width = 0,
	.max_width = 255,
	.power_on = tmg200_power,
	.dev_setup = tmg200_dev_setup,
	.nfingers = 2,
	.irq_gpio = TS_PEN_IRQ_GPIO,
	.resout_gpio = GPIO_CAP_TS_RESOUT_N,
};

static struct i2c_board_info cy8ctmg200_board_info[] = {
	{
		I2C_BOARD_INFO("cy8ctmg200", 0x2),
		.platform_data = &cy8ctmg200_pdata,
	}
};

#ifdef CONFIG_SERIAL_MSM_HS
static int configure_uart_gpios(int on)
{
	int ret = 0, i;
	int uart_gpios[] = {53, 54, 55, 56};
	for (i = 0; i < ARRAY_SIZE(uart_gpios); i++) {
		if (on) {
			ret = msm_gpiomux_get(uart_gpios[i]);
			if (unlikely(ret))
				break;
		} else {
			ret = msm_gpiomux_put(uart_gpios[i]);
			if (unlikely(ret))
				return ret;
		}
	}
	if (ret)
		for (; i >= 0; i--)
			msm_gpiomux_put(uart_gpios[i]);
	return ret;
}
#ifdef CONFIG_PANTECH_BT
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
       .inject_rx_on_wakeup = 1,
       .rx_to_inject = 0xFD,
       .gpio_config = configure_uart_gpios,
};
#endif // CONFIG_PANTECH_BT
#endif


#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)

static struct gpio_led gpio_exp_leds_config[] = {
	{
		.name = "left_led1:green",
		.gpio = GPIO_LEFT_LED_1,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "left_led2:red",
		.gpio = GPIO_LEFT_LED_2,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "left_led3:green",
		.gpio = GPIO_LEFT_LED_3,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "wlan_led:orange",
		.gpio = GPIO_LEFT_LED_WLAN,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "left_led5:green",
		.gpio = GPIO_LEFT_LED_5,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "right_led1:green",
		.gpio = GPIO_RIGHT_LED_1,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "right_led2:red",
		.gpio = GPIO_RIGHT_LED_2,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "right_led3:green",
		.gpio = GPIO_RIGHT_LED_3,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "bt_led:blue",
		.gpio = GPIO_RIGHT_LED_BT,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
	{
		.name = "right_led5:green",
		.gpio = GPIO_RIGHT_LED_5,
		.active_low = 1,
		.retain_state_suspended = 0,
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
};

static struct gpio_led_platform_data gpio_leds_pdata = {
	.num_leds = ARRAY_SIZE(gpio_exp_leds_config),
	.leds = gpio_exp_leds_config,
};

static struct platform_device gpio_leds = {
	.name          = "leds-gpio",
	.id            = -1,
	.dev           = {
		.platform_data = &gpio_leds_pdata,
	},
};

static struct gpio_led fluid_gpio_leds[] = {
	{
		.name			= "dual_led:green",
		.gpio			= GPIO_LED1_GREEN_N,
		.default_state		= LEDS_GPIO_DEFSTATE_OFF,
		.active_low		= 1,
		.retain_state_suspended = 0,
	},
	{
		.name			= "dual_led:red",
		.gpio			= GPIO_LED2_RED_N,
		.default_state		= LEDS_GPIO_DEFSTATE_OFF,
		.active_low		= 1,
		.retain_state_suspended = 0,
	},
};

static struct gpio_led_platform_data gpio_led_pdata = {
	.leds		= fluid_gpio_leds,
	.num_leds	= ARRAY_SIZE(fluid_gpio_leds),
};

static struct platform_device fluid_leds_gpio = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &gpio_led_pdata,
	},
};

#endif

#if defined(CONFIG_MSM_RPM_LOG) || defined(CONFIG_MSM_RPM_LOG_MODULE)

static struct msm_rpm_log_platform_data msm_rpm_log_pdata = {
	.phys_addr_base = 0x00106000,
	.reg_offsets = {
		[MSM_RPM_LOG_PAGE_INDICES] = 0x00000C80,
		[MSM_RPM_LOG_PAGE_BUFFER]  = 0x00000CA0,
	},
	.phys_size = SZ_8K,
	.log_len = 4096,		  /* log's buffer length in bytes */
	.log_len_mask = (4096 >> 2) - 1,  /* length mask in units of u32 */
};

static struct platform_device msm_rpm_log_device = {
	.name	= "msm_rpm_log",
	.id	= -1,
	.dev	= {
		.platform_data = &msm_rpm_log_pdata,
	},
};
#endif

#ifdef CONFIG_BATTERY_MSM8X60
static struct msm_charger_platform_data msm_charger_data = {
	.safety_time = 180,
	.update_time = 1,
	.max_voltage = 4200,
	.min_voltage = 3200,
};

static struct platform_device msm_charger_device = {
	.name = "msm-charger",
	.id = -1,
	.dev = {
		.platform_data = &msm_charger_data,
	}
};
#endif

static struct regulator_consumer_supply rpm_vreg_supply[RPM_VREG_ID_MAX] = {
	[RPM_VREG_ID_PM8058_L0]  = REGULATOR_SUPPLY("8058_l0", NULL),
	[RPM_VREG_ID_PM8058_L1]  = REGULATOR_SUPPLY("8058_l1", NULL),
	[RPM_VREG_ID_PM8058_L2]  = REGULATOR_SUPPLY("8058_l2", NULL),
	[RPM_VREG_ID_PM8058_L3]  = REGULATOR_SUPPLY("8058_l3", NULL),
	[RPM_VREG_ID_PM8058_L4]  = REGULATOR_SUPPLY("8058_l4", NULL),
	[RPM_VREG_ID_PM8058_L5]  = REGULATOR_SUPPLY("8058_l5", NULL),
	[RPM_VREG_ID_PM8058_L6]  = REGULATOR_SUPPLY("8058_l6", NULL),
	[RPM_VREG_ID_PM8058_L7]  = REGULATOR_SUPPLY("8058_l7", NULL),
	[RPM_VREG_ID_PM8058_L8]  = REGULATOR_SUPPLY("8058_l8", NULL),
	[RPM_VREG_ID_PM8058_L9]  = REGULATOR_SUPPLY("8058_l9", NULL),
	[RPM_VREG_ID_PM8058_L10] = REGULATOR_SUPPLY("8058_l10", NULL),
	[RPM_VREG_ID_PM8058_L11] = REGULATOR_SUPPLY("8058_l11", NULL),
	[RPM_VREG_ID_PM8058_L12] = REGULATOR_SUPPLY("8058_l12", NULL),
	[RPM_VREG_ID_PM8058_L13] = REGULATOR_SUPPLY("8058_l13", NULL),
	[RPM_VREG_ID_PM8058_L14] = REGULATOR_SUPPLY("8058_l14", NULL),
	[RPM_VREG_ID_PM8058_L15] = REGULATOR_SUPPLY("8058_l15", NULL),
	[RPM_VREG_ID_PM8058_L16] = REGULATOR_SUPPLY("8058_l16", NULL),
	[RPM_VREG_ID_PM8058_L17] = REGULATOR_SUPPLY("8058_l17", NULL),
	[RPM_VREG_ID_PM8058_L18] = REGULATOR_SUPPLY("8058_l18", NULL),
	[RPM_VREG_ID_PM8058_L19] = REGULATOR_SUPPLY("8058_l19", NULL),
	[RPM_VREG_ID_PM8058_L20] = REGULATOR_SUPPLY("8058_l20", NULL),
	[RPM_VREG_ID_PM8058_L21] = REGULATOR_SUPPLY("8058_l21", NULL),
	[RPM_VREG_ID_PM8058_L22] = REGULATOR_SUPPLY("8058_l22", NULL),
	[RPM_VREG_ID_PM8058_L23] = REGULATOR_SUPPLY("8058_l23", NULL),
	[RPM_VREG_ID_PM8058_L24] = REGULATOR_SUPPLY("8058_l24", NULL),
	[RPM_VREG_ID_PM8058_L25] = REGULATOR_SUPPLY("8058_l25", NULL),

	[RPM_VREG_ID_PM8058_S0] = REGULATOR_SUPPLY("8058_s0", NULL),
	[RPM_VREG_ID_PM8058_S1] = REGULATOR_SUPPLY("8058_s1", NULL),
	[RPM_VREG_ID_PM8058_S2] = REGULATOR_SUPPLY("8058_s2", NULL),
	[RPM_VREG_ID_PM8058_S3] = REGULATOR_SUPPLY("8058_s3", NULL),
	[RPM_VREG_ID_PM8058_S4] = REGULATOR_SUPPLY("8058_s4", NULL),

	[RPM_VREG_ID_PM8058_LVS0] = REGULATOR_SUPPLY("8058_lvs0", NULL),
	[RPM_VREG_ID_PM8058_LVS1] = REGULATOR_SUPPLY("8058_lvs1", NULL),

	[RPM_VREG_ID_PM8058_NCP] = REGULATOR_SUPPLY("8058_ncp", NULL),

	[RPM_VREG_ID_PM8901_L0]  = REGULATOR_SUPPLY("8901_l0",  NULL),
	[RPM_VREG_ID_PM8901_L1]  = REGULATOR_SUPPLY("8901_l1",  NULL),
	[RPM_VREG_ID_PM8901_L2]  = REGULATOR_SUPPLY("8901_l2",  NULL),
	[RPM_VREG_ID_PM8901_L3]  = REGULATOR_SUPPLY("8901_l3",  NULL),
	[RPM_VREG_ID_PM8901_L4]  = REGULATOR_SUPPLY("8901_l4",  NULL),
	[RPM_VREG_ID_PM8901_L5]  = REGULATOR_SUPPLY("8901_l5",  NULL),
	[RPM_VREG_ID_PM8901_L6]  = REGULATOR_SUPPLY("8901_l6",  NULL),

	[RPM_VREG_ID_PM8901_S2] = REGULATOR_SUPPLY("8901_s2", NULL),
	[RPM_VREG_ID_PM8901_S3] = REGULATOR_SUPPLY("8901_s3", NULL),
	[RPM_VREG_ID_PM8901_S4] = REGULATOR_SUPPLY("8901_s4", NULL),

	[RPM_VREG_ID_PM8901_LVS0] = REGULATOR_SUPPLY("8901_lvs0", NULL),
	[RPM_VREG_ID_PM8901_LVS1] = REGULATOR_SUPPLY("8901_lvs1", NULL),
	[RPM_VREG_ID_PM8901_LVS2] = REGULATOR_SUPPLY("8901_lvs2", NULL),
	[RPM_VREG_ID_PM8901_LVS3] = REGULATOR_SUPPLY("8901_lvs3", NULL),
	[RPM_VREG_ID_PM8901_MVS0] = REGULATOR_SUPPLY("8901_mvs0", NULL),
};

#define RPM_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
		      _default_uV, _peak_uA, _avg_uA, _pull_down, _pin_ctrl, \
		      _freq, _pin_fn, _rpm_mode, _state, _sleep_selectable, \
		      _always_on) \
	[RPM_VREG_ID_##_id] = { \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask = _modes, \
				.valid_ops_mask = _ops, \
				.min_uV = _min_uV, \
				.max_uV = _max_uV, \
				.input_uV = _min_uV, \
				.apply_uV = _apply_uV, \
				.always_on = _always_on, \
			}, \
			.num_consumer_supplies = 1, \
			.consumer_supplies = \
				&rpm_vreg_supply[RPM_VREG_ID_##_id], \
		}, \
		.default_uV = _default_uV, \
		.peak_uA = _peak_uA, \
		.avg_uA = _avg_uA, \
		.pull_down_enable = _pull_down, \
		.pin_ctrl = _pin_ctrl, \
		.freq = _freq, \
		.pin_fn = _pin_fn, \
		.mode = _rpm_mode, \
		.state = _state, \
		.sleep_selectable = _sleep_selectable, \
	}

/*
 * The default LPM/HPM state of an RPM controlled regulator can be controlled
 * via the peak_uA value specified in the table below.  If the value is less
 * than the high power min threshold for the regulator, then the regulator will
 * be set to LPM.  Otherwise, it will be set to HPM.
 *
 * This value can be further overridden by specifying an initial mode via
 * .init_data.constraints.initial_mode.
 */

#define RPM_VREG_INIT_LDO(_id, _always_on, _pd, _sleep_selectable, _min_uV, \
			  _max_uV, _init_peak_uA, _pin_ctrl) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, _pin_ctrl, RPM_VREG_FREQ_NONE, \
		      RPM_VREG_PIN_FN_ENABLE, RPM_VREG_MODE_NONE, \
		      RPM_VREG_STATE_OFF, _sleep_selectable, _always_on)

#define RPM_VREG_INIT_LDO_PF(_id, _always_on, _pd, _sleep_selectable, _min_uV, \
			  _max_uV, _init_peak_uA, _pin_ctrl, _pin_fn) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, _pin_ctrl, RPM_VREG_FREQ_NONE, \
		      _pin_fn, RPM_VREG_MODE_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_VREG_INIT_SMPS(_id, _always_on, _pd, _sleep_selectable, _min_uV, \
			   _max_uV, _init_peak_uA, _pin_ctrl, _freq) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, _pin_ctrl, _freq, \
		      RPM_VREG_PIN_FN_ENABLE, RPM_VREG_MODE_NONE, \
		      RPM_VREG_STATE_OFF, _sleep_selectable, _always_on)

#define RPM_VREG_INIT_VS(_id, _always_on, _pd, _sleep_selectable, _pin_ctrl) \
	RPM_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE, \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE, 0, 0, \
		      1000, 1000, _pd, _pin_ctrl, RPM_VREG_FREQ_NONE, \
		      RPM_VREG_PIN_FN_ENABLE, RPM_VREG_MODE_NONE, \
		      RPM_VREG_STATE_OFF, _sleep_selectable, _always_on)

#define RPM_VREG_INIT_NCP(_id, _always_on, _pd, _sleep_selectable, _min_uV, \
			  _max_uV, _pin_ctrl) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_NORMAL, \
		      REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS, 0, \
		      _min_uV, 1000, 1000, _pd, _pin_ctrl, RPM_VREG_FREQ_NONE, \
		      RPM_VREG_PIN_FN_ENABLE, RPM_VREG_MODE_NONE, \
		      RPM_VREG_STATE_OFF, _sleep_selectable, _always_on)

#define LDO50HMIN	RPM_VREG_LDO_50_HPM_MIN_LOAD
#define LDO150HMIN	RPM_VREG_LDO_150_HPM_MIN_LOAD
#define LDO300HMIN	RPM_VREG_LDO_300_HPM_MIN_LOAD
#define SMPS_HMIN	RPM_VREG_SMPS_HPM_MIN_LOAD
#define FTS_HMIN	RPM_VREG_FTSMPS_HPM_MIN_LOAD

static struct rpm_vreg_pdata rpm_vreg_init_pdata[RPM_VREG_ID_MAX] = {
	RPM_VREG_INIT_LDO_PF(PM8058_L0,  0, 1, 0, 1200000, 1200000, LDO150HMIN,
		RPM_VREG_PIN_CTRL_NONE, RPM_VREG_PIN_FN_SLEEP_B),
	RPM_VREG_INIT_LDO(PM8058_L1,  0, 1, 0, 1200000, 1200000, LDO300HMIN, 0),
/*#if defined(CONFIG_TOUCHSCREEN_QT602240) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K)  ||defined(CONFIG_MACH_MSM8X60_EF40S))
	RPM_VREG_INIT_LDO(PM8058_L2,  0, 1, 0, 2800000, 2800000, LDO300HMIN, 0), */	//20110909 ssoh Change_AVdd_3.3V
	
//S 20110908 ssoh Change_AVdd_3.3V
#if defined(CONFIG_TOUCHSCREEN_QT602240)
	RPM_VREG_INIT_LDO(PM8058_L2,  0, 1, 0, 3300000, 3300000, LDO300HMIN, 0),
//E 20110908 ssoh Change_AVdd_3.3V

#elif defined(CONFIG_MACH_MSM8X60_PRESTO) // jmlee presto ws10 qtr i2c power is not QC reference
#if (BOARD_REV > WS10)   // Presto NR CLK_IN TP Test ws20 remove boot err
	//RPM_VREG_INIT_LDO(PM8058_L2,  1, 1, 0, 1800000, 1800000, LDO300HMIN, 0),  // WS20 : XO_OUT_D0 ==> eS310
	RPM_VREG_INIT_LDO(PM8058_L2,  0, 1, 0, 1800000, 1800000, LDO300HMIN, 0),  // WS20 : XO_OUT_D0 ==> eS310
#else
	RPM_VREG_INIT_LDO(PM8058_L2,  0, 1, 0, 1800000, 2600000, LDO300HMIN, 0),
#endif	
#else
	RPM_VREG_INIT_LDO(PM8058_L2,  0, 1, 0, 1800000, 2600000, LDO300HMIN, 0),
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO //kej for PRESTO
	RPM_VREG_INIT_LDO(PM8058_L3,  0, 1, 0, 3000000, 3000000, LDO150HMIN, 0),
#else
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV >= WS10)
	RPM_VREG_INIT_LDO(PM8058_L3,  0, 1, 0, 2800000, 2800000, LDO150HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L3,  0, 1, 0, 1800000, 1800000, LDO150HMIN, 0),
#endif
#else
	RPM_VREG_INIT_LDO(PM8058_L3,  0, 1, 0, 1800000, 1800000, LDO150HMIN, 0),
#endif //Pantech_Camera
#endif
	RPM_VREG_INIT_LDO(PM8058_L4,  0, 1, 0, 2850000, 2850000,  LDO50HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L5,  0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L6,  0, 1, 0, 3000000, 3600000,  LDO50HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L7,  0, 1, 0, 1800000, 1800000,  LDO50HMIN, 0),
#if defined(CONFIG_INPUT_PANTECH_EARJACK) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K) ||defined(CONFIG_MACH_MSM8X60_EF40S)||(defined(CONFIG_MACH_MSM8X60_PRESTO) && (BOARD_REV >= TP10)))
	RPM_VREG_INIT_LDO(PM8058_L8,  0, 1, 0, 2700000, 2700000, LDO300HMIN, 0),
#elif defined(CONFIG_MACH_MSM8X60_PRESTO) && (BOARD_REV <= WS10) // jmlee presto ws10 qtr i2c power is not QC reference

	RPM_VREG_INIT_LDO(PM8058_L8,  1, 1, 0, 1800000, 1800000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO_PF(PM8058_L8,  0, 1, 0, 2900000, 3050000, LDO300HMIN,
		RPM_VREG_PIN_CTRL_NONE, RPM_VREG_PIN_FN_SLEEP_B),
#endif
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV == PT10)
	RPM_VREG_INIT_LDO(PM8058_L9,  0, 1, 0, 2800000, 2800000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L9,  0, 1, 0, 1800000, 1800000, LDO300HMIN, 0),
#endif
#elif defined (CONFIG_MACH_MSM8X60_PRESTO)  // jmlee
	//RPM_VREG_INIT_LDO(PM8058_L9,  0, 1, 0, 2700000, 2700000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L9,  1, 1, 0, 2700000, 2700000, LDO300HMIN, 0),  // jmlee 
#else
	RPM_VREG_INIT_LDO(PM8058_L9,  0, 1, 0, 1800000, 1800000, LDO300HMIN, 0),
#endif

	//---	'11/06/15 edited by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	//RPM_VREG_INIT_LDO(PM8058_L10, 0, 1, 0, 3000000, 3000000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L10, 0, 1, 0, 2600000, 2600000, LDO300HMIN, 0),
#elif defined(CONFIG_PANTECH_PRESTO_SENSORS_YAS530) || defined(CONFIG_PANTECH_PRESTO_SENSORS_BMA250)
	RPM_VREG_INIT_LDO(PM8058_L10, 0, 1, 0, 3000000, 3000000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L10, 0, 1, 0, 2600000, 2600000, LDO300HMIN, 0),
#endif

#ifdef CONFIG_MACH_MSM8X60_PRESTO //kej for PRESTO
 	RPM_VREG_INIT_LDO(PM8058_L11, 0, 1, 0, 1800000, 1800000, LDO150HMIN, 0),
#elif defined(CONFIG_SKY_TDMB) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
	RPM_VREG_INIT_LDO(PM8058_L11, 0, 1, 0, 2600000, 2600000, LDO150HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L11, 0, 1, 0, 1500000, 1500000, LDO150HMIN, 0),
#endif
// 201110422, kkcho,  for MHL-power
#ifdef PANTECH_MHL_SII9244_POWER_CTRL
	RPM_VREG_INIT_LDO(PM8058_L12, 0, 1, 0, 3300000, 3300000, LDO150HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L12, 0, 1, 0, 2900000, 2900000, LDO150HMIN, 0),
#endif
	RPM_VREG_INIT_LDO(PM8058_L13, 0, 1, 0, 2050000, 2050000, LDO300HMIN, 0),
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV >= WS10)
	RPM_VREG_INIT_LDO(PM8058_L14, 0, 1, 0, 2800000, 2800000, LDO300HMIN, 0),
#else // for SD card
	RPM_VREG_INIT_LDO(PM8058_L14, 0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif
#elif defined (CONFIG_MACH_MSM8X60_PRESTO)
#ifdef CONFIG_SKY_MMC /*for PRESTO*/
       RPM_VREG_INIT_LDO(PM8058_L14, 0, 0, 0, 2850000, 3000000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L14, 0, 0, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif
#else
    RPM_VREG_INIT_LDO(PM8058_L14, 0, 0, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif	
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV == PT10)
	RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 1500000, 1500000, LDO300HMIN, 0),
#else
#ifdef CONFIG_SKY_MMC /*for EF39S, EF40S, EF40K*/
    RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 2850000, 3000000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif
#endif
#elif defined(CONFIG_PANTECH_PRESTO_SENSORS_APDS9190)
#if (BOARD_REV >= TP10)
	RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 3000000, 3000000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif
#else
    RPM_VREG_INIT_LDO(PM8058_L15, 0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
#endif
	RPM_VREG_INIT_LDO(PM8058_L16, 1, 1, 1, 1800000, 1800000, LDO300HMIN, 0),
#ifdef CONFIG_MACH_MSM8X60_PRESTO  // kkcho_temp_presto
	RPM_VREG_INIT_LDO(PM8058_L17, 0, 1, 0, 3300000, 3300000, LDO150HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L17, 0, 1, 0, 2600000, 2600000, LDO150HMIN, 0),
#endif
	RPM_VREG_INIT_LDO(PM8058_L18, 0, 1, 1, 2200000, 2200000, LDO150HMIN, 0),
// 201110422, kkcho,  for EV10-board HDMI-power (not used MHL)
#if defined(CONFIG_MACH_MSM8X60_EF39S)  && (BOARD_REV == EV10)
	RPM_VREG_INIT_LDO(PM8058_L19, 0, 1, 0, 1800000, 1800000, LDO150HMIN, 0),
#else
  #if defined(CONFIG_TOUCHSCREEN_MELFAS_TKI)
	RPM_VREG_INIT_LDO(PM8058_L19, 0, 1, 0, 3300000, 3300000, LDO150HMIN, 0),
  #elif defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
    #if (BOARD_REV >= WS10)
	RPM_VREG_INIT_LDO(PM8058_L19, 0, 1, 0, 2800000, 2800000, LDO150HMIN, 0),
    #else
	RPM_VREG_INIT_LDO(PM8058_L19, 0, 1, 0, 2500000, 2500000, LDO150HMIN, 0),
    #endif
#else
	RPM_VREG_INIT_LDO(PM8058_L19, 0, 1, 0, 2500000, 2500000, LDO150HMIN, 0),
#endif
#endif
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
	RPM_VREG_INIT_LDO_PF(PM8058_L20, 0, 1, 0, 2800000, 2800000, LDO150HMIN,
		RPM_VREG_PIN_CTRL_NONE, RPM_VREG_PIN_FN_SLEEP_B),
#else	// Use Presto touch..
	RPM_VREG_INIT_LDO_PF(PM8058_L20, 0, 1, 0, 1800000, 1800000, LDO150HMIN,
		RPM_VREG_PIN_CTRL_NONE, RPM_VREG_PIN_FN_SLEEP_B),
#endif
	RPM_VREG_INIT_LDO_PF(PM8058_L21, 1, 1, 0, 1200000, 1200000, LDO150HMIN,
		RPM_VREG_PIN_CTRL_NONE, RPM_VREG_PIN_FN_SLEEP_B),
	RPM_VREG_INIT_LDO(PM8058_L22, 0, 1, 0, 1200000, 1200000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8058_L23, 0, 1, 0, 1200000, 1200000, LDO300HMIN, 0),
#if 0 //defined(CONFIG_SKY_TDMB) && (defined(CONFIG_MACH_MSM8X60_EF39S)  && (BOARD_REV > TP10))
	RPM_VREG_INIT_LDO(PM8058_L24, 0, 1, 0, 1800000, 1800000, LDO150HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8058_L24, 0, 1, 0, 1200000, 1200000, LDO150HMIN, 0),
#endif
	RPM_VREG_INIT_LDO(PM8058_L25, 0, 1, 0, 1200000, 1200000, LDO150HMIN, 0),

	RPM_VREG_INIT_SMPS(PM8058_S0, 0, 1, 1,  500000, 1250000,  SMPS_HMIN, 0,
		RPM_VREG_FREQ_1p60),
	RPM_VREG_INIT_SMPS(PM8058_S1, 0, 1, 1,  500000, 1250000,  SMPS_HMIN, 0,
		RPM_VREG_FREQ_1p60),
	RPM_VREG_INIT_SMPS(PM8058_S2, 0, 1, 1, 1200000, 1400000,  SMPS_HMIN,
		RPM_VREG_PIN_CTRL_A0, RPM_VREG_FREQ_1p60),
#if defined(CONFIG_TOUCHSCREEN_QT602240) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K) ||defined(CONFIG_MACH_MSM8X60_EF40S))
	RPM_VREG_INIT_SMPS(PM8058_S3, 1, 1, 0, 1800000, 1800000,  SMPS_HMIN, 0,
		RPM_VREG_FREQ_1p60),
#else
	RPM_VREG_INIT_SMPS(PM8058_S3, 1, 1, 0, 1800000, 1800000,  SMPS_HMIN, 0,
		RPM_VREG_FREQ_1p60),
#endif
	RPM_VREG_INIT_SMPS(PM8058_S4, 1, 1, 0, 2200000, 2200000,  SMPS_HMIN, 0,
		RPM_VREG_FREQ_1p60),

	RPM_VREG_INIT_VS(PM8058_LVS0, 0, 1, 0,				 0),
	//---	edited by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050) \
     || defined(CONFIG_PANTECH_PRESTO_SENSORS_YAS530) || defined(CONFIG_PANTECH_PRESTO_SENSORS_BMA250)
	RPM_VREG_INIT_VS(PM8058_LVS1, 0, 1, 1800000, 1800000),
#else
	RPM_VREG_INIT_VS(PM8058_LVS1, 0, 1, 0,				 0),
#endif

	RPM_VREG_INIT_NCP(PM8058_NCP, 0, 1, 0, 1800000, 1800000,	 0),

	RPM_VREG_INIT_LDO(PM8901_L0,  0, 1, 0, 1200000, 1200000, LDO300HMIN,
		RPM_VREG_PIN_CTRL_A0),
#if defined(CONFIG_PANTECH_CAMERA) && defined(CONFIG_MACH_MSM8X60_PRESTO)
	RPM_VREG_INIT_LDO(PM8901_L1,  0, 1, 0, 2800000, 2800000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L2,  0, 1, 0, 2800000, 2800000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L3,  0, 1, 0, 2800000, 2800000, LDO300HMIN, 0),
#else
	RPM_VREG_INIT_LDO(PM8901_L1,  0, 1, 0, 3300000, 3300000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L2,  0, 1, 0, 2850000, 3300000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L3,  0, 1, 0, 3300000, 3300000, LDO300HMIN, 0),
#endif
	RPM_VREG_INIT_LDO(PM8901_L4,  0, 1, 0, 2600000, 2600000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L5,  0, 1, 0, 2850000, 2850000, LDO300HMIN, 0),
	RPM_VREG_INIT_LDO(PM8901_L6,  0, 1, 0, 2200000, 2200000, LDO300HMIN, 0),
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV >= WS10)
	RPM_VREG_INIT_SMPS(PM8901_S2, 0, 1, 0, 1500000, 1500000,   FTS_HMIN, 0,
#else
	RPM_VREG_INIT_SMPS(PM8901_S2, 0, 1, 0, 1300000, 1300000,   FTS_HMIN, 0,
#endif
#else
	RPM_VREG_INIT_SMPS(PM8901_S2, 0, 1, 0, 1300000, 1300000,   FTS_HMIN, 0,
#endif
		RPM_VREG_FREQ_1p60),
	RPM_VREG_INIT_SMPS(PM8901_S3, 0, 1, 0, 1100000, 1100000,   FTS_HMIN, 0,
		RPM_VREG_FREQ_1p60),
	RPM_VREG_INIT_SMPS(PM8901_S4, 0, 1, 0, 1225000, 1225000,   FTS_HMIN,
		RPM_VREG_PIN_CTRL_A0, RPM_VREG_FREQ_1p60),

	RPM_VREG_INIT_VS(PM8901_LVS0, 1, 1, 0,				 0),
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K)) || defined(CONFIG_MACH_MSM8X60_PRESTO)
	RPM_VREG_INIT_VS(PM8901_LVS1, 0, 1, 1800000,	 1800000),
#else
	RPM_VREG_INIT_VS(PM8901_LVS1, 0, 1, 0,				 0),
#endif
#if (defined(CONFIG_PANTECH_CAMERA) && defined(CONFIG_MACH_MSM8X60_PRESTO)) || (defined(CONFIG_TOUCHSCREEN_QT602240) && ((defined(CONFIG_MACH_MSM8X60_EF40K)||defined(CONFIG_MACH_MSM8X60_EF40S)) && (BOARD_REV >= WS20)))
	RPM_VREG_INIT_VS(PM8901_LVS2, 0, 1, 1800000, 1800000),
#else
	RPM_VREG_INIT_VS(PM8901_LVS2, 0, 1, 0,				 0),
#endif
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K))
#if (BOARD_REV == PT10)
			RPM_VREG_INIT_VS(PM8901_LVS3, 0, 1, 1800000,	 1800000),
#else
			RPM_VREG_INIT_VS(PM8901_LVS3, 0, 1, 0,				 0),
#endif
#elif defined(CONFIG_MACH_MSM8X60_PRESTO) 
#if defined(CONFIG_PANTECH_CAMERA_MT9P111)
			RPM_VREG_INIT_VS(PM8901_LVS3, 0, 1, 1800000,	 1800000),
#else
			RPM_VREG_INIT_VS(PM8901_LVS3, 0, 1, 0,				 0),
#endif
#else
	RPM_VREG_INIT_VS(PM8901_LVS3, 0, 1, 0,				 0),
#endif
#if defined(CONFIG_PANTECH_CAMERA) && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K)) || defined(CONFIG_MACH_MSM8X60_PRESTO)
			RPM_VREG_INIT_VS(PM8901_MVS0, 0, 1, 1800000,	 1800000),
#else
	RPM_VREG_INIT_VS(PM8901_MVS0, 0, 1, 0,				 0),
#endif
};

#define RPM_VREG(_id) \
	[_id] = { \
		.name = "rpm-regulator", \
		.id = _id, \
		.dev = { \
			.platform_data = &rpm_vreg_init_pdata[_id], \
		}, \
	}

static struct platform_device rpm_vreg_device[RPM_VREG_ID_MAX] = {
	RPM_VREG(RPM_VREG_ID_PM8058_L0),
	RPM_VREG(RPM_VREG_ID_PM8058_L1),
	RPM_VREG(RPM_VREG_ID_PM8058_L2),
	RPM_VREG(RPM_VREG_ID_PM8058_L3),
	RPM_VREG(RPM_VREG_ID_PM8058_L4),
	RPM_VREG(RPM_VREG_ID_PM8058_L5),
	RPM_VREG(RPM_VREG_ID_PM8058_L6),
	RPM_VREG(RPM_VREG_ID_PM8058_L7),
	RPM_VREG(RPM_VREG_ID_PM8058_L8),
	RPM_VREG(RPM_VREG_ID_PM8058_L9),
	RPM_VREG(RPM_VREG_ID_PM8058_L10),
	RPM_VREG(RPM_VREG_ID_PM8058_L11),
	RPM_VREG(RPM_VREG_ID_PM8058_L12),
	RPM_VREG(RPM_VREG_ID_PM8058_L13),
	RPM_VREG(RPM_VREG_ID_PM8058_L14),
	RPM_VREG(RPM_VREG_ID_PM8058_L15),
	RPM_VREG(RPM_VREG_ID_PM8058_L16),
	RPM_VREG(RPM_VREG_ID_PM8058_L17),
	RPM_VREG(RPM_VREG_ID_PM8058_L18),
	RPM_VREG(RPM_VREG_ID_PM8058_L19),
	RPM_VREG(RPM_VREG_ID_PM8058_L20),
	RPM_VREG(RPM_VREG_ID_PM8058_L21),
	RPM_VREG(RPM_VREG_ID_PM8058_L22),
	RPM_VREG(RPM_VREG_ID_PM8058_L23),
	RPM_VREG(RPM_VREG_ID_PM8058_L24),
	RPM_VREG(RPM_VREG_ID_PM8058_L25),
	RPM_VREG(RPM_VREG_ID_PM8058_S0),
	RPM_VREG(RPM_VREG_ID_PM8058_S1),
	RPM_VREG(RPM_VREG_ID_PM8058_S2),
	RPM_VREG(RPM_VREG_ID_PM8058_S3),
	RPM_VREG(RPM_VREG_ID_PM8058_S4),
	RPM_VREG(RPM_VREG_ID_PM8058_LVS0),
	RPM_VREG(RPM_VREG_ID_PM8058_LVS1),
	RPM_VREG(RPM_VREG_ID_PM8058_NCP),
	RPM_VREG(RPM_VREG_ID_PM8901_L0),
	RPM_VREG(RPM_VREG_ID_PM8901_L1),
	RPM_VREG(RPM_VREG_ID_PM8901_L2),
	RPM_VREG(RPM_VREG_ID_PM8901_L3),
	RPM_VREG(RPM_VREG_ID_PM8901_L4),
	RPM_VREG(RPM_VREG_ID_PM8901_L5),
	RPM_VREG(RPM_VREG_ID_PM8901_L6),
	RPM_VREG(RPM_VREG_ID_PM8901_S2),
	RPM_VREG(RPM_VREG_ID_PM8901_S3),
	RPM_VREG(RPM_VREG_ID_PM8901_S4),
	RPM_VREG(RPM_VREG_ID_PM8901_LVS0),
	RPM_VREG(RPM_VREG_ID_PM8901_LVS1),
	RPM_VREG(RPM_VREG_ID_PM8901_LVS2),
	RPM_VREG(RPM_VREG_ID_PM8901_LVS3),
	RPM_VREG(RPM_VREG_ID_PM8901_MVS0),
};

static struct platform_device *early_regulators[] __initdata = {
	&msm_device_saw_s0,
	&msm_device_saw_s1,
#ifdef CONFIG_PMIC8058
	&rpm_vreg_device[RPM_VREG_ID_PM8058_S0],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_S1],
#endif
};

static struct platform_device *early_devices[] __initdata = {
#ifdef CONFIG_MSM_BUS_SCALING
	&msm_bus_apps_fabric,
	&msm_bus_sys_fabric,
	&msm_bus_mm_fabric,
	&msm_bus_sys_fpb,
	&msm_bus_cpss_fpb,
#endif
	&msm_device_dmov_adm0,
	&msm_device_dmov_adm1,
};

#ifdef CONFIG_PANTECH_BT //lsi@ps2.20110408 bluez by KSJ_device 2011_05_12
static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= 128,
		.end	= 128,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= 138,
		.end	= 138,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= MSM_GPIO_TO_INT(128),
		.end	= MSM_GPIO_TO_INT(128),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_bluesleep_device = {
	.name = "bluesleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};

static struct platform_device msm_bt_power_device = {
	.name = "bt_power",
	.id	 = -1,
};

enum {
	BT_WAKE,
	BT_RFR,
	BT_CTS,
	BT_RX,
	BT_TX,
	BT_PCM_DOUT,
	BT_PCM_DIN,
	BT_PCM_SYNC,
	BT_PCM_CLK,
	BT_HOST_WAKE,
	BT_SYSRST,
};
#else //CONFIG_PANTECH_BT
#if (defined(CONFIG_MARIMBA_CORE)) && \
	(defined(CONFIG_MSM_BT_POWER) || defined(CONFIG_MSM_BT_POWER_MODULE))

static int bluetooth_power(int);
static struct platform_device msm_bt_power_device = {
	.name	 = "bt_power",
	.id	 = -1,
	.dev	 = {
		.platform_data = &bluetooth_power,
	},
};
#endif
#endif //CONFIG_PANTECH_BT

static struct platform_device msm_tsens_device = {
	.name   = "tsens-tm",
	.id = -1,
};

#ifdef CONFIG_SKY_WLAN
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD)
static uint32_t wlan_on_gpio_cfgs[] = {
	GPIO_CFG(107, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	/* WLAN_SHDN */
//	GPIO_CFG(126, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* WLAN_HOST_WAKE */
	GPIO_CFG(126, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* WLAN_HOST_WAKE */

};
static uint32_t wlan_off_gpio_cfgs[] = {
	GPIO_CFG(107, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* WLAN_SHDN */
	GPIO_CFG(126, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	  /* WLAN_HOST_WAKE */
};
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
static uint32_t wlan_on_gpio_cfgs[] = {
	GPIO_CFG(107, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	/* WLAN_SHDN */
	GPIO_CFG(126, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* WLAN_HOST_WAKE */
};
static uint32_t wlan_off_gpio_cfgs[] = {
	GPIO_CFG(107, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* WLAN_SHDN */
	GPIO_CFG(126, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	  /* WLAN_HOST_WAKE */
};
#endif //CONFIG_SKY_EF39S_BOARD

static int wlan_power_state;
int wlan_power(int on)
{
	int rc = 0;
	int pin = 0;

	printk(KERN_ERR "%s: %d\n",__func__, on);

#ifdef CONFIG_WIFI_CONTROL_FUNC
	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(wlan_on_gpio_cfgs); pin++) {
			rc = gpio_tlmm_config(wlan_on_gpio_cfgs[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, wlan_on_gpio_cfgs[pin], rc);
			}
		}
		mdelay(30);
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		gpio_set_value(107, on);
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		gpio_set_value(107, on);
#endif //CONFIG_SKY_EF39S_BOARD
		mdelay(40);

		wlan_power_state = on;
	} else {
		mdelay(30);
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		gpio_set_value(107, on);
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		gpio_set_value(107, on);
#endif //CONFIG_SKY_EF39S_BOARD
		mdelay(40);

		wlan_power_state = on;

		for (pin = 0; pin < ARRAY_SIZE(wlan_off_gpio_cfgs); pin++) {
			rc = gpio_tlmm_config(wlan_off_gpio_cfgs[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, wlan_off_gpio_cfgs[pin], rc);
			}
		}
	}

#else //CONFIG_WIFI_CONTROL_FUNC

	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(wlan_on_gpio_cfgs); pin++) {
			rc = gpio_tlmm_config(wlan_on_gpio_cfgs[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, wlan_on_gpio_cfgs[pin], rc);
			}
		}
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		gpio_set_value(107, on);
		mdelay(10);
		gpio_set_value(107, 0);
		mdelay(10);
		gpio_set_value(107, on);
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		gpio_set_value(107, on);
		mdelay(10);
		gpio_set_value(107, 0);
		mdelay(10);
		gpio_set_value(107, on);
#endif //CONFIG_SKY_EF39S_BOARD
	} else {
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		gpio_set_value(107, 0);
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		gpio_set_value(107, on);
#endif //CONFIG_SKY_EF39S_BOARD

		for (pin = 0; pin < ARRAY_SIZE(wlan_off_gpio_cfgs); pin++) {
			rc = gpio_tlmm_config(wlan_off_gpio_cfgs[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, wlan_off_gpio_cfgs[pin], rc);
			}
		}
	}

#endif  //CONFIG_WIFI_CONTROL_FUNC
	return rc;
}

#ifndef CONFIG_WIFI_CONTROL_FUNC
static void wlan_init(void)
{
	int rc = 0;
	int pin = 0;

	for (pin = 0; pin < ARRAY_SIZE(wlan_off_gpio_cfgs); pin++) {
		rc = gpio_tlmm_config(wlan_off_gpio_cfgs[pin], GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, wlan_off_gpio_cfgs[pin], rc);
		}
	}
	wlan_power(1);

}
#endif  //CONFIG_WIFI_CONTROL_FUNC

#ifdef CONFIG_WIFI_MEM_PREALLOC
#define PREALLOC_WLAN_NUMBER_OF_SECTIONS	4
#define PREALLOC_WLAN_NUMBER_OF_BUFFERS		160
#define PREALLOC_WLAN_SECTION_HEADER		24

#define WLAN_SECTION_SIZE_0	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_1	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_2	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 512)
#define WLAN_SECTION_SIZE_3	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 1024)

#define WLAN_SKB_BUF_NUM	16

static struct sk_buff *wlan_static_skb[WLAN_SKB_BUF_NUM];

typedef struct wifi_mem_prealloc_struct {
	void *mem_ptr;
	unsigned long size;
} wifi_mem_prealloc_t;

static wifi_mem_prealloc_t wifi_mem_array[PREALLOC_WLAN_NUMBER_OF_SECTIONS] = {
	{ NULL, (WLAN_SECTION_SIZE_0 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_1 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_2 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_3 + PREALLOC_WLAN_SECTION_HEADER) }
};

void *wifi_mem_prealloc(int section, unsigned long size)
{
	if (section == PREALLOC_WLAN_NUMBER_OF_SECTIONS)
		return wlan_static_skb;
	if ((section < 0) || (section > PREALLOC_WLAN_NUMBER_OF_SECTIONS))
		return NULL;
	if (wifi_mem_array[section].size < size)
		return NULL;
	return wifi_mem_array[section].mem_ptr;
}
EXPORT_SYMBOL(wifi_mem_prealloc);

static int init_wifi_mem(void)
{
	int i;

	for(i=0;( i < WLAN_SKB_BUF_NUM );i++) {
		if (i < (WLAN_SKB_BUF_NUM/2))
			wlan_static_skb[i] = dev_alloc_skb(4096);
		else
			wlan_static_skb[i] = dev_alloc_skb(8192);
	}
	for(i=0;( i < PREALLOC_WLAN_NUMBER_OF_SECTIONS );i++) {
		wifi_mem_array[i].mem_ptr = kmalloc(wifi_mem_array[i].size,
				GFP_KERNEL);
		if (wifi_mem_array[i].mem_ptr == NULL)
			return -ENOMEM;
	}
	return 0;
}
#endif //CONFIG_WIFI_MEM_PREALLOC

#ifdef CONFIG_WIFI_CONTROL_FUNC
static int wlan_reset_state;
static int wlan_cd = 0; /* WIFI virtual 'card detect' status */
static void (*wlan_status_cb)(int card_present, void *dev_id);
static void *wlan_status_cb_devid;

/* BCM4329 returns wrong sdio_vsn(1) when we read cccr,
 * we use predefined value (sdio_vsn=2) here to initial sdio driver well
 */
static struct embedded_sdio_data wlan_emb_data = {
	.cccr	= {
		.sdio_vsn	= 2,
		.multi_block	= 1,
		.low_speed	= 0,
		.wide_bus	= 0,
		.high_power	= 1,
		.high_speed	= 1,
	},
};

int wlan_reset(int on)
{
	printk("%s: do nothing\n", __func__);
	wlan_reset_state = on;
	return 0;
}

static int wlan_status_register(
			void (*callback)(int card_present, void *dev_id),
			void *dev_id)
{
	if (wlan_status_cb)
		return -EINVAL;
	wlan_status_cb = callback;
	wlan_status_cb_devid = dev_id;
	return 0;
}

static unsigned int wlan_status(struct device *dev)
{
	return wlan_cd;
}

int wlan_set_carddetect(int val)
{
	printk(KERN_ERR"%s: %d\n", __func__, val);
	wlan_cd = val;
	if (wlan_status_cb) {
		wlan_status_cb(val, wlan_status_cb_devid);
	} else
		printk(KERN_ERR"%s: Nobody to notify\n", __func__);
	return 0;
}

static struct resource wlan_resources[] = {
	[0] = {
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		.name	= "bcm4330_wlan_irq",
		.start	= MSM_GPIO_TO_INT(126),
		.end		= MSM_GPIO_TO_INT(126),
		.flags      = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,//IORESOURCE_IRQ_LOWLEVEL, //IORESOURCE_IRQ_LOWEDGE,
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		.name	= "bcm4329_wlan_irq",
		.start	= MSM_GPIO_TO_INT(126),
		.end		= MSM_GPIO_TO_INT(126),
		.flags      = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWLEVEL, //IORESOURCE_IRQ_LOWEDGE,
#endif //CONFIG_SKY_EF39S_BOARD
	},
};

static struct wifi_platform_data  wlan_control = {
	.set_power	= wlan_power,
	.set_reset	= wlan_reset,
	.set_carddetect = wlan_set_carddetect,
	.mem_prealloc = wifi_mem_prealloc,
};

static struct platform_device wlan_device = {
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD)
		.name		= "bcm4330_wlan",
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
		.name		= "bcm4329_wlan",
#endif //CONFIG_SKY_EF39S_BOARD
		.id             	= 1,
		.num_resources = ARRAY_SIZE(wlan_resources),
		.resource		= wlan_resources,
		.dev			= {
			.platform_data = &wlan_control,
		},
	};
#endif //CONFIG_WIFI_CONTROL_FUNC

static void msm8x60_wlan_init(void)
{
	int rc = 0;
	wlan_power(0);

#ifdef CONFIG_WIFI_MEM_PREALLOC
	rc = init_wifi_mem();
	if (rc != 0){
		printk(KERN_ERR "%s: init_wifi_mem_return val: %d \n", __func__, rc);
	}
#endif //CONFIG_WIFI_MEM_PREALLOC
}
#endif  //CONFIG_SKY_WLAN

static struct platform_device *rumi_sim_devices[] __initdata = {
	&smc91x_device,
	&msm_device_uart_dm12,
#ifdef CONFIG_I2C_QUP
	&msm_gsbi3_qup_i2c_device,
	&msm_gsbi4_qup_i2c_device,
	&msm_gsbi7_qup_i2c_device,
	&msm_gsbi8_qup_i2c_device,
	&msm_gsbi9_qup_i2c_device,
#if defined(CONFIG_PN544)
	&msm_gsbi10_qup_i2c_device,
#endif
	&msm_gsbi12_qup_i2c_device,
#endif
#if defined(CONFIG_SKY_TDMB_SPI_QUP)
	&msm_gsbi2_qup_spi_device,
#endif
#ifdef CONFIG_SKY_TDMB_SPI_GPIO
	&gpio_spi_tdmb_device,
#endif
#ifdef CONFIG_I2C_SSBI
	&msm_device_ssbi1,
	&msm_device_ssbi2,
	&msm_device_ssbi3,
#endif
#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
	&android_pmem_kernel_ebi1_device,
#endif
#ifdef CONFIG_KERNEL_PMEM_SMI_REGION
	&android_pmem_kernel_smi_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device,
	&android_pmem_smipool_device,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
	&msm_fb_device,
	&msm_kgsl_3d0,
#ifdef CONFIG_MSM_KGSL_2D
	&msm_kgsl_2d0,
	&msm_kgsl_2d1,
#endif
#ifdef CONFIG_INPUT_PANTECH_EARJACK
	&pantech_earjack_device,
#endif
	&lcdc_samsung_panel_device,
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	&hdmi_msm_device,
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */
#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_MT9E013
	&msm_camera_sensor_mt9e013,
#endif
#ifdef CONFIG_IMX074
	&msm_camera_sensor_imx074,
#endif
#ifdef CONFIG_WEBCAM_OV7692
	&msm_camera_sensor_webcam_ov7692,
#endif
#ifdef CONFIG_WEBCAM_OV9726
	&msm_camera_sensor_webcam_ov9726,
#endif
#ifdef CONFIG_QS_S5K4E1
	&msm_camera_sensor_qs_s5k4e1,
#endif
#endif
#ifdef CONFIG_MSM_GEMINI
	&msm_gemini_device,
#endif
#ifdef CONFIG_MSM_VPE
	&msm_vpe_device,
#endif
	&msm_device_vidc,
};

#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)
enum {
	SX150X_CORE,
	SX150X_DOCKING,
	SX150X_SURF,
	SX150X_LEFT_FHA,
	SX150X_RIGHT_FHA,
	SX150X_SOUTH,
	SX150X_NORTH,
	SX150X_CORE_FLUID,
};

static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CORE] = {
		.gpio_base         = GPIO_CORE_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x0c08,
		.io_pulldn_ena     = 0x4060,
		.io_open_drain_ena = 0x000c,
		.io_polarity       = 0,
		.irq_summary       = -1, /* see fixup_i2c_configs() */
		.irq_base          = GPIO_EXPANDER_IRQ_BASE,
	},
	[SX150X_DOCKING] = {
		.gpio_base         = GPIO_DOCKING_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x5e06,
		.io_pulldn_ena     = 0x81b8,
		.io_open_drain_ena = 0,
		.io_polarity       = 0,
		.irq_summary       = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
						     UI_INT2_N),
		.irq_base          = GPIO_EXPANDER_IRQ_BASE +
				     GPIO_DOCKING_EXPANDER_BASE -
				     GPIO_EXPANDER_GPIO_BASE,
	},
	[SX150X_SURF] = {
		.gpio_base         = GPIO_SURF_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0,
		.io_pulldn_ena     = 0,
		.io_open_drain_ena = 0,
		.io_polarity       = 0,
		.irq_summary       = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
						     UI_INT1_N),
		.irq_base          = GPIO_EXPANDER_IRQ_BASE +
				     GPIO_SURF_EXPANDER_BASE -
				     GPIO_EXPANDER_GPIO_BASE,
	},
	[SX150X_LEFT_FHA] = {
		.gpio_base         = GPIO_LEFT_KB_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0,
		.io_pulldn_ena     = 0x40,
		.io_open_drain_ena = 0,
		.io_polarity       = 0,
		.irq_summary       = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
						     UI_INT3_N),
		.irq_base          = GPIO_EXPANDER_IRQ_BASE +
				     GPIO_LEFT_KB_EXPANDER_BASE -
				     GPIO_EXPANDER_GPIO_BASE,
	},
	[SX150X_RIGHT_FHA] = {
		.gpio_base         = GPIO_RIGHT_KB_EXPANDER_BASE,
		.oscio_is_gpo      = true,
		.io_pullup_ena     = 0,
		.io_pulldn_ena     = 0,
		.io_open_drain_ena = 0,
		.io_polarity       = 0,
		.irq_summary       = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
						     UI_INT3_N),
		.irq_base          = GPIO_EXPANDER_IRQ_BASE +
				     GPIO_RIGHT_KB_EXPANDER_BASE -
				     GPIO_EXPANDER_GPIO_BASE,
	},
	[SX150X_SOUTH] = {
		.gpio_base    = GPIO_SOUTH_EXPANDER_BASE,
		.irq_base     = GPIO_EXPANDER_IRQ_BASE +
				GPIO_SOUTH_EXPANDER_BASE -
				GPIO_EXPANDER_GPIO_BASE,
		.irq_summary  = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, UI_INT3_N),
	},
	[SX150X_NORTH] = {
		.gpio_base    = GPIO_NORTH_EXPANDER_BASE,
		.irq_base     = GPIO_EXPANDER_IRQ_BASE +
				GPIO_NORTH_EXPANDER_BASE -
				GPIO_EXPANDER_GPIO_BASE,
		.irq_summary  = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, UI_INT3_N),
		.oscio_is_gpo = true,
		.io_open_drain_ena = 0x30,
	},
	[SX150X_CORE_FLUID] = {
		.gpio_base         = GPIO_CORE_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x0408,
		.io_pulldn_ena     = 0x4060,
		.io_open_drain_ena = 0x0008,
		.io_polarity       = 0,
		.irq_summary       = -1, /* see fixup_i2c_configs() */
		.irq_base          = GPIO_EXPANDER_IRQ_BASE,
	},
};

#ifdef CONFIG_SENSORS_MSM_ADC
/* Configuration of EPM expander is done when client
 * request an adc read
 */
static struct sx150x_platform_data sx150x_epmdata = {
	.gpio_base         = GPIO_EPM_EXPANDER_BASE,
	.irq_base	   = GPIO_EXPANDER_IRQ_BASE +
				GPIO_EPM_EXPANDER_BASE -
				GPIO_EXPANDER_GPIO_BASE,
	.irq_summary       = -1,
};
#endif

/* sx150x_low_power_cfg
 *
 * This data and init function are used to put unused gpio-expander output
 * lines into their low-power states at boot. The init
 * function must be deferred until a later init stage because the i2c
 * gpio expander drivers do not probe until after they are registered
 * (see register_i2c_devices) and the work-queues for those registrations
 * are processed.  Because these lines are unused, there is no risk of
 * competing with a device driver for the gpio.
 *
 * gpio lines whose low-power states are input are naturally in their low-
 * power configurations once probed, see the platform data structures above.
 */
struct sx150x_low_power_cfg {
	unsigned gpio;
	unsigned val;
};

static struct sx150x_low_power_cfg
common_sx150x_lp_cfgs[] __initdata = {
	{GPIO_WLAN_DEEP_SLEEP_N, 0},
	{GPIO_EXT_GPS_LNA_EN,    0},
	{GPIO_MSM_WAKES_BT,      0},
	{GPIO_USB_UICC_EN,       0},
	{GPIO_BATT_GAUGE_EN,     0},
};

static struct sx150x_low_power_cfg
surf_ffa_sx150x_lp_cfgs[] __initdata = {
	{GPIO_MIPI_DSI_RST_N,      0},
	{GPIO_DONGLE_PWR_EN,       0},
	{GPIO_CAP_TS_SLEEP,        1},
	{GPIO_WEB_CAMIF_RESET_N,   0},
};

static void __init
cfg_gpio_low_power(struct sx150x_low_power_cfg *cfgs, unsigned nelems)
{
	unsigned n;
	int rc;

	for (n = 0; n < nelems; ++n) {
		rc = gpio_request(cfgs[n].gpio, NULL);
		if (!rc) {
			rc = gpio_direction_output(cfgs[n].gpio, cfgs[n].val);
			gpio_free(cfgs[n].gpio);
		}

		if (rc) {
			printk(KERN_NOTICE "%s: failed to sleep gpio %d: %d\n",
			       __func__, cfgs[n].gpio, rc);
		}
	}
}

static int __init cfg_sx150xs_low_power(void)
{
	cfg_gpio_low_power(common_sx150x_lp_cfgs,
		ARRAY_SIZE(common_sx150x_lp_cfgs));
	if (!machine_is_msm8x60_fluid())
		cfg_gpio_low_power(surf_ffa_sx150x_lp_cfgs,
			ARRAY_SIZE(surf_ffa_sx150x_lp_cfgs));
	return 0;
}
module_init(cfg_sx150xs_low_power);

#ifdef CONFIG_I2C
static struct i2c_board_info core_expander_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
		.platform_data = &sx150x_data[SX150X_CORE]
	},
};

static struct i2c_board_info docking_expander_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3f),
		.platform_data = &sx150x_data[SX150X_DOCKING]
	},
};

static struct i2c_board_info surf_expanders_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x70),
		.platform_data = &sx150x_data[SX150X_SURF]
	}
};

static struct i2c_board_info fha_expanders_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x21),
		.platform_data = &sx150x_data[SX150X_LEFT_FHA]
	},
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data = &sx150x_data[SX150X_RIGHT_FHA]
	}
};

static struct i2c_board_info fluid_expanders_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x23),
		.platform_data = &sx150x_data[SX150X_SOUTH]
	},
	{
		I2C_BOARD_INFO("sx1508q", 0x20),
		.platform_data = &sx150x_data[SX150X_NORTH]
	}
};

static struct i2c_board_info fluid_core_expander_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
		.platform_data = &sx150x_data[SX150X_CORE_FLUID]
	},
};

#ifdef CONFIG_SENSORS_MSM_ADC
static struct i2c_board_info fluid_expanders_i2c_epm_info[] = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
		.platform_data = &sx150x_epmdata
	},
};
#endif
#endif
#endif

#ifdef CONFIG_SENSORS_MSM_ADC
static struct resource resources_adc[] = {
	{
		.start = PM8058_ADC_IRQ(PM8058_IRQ_BASE),
		.end   = PM8058_ADC_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
};

static struct adc_access_fn xoadc_fn = {
	pm8058_xoadc_select_chan_and_start_conv,
	pm8058_xoadc_read_adc_code,
	pm8058_xoadc_get_properties,
	pm8058_xoadc_slot_request,
	pm8058_xoadc_restore_slot,
	pm8058_xoadc_calibrate,
};

#if defined(CONFIG_I2C) && \
	(defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE))
static struct regulator *vreg_adc_epm1;

static struct i2c_client *epm_expander_i2c_register_board(void)

{
	struct i2c_adapter *i2c_adap;
	struct i2c_client *client = NULL;
	i2c_adap = i2c_get_adapter(0x0);

	if (i2c_adap == NULL)
		printk(KERN_ERR "\nepm_expander_i2c_adapter is NULL\n");

	if (i2c_adap != NULL)
		client = i2c_new_device(i2c_adap,
			&fluid_expanders_i2c_epm_info[0]);
	return client;

}

static unsigned int msm_adc_gpio_configure_expander_enable(void)
{
	int rc = 0;
	static struct i2c_client *epm_i2c_client;

	printk(KERN_DEBUG "Enter msm_adc_gpio_configure_expander_enable\n");

	vreg_adc_epm1 = regulator_get(NULL, "8058_s3");

	if (IS_ERR(vreg_adc_epm1)) {
		printk(KERN_ERR "%s: Unable to get 8058_s3\n", __func__);
		return 0;
	}

	rc = regulator_set_voltage(vreg_adc_epm1, 1800000, 1800000);
	if (rc)
		printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: "
				"regulator set voltage failed\n");

	rc = regulator_enable(vreg_adc_epm1);
	if (rc) {
		printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: "
			"Error while enabling regulator for epm s3 %d\n", rc);
		return rc;
	}

	printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: Start"
			" setting the value of the EPM 3.3, 5v and lvlsft\n");

	msleep(1000);

	rc = gpio_request(GPIO_EPM_5V_BOOST_EN, "boost_epm_5v");
	if (!rc) {
		printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: "
				"Configure 5v boost\n");
		gpio_direction_output(GPIO_EPM_5V_BOOST_EN, 1);
	} else {
		printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: "
				"Error for epm 5v boost en\n");
		goto exit_vreg_epm;
	}

	msleep(500);

	rc = gpio_request(GPIO_EPM_3_3V_EN, "epm_3_3v");
	if (!rc) {
		gpio_direction_output(GPIO_EPM_3_3V_EN, 1);
		printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: "
				"Configure epm 3.3v\n");
	} else {
		printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: "
				"Error for gpio 3.3ven\n");
		goto exit_vreg_epm;
	}
	msleep(500);

	printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: "
			"Trying to request EPM LVLSFT_EN\n");
	rc = gpio_request(GPIO_EPM_LVLSFT_EN, "lvsft_en");
	if (!rc) {
		gpio_direction_output(GPIO_EPM_LVLSFT_EN, 1);
		printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: "
				"Configure the lvlsft\n");
	} else {
		printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: "
				"Error for epm lvlsft_en\n");
		goto exit_vreg_epm;
	}

	msleep(500);

	if (!epm_i2c_client)
		epm_i2c_client = epm_expander_i2c_register_board();

	rc = gpio_request(GPIO_PWR_MON_ENABLE, "pwr_mon_enable");
	if (!rc)
		rc = gpio_direction_output(GPIO_PWR_MON_ENABLE, 1);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": GPIO PWR MON Enable issue\n");
			goto exit_vreg_epm;
		}

	msleep(1000);

	rc = gpio_request(GPIO_ADC1_PWDN_N, "adc1_pwdn");
	if (!rc) {
		rc = gpio_direction_output(GPIO_ADC1_PWDN_N, 1);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": ADC1_PWDN error direction out\n");
			goto exit_vreg_epm;
		}
	}

	msleep(100);

	rc = gpio_request(GPIO_ADC2_PWDN_N, "adc2_pwdn");
	if (!rc) {
		rc = gpio_direction_output(GPIO_ADC2_PWDN_N, 1);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": ADC2_PWD error direction out\n");
			goto exit_vreg_epm;
		}
	}

	msleep(1000);

	rc = gpio_request(GPIO_PWR_MON_START, "pwr_mon_start");
	if (!rc) {
		rc = gpio_direction_output(GPIO_PWR_MON_START, 0);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
				"Gpio request problem %d\n", rc);
			goto exit_vreg_epm;
		}
	}

	rc = gpio_request(GPIO_EPM_SPI_ADC1_CS_N, "spi_adc1_cs");
	if (!rc) {
		rc = gpio_direction_output(GPIO_EPM_SPI_ADC1_CS_N, 0);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": EPM_SPI_ADC1_CS_N error\n");
			goto exit_vreg_epm;
		}
	}

	rc = gpio_request(GPIO_EPM_SPI_ADC2_CS_N, "spi_adc2_cs");
	if (!rc) {
		rc = gpio_direction_output(GPIO_EPM_SPI_ADC2_CS_N, 0);
		if (rc) {
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": EPM_SPI_ADC2_Cs_N error\n");
			goto exit_vreg_epm;
		}
	}

	printk(KERN_DEBUG "msm_adc_gpio_configure_expander_enable: Set "
			"the power monitor reset for epm\n");

	rc = gpio_request(GPIO_PWR_MON_RESET_N, "pwr_mon_reset_n");
	if (!rc) {
		gpio_direction_output(GPIO_PWR_MON_RESET_N, 0);
		if (rc)	{
			printk(KERN_ERR "msm_adc_gpio_configure_expander_enable"
					": Error in the power mon reset\n");
			goto exit_vreg_epm;
		}
	}

	msleep(1000);

	gpio_set_value_cansleep(GPIO_PWR_MON_RESET_N, 1);

	msleep(500);

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC1_CS_N, 1);

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC2_CS_N, 1);

	return rc;

exit_vreg_epm:
	regulator_disable(vreg_adc_epm1);

	printk(KERN_ERR "msm_adc_gpio_configure_expander_enable: Exit."
			" rc = %d.\n", rc);
	return rc;
};

static unsigned int msm_adc_gpio_configure_expander_disable(void)
{
	int rc = 0;

	gpio_set_value_cansleep(GPIO_PWR_MON_RESET_N, 0);
	gpio_free(GPIO_PWR_MON_RESET_N);

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC1_CS_N, 0);
	gpio_free(GPIO_EPM_SPI_ADC1_CS_N);

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC2_CS_N, 0);
	gpio_free(GPIO_EPM_SPI_ADC2_CS_N);

	gpio_set_value_cansleep(GPIO_PWR_MON_START, 0);
	gpio_free(GPIO_PWR_MON_START);

	gpio_direction_output(GPIO_ADC1_PWDN_N, 0);
	gpio_free(GPIO_ADC1_PWDN_N);

	gpio_direction_output(GPIO_ADC2_PWDN_N, 0);
	gpio_free(GPIO_ADC2_PWDN_N);

	gpio_set_value_cansleep(GPIO_PWR_MON_ENABLE, 0);
	gpio_free(GPIO_PWR_MON_ENABLE);

	gpio_set_value_cansleep(GPIO_EPM_LVLSFT_EN, 0);
	gpio_free(GPIO_EPM_LVLSFT_EN);

	gpio_set_value_cansleep(GPIO_EPM_5V_BOOST_EN, 0);
	gpio_free(GPIO_EPM_5V_BOOST_EN);

	gpio_set_value_cansleep(GPIO_EPM_3_3V_EN, 0);
	gpio_free(GPIO_EPM_3_3V_EN);

	rc = regulator_disable(vreg_adc_epm1);
	if (rc)
		printk(KERN_DEBUG "msm_adc_gpio_configure_expander_disable: "
			"Error while enabling regulator for epm s3 %d\n", rc);
	regulator_put(vreg_adc_epm1);

	printk(KERN_DEBUG "Exi msm_adc_gpio_configure_expander_disable\n");
	return rc;
};

unsigned int msm_adc_gpio_expander_enable(int cs_enable)
{
	int rc = 0;

	printk(KERN_DEBUG "msm_adc_gpio_expander_enable: cs_enable = %d",
		cs_enable);

	if (cs_enable < 16) {
		gpio_set_value_cansleep(GPIO_EPM_SPI_ADC1_CS_N, 0);
		gpio_set_value_cansleep(GPIO_EPM_SPI_ADC2_CS_N, 1);
	} else {
		gpio_set_value_cansleep(GPIO_EPM_SPI_ADC2_CS_N, 0);
		gpio_set_value_cansleep(GPIO_EPM_SPI_ADC1_CS_N, 1);
	}
	return rc;
};

unsigned int msm_adc_gpio_expander_disable(int cs_disable)
{
	int rc = 0;

	printk(KERN_DEBUG "Enter msm_adc_gpio_expander_disable.\n");

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC1_CS_N, 1);

	gpio_set_value_cansleep(GPIO_EPM_SPI_ADC2_CS_N, 1);

	return rc;
};
#endif

static struct msm_adc_channels msm_adc_channels_data[] = {
	{"vbatt", CHANNEL_ADC_VBATT, 0, &xoadc_fn, CHAN_PATH_TYPE2,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE3, scale_default},
	{"vcoin", CHANNEL_ADC_VCOIN, 0, &xoadc_fn, CHAN_PATH_TYPE1,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_default},
	{"vcharger_channel", CHANNEL_ADC_VCHG, 0, &xoadc_fn, CHAN_PATH_TYPE3,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE4, scale_default},
	{"charger_current_monitor", CHANNEL_ADC_CHG_MONITOR, 0, &xoadc_fn,
		CHAN_PATH_TYPE4,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE1, scale_default},
	{"vph_pwr", CHANNEL_ADC_VPH_PWR, 0, &xoadc_fn, CHAN_PATH_TYPE5,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE3, scale_default},
	{"usb_vbus", CHANNEL_ADC_USB_VBUS, 0, &xoadc_fn, CHAN_PATH_TYPE11,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE3, scale_default},
	{"pmic_therm", CHANNEL_ADC_DIE_TEMP, 0, &xoadc_fn, CHAN_PATH_TYPE12,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE1, scale_pmic_therm},
	{"pmic_therm_4K", CHANNEL_ADC_DIE_TEMP_4K, 0, &xoadc_fn,
		CHAN_PATH_TYPE12,
		ADC_CONFIG_TYPE1, ADC_CALIB_CONFIG_TYPE7, scale_pmic_therm},
	{"xo_therm", CHANNEL_ADC_XOTHERM, 0, &xoadc_fn, CHAN_PATH_TYPE_NONE,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE5, tdkntcgtherm},
	{"xo_therm_4K", CHANNEL_ADC_XOTHERM_4K, 0, &xoadc_fn,
		CHAN_PATH_TYPE_NONE,
		ADC_CONFIG_TYPE1, ADC_CALIB_CONFIG_TYPE6, tdkntcgtherm},
	{"hdset_detect", CHANNEL_ADC_HDSET, 0, &xoadc_fn, CHAN_PATH_TYPE6,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE1, scale_default},
	{"chg_batt_amon", CHANNEL_ADC_BATT_AMON, 0, &xoadc_fn, CHAN_PATH_TYPE10,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE1,
		scale_xtern_chgr_cur},
	{"batt_therm", CHANNEL_ADC_BATT_THERM, 0, &xoadc_fn, CHAN_PATH_TYPE7,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_batt_therm},
	{"batt_id", CHANNEL_ADC_BATT_ID, 0, &xoadc_fn, CHAN_PATH_TYPE9,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_default},
	{"ref_625mv", CHANNEL_ADC_625_REF, 0, &xoadc_fn, CHAN_PATH_TYPE15,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_default},
	{"ref_1250mv", CHANNEL_ADC_1250_REF, 0, &xoadc_fn, CHAN_PATH_TYPE13,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_default},
	{"ref_325mv", CHANNEL_ADC_325_REF, 0, &xoadc_fn, CHAN_PATH_TYPE14,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE2, scale_default},
	{"touch_id", CHANNEL_ADC_TOUCH_ID, 0, &xoadc_fn, CHAN_PATH_TYPE8,
		ADC_CONFIG_TYPE2, ADC_CALIB_CONFIG_TYPE1, scale_default},
};

static char *msm_adc_fluid_device_names[] = {
	"ADS_ADC1",
	"ADS_ADC2",
};

static struct msm_adc_platform_data msm_adc_pdata = {
	.channel = msm_adc_channels_data,
	.num_chan_supported = ARRAY_SIZE(msm_adc_channels_data),
#if defined(CONFIG_I2C) && \
	(defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE))
	.adc_gpio_enable = msm_adc_gpio_expander_enable,
	.adc_gpio_disable   = msm_adc_gpio_expander_disable,
	.adc_fluid_enable = msm_adc_gpio_configure_expander_enable,
	.adc_fluid_disable  = msm_adc_gpio_configure_expander_disable,
#endif
};

static struct platform_device msm_adc_device = {
	.name   = "msm_adc",
	.id = -1,
	.dev = {
		.platform_data = &msm_adc_pdata,
	},
};

static void pmic8058_xoadc_mpp_config(void)
{
	int rc;

	rc = pm8901_mpp_config_digital_out(XOADC_MPP_4,
			PM8901_MPP_DIG_LEVEL_S4, PM_MPP_DOUT_CTL_LOW);
	if (rc)
		pr_err("%s: Config mpp4 on pmic 8901 failed\n", __func__);
#if defined(CONFIG_INPUT_PANTECH_EARJACK) // && (defined(CONFIG_MACH_MSM8X60_EF39S) ||defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_PRESTO))
#else
	rc = pm8058_mpp_config_digital_in(XOADC_MPP_3,
			PM8058_MPP_DIG_LEVEL_S3, PM_MPP_DIN_TO_INT);
	if (rc)
		pr_err("%s: Config mpp3 on pmic 8058 failed\n", __func__);
#endif

// paiksun...
#if defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV == WS20)
	rc = pm8058_mpp_config_digital_out(XOADC_MPP_5,
			PM8058_MPP_DIG_LEVEL_S3, PM_MPP_DOUT_CTL_LOW);
#else
	rc = pm8058_mpp_config_analog_input(XOADC_MPP_5,
			PM_MPP_AIN_AMUX_CH9, PM_MPP_AOUT_CTL_DISABLE);
#endif
	if (rc)
		pr_err("%s: Config mpp5 on pmic 8058 failed\n", __func__);
//pz1946 20110904 battery remove issue
//#ifndef CONFIG_SKY_CHARGING  //p14682 kobj 110816
#if 0
	rc = pm8058_mpp_config_analog_input(XOADC_MPP_7,
			PM_MPP_AIN_AMUX_CH6, PM_MPP_AOUT_CTL_DISABLE);
	if (rc)
		pr_err("%s: Config mpp7 on pmic 8058 failed\n", __func__);


	rc = pm8058_mpp_config_analog_input(XOADC_MPP_8,
			PM_MPP_AIN_AMUX_CH8, PM_MPP_AOUT_CTL_DISABLE);
	if (rc)
		pr_err("%s: Config mpp8 on pmic 8058 failed\n", __func__);
#endif //CONFIG_SKY_CHARGING
/*EF39S TOUCH_ID
	rc = pm8058_mpp_config_analog_input(XOADC_MPP_10,
			PM_MPP_AIN_AMUX_CH7, PM_MPP_AOUT_CTL_DISABLE);
	if (rc)
		pr_err("%s: Config mpp10 on pmic 8058 failed\n", __func__);
		*/
}

static struct regulator *vreg_ldo18_adc;

static int pmic8058_xoadc_vreg_config(int on)
{
	int rc;

	if (on) {
		rc = regulator_enable(vreg_ldo18_adc);
		if (rc)
			pr_err("%s: Enable of regulator ldo18_adc "
						"failed\n", __func__);
	} else {
#if defined(CONFIG_SKY_CHARGING) || defined(CONFIG_SKY_SMB_CHARGER)
		rc = 0;
#else  //CONFIG_SKY_CHARGING
		rc = regulator_disable(vreg_ldo18_adc);
		if (rc)
			pr_err("%s: Disable of regulator ldo18_adc "
						"failed\n", __func__);
#endif  //CONFIG_SKY_CHARGING
	}

	return rc;
}

static int pmic8058_xoadc_vreg_setup(void)
{
	int rc;

	vreg_ldo18_adc = regulator_get(NULL, "8058_l18");
	if (IS_ERR(vreg_ldo18_adc)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			__func__, PTR_ERR(vreg_ldo18_adc));
		rc = PTR_ERR(vreg_ldo18_adc);
		goto fail;
	}

	rc = regulator_set_voltage(vreg_ldo18_adc, 2200000, 2200000);
	if (rc) {
		pr_err("%s: unable to set ldo18 voltage to 2.2V\n", __func__);
		goto fail;
	}

	return rc;
fail:
	regulator_put(vreg_ldo18_adc);
	return rc;
}

static void pmic8058_xoadc_vreg_shutdown(void)
{
	regulator_put(vreg_ldo18_adc);
}

/* usec. For this ADC,
 * this time represents clk rate @ txco w/ 1024 decimation ratio.
 * Each channel has different configuration, thus at the time of starting
 * the conversion, xoadc will return actual conversion time
 * */
static struct adc_properties pm8058_xoadc_data = {
	.adc_reference          = 2200, /* milli-voltage for this adc */
	.bitresolution         = 15,
	.bipolar                = 0,
	.conversiontime         = 54,
};

static struct xoadc_platform_data xoadc_pdata = {
	.xoadc_prop = &pm8058_xoadc_data,
	.xoadc_mpp_config = pmic8058_xoadc_mpp_config,
	.xoadc_vreg_set = pmic8058_xoadc_vreg_config,
	.xoadc_num = XOADC_PMIC_0,
	.xoadc_vreg_setup = pmic8058_xoadc_vreg_setup,
	.xoadc_vreg_shutdown = pmic8058_xoadc_vreg_shutdown,
};
#endif

#ifdef CONFIG_MSM_SDIO_AL

static unsigned mdm2ap_status = 140;

static int configure_mdm2ap_status(int on)
{
	int ret = 0;
	if (on)
		ret = msm_gpiomux_get(mdm2ap_status);
	else
		ret = msm_gpiomux_put(mdm2ap_status);

	if (ret)
		pr_err("%s: mdm2ap_status config failed, on = %d\n", __func__,
		       on);

	return ret;
}


static int get_mdm2ap_status(void)
{
	return gpio_get_value(mdm2ap_status);
}

static struct sdio_al_platform_data sdio_al_pdata = {
	.config_mdm2ap_status = configure_mdm2ap_status,
	.get_mdm2ap_status = get_mdm2ap_status,
	.allow_sdioc_version_major_2 = 0,
	.peer_sdioc_version_minor = 0x0202,
	.peer_sdioc_version_major = 0x0004,
	.peer_sdioc_boot_version_minor = 0x0001,
	.peer_sdioc_boot_version_major = 0x0003
};

struct platform_device msm_device_sdio_al = {
	.name = "msm_sdio_al",
	.id = -1,
	.dev		= {
		.parent = &msm_charm_modem.dev,
		.platform_data	= &sdio_al_pdata,
	},
};

#endif /* CONFIG_MSM_SDIO_AL */

static struct platform_device *charm_devices[] __initdata = {
	&msm_charm_modem,
#ifdef CONFIG_USB_ANDROID
	&usb_diag_mdm_device,
#endif
#ifdef CONFIG_MSM_SDIO_AL
	&msm_device_sdio_al,
#endif

};

static struct platform_device *surf_devices[] __initdata = {
	&msm_device_smd,
	&msm_device_uart_dm12,
#ifdef CONFIG_I2C_QUP
	&msm_gsbi3_qup_i2c_device,
	&msm_gsbi4_qup_i2c_device,
	&msm_gsbi7_qup_i2c_device,
	&msm_gsbi8_qup_i2c_device,
	&msm_gsbi9_qup_i2c_device,
#if defined(CONFIG_PN544)
	&msm_gsbi10_qup_i2c_device,
#endif
	&msm_gsbi12_qup_i2c_device,
#endif

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))	
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	&msm_gsbi5_qup_i2c_device,
#endif
#else
#ifdef CONFIG_SKY_SMB_CHARGER
	&msm_gsbi5_qup_i2c_device,
#endif
#endif
#ifdef CONFIG_SKY_BATTERY_MAX17040  // p14682 kobj 110607
	&msm_gsbi11_qup_i2c_device,
#endif	//CONFIG_SKY_BATTERY_MAX17040
#if defined(CONFIG_SKY_TDMB_SPI_QUP)
	&msm_gsbi2_qup_spi_device,
#endif
#ifdef CONFIG_SKY_TDMB_SPI_GPIO
	&gpio_spi_tdmb_device,
#endif
#if defined(CONFIG_SERIAL_MSM_HS) || defined(CONFIG_PANTECH_BT) //lsi@ps2.bluez
	&msm_device_uart_dm1,
#endif
#ifdef CONFIG_I2C_SSBI
	&msm_device_ssbi1,
	&msm_device_ssbi2,
	&msm_device_ssbi3,
#endif
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
	&isp1763_device,
#endif

#if defined(CONFIG_USB_GADGET_MSM_72K) || defined(CONFIG_USB_EHCI_HCD)
	&msm_device_otg,
#endif
#ifdef CONFIG_USB_GADGET_MSM_72K
	&msm_device_gadget_peripheral,
#endif
#ifdef CONFIG_USB_ANDROID
	&usb_mass_storage_device,
	&rndis_device,
#ifdef CONFIG_USB_ANDROID_DIAG
	&usb_diag_device,
#endif
#ifdef CONFIG_USB_F_SERIAL
	&usb_gadget_fserial_device,
#endif
#ifdef CONFIG_USB_ANDROID_ACM
	&usb_gadget_facm_device,
#endif
	&android_usb_device,
#endif
#ifdef CONFIG_BATTERY_MSM
	&msm_batt_device,
#endif
#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
	&android_pmem_kernel_ebi1_device,
#endif
#ifdef CONFIG_KERNEL_PMEM_SMI_REGION
	&android_pmem_kernel_smi_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device,
	&android_pmem_smipool_device,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
#ifdef CONFIG_INPUT_PANTECH_EARJACK
	&pantech_earjack_device,
#endif
	&msm_fb_device,
	&msm_kgsl_3d0,
#ifdef CONFIG_MSM_KGSL_2D
	&msm_kgsl_2d0,
	&msm_kgsl_2d1,
#endif
	&lcdc_samsung_panel_device,
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
	&lcdc_samsung_oled_panel_device,
#endif
#ifdef CONFIG_FB_MSM_LCDC_AUO_WVGA
	&lcdc_auo_wvga_panel_device,
#endif
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	&hdmi_msm_device,
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */
#ifdef CONFIG_FB_MSM_MIPI_DSI_SAMSUNG
	&mipi_dsi_samsung_panel_device,
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI_SONY
	&mipi_dsi_sony_panel_device,
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI_TOSHIBA
	&mipi_dsi_toshiba_panel_device,
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI_NOVATEK
	&mipi_dsi_novatek_panel_device,
#endif
#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_MT9E013
	&msm_camera_sensor_mt9e013,
#endif
#ifdef CONFIG_IMX074
	&msm_camera_sensor_imx074,
#endif
#ifdef CONFIG_WEBCAM_OV7692
	&msm_camera_sensor_webcam_ov7692,
#endif
#ifdef CONFIG_WEBCAM_OV9726
	&msm_camera_sensor_webcam_ov9726,
#endif
#ifdef CONFIG_QS_S5K4E1
	&msm_camera_sensor_qs_s5k4e1,
#endif
#ifdef CONFIG_PANTECH_CAMERA_CE1612
	&msm_camera_sensor_ce1612,
#endif
#ifdef CONFIG_PANTECH_CAMERA_ICP_HD
	&msm_camera_sensor_icp_hd,
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9P111
	&msm_camera_sensor_mt9p111,
#endif
#ifdef CONFIG_PANTECH_CAMERA_S5K4ECGX
	&msm_camera_sensor_s5k4ecgx,
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9D113
	&msm_camera_sensor_mt9d113,
#endif
#ifdef CONFIG_PANTECH_CAMERA_S5K6AAFX13
	&msm_camera_sensor_s5k6aafx13,
#endif
#ifdef CONFIG_PANTECH_CAMERA_MT9V113
	&msm_camera_sensor_mt9v113,
#endif
#endif
#ifdef CONFIG_MSM_GEMINI
	&msm_gemini_device,
#endif
#ifdef CONFIG_MSM_VPE
	&msm_vpe_device,
#endif

#if defined(CONFIG_MSM_RPM_LOG) || defined(CONFIG_MSM_RPM_LOG_MODULE)
	&msm_rpm_log_device,
#endif
#if defined(CONFIG_MSM_RPM_STATS_LOG)
	&msm_rpm_stat_device,
#endif
	&msm_device_vidc,
#if (defined(CONFIG_PANTECH_BT)) || /*//lsi@ps2.bluez*/ ((defined(CONFIG_MARIMBA_CORE)) && (defined(CONFIG_MSM_BT_POWER) || defined(CONFIG_MSM_BT_POWER_MODULE)))
	&msm_bt_power_device,
#endif
#ifdef CONFIG_PANTECH_BT
	&msm_bluesleep_device,
#endif//CONFIG_PANTECH_BT
#ifdef CONFIG_SENSORS_MSM_ADC
	&msm_adc_device,
#endif
#ifdef CONFIG_PMIC8058
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L0],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L1],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L2],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L3],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L4],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L5],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L6],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L7],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L8],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L9],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L10],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L11],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L12],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L13],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L14],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L15],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L16],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L17],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L18],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L19],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L20],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L21],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L22],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L23],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L24],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_L25],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_S2],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_S3],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_S4],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_LVS0],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_LVS1],
	&rpm_vreg_device[RPM_VREG_ID_PM8058_NCP],
#endif
#ifdef CONFIG_PMIC8901
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L0],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L1],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L2],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L3],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L4],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L5],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_L6],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_S2],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_S3],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_S4],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_LVS0],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_LVS1],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_LVS2],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_LVS3],
	&rpm_vreg_device[RPM_VREG_ID_PM8901_MVS0],
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif


#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
#ifdef CONFIG_MSM_USE_TSIF1
	&msm_device_tsif[1],
#else
	&msm_device_tsif[0],
#endif /* CONFIG_MSM_USE_TSIF1 */
#endif /* CONFIG_TSIF */

#ifdef CONFIG_HW_RANDOM_MSM
	&msm_device_rng,
#endif

#ifdef MHL_I2C_CONTROL
	&msm_device_i2c_gpio_mhl,
#endif
#ifdef CONFIG_WIFI_CONTROL_FUNC
	&wlan_device,
#endif

#ifdef LCD_BL_I2C_CONTROL
	&msm_device_i2c_gpio_led,
#endif /*LCD_BL_I2C_CONTROL */

#ifdef CONFIG_TOUCHSCREEN_MELFAS_TKI
	&msm_device_i2c_gpio_tki,
#endif

	&msm_tsens_device,

#ifdef CONFIG_INPUT_SENSOR
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	&msm_device_sensor_geomagnetic,
	&msm_device_sensor_accelerometer,
#else
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV < TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV < WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV < WS20))
	&msm_device_sensor_gyroscope,
#endif
#endif	
	&msm_device_sensor_proximity,
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
       &msm_i2c_gpio_device_audience,
#endif

#ifdef CONFIG_SKY_SMB_CHARGER // i2c
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
        &msm_device_i2c_gpio_smb,
#endif
#endif
};

#define EXT_CHG_VALID_MPP 10
#define EXT_CHG_VALID_MPP_2 11

#ifdef CONFIG_ISL9519_CHARGER
static int isl_detection_setup(void)
{
	int ret = 0;

	ret = pm8058_mpp_config_digital_in(EXT_CHG_VALID_MPP,
					   PM8058_MPP_DIG_LEVEL_S3,
					   PM_MPP_DIN_TO_INT);
	ret |=  pm8058_mpp_config_bi_dir(EXT_CHG_VALID_MPP_2,
					   PM8058_MPP_DIG_LEVEL_S3,
					   PM_MPP_BI_PULLUP_10KOHM
					   );
	return ret;
}

static struct isl_platform_data isl_data __initdata = {
	.chgcurrent		= 700,
	.valid_n_gpio		= PM8058_MPP_PM_TO_SYS(10),
	.chg_detection_config	= isl_detection_setup,
	.max_system_voltage	= 4200,
	.min_system_voltage	= 3200,
	.term_current		= 120,
	.input_current		= 2048,
};

static struct i2c_board_info isl_charger_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("isl9519q", 0x9),
		.irq = PM8058_CBLPWR_IRQ(PM8058_IRQ_BASE),
		.platform_data = &isl_data,
	},
};
#endif

#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
static int smb137b_detection_setup(void)
{
	int ret = 0;
#ifdef CONFIG_SKY_SMB_CHARGER
	ret = pm8058_mpp_config_digital_in(EXT_CHG_VALID_MPP_2,
					PM8058_MPP_DIG_LEVEL_S3,
					PM_MPP_DIN_TO_INT);
#else
	ret = pm8058_mpp_config_digital_in(EXT_CHG_VALID_MPP,
					PM8058_MPP_DIG_LEVEL_S3,
					PM_MPP_DIN_TO_INT);
	ret |=  pm8058_mpp_config_bi_dir(EXT_CHG_VALID_MPP_2,
					PM8058_MPP_DIG_LEVEL_S3,
					PM_MPP_BI_PULLUP_10KOHM);
#endif
	return ret;
}

static struct smb137b_platform_data smb137b_data __initdata = {
	.chg_detection_config = smb137b_detection_setup,
//pz1946 20110907 interrupt pin change
#ifdef CONFIG_SKY_SMB_CHARGER
	//.valid_n_gpio = PM8058_MPP_PM_TO_SYS(11),
	.valid_n_gpio = SC_STAT_IRQ_GPIO,
#else
	.valid_n_gpio = PM8058_MPP_PM_TO_SYS(10),
#endif
	.batt_mah_rating = 950,
};

static struct i2c_board_info smb137b_charger_i2c_info[] __initdata = {
	{
#ifdef CONFIG_SKY_SMB_CHARGER
		I2C_BOARD_INFO("smb137b", 0x9A >> 1),
#else
		I2C_BOARD_INFO("smb137b", 0x08),
#endif
//pz1946 20110907 interrupt pin change
#ifdef CONFIG_SKY_SMB_CHARGER
		.irq = MSM_GPIO_TO_INT(SC_STAT_IRQ_GPIO),
#else
		.irq = PM8058_CBLPWR_IRQ(PM8058_IRQ_BASE),
#endif
		.platform_data = &smb137b_data,
	},
};
#endif

#ifdef CONFIG_PMIC8058
#define PMIC_GPIO_SDC3_DET 22

static int pm8058_gpios_init(void)
{
	int i;
	int rc;
	struct pm8058_gpio_cfg {
		int                gpio;
		struct pm8058_gpio cfg;
	};

	struct pm8058_gpio_cfg gpio_cfgs[] = {
		{ /* FFA ethernet */
			6,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_DN,
				.vin_sel        = 2,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			},
		},
#if defined (CONFIG_MMC_MSM_CARD_HW_DETECTION) || defined(CONFIG_SKY_MMC)
		{
			PMIC_GPIO_SDC3_DET - 1,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_UP_30,
				.vin_sel        = 2,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			},
		},
#endif
		{ /* core&surf gpio expander */
			UI_INT1_N,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			},
		},
		{ /* docking gpio expander */
			UI_INT2_N,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			},
		},
		{ /* FHA/keypad gpio expanders */
			UI_INT3_N,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			},
		},
		{ /* TouchDisc Interrupt */
			5,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_UP_1P5,
				.vin_sel        = 2,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			}
		},
		{ /* Timpani Reset */
			20,
			{
				.direction	= PM_GPIO_DIR_OUT,
				.output_value	= 1,
				.output_buffer	= PM_GPIO_OUT_BUF_CMOS,
				.pull		= PM_GPIO_PULL_DN,
				.out_strength	= PM_GPIO_STRENGTH_HIGH,
				.function	= PM_GPIO_FUNC_NORMAL,
				.vin_sel	= 2,
				.inv_int_pol	= 0,
			}
		},
//20101015 choiseulkee add for uart console, PM8058 UART MUX GPIO config
#ifdef CONFIG_SKY_GSBI12_UART_CONSOLE
		{ /* UART_TX3 : PM8058 GPIO_23 */
			22,
			{
				.direction      = PM_GPIO_DIR_OUT,
				.pull           = PM_GPIO_PULL_NO,//PM_GPIO_PULL_DN,
				.vin_sel        = PM_GPIO_VIN_L6,
				.function       = PM_GPIO_FUNC_2,
				.inv_int_pol    = 0,
		}
		},
		{ /* UART_RX3 : PM8058 GPIO_35 */
			34,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,//PM_GPIO_PULL_DN,
				.vin_sel        = PM_GPIO_VIN_L6,
				.function       = PM_GPIO_FUNC_NORMAL,//PM_GPIO_FUNC_2,
				.inv_int_pol    = 0,
			}
		},
		{ /* UART_M_TX : PM8058 GPIO_36 */
			35,
			{
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,//PM_GPIO_PULL_DN,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_NORMAL,//PM_GPIO_FUNC_2,
				.inv_int_pol    = 0,
			}
		},
		{ /* UART_M_RX : PM8058 GPIO_37 */
			36,
			{
				.direction      = PM_GPIO_DIR_OUT,
				.pull           = PM_GPIO_PULL_NO,//PM_GPIO_PULL_DN,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_2,
				.inv_int_pol    = 0,
			}
		}
#else /* CONFIG_SKY_GSBI12_UART_CONSOLE */
		{ /* PMIC ID interrupt */
			36,
			{
				.direction	= PM_GPIO_DIR_IN,
				.pull		= PM_GPIO_PULL_UP_1P5,
				.function	= PM_GPIO_FUNC_NORMAL,
				.vin_sel	= 2,
				.inv_int_pol	= 0,
			}
		},
#endif /* CONFIG_SKY_GSBI12_UART_CONSOLE */
	};

#if defined(CONFIG_HAPTIC_ISA1200) || \
		defined(CONFIG_HAPTIC_ISA1200_MODULE)

	struct pm8058_gpio_cfg en_hap_gpio_cfg = {
			PMIC_GPIO_HAP_ENABLE,
			{
				.direction      = PM_GPIO_DIR_OUT,
				.pull           = PM_GPIO_PULL_NO,
				.out_strength   = PM_GPIO_STRENGTH_HIGH,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
				.vin_sel        = 2,
				.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
				.output_value   = 0,
			}

	};
#endif

#if defined(CONFIG_PMIC8058_OTHC) || defined(CONFIG_PMIC8058_OTHC_MODULE)
	struct pm8058_gpio_cfg line_in_gpio_cfg = {
			18,
			{
				.direction	= PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_UP_1P5,
				.vin_sel        = 2,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
			}
	};
#endif

#if defined(CONFIG_QS_S5K4E1)
		{
			struct pm8058_gpio_cfg qs_hc37_cam_pd_gpio_cfg = {
			26,
			{
				.direction		= PM_GPIO_DIR_OUT,
				.output_value	= 0,
				.output_buffer	= PM_GPIO_OUT_BUF_CMOS,
				.pull			= PM_GPIO_PULL_DN,
				.out_strength	= PM_GPIO_STRENGTH_HIGH,
				.function		= PM_GPIO_FUNC_NORMAL,
				.vin_sel		= 2,
				.inv_int_pol	= 0,
			}
		};
#endif

#if defined(CONFIG_HAPTIC_ISA1200) || \
			defined(CONFIG_HAPTIC_ISA1200_MODULE)
	if (machine_is_msm8x60_fluid()) {
		rc = pm8058_gpio_config(en_hap_gpio_cfg.gpio,
				&en_hap_gpio_cfg.cfg);
		if (rc < 0) {
			pr_err("%s pmic haptics gpio config failed\n",
							__func__);
			return rc;
		}
	}
#endif

#if defined(CONFIG_PMIC8058_OTHC) || defined(CONFIG_PMIC8058_OTHC_MODULE)
	/* Line_in only for 8660 ffa & surf */
	if (machine_is_msm8x60_ffa() || machine_is_msm8x60_surf() ||
		machine_is_msm8x60_charm_surf() ||
		machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
		|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
		|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
		|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
		|| machine_is_msm8x60_presto()
#endif
		) {
		rc = pm8058_gpio_config(line_in_gpio_cfg.gpio,
				&line_in_gpio_cfg.cfg);
		if (rc < 0) {
			pr_err("%s pmic line_in gpio config failed\n",
							__func__);
			return rc;
		}
	}
#endif

#if defined(CONFIG_QS_S5K4E1)
		/* qs_cam_hc37_cam_pd only for 8660 fluid qs camera*/
		if (machine_is_msm8x60_fluid()) {
			rc = pm8058_gpio_config(qs_hc37_cam_pd_gpio_cfg.gpio,
					&qs_hc37_cam_pd_gpio_cfg.cfg);
			if (rc < 0) {
				pr_err("%s pmic qs_hc37_cam_pd gpio config failed\n",
								__func__);
				return rc;
			}
		}
	}
#endif

	for (i = 0; i < ARRAY_SIZE(gpio_cfgs); ++i) {
		rc = pm8058_gpio_config(gpio_cfgs[i].gpio,
				&gpio_cfgs[i].cfg);
		if (rc < 0) {
			pr_err("%s pmic gpio config failed\n",
				__func__);
			return rc;
		}
	}

	return 0;
}

#if defined(CONFIG_MACH_MSM8X60_EF39S) 
static const unsigned int ffa_keymap[] = {
	KEY(0, 0, KEY_HOME), // DRV1, SNS1 : Home
	KEY(0, 1, KEY_VOLUMEDOWN), // DRV1, SNS2 : Volume Down
	KEY(0, 2, KEY_UNKNOWN), //DRV1, SNS3 : LTE Key
	KEY(0, 3, KEY_UNKNOWN),
	KEY(1, 0, KEY_VOLUMEDOWN),
	KEY(1, 1, KEY_VOLUMEUP), // DRV2, SNS2 : Volume Up
	KEY(1, 2, KEY_SEARCH), //DRV2, SNS3 : Search
	KEY(1, 3, KEY_UNKNOWN),
	KEY(2, 3, KEY_UNKNOWN),
};
#elif (defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S))
#if (BOARD_REV >= WS20)
static const unsigned int ffa_keymap[] = {
        KEY(0, 0, KEY_VOLUMEDOWN), // DRV1, SNS1 : Volume Up
        KEY(0, 1, KEY_SEARCH), // DRV1, SNS2 : Special Key
        KEY(0, 2, KEY_UNKNOWN), //DRV1, SNS3 :
        KEY(0, 3, KEY_UNKNOWN),

        KEY(1, 0, KEY_VOLUMEUP), // DRV2, SNS1: Volume Down
        KEY(1, 1, KEY_UNKNOWN), // DRV2, SNS2 : Search Key
        KEY(1, 2, KEY_UNKNOWN), //DRV2, SNS3 :
        KEY(1, 3, KEY_UNKNOWN),

        KEY(2, 3, KEY_UNKNOWN),

};
#else
static const unsigned int ffa_keymap[] = {
	KEY(0, 0, KEY_VOLUMEUP), // DRV1, SNS1 : Volume Up 
	KEY(0, 1, KEY_1), // DRV1, SNS2 : Special Key 
	KEY(0, 2, KEY_UNKNOWN), //DRV1, SNS3 :
	KEY(0, 3, KEY_UNKNOWN),
	KEY(1, 0, KEY_VOLUMEDOWN), // DRV2, SNS1: Volume Down
	KEY(1, 1, KEY_SEARCH), // DRV2, SNS2 : Search Key
	KEY(1, 2, KEY_UNKNOWN), //DRV2, SNS3 : 
	KEY(1, 3, KEY_UNKNOWN),
	KEY(2, 3, KEY_UNKNOWN),
};
#endif
#elif defined(CONFIG_MACH_MSM8X60_PRESTO)
static const unsigned int ffa_keymap[] = {
	KEY(0, 0, KEY_VOLUMEUP), // DRV1, SNS1 : Home
	KEY(0, 1, KEY_VOLUMEDOWN), // DRV1, SNS2 : Volume Down
	KEY(0, 2, KEY_UNKNOWN), //DRV1, SNS3 : LTE Key
	KEY(0, 3, KEY_UNKNOWN),
	KEY(1, 0, KEY_VOLUMEDOWN),
	KEY(1, 1, KEY_VOLUMEUP), // DRV2, SNS2 : Volume Up
	KEY(1, 2, KEY_SEARCH), //DRV2, SNS3 : Search
	KEY(1, 3, KEY_UNKNOWN),
	KEY(2, 3, KEY_UNKNOWN),
};
#else
static const unsigned int ffa_keymap[] = {
	KEY(0, 0, KEY_FN_F1),	 /* LS - PUSH1 */
	KEY(0, 1, KEY_UP),	 /* NAV - UP */
	KEY(0, 2, KEY_LEFT),	 /* NAV - LEFT */
	KEY(0, 3, KEY_VOLUMEUP), /* Shuttle SW_UP */

	KEY(1, 0, KEY_FN_F2), 	 /* LS - PUSH2 */
	KEY(1, 1, KEY_RIGHT),    /* NAV - RIGHT */
	KEY(1, 2, KEY_DOWN),     /* NAV - DOWN */
	KEY(1, 3, KEY_VOLUMEDOWN),

	KEY(2, 3, KEY_ENTER),     /* SW_PUSH key */

	KEY(4, 0, KEY_CAMERA_FOCUS), /* RS - PUSH1 */
	KEY(4, 1, KEY_UP),	  /* USER_UP */
	KEY(4, 2, KEY_LEFT),	  /* USER_LEFT */
	KEY(4, 3, KEY_HOME),	  /* Right switch: MIC Bd */
	KEY(4, 4, KEY_FN_F3),	  /* Reserved MIC */

	KEY(5, 0, KEY_CAMERA), /* RS - PUSH2 */
	KEY(5, 1, KEY_RIGHT),	  /* USER_RIGHT */
	KEY(5, 2, KEY_DOWN),	  /* USER_DOWN */
	KEY(5, 3, KEY_BACK),	  /* Left switch: MIC */
	KEY(5, 4, KEY_MENU),	  /* Center switch: MIC */
};
#endif

static struct resource resources_keypad[] = {
	{
		.start	= PM8058_KEYPAD_IRQ(PM8058_IRQ_BASE),
		.end	= PM8058_KEYPAD_IRQ(PM8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= PM8058_KEYSTUCK_IRQ(PM8058_IRQ_BASE),
		.end	= PM8058_KEYSTUCK_IRQ(PM8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct matrix_keymap_data ffa_keymap_data = {
	.keymap_size	= ARRAY_SIZE(ffa_keymap),
	.keymap		= ffa_keymap,
};

static struct pmic8058_keypad_data ffa_keypad_data = {
	.input_name		= "ffa-keypad",
	.input_phys_device	= "ffa-keypad/input0",
	.num_rows		= 6,
	.num_cols		= 5,
	.rows_gpio_start	= 8,
	.cols_gpio_start	= 0,
	.debounce_ms		= {8, 10},
	.scan_delay_ms		= 32,
	.row_hold_ns            = 91500,
	.wakeup			= 1,
	.keymap_data		= &ffa_keymap_data,
};

static const unsigned int fluid_keymap[] = {
	KEY(0, 0, KEY_FN_F1),	 /* LS - PUSH1 */
	KEY(0, 1, KEY_UP),	 /* NAV - UP */
	KEY(0, 2, KEY_LEFT),	 /* NAV - LEFT */
	KEY(0, 3, KEY_VOLUMEDOWN), /* Shuttle SW_UP */

	KEY(1, 0, KEY_FN_F2),	 /* LS - PUSH2 */
	KEY(1, 1, KEY_RIGHT),    /* NAV - RIGHT */
	KEY(1, 2, KEY_DOWN),     /* NAV - DOWN */
	KEY(1, 3, KEY_VOLUMEUP),

	KEY(2, 3, KEY_ENTER),     /* SW_PUSH key */

	KEY(4, 0, KEY_CAMERA_FOCUS), /* RS - PUSH1 */
	KEY(4, 1, KEY_UP),	  /* USER_UP */
	KEY(4, 2, KEY_LEFT),	  /* USER_LEFT */
	KEY(4, 3, KEY_HOME),	  /* Right switch: MIC Bd */
	KEY(4, 4, KEY_FN_F3),	  /* Reserved MIC */

	KEY(5, 0, KEY_CAMERA), /* RS - PUSH2 */
	KEY(5, 1, KEY_RIGHT),	  /* USER_RIGHT */
	KEY(5, 2, KEY_DOWN),	  /* USER_DOWN */
	KEY(5, 3, KEY_BACK),	  /* Left switch: MIC */
	KEY(5, 4, KEY_MENU),	  /* Center switch: MIC */
};

static struct matrix_keymap_data fluid_keymap_data = {
	.keymap_size	= ARRAY_SIZE(fluid_keymap),
	.keymap		= fluid_keymap,
};

static struct pmic8058_keypad_data fluid_keypad_data = {
	.input_name		= "fluid-keypad",
	.input_phys_device	= "fluid-keypad/input0",
	.num_rows		= 6,
	.num_cols		= 5,
	.rows_gpio_start	= 8,
	.cols_gpio_start	= 0,
	.debounce_ms		= {8, 10},
	.scan_delay_ms		= 32,
	.row_hold_ns            = 91500,
	.wakeup			= 1,
	.keymap_data		= &fluid_keymap_data,
};

static struct resource resources_pwrkey[] = {
	{
		.start	= PM8058_PWRKEY_REL_IRQ(PM8058_IRQ_BASE),
		.end	= PM8058_PWRKEY_REL_IRQ(PM8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= PM8058_PWRKEY_PRESS_IRQ(PM8058_IRQ_BASE),
		.end	= PM8058_PWRKEY_PRESS_IRQ(PM8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct pmic8058_pwrkey_pdata pwrkey_pdata = {
	.pull_up		= 1,
	.kpd_trigger_delay_us   = 970,
	.wakeup			= 1,
	.pwrkey_time_ms		= 500,
};

static struct pmic8058_vibrator_pdata pmic_vib_pdata = {
	.initial_vibrate_ms  = 500,
	.level_mV = 3000,
	.max_timeout_ms = 15000,
};

#if defined(CONFIG_PMIC8058_OTHC) || defined(CONFIG_PMIC8058_OTHC_MODULE)
#define PM8058_OTHC_CNTR_BASE0	0xA0
#define PM8058_OTHC_CNTR_BASE1	0x134
#define PM8058_OTHC_CNTR_BASE2	0x137
#define PM8058_LINE_IN_DET_GPIO	PM8058_GPIO_PM_TO_SYS(18)

static struct othc_accessory_info othc_accessories[]  = {
	{
		.accessory = OTHC_SVIDEO_OUT,
		.detect_flags = OTHC_MICBIAS_DETECT | OTHC_SWITCH_DETECT
							| OTHC_ADC_DETECT,
		.key_code = SW_VIDEOOUT_INSERT,
		.enabled = false,
		.adc_thres = {
				.min_threshold = 20,
				.max_threshold = 40,
			},
	},
	{
		.accessory = OTHC_ANC_HEADPHONE,
		.detect_flags = OTHC_MICBIAS_DETECT | OTHC_GPIO_DETECT |
							OTHC_SWITCH_DETECT,
		.gpio = PM8058_LINE_IN_DET_GPIO,
		.active_low = 1,
		.key_code = SW_HEADPHONE_INSERT,
		.enabled = true,
	},
	{
		.accessory = OTHC_ANC_HEADSET,
		.detect_flags = OTHC_MICBIAS_DETECT | OTHC_GPIO_DETECT,
		.gpio = PM8058_LINE_IN_DET_GPIO,
		.active_low = 1,
		.key_code = SW_HEADPHONE_INSERT,
		.enabled = true,
	},
	{
		.accessory = OTHC_HEADPHONE,
		.detect_flags = OTHC_MICBIAS_DETECT | OTHC_SWITCH_DETECT,
		.key_code = SW_HEADPHONE_INSERT,
		.enabled = true,
	},
	{
		.accessory = OTHC_MICROPHONE,
		.detect_flags = OTHC_GPIO_DETECT,
		.gpio = PM8058_LINE_IN_DET_GPIO,
		.active_low = 1,
		.key_code = SW_MICROPHONE_INSERT,
		.enabled = true,
	},
	{
		.accessory = OTHC_HEADSET,
		.detect_flags = OTHC_MICBIAS_DETECT,
		.key_code = SW_HEADPHONE_INSERT,
		.enabled = true,
	},
};

static struct othc_switch_info switch_info[] = {
	{
		.min_adc_threshold = 0,
		.max_adc_threshold = 100,
		.key_code = KEY_PLAYPAUSE,
	},
	{
		.min_adc_threshold = 100,
		.max_adc_threshold = 200,
		.key_code = KEY_REWIND,
	},
	{
		.min_adc_threshold = 200,
		.max_adc_threshold = 500,
		.key_code = KEY_FASTFORWARD,
	},
};

static struct othc_n_switch_config switch_config = {
	.voltage_settling_time_ms = 0,
	.num_adc_samples = 3,
	.adc_channel = CHANNEL_ADC_HDSET,
	.switch_info = switch_info,
	.num_keys = ARRAY_SIZE(switch_info),
	.default_sw_en = true,
	.default_sw_idx = 0,
};

static struct hsed_bias_config hsed_bias_config = {
	/* HSED mic bias config info */
	.othc_headset = OTHC_HEADSET_NO,
	.othc_lowcurr_thresh_uA = 100,
	.othc_highcurr_thresh_uA = 600,
	.othc_hyst_prediv_us = 7800,
	.othc_period_clkdiv_us = 62500,
	.othc_hyst_clk_us = 121000,
	.othc_period_clk_us = 312500,
	.othc_wakeup = 1,
};

static struct othc_hsed_config hsed_config_1 = {
	.hsed_bias_config = &hsed_bias_config,
	/*
	 * The detection delay and switch reporting delay are
	 * required to encounter a hardware bug (spurious switch
	 * interrupts on slow insertion/removal of the headset).
	 * This will introduce a delay in reporting the accessory
	 * insertion and removal to the userspace.
	 */
	.detection_delay_ms = 1500,
	/* Switch info */
	.switch_debounce_ms = 1500,
	.othc_support_n_switch = false,
	.switch_config = &switch_config,
	.ir_gpio = -1,
	/* Accessory info */
	.accessories_support = true,
	.accessories = othc_accessories,
	.othc_num_accessories = ARRAY_SIZE(othc_accessories),
};

static struct othc_regulator_config othc_reg = {
	.regulator	 = "8058_l5",
	.max_uV		 = 2850000,
	.min_uV		 = 2850000,
};

/* MIC_BIAS0 is configured as normal MIC BIAS */
static struct pmic8058_othc_config_pdata othc_config_pdata_0 = {
	.micbias_select = OTHC_MICBIAS_0,
	.micbias_capability = OTHC_MICBIAS,
	.micbias_enable = OTHC_SIGNAL_OFF,
	.micbias_regulator = &othc_reg,
};

/* MIC_BIAS1 is configured as HSED_BIAS for OTHC */
static struct pmic8058_othc_config_pdata othc_config_pdata_1 = {
	.micbias_select = OTHC_MICBIAS_1,
	.micbias_capability = OTHC_MICBIAS_HSED,
	.micbias_enable = OTHC_SIGNAL_PWM_TCXO,
	.micbias_regulator = &othc_reg,
	.hsed_config = &hsed_config_1,
	.hsed_name = "8660_handset",
};

/* MIC_BIAS2 is configured as normal MIC BIAS */
static struct pmic8058_othc_config_pdata othc_config_pdata_2 = {
	.micbias_select = OTHC_MICBIAS_2,
	.micbias_capability = OTHC_MICBIAS,
	.micbias_enable = OTHC_SIGNAL_OFF,
	.micbias_regulator = &othc_reg,
};

static struct resource resources_othc_0[] = {
	{
		.name = "othc_base",
		.start = PM8058_OTHC_CNTR_BASE0,
		.end   = PM8058_OTHC_CNTR_BASE0,
		.flags = IORESOURCE_IO,
	},
};

static struct resource resources_othc_1[] = {
	{
		.start = PM8058_SW_1_IRQ(PM8058_IRQ_BASE),
		.end   = PM8058_SW_1_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = PM8058_IR_1_IRQ(PM8058_IRQ_BASE),
		.end   = PM8058_IR_1_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "othc_base",
		.start = PM8058_OTHC_CNTR_BASE1,
		.end   = PM8058_OTHC_CNTR_BASE1,
		.flags = IORESOURCE_IO,
	},
};

static struct resource resources_othc_2[] = {
	{
		.name = "othc_base",
		.start = PM8058_OTHC_CNTR_BASE2,
		.end   = PM8058_OTHC_CNTR_BASE2,
		.flags = IORESOURCE_IO,
	},
};

static void __init msm8x60_init_pm8058_othc(void)
{
	int i;

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2 ||
		machine_is_msm8x60_fluid() || machine_is_msm8x60_charm_surf() ||
		machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
		|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
		|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
		|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
		|| machine_is_msm8x60_presto()
#endif
		) {
		/* 3-switch headset supported only by V2 FFA and FLUID */
		hsed_config_1.accessories_adc_support = true,
		/* ADC based accessory detection works only on V2 and FLUID */
		hsed_config_1.accessories_adc_channel = CHANNEL_ADC_HDSET,
		hsed_config_1.othc_support_n_switch = true;
	}

	/* IR GPIO is absent on FLUID */
	if (machine_is_msm8x60_fluid())
		hsed_config_1.ir_gpio = -1;

	for (i = 0; i < ARRAY_SIZE(othc_accessories); i++) {
		if (machine_is_msm8x60_fluid()) {
			switch (othc_accessories[i].accessory) {
			case OTHC_ANC_HEADPHONE:
			case OTHC_ANC_HEADSET:
				othc_accessories[i].gpio = GPIO_HEADSET_DET_N;
				break;
			case OTHC_MICROPHONE:
				othc_accessories[i].enabled = false;
				break;
			case OTHC_SVIDEO_OUT:
				othc_accessories[i].enabled = true;
				hsed_config_1.video_out_gpio = GPIO_HS_SW_DIR;
				break;
			}
		}
	}
}
#endif

static struct resource resources_pm8058_charger[] = {
	{	.name = "CHGVAL",
		.start = PM8058_CHGVAL_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGVAL_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{	.name = "CHGINVAL",
		.start = PM8058_CHGINVAL_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGINVAL_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHGILIM",
		.start = PM8058_CHGILIM_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGILIM_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "VCP",
		.start = PM8058_VCP_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_VCP_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
		{
		.name = "ATC_DONE",
		.start = PM8058_ATC_DONE_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_ATC_DONE_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "ATCFAIL",
		.start = PM8058_ATCFAIL_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_ATCFAIL_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "AUTO_CHGDONE",
		 .start = PM8058_AUTO_CHGDONE_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_AUTO_CHGDONE_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "AUTO_CHGFAIL",
		.start = PM8058_AUTO_CHGFAIL_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_AUTO_CHGFAIL_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHGSTATE",
		.start = PM8058_CHGSTATE_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGSTATE_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "FASTCHG",
		.start = PM8058_FASTCHG_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_FASTCHG_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHG_END",
		 .start = PM8058_CHG_END_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_CHG_END_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "BATTTEMP",
		.start = PM8058_BATTTEMP_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_BATTTEMP_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHGHOT",
		.start = PM8058_CHGHOT_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGHOT_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHGTLIMIT",
		.start = PM8058_CHGTLIMIT_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_CHGTLIMIT_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "CHG_GONE",
		 .start = PM8058_CHG_GONE_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_CHG_GONE_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "VCPMAJOR",
		 .start = PM8058_VCPMAJOR_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_VCPMAJOR_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "VBATDET",
		 .start = PM8058_VBATDET_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_VBATDET_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "BATFET",
		 .start = PM8058_BATFET_IRQ(PM8058_IRQ_BASE),
		 .end = PM8058_BATFET_IRQ(PM8058_IRQ_BASE),
		 .flags = IORESOURCE_IRQ,
	},
	{
		.name = "BATT_REPLACE",
		.start = PM8058_BATT_REPLACE_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_BATT_REPLACE_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "BATTCONNECT",
		.start = PM8058_BATTCONNECT_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_BATTCONNECT_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.name = "VBATDET_LOW",
		.start = PM8058_VBATDET_LOW_IRQ(PM8058_IRQ_BASE),
		.end = PM8058_VBATDET_LOW_IRQ(PM8058_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
//pz1946 20110904 battery remove issue
//#ifdef CONFIG_SKY_CHARGING  //kobj 110513
#if defined(CONFIG_SKY_CHARGING) || defined(CONFIG_SKY_SMB_CHARGER)
	{
		.name = "BATT_ID_CHANGED",
		.start = PM8058_MPP_IRQ(PM8058_IRQ_BASE, XOADC_MPP_8),
		.end = PM8058_MPP_IRQ(PM8058_IRQ_BASE, XOADC_MPP_8),
		.flags = IORESOURCE_IRQ,
	},
#endif //CONFIG_SKY_CHARGING
};

static int pm8058_pwm_config(struct pwm_device *pwm, int ch, int on)
{
	struct pm8058_gpio pwm_gpio_config = {
		.direction      = PM_GPIO_DIR_OUT,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM_GPIO_VIN_VPH,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_2,
	};

	int rc = -EINVAL;
	int id, mode, max_mA;

	id = mode = max_mA = 0;
	switch (ch) {
	case 0:
	case 1:
	case 2:
		if (on) {
			id = 24 + ch;
			rc = pm8058_gpio_config(id - 1, &pwm_gpio_config);
			if (rc)
				pr_err("%s: pm8058_gpio_config(%d): rc=%d\n",
					__func__, id, rc);
		}
		break;

	case 6:
		id = PM_PWM_LED_FLASH;
		mode = PM_PWM_CONF_PWM1;
		max_mA = 300;
		break;

	case 7:
		id = PM_PWM_LED_FLASH1;
		mode = PM_PWM_CONF_PWM1;
		max_mA = 300;
		break;

	default:
		break;
	}

	if (ch >= 6 && ch <= 7) {
		if (!on) {
			mode = PM_PWM_CONF_NONE;
			max_mA = 0;
		}
		rc = pm8058_pwm_config_led(pwm, id, mode, max_mA);
		if (rc)
			pr_err("%s: pm8058_pwm_config_led(ch=%d): rc=%d\n",
			       __func__, ch, rc);
	}
	return rc;

}

static struct pm8058_pwm_pdata pm8058_pwm_data = {
	.config		= pm8058_pwm_config,
};

#define PM8058_GPIO_INT           88

static struct pm8058_gpio_platform_data pm8058_gpio_data = {
	.gpio_base	= PM8058_GPIO_PM_TO_SYS(0),
	.irq_base	= PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 0),
	.init		= pm8058_gpios_init,
};

static struct pm8058_gpio_platform_data pm8058_mpp_data = {
	.gpio_base	= PM8058_GPIO_PM_TO_SYS(PM8058_GPIOS),
	.irq_base	= PM8058_MPP_IRQ(PM8058_IRQ_BASE, 0),
};

static struct resource resources_rtc[] = {
       {
		.start  = PM8058_RTC_IRQ(PM8058_IRQ_BASE),
		.end    = PM8058_RTC_IRQ(PM8058_IRQ_BASE),
		.flags  = IORESOURCE_IRQ,
       },
       {
		.start  = PM8058_RTC_ALARM_IRQ(PM8058_IRQ_BASE),
		.end    = PM8058_RTC_ALARM_IRQ(PM8058_IRQ_BASE),
		.flags  = IORESOURCE_IRQ,
       },
};

static struct pm8058_rtc_platform_data pm8058_rtc_pdata = {
	.rtc_alarm_powerup	= false,
};

static struct pmic8058_led pmic8058_flash_leds[] = {
	[0] = {
		.name		= "camera:flash0",
		.max_brightness = 15,
		.id		= PMIC8058_ID_FLASH_LED_0,
	},
	[1] = {
		.name		= "camera:flash1",
		.max_brightness = 15,
		.id		= PMIC8058_ID_FLASH_LED_1,
	},
#if defined(CONFIG_MACH_MSM8X60_EF39S)||defined(CONFIG_MACH_MSM8X60_EF40K)||defined(CONFIG_MACH_MSM8X60_EF40S)||defined(CONFIG_MACH_MSM8X60_PRESTO)
	[2] = {
		.name		= "keyboard-backlight",
		.max_brightness = 3,
		.id		= PMIC8058_ID_LED_KB_LIGHT,
	},
#endif
};

static struct pmic8058_leds_platform_data pm8058_flash_leds_data = {
	.num_leds = ARRAY_SIZE(pmic8058_flash_leds),
	.leds	= pmic8058_flash_leds,
};

static struct pmic8058_led pmic8058_fluid_flash_leds[] = {
	[0] = {
		.name		= "led:drv0",
		.max_brightness = 15,
		.id		= PMIC8058_ID_FLASH_LED_0,
	},/* 300 mA flash led0 drv sink */
	[1] = {
		.name		= "led:drv1",
		.max_brightness = 15,
		.id		= PMIC8058_ID_FLASH_LED_1,
	},/* 300 mA flash led1 sink */
	[2] = {
		.name		= "led:drv2",
		.max_brightness = 20,
		.id		= PMIC8058_ID_LED_0,
	},/* 40 mA led0 sink */
	[3] = {
		.name		= "keypad:drv",
		.max_brightness = 15,
		.id		= PMIC8058_ID_LED_KB_LIGHT,
	},/* 300 mA keypad drv sink */
};

static struct pmic8058_leds_platform_data pm8058_fluid_flash_leds_data = {
	.num_leds = ARRAY_SIZE(pmic8058_fluid_flash_leds),
	.leds	= pmic8058_fluid_flash_leds,
};

static struct resource resources_temp_alarm[] = {
	{
		.start  = PM8058_TEMP_ALARM_IRQ(PM8058_IRQ_BASE),
		.end    = PM8058_TEMP_ALARM_IRQ(PM8058_IRQ_BASE),
		.flags  = IORESOURCE_IRQ,
	},
};

static struct resource resources_pm8058_misc[] = {
	{
		.start  = PM8058_OSCHALT_IRQ(PM8058_IRQ_BASE),
		.end    = PM8058_OSCHALT_IRQ(PM8058_IRQ_BASE),
		.flags  = IORESOURCE_IRQ,
	},
};

static struct resource resources_pm8058_batt_alarm[] = {
	{
		.start  = PM8058_BATT_ALARM_IRQ(PM8058_IRQ_BASE),
		.end    = PM8058_BATT_ALARM_IRQ(PM8058_IRQ_BASE),
		.flags  = IORESOURCE_IRQ,
	},
};

#define PM8058_SUBDEV_KPD 0
#define PM8058_SUBDEV_LED 1

static struct mfd_cell pm8058_subdevs[] = {
	{
		.name = "pm8058-keypad",
		.id		= -1,
		.num_resources	= ARRAY_SIZE(resources_keypad),
		.resources	= resources_keypad,
	},
	{	.name = "pm8058-led",
		.id		= -1,
	},
	{	.name = "pm8058-gpio",
		.id		= -1,
		.platform_data	= &pm8058_gpio_data,
		.data_size	= sizeof(pm8058_gpio_data),
	},
	{	.name = "pm8058-mpp",
		.id		= -1,
		.platform_data	= &pm8058_mpp_data,
		.data_size	= sizeof(pm8058_mpp_data),
	},
	{	.name = "pm8058-pwrkey",
		.id	= -1,
		.resources = resources_pwrkey,
		.num_resources = ARRAY_SIZE(resources_pwrkey),
		.platform_data = &pwrkey_pdata,
		.data_size = sizeof(pwrkey_pdata),
	},
	{
		.name = "pm8058-vib",
		.id = -1,
		.platform_data = &pmic_vib_pdata,
		.data_size     = sizeof(pmic_vib_pdata),
	},
	{
		.name = "pm8058-pwm",
		.id = -1,
		.platform_data = &pm8058_pwm_data,
		.data_size = sizeof(pm8058_pwm_data),
	},
#ifdef CONFIG_SENSORS_MSM_ADC
	{
		.name = "pm8058-xoadc",
		.id = -1,
		.num_resources = ARRAY_SIZE(resources_adc),
		.resources = resources_adc,
		.platform_data = &xoadc_pdata,
		.data_size = sizeof(xoadc_pdata),
	},
#endif
#if defined(CONFIG_PMIC8058_OTHC) || defined(CONFIG_PMIC8058_OTHC_MODULE)
	{
		.name = "pm8058-othc",
		.id = 0,
		.platform_data = &othc_config_pdata_0,
		.data_size = sizeof(othc_config_pdata_0),
		.num_resources = ARRAY_SIZE(resources_othc_0),
		.resources = resources_othc_0,
	},
	{
		/* OTHC1 module has headset/switch dection */
		.name = "pm8058-othc",
		.id = 1,
		.num_resources = ARRAY_SIZE(resources_othc_1),
		.resources = resources_othc_1,
		.platform_data = &othc_config_pdata_1,
		.data_size = sizeof(othc_config_pdata_1),
	},
	{
		.name = "pm8058-othc",
		.id = 2,
		.platform_data = &othc_config_pdata_2,
		.data_size = sizeof(othc_config_pdata_2),
		.num_resources = ARRAY_SIZE(resources_othc_2),
		.resources = resources_othc_2,
	},
#endif
	{
		.name = "pm8058-rtc",
		.id = -1,
		.num_resources  = ARRAY_SIZE(resources_rtc),
		.resources      = resources_rtc,
		.platform_data = &pm8058_rtc_pdata,
		.data_size = sizeof(pm8058_rtc_pdata),
	},
	{
		.name = "pm8058-tm",
		.id = -1,
		.num_resources  = ARRAY_SIZE(resources_temp_alarm),
		.resources      = resources_temp_alarm,
	},
	{	.name = "pm8058-upl",
		.id		= -1,
	},
	{
		.name = "pm8058-misc",
		.id = -1,
		.num_resources  = ARRAY_SIZE(resources_pm8058_misc),
		.resources      = resources_pm8058_misc,
	},
	{	.name = "pm8058-batt-alarm",
		.id		= -1,
		.num_resources  = ARRAY_SIZE(resources_pm8058_batt_alarm),
		.resources      = resources_pm8058_batt_alarm,
	},
};

static struct mfd_cell pm8058_charger_sub_dev = {
		.name = "pm8058-charger",
		.id = -1,
		.num_resources = ARRAY_SIZE(resources_pm8058_charger),
		.resources = resources_pm8058_charger,
};

static struct pm8058_platform_data pm8058_platform_data = {
	.irq_base = PM8058_IRQ_BASE,

	.num_subdevs = ARRAY_SIZE(pm8058_subdevs),
	.sub_devices = pm8058_subdevs,
	.irq_trigger_flags = IRQF_TRIGGER_LOW,
};

static struct i2c_board_info pm8058_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO("pm8058-core", 0x55),
		.irq = MSM_GPIO_TO_INT(PM8058_GPIO_INT),
		.platform_data = &pm8058_platform_data,
	},
};
#endif /* CONFIG_PMIC8058 */

#if defined(CONFIG_TOUCHDISC_VTD518_SHINETSU) || \
		defined(CONFIG_TOUCHDISC_VTD518_SHINETSU_MODULE)
#define TDISC_I2C_SLAVE_ADDR	0x67
#define PMIC_GPIO_TDISC		PM8058_GPIO_PM_TO_SYS(5)
#define TDISC_INT		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 5)

static const char *vregs_tdisc_name[] = {
	"8058_l5",
	"8058_s3",
};

static const int vregs_tdisc_val[] = {
	2850000,/* uV */
	1800000,
};
static struct regulator *vregs_tdisc[ARRAY_SIZE(vregs_tdisc_name)];

static int tdisc_shinetsu_setup(void)
{
	int rc, i;

	rc = gpio_request(PMIC_GPIO_TDISC, "tdisc_interrupt");
	if (rc) {
		pr_err("%s: gpio_request failed for PMIC_GPIO_TDISC\n",
								__func__);
		return rc;
	}

	rc = gpio_request(GPIO_JOYSTICK_EN, "tdisc_oe");
	if (rc) {
		pr_err("%s: gpio_request failed for GPIO_JOYSTICK_EN\n",
							__func__);
		goto fail_gpio_oe;
	}

	rc = gpio_direction_output(GPIO_JOYSTICK_EN, 1);
	if (rc) {
		pr_err("%s: gpio_direction_output failed for GPIO_JOYSTICK_EN\n",
								__func__);
		gpio_free(GPIO_JOYSTICK_EN);
		goto fail_gpio_oe;
	}

	for (i = 0; i < ARRAY_SIZE(vregs_tdisc_name); i++) {
		vregs_tdisc[i] = regulator_get(NULL, vregs_tdisc_name[i]);
		if (IS_ERR(vregs_tdisc[i])) {
			printk(KERN_ERR "%s: regulator get %s failed (%ld)\n",
				__func__, vregs_tdisc_name[i],
				PTR_ERR(vregs_tdisc[i]));
			rc = PTR_ERR(vregs_tdisc[i]);
			goto vreg_get_fail;
		}

		rc = regulator_set_voltage(vregs_tdisc[i],
				vregs_tdisc_val[i], vregs_tdisc_val[i]);
		if (rc) {
			printk(KERN_ERR "%s: regulator_set_voltage() = %d\n",
				__func__, rc);
			goto vreg_set_voltage_fail;
		}
	}

	return rc;
vreg_set_voltage_fail:
	i++;
vreg_get_fail:
	while (i)
		regulator_put(vregs_tdisc[--i]);
fail_gpio_oe:
	gpio_free(PMIC_GPIO_TDISC);
	return rc;
}

static void tdisc_shinetsu_release(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vregs_tdisc_name); i++)
		regulator_put(vregs_tdisc[i]);

	gpio_free(PMIC_GPIO_TDISC);
	gpio_free(GPIO_JOYSTICK_EN);
}

static int tdisc_shinetsu_enable(void)
{
	int i, rc = -EINVAL;

	for (i = 0; i < ARRAY_SIZE(vregs_tdisc_name); i++) {
		rc = regulator_enable(vregs_tdisc[i]);
		if (rc < 0) {
			printk(KERN_ERR "%s: vreg %s enable failed (%d)\n",
				__func__, vregs_tdisc_name[i], rc);
			goto vreg_fail;
		}
	}

	/* Enable the OE (output enable) gpio */
	gpio_set_value_cansleep(GPIO_JOYSTICK_EN, 1);
	/* voltage and gpio stabilization delay */
	msleep(50);

	return 0;
vreg_fail:
	while (i)
		regulator_disable(vregs_tdisc[--i]);
	return rc;
}

static int tdisc_shinetsu_disable(void)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(vregs_tdisc_name); i++) {
		rc = regulator_disable(vregs_tdisc[i]);
		if (rc < 0) {
			printk(KERN_ERR "%s: vreg %s disable failed (%d)\n",
				__func__, vregs_tdisc_name[i], rc);
			goto tdisc_reg_fail;
		}
	}

	/* Disable the OE (output enable) gpio */
	gpio_set_value_cansleep(GPIO_JOYSTICK_EN, 0);

	return 0;

tdisc_reg_fail:
	while (i)
		regulator_enable(vregs_tdisc[--i]);
	return rc;
}

static struct tdisc_abs_values tdisc_abs = {
	.x_max = 32,
	.y_max = 32,
	.x_min = -32,
	.y_min = -32,
	.pressure_max = 32,
	.pressure_min = 0,
};

static struct tdisc_platform_data tdisc_data = {
	.tdisc_setup = tdisc_shinetsu_setup,
	.tdisc_release = tdisc_shinetsu_release,
	.tdisc_enable = tdisc_shinetsu_enable,
	.tdisc_disable = tdisc_shinetsu_disable,
	.tdisc_wakeup  = 0,
	.tdisc_gpio = PMIC_GPIO_TDISC,
	.tdisc_report_keys = true,
	.tdisc_report_relative = true,
	.tdisc_report_absolute = false,
	.tdisc_report_wheel = false,
	.tdisc_reverse_x = false,
	.tdisc_reverse_y = true,
	.tdisc_abs  = &tdisc_abs,
};

static struct i2c_board_info msm_i2c_gsbi3_tdisc_info[] = {
	{
		I2C_BOARD_INFO("vtd518", TDISC_I2C_SLAVE_ADDR),
		.irq =  TDISC_INT,
		.platform_data = &tdisc_data,
	},
};
#endif

#define PM_GPIO_CDC_RST_N 20
#define GPIO_CDC_RST_N PM8058_GPIO_PM_TO_SYS(PM_GPIO_CDC_RST_N)

static struct regulator *vreg_timpani_1;
static struct regulator *vreg_timpani_2;

static unsigned int msm_timpani_setup_power(void)
{
	int rc;

	vreg_timpani_1 = regulator_get(NULL, "8058_l0");
	if (IS_ERR(vreg_timpani_1)) {
		pr_err("%s: Unable to get 8058_l0\n", __func__);
		return -ENODEV;
	}

	vreg_timpani_2 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(vreg_timpani_2)) {
		pr_err("%s: Unable to get 8058_s3\n", __func__);
		regulator_put(vreg_timpani_1);
		return -ENODEV;
	}

	rc = regulator_set_voltage(vreg_timpani_1, 1200000, 1200000);
	if (rc) {
		pr_err("%s: unable to set L0 voltage to 1.2V\n", __func__);
		goto fail;
	}

	rc = regulator_set_voltage(vreg_timpani_2, 1800000, 1800000);
	if (rc) {
		pr_err("%s: unable to set S3 voltage to 1.8V\n", __func__);
		goto fail;
	}

	rc = regulator_enable(vreg_timpani_1);
	if (rc) {
		pr_err("%s: Enable regulator 8058_l0 failed\n", __func__);
		goto fail;
	}

	/* The settings for LDO0 should be set such that
	*  it doesn't require to reset the timpani. */
	rc = regulator_set_optimum_mode(vreg_timpani_1, 5000);
	if (rc < 0) {
		pr_err("Timpani regulator optimum mode setting failed\n");
		goto fail;
	}

	rc = regulator_enable(vreg_timpani_2);
	if (rc) {
		pr_err("%s: Enable regulator 8058_s3 failed\n", __func__);
		regulator_disable(vreg_timpani_1);
		goto fail;
	}

	rc = gpio_request(GPIO_CDC_RST_N, "CDC_RST_N");
	if (rc) {
		pr_err("%s: GPIO Request %d failed\n", __func__,
			GPIO_CDC_RST_N);
		regulator_disable(vreg_timpani_1);
		regulator_disable(vreg_timpani_2);
		goto fail;
	} else {
		gpio_direction_output(GPIO_CDC_RST_N, 1);
		usleep_range(1000, 1050);
		gpio_direction_output(GPIO_CDC_RST_N, 0);
		usleep_range(1000, 1050);
		gpio_direction_output(GPIO_CDC_RST_N, 1);
		gpio_free(GPIO_CDC_RST_N);
	}
	return rc;

fail:
	regulator_put(vreg_timpani_1);
	regulator_put(vreg_timpani_2);
	return rc;
}

static void msm_timpani_shutdown_power(void)
{
	int rc;

	rc = regulator_disable(vreg_timpani_1);
	if (rc)
		pr_err("%s: Disable regulator 8058_l0 failed\n", __func__);

	regulator_put(vreg_timpani_1);

	rc = regulator_disable(vreg_timpani_2);
	if (rc)
		pr_err("%s: Disable regulator 8058_s3 failed\n", __func__);

	regulator_put(vreg_timpani_2);
}

/* Power analog function of codec */
static struct regulator *vreg_timpani_cdc_apwr;
static int msm_timpani_codec_power(int vreg_on)
{
	int rc = 0;

	if (!vreg_timpani_cdc_apwr) {

		vreg_timpani_cdc_apwr = regulator_get(NULL, "8058_s4");

		if (IS_ERR(vreg_timpani_cdc_apwr)) {
			pr_err("%s: vreg_get failed (%ld)\n",
			__func__, PTR_ERR(vreg_timpani_cdc_apwr));
			rc = PTR_ERR(vreg_timpani_cdc_apwr);
			return rc;
		}
	}

	if (vreg_on) {

		rc = regulator_set_voltage(vreg_timpani_cdc_apwr,
				2200000, 2200000);
		if (rc) {
			pr_err("%s: unable to set 8058_s4 voltage to 2.2 V\n",
					__func__);
			goto vreg_fail;
		}

		rc = regulator_enable(vreg_timpani_cdc_apwr);
		if (rc) {
			pr_err("%s: vreg_enable failed %d\n", __func__, rc);
			goto vreg_fail;
		}
	} else {
		rc = regulator_disable(vreg_timpani_cdc_apwr);
		if (rc) {
			pr_err("%s: vreg_disable failed %d\n",
			__func__, rc);
			goto vreg_fail;
		}
	}

	return 0;

vreg_fail:
	regulator_put(vreg_timpani_cdc_apwr);
	vreg_timpani_cdc_apwr = NULL;
	return rc;
}

static struct marimba_codec_platform_data timpani_codec_pdata = {
	.marimba_codec_power =  msm_timpani_codec_power,
};

#define TIMPANI_SLAVE_ID_CDC_ADDR		0X77
#define TIMPANI_SLAVE_ID_QMEMBIST_ADDR		0X66

static struct marimba_platform_data timpani_pdata = {
	.slave_id[MARIMBA_SLAVE_ID_CDC]	= TIMPANI_SLAVE_ID_CDC_ADDR,
	.slave_id[MARIMBA_SLAVE_ID_QMEMBIST] = TIMPANI_SLAVE_ID_QMEMBIST_ADDR,
	.marimba_setup = msm_timpani_setup_power,
	.marimba_shutdown = msm_timpani_shutdown_power,
	.codec = &timpani_codec_pdata,
};

#define TIMPANI_I2C_SLAVE_ADDR	0xD

static struct i2c_board_info msm_i2c_gsbi7_timpani_info[] = {
	{
		I2C_BOARD_INFO("timpani", TIMPANI_I2C_SLAVE_ADDR),
		.platform_data = &timpani_pdata,
	},
};

#ifdef CONFIG_PMIC8901

#define PM8901_GPIO_INT           91

static struct pm8901_gpio_platform_data pm8901_mpp_data = {
	.gpio_base	= PM8901_GPIO_PM_TO_SYS(0),
	.irq_base	= PM8901_MPP_IRQ(PM8901_IRQ_BASE, 0),
};

static struct resource pm8901_temp_alarm[] = {
	{
		.start = PM8901_TEMP_ALARM_IRQ(PM8901_IRQ_BASE),
		.end = PM8901_TEMP_ALARM_IRQ(PM8901_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = PM8901_TEMP_HI_ALARM_IRQ(PM8901_IRQ_BASE),
		.end = PM8901_TEMP_HI_ALARM_IRQ(PM8901_IRQ_BASE),
		.flags = IORESOURCE_IRQ,
	},
};

static struct regulator_consumer_supply pm8901_vreg_supply[PM8901_VREG_MAX] = {
	[PM8901_VREG_ID_MPP0] =     REGULATOR_SUPPLY("8901_mpp0",     NULL),
	[PM8901_VREG_ID_USB_OTG]  = REGULATOR_SUPPLY("8901_usb_otg",  NULL),
	[PM8901_VREG_ID_HDMI_MVS] = REGULATOR_SUPPLY("8901_hdmi_mvs", NULL),
};

#define PM8901_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
			 _always_on, _active_high) \
	[_id] = { \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask = _modes, \
				.valid_ops_mask = _ops, \
				.min_uV = _min_uV, \
				.max_uV = _max_uV, \
				.input_uV = _min_uV, \
				.apply_uV = _apply_uV, \
				.always_on = _always_on, \
			}, \
			.num_consumer_supplies = 1, \
			.consumer_supplies = &pm8901_vreg_supply[_id], \
		}, \
		.active_high = _active_high, \
	}

#define PM8901_VREG_INIT_MPP(_id, _active_high) \
	PM8901_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL, \
			REGULATOR_CHANGE_STATUS, 0, 0, _active_high)

#define PM8901_VREG_INIT_VS(_id) \
	PM8901_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL, \
			REGULATOR_CHANGE_STATUS, 0, 0, 0)

static struct pm8901_vreg_pdata pm8901_vreg_init_pdata[PM8901_VREG_MAX] = {
	PM8901_VREG_INIT_MPP(PM8901_VREG_ID_MPP0, 1),

	PM8901_VREG_INIT_VS(PM8901_VREG_ID_USB_OTG),
	PM8901_VREG_INIT_VS(PM8901_VREG_ID_HDMI_MVS),
};

#define PM8901_VREG(_id) { \
	.name = "pm8901-regulator", \
	.id = _id, \
	.platform_data = &pm8901_vreg_init_pdata[_id], \
	.data_size = sizeof(pm8901_vreg_init_pdata[_id]), \
}

static struct mfd_cell pm8901_subdevs[] = {
	{	.name = "pm8901-mpp",
		.id		= -1,
		.platform_data	= &pm8901_mpp_data,
		.data_size	= sizeof(pm8901_mpp_data),
	},
	{	.name = "pm8901-tm",
		.id		= -1,
		.num_resources  = ARRAY_SIZE(pm8901_temp_alarm),
		.resources      = pm8901_temp_alarm,
	},
	PM8901_VREG(PM8901_VREG_ID_MPP0),
	PM8901_VREG(PM8901_VREG_ID_USB_OTG),
	PM8901_VREG(PM8901_VREG_ID_HDMI_MVS),
};

static struct pm8901_platform_data pm8901_platform_data = {
	.irq_base = PM8901_IRQ_BASE,
	.num_subdevs = ARRAY_SIZE(pm8901_subdevs),
	.sub_devices = pm8901_subdevs,
	.irq_trigger_flags = IRQF_TRIGGER_LOW,
};

static struct i2c_board_info pm8901_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO("pm8901-core", 0x55),
		.irq = MSM_GPIO_TO_INT(PM8901_GPIO_INT),
		.platform_data = &pm8901_platform_data,
	},
};

#endif /* CONFIG_PMIC8901 */

#if defined(CONFIG_MARIMBA_CORE) && (defined(CONFIG_GPIO_SX150X) \
	|| defined(CONFIG_GPIO_SX150X_MODULE))

static struct regulator *vreg_bahama;
#ifdef CONFIG_MACH_MSM8X60_PRESTO   // jmlee presto ws10 qtr i2c power is not QC reference
#if (BOARD_REV <= WS10)
static struct regulator *vreg_bahama_pantech;
#endif
#endif

struct bahama_config_register{
	u8 reg;
	u8 value;
	u8 mask;
};

enum version{
	VER_1_0,
	VER_2_0,
	VER_UNSUPPORTED = 0xFF
};

static u8 read_bahama_ver(void)
{
	int rc;
	struct marimba config = { .mod_id = SLAVE_ID_BAHAMA };
	u8 bahama_version;

	rc = marimba_read_bit_mask(&config, 0x00,  &bahama_version, 1, 0x1F);
	if (rc < 0) {
		printk(KERN_ERR
			 "%s: version read failed: %d\n",
			__func__, rc);
			return VER_UNSUPPORTED;
	} else {
		printk(KERN_INFO
		"%s: version read got: 0x%x\n",
		__func__, bahama_version);
	}

	switch (bahama_version) {
	case 0x08: /* varient of bahama v1 */
	case 0x10:
	case 0x00:
		return VER_1_0;
	case 0x09: /* variant of bahama v2 */
		return VER_2_0;
	default:
		return VER_UNSUPPORTED;
	}
}

static unsigned int msm_bahama_setup_power(void)
{
	int rc = 0;
	const char *msm_bahama_regulator = "8058_s3";
#ifdef CONFIG_MACH_MSM8X60_PRESTO   // jmlee presto ws10 qtr i2c power is not QC reference
#if (BOARD_REV <= WS10)
	const char *msm_bahama_regulator_pantech = "8058_l8";

	vreg_bahama_pantech = regulator_get(NULL, msm_bahama_regulator_pantech);
//printk(KERN_ERR "%s:[JMLEE LOG] msm_bahama_setup_power 111@@@\n", __func__);

	if (IS_ERR(vreg_bahama_pantech)) {
		rc = PTR_ERR(vreg_bahama_pantech);
		pr_err("%s: regulator_get %s = %d\n", __func__,
			msm_bahama_regulator_pantech, rc);
	}

	if (!rc)
		rc = regulator_set_voltage(vreg_bahama_pantech, 1800000, 1800000);
	else {
		pr_err("%s: regulator_set_voltage %s = %d\n", __func__,
			msm_bahama_regulator_pantech, rc);
		goto unget;
	}
//printk(KERN_ERR "%s:[JMLEE LOG] msm_bahama_setup_power 2222@@@\n", __func__);

	if (!rc)
		rc = regulator_enable(vreg_bahama_pantech);
	else {
		pr_err("%s: regulator_enable %s = %d\n", __func__,
			msm_bahama_regulator_pantech, rc);
		goto unget;
	}
#endif // #if (BOARD_REV <= WS10)
#endif
	vreg_bahama = regulator_get(NULL, msm_bahama_regulator);

	if (IS_ERR(vreg_bahama)) {
		rc = PTR_ERR(vreg_bahama);
		pr_err("%s: regulator_get %s = %d\n", __func__,
			msm_bahama_regulator, rc);
	}

	if (!rc)
		rc = regulator_set_voltage(vreg_bahama, 1800000, 1800000);
	else {
		pr_err("%s: regulator_set_voltage %s = %d\n", __func__,
			msm_bahama_regulator, rc);
		goto unget;
	}

	if (!rc)
		rc = regulator_enable(vreg_bahama);
	else {
		pr_err("%s: regulator_enable %s = %d\n", __func__,
			msm_bahama_regulator, rc);
		goto unget;
	}

	if (!rc)
		rc = gpio_request(GPIO_MS_SYS_RESET_N, "bahama sys_rst_n");
	else {
		pr_err("%s: gpio_request %d = %d\n", __func__,
			GPIO_MS_SYS_RESET_N, rc);
		goto unenable;
	}

	if (!rc) {
		gpio_direction_output(GPIO_MS_SYS_RESET_N, 0);
		usleep_range(1000, 1050);
		gpio_direction_output(GPIO_MS_SYS_RESET_N, 1);
		usleep_range(1000, 1050);
	} else {
		pr_err("%s: gpio_direction_output %d = %d\n", __func__,
			GPIO_MS_SYS_RESET_N, rc);
		goto unrequest;
	}

	return rc;

unrequest:
	gpio_free(GPIO_MS_SYS_RESET_N);
unenable:
	regulator_disable(vreg_bahama);
unget:
	regulator_put(vreg_bahama);
	return rc;
};
static unsigned int msm_bahama_shutdown_power(int value)


{
//20110323 choiseulkee chg for EF39S bring-up
#ifndef CONFIG_PANTECH
	gpio_set_value_cansleep(GPIO_MS_SYS_RESET_N, 0);

	gpio_free(GPIO_MS_SYS_RESET_N);

	regulator_disable(vreg_bahama);

	regulator_put(vreg_bahama);
#endif

	return 0;
};

static unsigned int msm_bahama_core_config(int type)
{
	int rc = 0;

	if (type == BAHAMA_ID) {

		int i;
		struct marimba config = { .mod_id = SLAVE_ID_BAHAMA };

		const struct bahama_config_register v20_init[] = {
			/* reg, value, mask */
			{ 0xF4, 0x84, 0xFF }, /* AREG */
			{ 0xF0, 0x04, 0xFF } /* DREG */
		};

		if (read_bahama_ver() == VER_2_0) {
			for (i = 0; i < ARRAY_SIZE(v20_init); i++) {
				u8 value = v20_init[i].value;
				rc = marimba_write_bit_mask(&config,
					v20_init[i].reg,
					&value,
					sizeof(v20_init[i].value),
					v20_init[i].mask);
				if (rc < 0) {
					printk(KERN_ERR
						"%s: reg %d write failed: %d\n",
						__func__, v20_init[i].reg, rc);
					return rc;
				}
				printk(KERN_INFO "%s: reg 0x%02x value 0x%02x"
					" mask 0x%02x\n",
					__func__, v20_init[i].reg,
					v20_init[i].value, v20_init[i].mask);
			}
		}
	}
	printk(KERN_INFO "core type: %d\n", type);

	return rc;
}

static struct regulator *fm_regulator_s3;
static struct msm_xo_voter *fm_clock;

static int fm_radio_setup(struct marimba_fm_platform_data *pdata)
{
	int rc = 0;
	struct pm8058_gpio cfg = {
				.direction      = PM_GPIO_DIR_IN,
				.pull           = PM_GPIO_PULL_NO,
				.vin_sel        = PM_GPIO_VIN_S3,
				.function       = PM_GPIO_FUNC_NORMAL,
				.inv_int_pol    = 0,
				};

	if (!fm_regulator_s3) {
		fm_regulator_s3 = regulator_get(NULL, "8058_s3");
		if (IS_ERR(fm_regulator_s3)) {
			rc = PTR_ERR(fm_regulator_s3);
			printk(KERN_ERR "%s: regulator get s3 (%d)\n",
				__func__, rc);
			goto out;
			}
	}


	rc = regulator_set_voltage(fm_regulator_s3, 1800000, 1800000);
	if (rc < 0) {
		printk(KERN_ERR "%s: regulator set voltage failed (%d)\n",
				__func__, rc);
		goto fm_fail_put;
	}

	rc = regulator_enable(fm_regulator_s3);
	if (rc < 0) {
		printk(KERN_ERR "%s: regulator s3 enable failed (%d)\n",
					__func__, rc);
		goto fm_fail_put;
	}

	/*Vote for XO clock*/
	fm_clock = msm_xo_get(MSM_XO_TCXO_D0, "fm_power");

	if (IS_ERR(fm_clock)) {
		rc = PTR_ERR(fm_clock);
		printk(KERN_ERR "%s: Couldn't get TCXO_D0 vote for FM (%d)\n",
					__func__, rc);
		goto fm_fail_switch;
	}

	rc = msm_xo_mode_vote(fm_clock, MSM_XO_MODE_ON);
	if (rc < 0) {
		printk(KERN_ERR "%s:  Failed to vote for TCX0_D0 ON (%d)\n",
					__func__, rc);
		goto fm_fail_vote;
	}

	/*GPIO 18 on PMIC is FM_IRQ*/
	rc = pm8058_gpio_config(FM_GPIO, &cfg);
	if (rc) {
		printk(KERN_ERR "%s: return val of pm8058_gpio_config: %d\n",
						__func__,  rc);
		goto fm_fail_clock;
	}
	goto out;

fm_fail_clock:
		msm_xo_mode_vote(fm_clock, MSM_XO_MODE_OFF);
fm_fail_vote:
		msm_xo_put(fm_clock);
fm_fail_switch:
		regulator_disable(fm_regulator_s3);
fm_fail_put:
		regulator_put(fm_regulator_s3);
out:
	return rc;
};

static void fm_radio_shutdown(struct marimba_fm_platform_data *pdata)
{
	int rc = 0;
	if (fm_regulator_s3 != NULL) {
		rc = regulator_disable(fm_regulator_s3);
		if (rc < 0) {
			printk(KERN_ERR "%s: regulator s3 disable (%d)\n",
							__func__, rc);
		}
		regulator_put(fm_regulator_s3);
		fm_regulator_s3 = NULL;
	}
	printk(KERN_ERR "%s: Voting off for XO", __func__);

	if (fm_clock != NULL) {
		rc = msm_xo_mode_vote(fm_clock, MSM_XO_MODE_OFF);
		if (rc < 0) {
			printk(KERN_ERR "%s: Voting off XO clock (%d)\n",
					__func__, rc);
		}
		msm_xo_put(fm_clock);
	}
	printk(KERN_ERR "%s: coming out of fm_radio_shutdown", __func__);
}

/* Slave id address for FM/CDC/QMEMBIST
 * Values can be programmed using Marimba slave id 0
 * should there be a conflict with other I2C devices
 * */
#define BAHAMA_SLAVE_ID_FM_ADDR         0x2A
#define BAHAMA_SLAVE_ID_QMEMBIST_ADDR   0x7B

static struct marimba_fm_platform_data marimba_fm_pdata = {
	.fm_setup =  fm_radio_setup,
	.fm_shutdown = fm_radio_shutdown,
	.irq = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, FM_GPIO),
	.is_fm_soc_i2s_master = false,
};

/*
Just initializing the BAHAMA related slave
*/
static struct marimba_platform_data marimba_pdata = {
	.slave_id[SLAVE_ID_BAHAMA_FM]        = BAHAMA_SLAVE_ID_FM_ADDR,
	.slave_id[SLAVE_ID_BAHAMA_QMEMBIST]  = BAHAMA_SLAVE_ID_QMEMBIST_ADDR,
	.bahama_setup = msm_bahama_setup_power,
	.bahama_shutdown = msm_bahama_shutdown_power,
	.bahama_core_config = msm_bahama_core_config,
	.fm = &marimba_fm_pdata,
};


static struct i2c_board_info msm_marimba_board_info[] = {
	{
		I2C_BOARD_INFO("marimba", 0xc),
		.platform_data = &marimba_pdata,
	}
};
#endif /* CONFIG_MAIMBA_CORE */

#ifdef CONFIG_I2C
#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_FLUID (1 << 4)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

static struct i2c_registry msm8x60_i2c_devices[] __initdata = {
#ifdef CONFIG_PMIC8058
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_SSBI1_I2C_BUS_ID,
		pm8058_boardinfo,
		ARRAY_SIZE(pm8058_boardinfo),
	},
#endif
#ifdef CONFIG_PMIC8901
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_SSBI2_I2C_BUS_ID,
		pm8901_boardinfo,
		ARRAY_SIZE(pm8901_boardinfo),
	},
#endif
#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		core_expander_i2c_info,
		ARRAY_SIZE(core_expander_i2c_info),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		docking_expander_i2c_info,
		ARRAY_SIZE(docking_expander_i2c_info),
	},
	{
		I2C_SURF,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		surf_expanders_i2c_info,
		ARRAY_SIZE(surf_expanders_i2c_info),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		fha_expanders_i2c_info,
		ARRAY_SIZE(fha_expanders_i2c_info),
	},
	{
		I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		fluid_expanders_i2c_info,
		ARRAY_SIZE(fluid_expanders_i2c_info),
	},
	{
		I2C_FLUID,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		fluid_core_expander_i2c_info,
		ARRAY_SIZE(fluid_core_expander_i2c_info),
	},
#endif
#if defined(CONFIG_TOUCHDISC_VTD518_SHINETSU) || \
		defined(CONFIG_TOUCHDISC_VTD518_SHINETSU_MODULE)
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		msm_i2c_gsbi3_tdisc_info,
		ARRAY_SIZE(msm_i2c_gsbi3_tdisc_info),
	},
#endif
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		cy8ctmg200_board_info,
		ARRAY_SIZE(cy8ctmg200_board_info),
	},
#if defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C) || \
		defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C_MODULE)
	{
		I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		cyttsp_fluid_info,
		ARRAY_SIZE(cyttsp_fluid_info),
	},
	{
		I2C_FFA | I2C_SURF,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		cyttsp_ffa_info,
		ARRAY_SIZE(cyttsp_ffa_info),
	},	
#endif
#if defined(CONFIG_TOUCHSCREEN_QT602240) 
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		qt602240_i2c_boardinfo,
		ARRAY_SIZE(qt602240_i2c_boardinfo),
	},
#endif
#ifdef CONFIG_MSM_CAMERA
    {
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI4_QUP_I2C_BUS_ID,
		msm_camera_boardinfo,
		ARRAY_SIZE(msm_camera_boardinfo),
	},
#endif
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_timpani_info,
		ARRAY_SIZE(msm_i2c_gsbi7_timpani_info),
	},
#if defined(CONFIG_MARIMBA_CORE)
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_marimba_board_info,
		ARRAY_SIZE(msm_marimba_board_info),
	},
#endif /* CONFIG_MARIMBA_CORE */
#ifdef CONFIG_ISL9519_CHARGER
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		isl_charger_i2c_info,
		ARRAY_SIZE(isl_charger_i2c_info),
	},
#endif
#if defined(CONFIG_HAPTIC_ISA1200) || \
		defined(CONFIG_HAPTIC_ISA1200_MODULE)
	{
		I2C_FLUID,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		msm_isa1200_board_info,
		ARRAY_SIZE(msm_isa1200_board_info),
	},
#endif
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	{
		I2C_SURF | I2C_FFA,
        MSM_GSBI5_QUP_I2C_BUS_ID,
		gyroscope_i2c_info,
		ARRAY_SIZE(gyroscope_i2c_info),
	},
#endif
#endif
#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
	{
#ifdef CONFIG_SKY_SMB_CHARGER
		I2C_SURF | I2C_FFA | I2C_FLUID,
                MSM_SMB_I2C_BUS_ID,
#else
		I2C_FLUID,
		MSM_GSBI8_QUP_I2C_BUS_ID,
#endif
		smb137b_charger_i2c_info,
		ARRAY_SIZE(smb137b_charger_i2c_info),
	},
#endif

#ifdef CONFIG_SKY_BATTERY_MAX17040 //p14682 kobj 110607PS2 TEAM shs : fuel gauge porting
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI11_QUP_I2C_BUS_ID,
		max17040_i2c_boardinfo,
		ARRAY_SIZE(max17040_i2c_boardinfo),
	},
#endif //CONFIG_SKY_BATTERY_MAX17040
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200  // jmlee 20110505 add
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		msm_audio_i2c_board_info,
		ARRAY_SIZE(msm_audio_i2c_board_info),
	},			
#endif

#if defined(CONFIG_BATTERY_BQ27520) || \
		defined(CONFIG_BATTERY_BQ27520_MODULE)
	{
		I2C_FLUID,
		MSM_GSBI8_QUP_I2C_BUS_ID,
		msm_bq27520_board_info,
		ARRAY_SIZE(msm_bq27520_board_info),
	},
#endif
#if defined(CONFIG_PN544)
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI10_QUP_I2C_BUS_ID,
		nfc_i2c_boardinfo,
		ARRAY_SIZE(nfc_i2c_boardinfo),
	},
#endif
#ifdef CONFIG_SKY_SND_EXTAMP 	/* elecjang 20110421 for max97001 subsystem */
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		i2c_subsystem_devices,
		ARRAY_SIZE(i2c_subsystem_devices),
	},
#endif

};
#endif /* CONFIG_I2C */

static void fixup_i2c_configs(void)
{
#ifdef CONFIG_I2C
#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_charm_surf())
		sx150x_data[SX150X_CORE].irq_summary =
			PM8058_GPIO_IRQ(PM8058_IRQ_BASE, UI_INT2_N);
	else if (machine_is_msm8x60_ffa() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
					)
		sx150x_data[SX150X_CORE].irq_summary =
			PM8058_GPIO_IRQ(PM8058_IRQ_BASE, UI_INT1_N);
	else if (machine_is_msm8x60_fluid())
		sx150x_data[SX150X_CORE_FLUID].irq_summary =
			PM8058_GPIO_IRQ(PM8058_IRQ_BASE, UI_INT1_N);
#endif
	/*
	 * Set PMIC 8901 MPP0 active_high to 0 for surf and charm_surf. This
	 * implies that the regulator connected to MPP0 is enabled when
	 * MPP0 is low.
	 */
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_charm_surf())
		pm8901_vreg_init_pdata[PM8901_VREG_ID_MPP0].active_high = 0;
	else
		pm8901_vreg_init_pdata[PM8901_VREG_ID_MPP0].active_high = 1;
#endif
}

static void register_i2c_devices(void)
{
#ifdef CONFIG_I2C
	u8 mach_mask = 0;
	int i;

	/* Build the matching 'supported_machs' bitmask */
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_charm_surf())
		mach_mask = I2C_SURF;
	else if (machine_is_msm8x60_ffa() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
					)
		mach_mask = I2C_FFA;
	else if (machine_is_msm8x60_rumi3())
		mach_mask = I2C_RUMI;
	else if (machine_is_msm8x60_sim())
		mach_mask = I2C_SIM;
	else if (machine_is_msm8x60_fluid())
		mach_mask = I2C_FLUID;
	else
		pr_err("unmatched machine ID in register_i2c_devices\n");

	/* Run the array and install devices as appropriate */
	for (i = 0; i < ARRAY_SIZE(msm8x60_i2c_devices); ++i) {
		if (msm8x60_i2c_devices[i].machs & mach_mask)
			i2c_register_board_info(msm8x60_i2c_devices[i].bus,
						msm8x60_i2c_devices[i].info,
						msm8x60_i2c_devices[i].len);
	}
#endif
}


//20101015 choiseulkee add for uart console, GSBI12 port UART GPIO config
#ifdef CONFIG_SKY_GSBI12_UART_CONSOLE
static uint32_t uart12_config_gpio[] = {
	GPIO_CFG(117, 2, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(118, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
};
#endif

static void __init msm8x60_init_uart12dm(void)
{
//20101015 choiseulkee add for uart console, GSBI12 port UART GPIO config
#ifdef CONFIG_SKY_GSBI12_UART_CONSOLE
	pr_info("%s: GSBI12 port uart12_config_gpio configuration\n", __func__);
	gpio_tlmm_config(uart12_config_gpio[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(uart12_config_gpio[1], GPIO_CFG_ENABLE);
#else
#if !defined(CONFIG_USB_PEHCI_HCD) && !defined(CONFIG_USB_PEHCI_HCD_MODULE)
	/* 0x1D000000 now belongs to EBI2:CS3 i.e. USB ISP Controller */
	void *fpga_mem = ioremap_nocache(0x1D000000, SZ_4K);
	/* Advanced mode */
	writew(0xFFFF, fpga_mem + 0x15C);
	/* FPGA_UART_SEL */
	writew(0, fpga_mem + 0x172);
	/* FPGA_GPIO_CONFIG_117 */
	writew(1, fpga_mem + 0xEA);
	/* FPGA_GPIO_CONFIG_118 */
	writew(1, fpga_mem + 0xEC);
	dsb();
	iounmap(fpga_mem);
#endif
#endif /* CONFIG_SKY_GSBI12_UART_CONSOLE */
}

#define MSM_GSBI9_PHYS		0x19900000
#define GSBI_DUAL_MODE_CODE	0x60

static void __init msm8x60_init_buses(void)
{
#ifdef CONFIG_I2C_QUP
	void *gsbi_mem = ioremap_nocache(0x19C00000, 4);
	/* Setting protocol code to 0x60 for dual UART/I2C in GSBI12 */
	writel_relaxed(0x6 << 4, gsbi_mem);
	/* Ensure protocol code is written before proceeding further */
	dsb();
	iounmap(gsbi_mem);

	msm_gsbi3_qup_i2c_device.dev.platform_data = &msm_gsbi3_qup_i2c_pdata;
	msm_gsbi4_qup_i2c_device.dev.platform_data = &msm_gsbi4_qup_i2c_pdata;
	msm_gsbi7_qup_i2c_device.dev.platform_data = &msm_gsbi7_qup_i2c_pdata;
	msm_gsbi8_qup_i2c_device.dev.platform_data = &msm_gsbi8_qup_i2c_pdata;

#ifdef CONFIG_MSM_GSBI9_UART
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) {
		/* Setting protocol code to 0x60 for dual UART/I2C in GSBI9 */
		gsbi_mem = ioremap_nocache(MSM_GSBI9_PHYS, 4);
		writel(GSBI_DUAL_MODE_CODE, gsbi_mem);
		iounmap(gsbi_mem);
		msm_gsbi9_qup_i2c_pdata.use_gsbi_shared_mode = 1;
	}
#endif
	msm_gsbi9_qup_i2c_device.dev.platform_data = &msm_gsbi9_qup_i2c_pdata;
#ifdef CONFIG_SKY_BATTERY_MAX17040 //p14682 kobj 110607 PS2 TEAM SHS : fuel gauge porting
	msm_gsbi11_qup_i2c_device.dev.platform_data = &msm_gsbi11_qup_i2c_pdata;
#endif //CONFIG_SKY_BATTERY_MAX17040
#if defined(CONFIG_PN544)
	msm_gsbi10_qup_i2c_device.dev.platform_data = &msm_gsbi10_qup_i2c_pdata;
#endif
	msm_gsbi12_qup_i2c_device.dev.platform_data = &msm_gsbi12_qup_i2c_pdata;

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	msm_gsbi5_qup_i2c_device.dev.platform_data = &msm_gsbi5_qup_i2c_pdata;
#endif
#else
#ifdef CONFIG_SKY_SMB_CHARGER
	msm_gsbi5_qup_i2c_device.dev.platform_data = &msm_gsbi5_qup_i2c_pdata;
#endif
#endif
#endif
#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
	msm_gsbi1_qup_spi_device.dev.platform_data = &msm_gsbi1_qup_spi_pdata;
#endif
#if defined(CONFIG_SKY_TDMB_SPI_QUP)
	msm_gsbi2_qup_spi_device.dev.platform_data = &msm_gsbi2_qup_spi_pdata;
#endif
//#ifdef CONFIG_SKY_TDMB_SPI_GPIO
//	gpio_spi_tdmb_device.dev.platform_data = &gpio_spi_tdmb_data;
//#endif

#ifdef CONFIG_I2C_SSBI
	msm_device_ssbi1.dev.platform_data = &msm_ssbi1_pdata;
	msm_device_ssbi2.dev.platform_data = &msm_ssbi2_pdata;
	msm_device_ssbi3.dev.platform_data = &msm_ssbi3_pdata;
#endif

	if (machine_is_msm8x60_fluid()) {
#if (defined(CONFIG_USB_EHCI_MSM_72K) && \
	(defined(CONFIG_SMB137B_CHARGER) || \
	 defined(CONFIG_SMB137B_CHARGER_MODULE)))
		msm_otg_pdata.vbus_power = msm_hsusb_smb137b_vbus_power;
#endif
#ifndef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020  // jmlee 20110505 add
#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
		msm_gsbi10_qup_spi_device.dev.platform_data =
					&msm_gsbi10_qup_spi_pdata;
#endif
#endif
	}

#if defined(CONFIG_USB_GADGET_MSM_72K) || defined(CONFIG_USB_EHCI_HCD)
	/*
	 * We can not put USB regulators (8058_l6 and 8058_l7) in LPM
	 * when we depend on USB PHY for VBUS/ID notifications. VBUS
	 * and ID notifications are available only on V2 surf and FFA
	 * with a hardware workaround.
	 */
	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2 &&
			(machine_is_msm8x60_surf() ||
			(machine_is_msm8x60_ffa() &&
			pmic_id_notif_supported)))
		msm_otg_pdata.phy_can_powercollapse = 1;
	msm_device_otg.dev.platform_data = &msm_otg_pdata;
#endif

#ifdef CONFIG_USB_GADGET_MSM_72K
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
#endif

#if defined(CONFIG_USB_ANDROID_ACM_SMD) || defined(CONFIG_USB_ANDROID_ACM_SDIO)
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) {
		struct usb_gadget_facm_pdata *facm_pdata =
			usb_gadget_facm_device.dev.platform_data;

		/* for charm: Port1: SDIO - DUN,  Port2: TTY - NMEA
		 * for svlte-2: Port1: SDIO - DUN1,  Port2: SMD - DUN2
		 */
		if (socinfo_get_platform_subtype() == 3) {
			facm_pdata->transport[0] =
				USB_GADGET_FSERIAL_TRANSPORT_SDIO;
			facm_pdata->transport[1] =
				USB_GADGET_FSERIAL_TRANSPORT_SMD;
		} else {
			facm_pdata->transport[0] =
				USB_GADGET_FSERIAL_TRANSPORT_SDIO;
			facm_pdata->transport[1] =
				USB_GADGET_FSERIAL_TRANSPORT_TTY;
		}
	}
#endif
#if defined(CONFIG_USB_F_SERIAL_SDIO) || defined(CONFIG_USB_F_SERIAL_SMD)
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) {
		struct usb_gadget_fserial_platform_data *fserial_pdata =
			usb_gadget_fserial_device.dev.platform_data;

		/* for charm: Port1: SDIO - DUN,  Port2: TTY - NMEA
		 * for svlte-2: Port1: SDIO - DUN1,  Port2: SDIO - DUN2
		 */
		if (socinfo_get_platform_subtype() == 3) {
			fserial_pdata->transport[0] =
				USB_GADGET_FSERIAL_TRANSPORT_SDIO;
			fserial_pdata->transport[1] =
				USB_GADGET_FSERIAL_TRANSPORT_SMD;
		} else {
			fserial_pdata->transport[0] =
				USB_GADGET_FSERIAL_TRANSPORT_SDIO;
			fserial_pdata->transport[1] =
				USB_GADGET_FSERIAL_TRANSPORT_TTY;
		}
	}
#endif

#if defined(CONFIG_SERIAL_MSM_HS) || defined(CONFIG_PANTECH_BT) //lsi@ps2.bluez
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(54); /* GSBI6(2) */
	msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif
#ifdef CONFIG_MSM_GSBI9_UART
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) {
		msm_device_uart_gsbi9 = msm_add_gsbi9_uart();
		if (IS_ERR(msm_device_uart_gsbi9))
			pr_err("%s(): Failed to create uart gsbi9 device\n",
								__func__);
	}
#endif

#ifdef CONFIG_MSM_BUS_SCALING

	/* RPM calls are only enabled on V2 */
	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2) {
		msm_bus_apps_fabric_pdata.rpm_enabled = 1;
		msm_bus_sys_fabric_pdata.rpm_enabled = 1;
		msm_bus_mm_fabric_pdata.rpm_enabled = 1;
		msm_bus_sys_fpb_pdata.rpm_enabled = 1;
		msm_bus_cpss_fpb_pdata.rpm_enabled = 1;
	}

	msm_bus_apps_fabric.dev.platform_data = &msm_bus_apps_fabric_pdata;
	msm_bus_sys_fabric.dev.platform_data = &msm_bus_sys_fabric_pdata;
	msm_bus_mm_fabric.dev.platform_data = &msm_bus_mm_fabric_pdata;
	msm_bus_sys_fpb.dev.platform_data = &msm_bus_sys_fpb_pdata;
	msm_bus_cpss_fpb.dev.platform_data = &msm_bus_cpss_fpb_pdata;
#endif
}

static void __init msm8x60_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8x60_io();
	msm8x60_allocate_memory_regions();
}

/*
 * Most segments of the EBI2 bus are disabled by default.
 */
static void __init msm8x60_init_ebi2(void)
{
	uint32_t ebi2_cfg;
	void *ebi2_cfg_ptr;

	ebi2_cfg_ptr = ioremap_nocache(0x1a100000, sizeof(uint32_t));
	if (ebi2_cfg_ptr != 0) {
		ebi2_cfg = readl(ebi2_cfg_ptr);

		if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() ||
			machine_is_msm8x60_fluid())
			ebi2_cfg |= (1 << 4) | (1 << 5); /* CS2, CS3 */
		else if (machine_is_msm8x60_sim())
			ebi2_cfg |= (1 << 4); /* CS2 */
		else if (machine_is_msm8x60_rumi3())
			ebi2_cfg |= (1 << 5); /* CS3 */

		writel(ebi2_cfg, ebi2_cfg_ptr);
		iounmap(ebi2_cfg_ptr);
	}

	if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() ||
	    machine_is_msm8x60_fluid()) {
		ebi2_cfg_ptr = ioremap_nocache(0x1a110000, SZ_4K);
		if (ebi2_cfg_ptr != 0) {
			/* EBI2_XMEM_CFG:PWRSAVE_MODE off */
			writel(0UL, ebi2_cfg_ptr);

			/* CS2: Delay 9 cycles (140ns@64MHz) between SMSC
			 * LAN9221 Ethernet controller reads and writes.
			 * The lowest 4 bits are the read delay, the next
			 * 4 are the write delay. */
			writel(0x031F1C99, ebi2_cfg_ptr + 0x10);
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
			/*
			 * RECOVERY=5, HOLD_WR=1
			 * INIT_LATENCY_WR=1, INIT_LATENCY_RD=1
			 * WAIT_WR=1, WAIT_RD=2
			 */
			writel(0x51010112, ebi2_cfg_ptr + 0x14);
			/*
			 * HOLD_RD=1
			 * ADV_OE_RECOVERY=0, ADDR_HOLD_ENA=1
			 */
			writel(0x01000020, ebi2_cfg_ptr + 0x34);
#else
			/* EBI2 CS3 muxed address/data,
			* two cyc addr enable */
			writel(0xA3030020, ebi2_cfg_ptr + 0x34);

#endif
			iounmap(ebi2_cfg_ptr);
		}
	}
}

static void __init msm8x60_configure_smc91x(void)
{
	if (machine_is_msm8x60_sim()) {

		smc91x_resources[0].start = 0x1b800300;
		smc91x_resources[0].end   = 0x1b8003ff;

		smc91x_resources[1].start = (NR_MSM_IRQS + 40);
		smc91x_resources[1].end   = (NR_MSM_IRQS + 40);

	} else if (machine_is_msm8x60_rumi3()) {

		smc91x_resources[0].start = 0x1d000300;
		smc91x_resources[0].end   = 0x1d0003ff;

		smc91x_resources[1].start = TLMM_MSM_DIR_CONN_IRQ_0;
		smc91x_resources[1].end   = TLMM_MSM_DIR_CONN_IRQ_0;
	}
}

static void __init msm8x60_init_tlmm(void)
{
// paiksun...
#ifdef CONFIG_PANTECH
	int rc = 0;
	int pin = 0;

        static unsigned BOOT_CONFIG[] = {
        	GPIO_CFG(76, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
                GPIO_CFG(81, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
                GPIO_CFG(84, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
#if defined(CONFIG_MACH_MSM8X60_EF39S)||defined(CONFIG_MACH_MSM8X60_EF40S)||defined(CONFIG_MACH_MSM8X60_EF40K)
                GPIO_CFG(63, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
#endif
        };

	for (pin = 0; pin < ARRAY_SIZE(BOOT_CONFIG); pin++) {
		rc = gpio_tlmm_config(BOOT_CONFIG[pin], GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, BOOT_CONFIG[pin], rc);
		}
	}
#endif

	if (machine_is_msm8x60_rumi3())
		msm_gpio_install_direct_irq(0, 0, 1);
}

#if (defined(CONFIG_MMC_MSM_SDC1_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC2_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC3_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC4_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC5_SUPPORT))

/* 8x60 is having 5 SDCC controllers */
#define MAX_SDCC_CONTROLLER	5

struct msm_sdcc_gpio {
	/* maximum 10 GPIOs per SDCC controller */
	s16 no;
	/* name of this GPIO */
	const char *name;
	bool always_on;
	bool is_enabled;
};

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct msm_sdcc_gpio sdc1_gpio_cfg[] = {
	{159, "sdc1_dat_0"},
	{160, "sdc1_dat_1"},
	{161, "sdc1_dat_2"},
	{162, "sdc1_dat_3"},
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	{163, "sdc1_dat_4"},
	{164, "sdc1_dat_5"},
	{165, "sdc1_dat_6"},
	{166, "sdc1_dat_7"},
#endif
	{167, "sdc1_clk"},
	{168, "sdc1_cmd"}
};
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct msm_sdcc_gpio sdc2_gpio_cfg[] = {
	{143, "sdc2_dat_0"},
	{144, "sdc2_dat_1", 1},
	{145, "sdc2_dat_2"},
	{146, "sdc2_dat_3"},
#ifdef CONFIG_MMC_MSM_SDC2_8_BIT_SUPPORT
	{147, "sdc2_dat_4"},
	{148, "sdc2_dat_5"},
	{149, "sdc2_dat_6"},
	{150, "sdc2_dat_7"},
#endif
	{151, "sdc2_cmd"},
	{152, "sdc2_clk", 1}
};
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static struct msm_sdcc_gpio sdc5_gpio_cfg[] = {
	{95, "sdc5_cmd"},
	{96, "sdc5_dat_3"},
	{97, "sdc5_clk", 1},
	{98, "sdc5_dat_2"},
	{99, "sdc5_dat_1", 1},
	{100, "sdc5_dat_0"}
};
#endif

struct msm_sdcc_pad_pull_cfg {
	enum msm_tlmm_pull_tgt pull;
	u32 pull_val;
};

struct msm_sdcc_pad_drv_cfg {
	enum msm_tlmm_hdrive_tgt drv;
	u32 drv_val;
};

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct msm_sdcc_pad_drv_cfg sdc3_pad_on_drv_cfg[] = {
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_8MA}
};

static struct msm_sdcc_pad_pull_cfg sdc3_pad_on_pull_cfg[] = {
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_sdcc_pad_drv_cfg sdc3_pad_off_drv_cfg[] = {
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_2MA}
};

static struct msm_sdcc_pad_pull_cfg sdc3_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_DOWN},
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_DOWN}
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct msm_sdcc_pad_drv_cfg sdc4_pad_on_drv_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_8MA}
};

static struct msm_sdcc_pad_pull_cfg sdc4_pad_on_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_sdcc_pad_drv_cfg sdc4_pad_off_drv_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_2MA}
};

#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD)	
static struct msm_sdcc_pad_pull_cfg sdc4_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_UP}
};
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
// 20110519 khlee_wifi for PRESTO wifi sleep gpio settings
static struct msm_sdcc_pad_pull_cfg sdc4_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_UP}
};
#else
static struct msm_sdcc_pad_pull_cfg sdc4_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_DOWN},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_DOWN}
};
#endif
#endif

struct msm_sdcc_pin_cfg {
	/*
	 * = 1 if controller pins are using gpios
	 * = 0 if controller has dedicated MSM pins
	 */
	u8 is_gpio;
	u8 cfg_sts;
	u8 gpio_data_size;
	struct msm_sdcc_gpio *gpio_data;
	struct msm_sdcc_pad_drv_cfg *pad_drv_on_data;
	struct msm_sdcc_pad_drv_cfg *pad_drv_off_data;
	struct msm_sdcc_pad_pull_cfg *pad_pull_on_data;
	struct msm_sdcc_pad_pull_cfg *pad_pull_off_data;
	u8 pad_drv_data_size;
	u8 pad_pull_data_size;
	u8 sdio_lpm_gpio_cfg;
};


static struct msm_sdcc_pin_cfg sdcc_pin_cfg_data[MAX_SDCC_CONTROLLER] = {
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	[0] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc1_gpio_cfg),
		.gpio_data = sdc1_gpio_cfg
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	[1] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc2_gpio_cfg),
		.gpio_data = sdc2_gpio_cfg
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	[2] = {
		.is_gpio = 0,
		.pad_drv_on_data = sdc3_pad_on_drv_cfg,
		.pad_drv_off_data = sdc3_pad_off_drv_cfg,
		.pad_pull_on_data = sdc3_pad_on_pull_cfg,
		.pad_pull_off_data = sdc3_pad_off_pull_cfg,
		.pad_drv_data_size = ARRAY_SIZE(sdc3_pad_on_drv_cfg),
		.pad_pull_data_size = ARRAY_SIZE(sdc3_pad_on_pull_cfg)
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	[3] = {
		.is_gpio = 0,
		.pad_drv_on_data = sdc4_pad_on_drv_cfg,
		.pad_drv_off_data = sdc4_pad_off_drv_cfg,
		.pad_pull_on_data = sdc4_pad_on_pull_cfg,
		.pad_pull_off_data = sdc4_pad_off_pull_cfg,
		.pad_drv_data_size = ARRAY_SIZE(sdc4_pad_on_drv_cfg),
		.pad_pull_data_size = ARRAY_SIZE(sdc4_pad_on_pull_cfg)
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
	[4] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc5_gpio_cfg),
		.gpio_data = sdc5_gpio_cfg
	}
#endif
};

static int msm_sdcc_setup_gpio(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct msm_sdcc_pin_cfg *curr;
	int n;

	curr = &sdcc_pin_cfg_data[dev_id - 1];
	if (!curr->gpio_data)
		goto out;

	for (n = 0; n < curr->gpio_data_size; n++) {
		if (enable) {

			if (curr->gpio_data[n].always_on &&
				curr->gpio_data[n].is_enabled)
				continue;
			pr_debug("%s: enable: %s\n", __func__,
					curr->gpio_data[n].name);
			rc = gpio_request(curr->gpio_data[n].no,
				curr->gpio_data[n].name);
			if (rc) {
				pr_err("%s: gpio_request(%d, %s)"
					"failed", __func__,
					curr->gpio_data[n].no,
					curr->gpio_data[n].name);
				goto free_gpios;
			}
			/* set direction as output for all GPIOs */
			rc = gpio_direction_output(
				curr->gpio_data[n].no, 1);
			if (rc) {
				pr_err("%s: gpio_direction_output"
					"(%d, 1) failed\n", __func__,
					curr->gpio_data[n].no);
				goto free_gpios;
			}
			curr->gpio_data[n].is_enabled = 1;
		} else {
			/*
			 * now free this GPIO which will put GPIO
			 * in low power mode and will also put GPIO
			 * in input mode
			 */
			if (curr->gpio_data[n].always_on)
				continue;
			pr_debug("%s: disable: %s\n", __func__,
					curr->gpio_data[n].name);
			gpio_free(curr->gpio_data[n].no);
			curr->gpio_data[n].is_enabled = 0;
		}
	}
	curr->cfg_sts = enable;
	goto out;

free_gpios:
	for (; n >= 0; n--)
		gpio_free(curr->gpio_data[n].no);
out:
	return rc;
}

static int msm_sdcc_setup_pad(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct msm_sdcc_pin_cfg *curr;
	int n;

	curr = &sdcc_pin_cfg_data[dev_id - 1];
	if (!curr->pad_drv_on_data || !curr->pad_pull_on_data)
		goto out;

	if (enable) {
		/*
		 * set up the normal driver strength and
		 * pull config for pads
		 */
		for (n = 0; n < curr->pad_drv_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_drv_on_data[n].drv ==
						TLMM_HDRV_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_hdrive(curr->pad_drv_on_data[n].drv,
				curr->pad_drv_on_data[n].drv_val);
		}
		for (n = 0; n < curr->pad_pull_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_pull_on_data[n].pull ==
						TLMM_PULL_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_pull(curr->pad_pull_on_data[n].pull,
				curr->pad_pull_on_data[n].pull_val);
		}
	} else {
		/* set the low power config for pads */
		for (n = 0; n < curr->pad_drv_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_drv_off_data[n].drv ==
						TLMM_HDRV_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_hdrive(
				curr->pad_drv_off_data[n].drv,
				curr->pad_drv_off_data[n].drv_val);
		}
		for (n = 0; n < curr->pad_pull_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_pull_off_data[n].pull ==
						TLMM_PULL_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_pull(
				curr->pad_pull_off_data[n].pull,
				curr->pad_pull_off_data[n].pull_val);
		}
	}
	curr->cfg_sts = enable;
out:
	return rc;
}

struct sdcc_reg {
	/* VDD/VCC/VCCQ regulator name on PMIC8058/PMIC8089*/
	const char *reg_name;
	/*
	 * is set voltage supported for this regulator?
	 * 0 = not supported, 1 = supported
	 */
	unsigned char set_voltage_sup;
	/* voltage level to be set */
	unsigned int level;
	/* VDD/VCC/VCCQ voltage regulator handle */
	struct regulator *reg;
	/* is this regulator enabled? */
	bool enabled;
	/* is this regulator needs to be always on? */
	bool always_on;
	/* is operating power mode setting required for this regulator? */
	bool op_pwr_mode_sup;
	/* Load values for low power and high power mode */
	unsigned int lpm_uA;
	unsigned int hpm_uA;
};
/* all SDCC controllers requires VDD/VCC voltage */
static struct sdcc_reg sdcc_vdd_reg_data[MAX_SDCC_CONTROLLER];
/* only SDCC1 requires VCCQ voltage */
static struct sdcc_reg sdcc_vccq_reg_data[1];
/* all SDCC controllers may require voting for VDD PAD voltage */
static struct sdcc_reg sdcc_vddp_reg_data[MAX_SDCC_CONTROLLER];

struct sdcc_reg_data {
	struct sdcc_reg *vdd_data; /* keeps VDD/VCC regulator info */
	struct sdcc_reg *vccq_data; /* keeps VCCQ regulator info */
	struct sdcc_reg *vddp_data; /* keeps VDD Pad regulator info */
	unsigned char sts; /* regulator enable/disable status */
};
/* msm8x60 have 5 SDCC controllers */
static struct sdcc_reg_data sdcc_vreg_data[MAX_SDCC_CONTROLLER];

static int msm_sdcc_vreg_init_reg(struct sdcc_reg *vreg)
{
	int rc = 0;

	/* Get the regulator handle */
	vreg->reg = regulator_get(NULL, vreg->reg_name);
	if (IS_ERR(vreg->reg)) {
		rc = PTR_ERR(vreg->reg);
		pr_err("%s: regulator_get(%s) failed. rc=%d\n",
			__func__, vreg->reg_name, rc);
		goto out;
	}

	/* Set the voltage level if required */
	if (vreg->set_voltage_sup) {
		rc = regulator_set_voltage(vreg->reg, vreg->level,
					vreg->level);
		if (rc) {
			pr_err("%s: regulator_set_voltage(%s) failed rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto vreg_put;
		}
	}
	goto out;

vreg_put:
	regulator_put(vreg->reg);
out:
	return rc;
}

static inline void msm_sdcc_vreg_deinit_reg(struct sdcc_reg *vreg)
{
	regulator_put(vreg->reg);
}

/* this init function should be called only once for each SDCC */
static int msm_sdcc_vreg_init(int dev_id, unsigned char init)
{
	int rc = 0;
	struct sdcc_reg *curr_vdd_reg, *curr_vccq_reg, *curr_vddp_reg;
	struct sdcc_reg_data *curr;

	curr = &sdcc_vreg_data[dev_id - 1];
	curr_vdd_reg = curr->vdd_data;
	curr_vccq_reg = curr->vccq_data;
	curr_vddp_reg = curr->vddp_data;

	if (init) {
		/*
		 * get the regulator handle from voltage regulator framework
		 * and then try to set the voltage level for the regulator
		 */
		if (curr_vdd_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vdd_reg);
			if (rc)
				goto out;
		}
		if (curr_vccq_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vccq_reg);
			if (rc)
				goto vdd_reg_deinit;
		}
		if (curr_vddp_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vddp_reg);
			if (rc)
				goto vccq_reg_deinit;
		}
		goto out;
	} else
		/* deregister with all regulators from regulator framework */
		goto vddp_reg_deinit;

vddp_reg_deinit:
	if (curr_vddp_reg)
		msm_sdcc_vreg_deinit_reg(curr_vddp_reg);
vccq_reg_deinit:
	if (curr_vccq_reg)
		msm_sdcc_vreg_deinit_reg(curr_vccq_reg);
vdd_reg_deinit:
	if (curr_vdd_reg)
		msm_sdcc_vreg_deinit_reg(curr_vdd_reg);
out:
	return rc;
}

static int msm_sdcc_vreg_enable(struct sdcc_reg *vreg)
{
	int rc;

	if (!vreg->enabled) {
		rc = regulator_enable(vreg->reg);
		if (rc) {
			pr_err("%s: regulator_enable(%s) failed. rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto out;
		}
		vreg->enabled = 1;
	}

	/* Put always_on regulator in HPM (high power mode) */
	if (vreg->always_on && vreg->op_pwr_mode_sup) {
		rc = regulator_set_optimum_mode(vreg->reg, vreg->hpm_uA);
		if (rc < 0) {
			pr_err("%s: reg=%s: HPM setting failed"
				" hpm_uA=%d, rc=%d\n",
				__func__, vreg->reg_name,
				vreg->hpm_uA, rc);
			goto vreg_disable;
		}
		rc = 0;
	}
	goto out;

vreg_disable:
	regulator_disable(vreg->reg);
	vreg->enabled = 0;
out:
	return rc;
}

static int msm_sdcc_vreg_disable(struct sdcc_reg *vreg)
{
	int rc;

	/* Never disable always_on regulator */
	if (!vreg->always_on) {
		rc = regulator_disable(vreg->reg);
		if (rc) {
			pr_err("%s: regulator_disable(%s) failed. rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto out;
		}
		vreg->enabled = 0;
	}

	/* Put always_on regulator in LPM (low power mode) */
	if (vreg->always_on && vreg->op_pwr_mode_sup) {
		rc = regulator_set_optimum_mode(vreg->reg, vreg->lpm_uA);
		if (rc < 0) {
			pr_err("%s: reg=%s: LPM setting failed"
				" lpm_uA=%d, rc=%d\n",
				__func__,
				vreg->reg_name,
				vreg->lpm_uA, rc);
			goto out;
		}
		rc = 0;
	}

out:
	return rc;
}

static int msm_sdcc_setup_vreg(int dev_id, unsigned char enable)
{
	int rc = 0;
	struct sdcc_reg *curr_vdd_reg, *curr_vccq_reg, *curr_vddp_reg;
	struct sdcc_reg_data *curr;

	curr = &sdcc_vreg_data[dev_id - 1];
	curr_vdd_reg = curr->vdd_data;
	curr_vccq_reg = curr->vccq_data;
	curr_vddp_reg = curr->vddp_data;

	/* check if regulators are initialized or not? */
	if ((curr_vdd_reg && !curr_vdd_reg->reg) ||
		(curr_vccq_reg && !curr_vccq_reg->reg) ||
		(curr_vddp_reg && !curr_vddp_reg->reg)) {
		/* initialize voltage regulators required for this SDCC */
		rc = msm_sdcc_vreg_init(dev_id, 1);
		if (rc) {
			pr_err("%s: regulator init failed = %d\n",
				__func__, rc);
			goto out;
		}
	}

	if (curr->sts == enable)
		goto out;

	if (curr_vdd_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vdd_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vdd_reg);
		if (rc)
			goto out;
	}

	if (curr_vccq_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vccq_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vccq_reg);
		if (rc)
			goto out;
	}

	if (curr_vddp_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vddp_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vddp_reg);
		if (rc)
			goto out;
	}
	curr->sts = enable;

out:
	return rc;
}

static u32 msm_sdcc_setup_power(struct device *dv, unsigned int vdd)
{
	u32 rc_pin_cfg = 0;
	u32 rc_vreg_cfg = 0;
	u32 rc = 0;
	struct platform_device *pdev;
	struct msm_sdcc_pin_cfg *curr_pin_cfg;

	pdev = container_of(dv, struct platform_device, dev);

	/* setup gpio/pad */
	curr_pin_cfg = &sdcc_pin_cfg_data[pdev->id - 1];
	if (curr_pin_cfg->cfg_sts == !!vdd)
		goto setup_vreg;

	if (curr_pin_cfg->is_gpio)
		rc_pin_cfg = msm_sdcc_setup_gpio(pdev->id, !!vdd);
	else
		rc_pin_cfg = msm_sdcc_setup_pad(pdev->id, !!vdd);

setup_vreg:
	/* setup voltage regulators */
	rc_vreg_cfg = msm_sdcc_setup_vreg(pdev->id, !!vdd);

	if (rc_pin_cfg || rc_vreg_cfg)
		rc = rc_pin_cfg ? rc_pin_cfg : rc_vreg_cfg;

	return rc;
}

#if (defined(CONFIG_MMC_MSM_SDC2_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC5_SUPPORT))
static void msm_sdcc_sdio_lpm_gpio(struct device *dv, unsigned int active)
{
	struct msm_sdcc_pin_cfg *curr_pin_cfg;
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);
	/* setup gpio/pad */
	curr_pin_cfg = &sdcc_pin_cfg_data[pdev->id - 1];

	if (curr_pin_cfg->cfg_sts == active)
		return;

	curr_pin_cfg->sdio_lpm_gpio_cfg = 1;
	if (curr_pin_cfg->is_gpio)
		msm_sdcc_setup_gpio(pdev->id, active);
	else
		msm_sdcc_setup_pad(pdev->id, active);
	curr_pin_cfg->sdio_lpm_gpio_cfg = 0;
}
#endif

static int msm_sdc3_get_wpswitch(struct device *dev)
{
	struct platform_device *pdev;
	int status;
	pdev = container_of(dev, struct platform_device, dev);

	status = gpio_request(GPIO_SDC_WP, "SD_WP_Switch");
	if (status) {
		pr_err("%s:Failed to request GPIO %d\n",
					__func__, GPIO_SDC_WP);
	} else {
		status = gpio_direction_input(GPIO_SDC_WP);
		if (!status) {
			status = gpio_get_value_cansleep(GPIO_SDC_WP);
			pr_info("%s: WP Status for Slot %d = %d\n",
				 __func__, pdev->id, status);
		}
		gpio_free(GPIO_SDC_WP);
	}
	return status;
}

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
int sdc5_register_status_notify(void (*callback)(int, void *),
	void *dev_id)
{
	sdc5_status_notify_cb = callback;
	sdc5_status_notify_cb_devid = dev_id;
	return 0;
}
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
int sdc2_register_status_notify(void (*callback)(int, void *),
	void *dev_id)
{
	sdc2_status_notify_cb = callback;
	sdc2_status_notify_cb_devid = dev_id;
	return 0;
}
#endif

/* Interrupt handler for SDC2 and SDC5 detection
 * This function uses dual-edge interrputs settings in order
 * to get SDIO detection when the GPIO is rising and SDIO removal
 * when the GPIO is falling */
static irqreturn_t msm8x60_multi_sdio_slot_status_irq(int irq, void *dev_id)
{
	int status;

	if (!machine_is_msm8x60_charm_surf() &&
	    !machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			&& !machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			&& !machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			&& !machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			&& !machine_is_msm8x60_presto()
#endif
	    )
		return IRQ_NONE;

	status = gpio_get_value(MDM2AP_SYNC);
	pr_info("%s: MDM2AP_SYNC Status = %d\n",
		 __func__, status);

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	if (sdc2_status_notify_cb) {
		pr_info("%s: calling sdc2_status_notify_cb\n", __func__);
		sdc2_status_notify_cb(status,
			sdc2_status_notify_cb_devid);
	}
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
	if (sdc5_status_notify_cb) {
		pr_info("%s: calling sdc5_status_notify_cb\n", __func__);
		sdc5_status_notify_cb(status,
			sdc5_status_notify_cb_devid);
	}
#endif
	return IRQ_HANDLED;
}

static int msm8x60_multi_sdio_init(void)
{
	int ret, irq_num;

	if (!machine_is_msm8x60_charm_surf() &&
	    !machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			&& !machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			&& !machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			&& !machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			&& !machine_is_msm8x60_presto()
#endif
	    )
		return 0;

	ret = msm_gpiomux_get(MDM2AP_SYNC);
	if (ret) {
		pr_err("%s:Failed to request GPIO %d, ret=%d\n",
					__func__, MDM2AP_SYNC, ret);
		return ret;
	}

	irq_num = gpio_to_irq(MDM2AP_SYNC);

	ret = request_irq(irq_num,
		msm8x60_multi_sdio_slot_status_irq,
		IRQ_TYPE_EDGE_BOTH,
		"sdio_multidetection", NULL);

	if (ret) {
		pr_err("%s:Failed to request irq, ret=%d\n",
					__func__, ret);
		return ret;
	}

	return ret;
}

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
#ifdef CONFIG_SKY_MMC
unsigned int msm8x60_sdcc_slot_status(void)
{
	int status;

	status = gpio_request(PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1)
				, "SD_HW_Detect");
	if (status) {
		pr_err("%s:Failed to request GPIO %d\n", __func__,
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
	} else {
		status = gpio_direction_input(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
		if (!status)
			status = !(gpio_get_value_cansleep(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1)));
		gpio_free(PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
	}
	return (unsigned int) status;
}
#else
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
static unsigned int msm8x60_sdcc_slot_status(struct device *dev)
{
	int status;

	status = gpio_request(PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1)
				, "SD_HW_Detect");
	if (status) {
		pr_err("%s:Failed to request GPIO %d\n", __func__,
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
	} else {
		status = gpio_direction_input(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
		if (!status)
			status = !(gpio_get_value_cansleep(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1)));
		gpio_free(PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC3_DET - 1));
	}
	return (unsigned int) status;
}
#endif
#endif
#endif

#ifdef	CONFIG_MMC_MSM_SDC4_SUPPORT
#ifndef CONFIG_SKY_WLAN
static int msm_sdcc_cfg_mpm_sdiowakeup(struct device *dev, unsigned mode)
{
	struct platform_device *pdev;
	enum msm_mpm_pin pin;
	int ret = 0;

	pdev = container_of(dev, struct platform_device, dev);

	/* Only SDCC4 slot connected to WLAN chip has wakeup capability */
	if (pdev->id == 4)
		pin = MSM_MPM_PIN_SDC4_DAT1;
	else
		return -EINVAL;

	switch (mode) {
	case SDC_DAT1_DISABLE:
		ret = msm_mpm_enable_pin(pin, 0);
		break;
	case SDC_DAT1_ENABLE:
		ret = msm_mpm_set_pin_type(pin, IRQ_TYPE_LEVEL_LOW);
		ret = msm_mpm_enable_pin(pin, 1);
		break;
	case SDC_DAT1_ENWAKE:
		ret = msm_mpm_set_pin_wake(pin, 1);
		break;
	case SDC_DAT1_DISWAKE:
		ret = msm_mpm_set_pin_wake(pin, 0);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}
#endif //CONFIG_SKY_WLAN
#endif
#endif

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct mmc_platform_data msm8x60_sdc1_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd  = msm_sdcc_setup_power,
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
#else
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 1,
	.pclk_src_dfab	= 1,
#ifdef CONFIG_MMC_MSM_SDC1_DUMMY52_REQUIRED
	.dummy52_required = 1,
#endif
};
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct mmc_platform_data msm8x60_sdc2_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_165_195,
	.translate_vdd  = msm_sdcc_setup_power,
	.sdio_lpm_gpio_setup = msm_sdcc_sdio_lpm_gpio,
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.pclk_src_dfab  = 1,
	.register_status_notify = sdc2_register_status_notify,
#ifdef CONFIG_MMC_MSM_SDC2_DUMMY52_REQUIRED
	.dummy52_required = 1,
#endif
#ifdef CONFIG_MSM_SDIO_AL
	.is_sdio_al_client = 1,
#endif
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct mmc_platform_data msm8x60_sdc3_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd  = msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.wpswitch  	= msm_sdc3_get_wpswitch,
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	.status      = msm8x60_sdcc_slot_status,
	.status_irq  = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
				       PMIC_GPIO_SDC3_DET - 1),
	.irq_flags   = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
#endif
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.pclk_src_dfab  = 1,
#ifdef CONFIG_MMC_MSM_SDC3_DUMMY52_REQUIRED
	.dummy52_required = 1,
#endif
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct mmc_platform_data msm8x60_sdc4_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd  = msm_sdcc_setup_power,
#ifdef CONFIG_WIFI_CONTROL_FUNC
	.status				= wlan_status,
	.register_status_notify	= wlan_status_register,
	.embedded_sdio		= &wlan_emb_data,
#endif
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.pclk_src_dfab  = 1,
#ifndef CONFIG_SKY_WLAN
	.cfg_mpm_sdiowakeup = msm_sdcc_cfg_mpm_sdiowakeup,
#endif //CONFIG_SKY_WLAN
#ifdef CONFIG_MMC_MSM_SDC4_DUMMY52_REQUIRED
	.dummy52_required = 1,
#endif
};
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static struct mmc_platform_data msm8x60_sdc5_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_165_195,
	.translate_vdd  = msm_sdcc_setup_power,
	.sdio_lpm_gpio_setup = msm_sdcc_sdio_lpm_gpio,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.pclk_src_dfab  = 1,
	.register_status_notify = sdc5_register_status_notify,
#ifdef CONFIG_MMC_MSM_SDC5_DUMMY52_REQUIRED
	.dummy52_required = 1,
#endif
#ifdef CONFIG_MSM_SDIO_AL
	.is_sdio_al_client = 1,
#endif
};
#endif

static void __init msm8x60_init_mmc(void)
{
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	/* SDCC1 : eMMC card connected */
	sdcc_vreg_data[0].vdd_data = &sdcc_vdd_reg_data[0];
	sdcc_vreg_data[0].vdd_data->reg_name = "8901_l5";
	sdcc_vreg_data[0].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[0].vdd_data->level = 2850000;

/* p14774 : 8901_l5 : always-on */
  sdcc_vreg_data[0].vdd_data->always_on = 1;
  sdcc_vreg_data[0].vdd_data->op_pwr_mode_sup = 1;
  sdcc_vreg_data[0].vdd_data->lpm_uA = 9000;
  sdcc_vreg_data[0].vdd_data->hpm_uA = 200000;

	sdcc_vreg_data[0].vccq_data = &sdcc_vccq_reg_data[0];
	sdcc_vreg_data[0].vccq_data->reg_name = "8901_lvs0";
	sdcc_vreg_data[0].vccq_data->set_voltage_sup = 0;
	sdcc_vreg_data[0].vccq_data->always_on = 1;

	msm_add_sdcc(1, &msm8x60_sdc1_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	/*
	 * MDM SDIO client is connected to SDC2 on charm SURF/FFA
	 * and no card is connected on 8660 SURF/FFA/FLUID.
	 */
	sdcc_vreg_data[1].vdd_data = &sdcc_vdd_reg_data[1];
	sdcc_vreg_data[1].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[1].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[1].vdd_data->level = 1800000;

	sdcc_vreg_data[1].vccq_data = NULL;

	if (machine_is_msm8x60_charm_surf())
		msm8x60_sdc2_data.msmsdcc_fmax = 24000000;
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			)
	{
#ifdef CONFIG_MMC_MSM_SDIO_SUPPORT
		msm8x60_sdc2_data.sdiowakeup_irq = gpio_to_irq(144);
		msm_sdcc_setup_gpio(2, 1);
#endif
		msm_add_sdcc(2, &msm8x60_sdc2_data);
	}
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	/* SDCC3 : External card slot connected */
	sdcc_vreg_data[2].vdd_data = &sdcc_vdd_reg_data[2];

#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined(CONFIG_MACH_MSM8X60_EF40S) || defined(CONFIG_MACH_MSM8X60_EF40K)
#if (BOARD_REV >= WS10)
	sdcc_vreg_data[2].vdd_data->reg_name = "8058_l15";
#else
       sdcc_vreg_data[2].vdd_data->reg_name = "8058_l14";
#endif
#else
	sdcc_vreg_data[2].vdd_data->reg_name = "8058_l14";
#endif /* CONFIG_MACH_MSM8X60_EF39S */

	sdcc_vreg_data[2].vdd_data->set_voltage_sup = 1;
#ifdef CONFIG_SKY_MMC
	sdcc_vreg_data[2].vdd_data->level = 3000000;
	sdcc_vreg_data[2].vdd_data->always_on = 0;
#else
	sdcc_vreg_data[2].vdd_data->level = 2850000;
	sdcc_vreg_data[2].vdd_data->always_on = 1;
#endif
	sdcc_vreg_data[2].vdd_data->op_pwr_mode_sup = 1;
	sdcc_vreg_data[2].vdd_data->lpm_uA = 9000;
	sdcc_vreg_data[2].vdd_data->hpm_uA = 200000;

	sdcc_vreg_data[2].vccq_data = NULL;

	sdcc_vreg_data[2].vddp_data = &sdcc_vddp_reg_data[2];
	sdcc_vreg_data[2].vddp_data->reg_name = "8058_l5";
	sdcc_vreg_data[2].vddp_data->set_voltage_sup = 1;
	sdcc_vreg_data[2].vddp_data->level = 2850000;
	sdcc_vreg_data[2].vddp_data->always_on = 1;
	sdcc_vreg_data[2].vddp_data->op_pwr_mode_sup = 1;
	/* Sleep current required is ~300 uA. But min. RPM
	 * vote can be in terms of mA (min. 1 mA).
	 * So let's vote for 2 mA during sleep.
	 */
	sdcc_vreg_data[2].vddp_data->lpm_uA = 2000;
	/* Max. Active current required is 16 mA */
	sdcc_vreg_data[2].vddp_data->hpm_uA = 16000;

	if (machine_is_msm8x60_fluid())
		msm8x60_sdc3_data.wpswitch = NULL;
	msm_add_sdcc(3, &msm8x60_sdc3_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	/* SDCC4 : WLAN WCN1314 chip is connected */
	sdcc_vreg_data[3].vdd_data = &sdcc_vdd_reg_data[3];
	sdcc_vreg_data[3].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[3].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[3].vdd_data->level = 1800000;

	sdcc_vreg_data[3].vccq_data = NULL;

	msm_add_sdcc(4, &msm8x60_sdc4_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
	/*
	 * MDM SDIO client is connected to SDC5 on charm SURF/FFA
	 * and no card is connected on 8660 SURF/FFA/FLUID.
	 */
	sdcc_vreg_data[4].vdd_data = &sdcc_vdd_reg_data[4];
	sdcc_vreg_data[4].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[4].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[4].vdd_data->level = 1800000;

	sdcc_vreg_data[4].vccq_data = NULL;

	if (machine_is_msm8x60_charm_surf())
		msm8x60_sdc5_data.msmsdcc_fmax = 24000000;
	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			)
	{
#ifdef CONFIG_MMC_MSM_SDIO_SUPPORT
		msm8x60_sdc5_data.sdiowakeup_irq = gpio_to_irq(99);
		msm_sdcc_setup_gpio(5, 1);
#endif
		msm_add_sdcc(5, &msm8x60_sdc5_data);
	}
#endif
}

#if !defined(CONFIG_GPIO_SX150X) && !defined(CONFIG_GPIO_SX150X_MODULE)
static inline void display_common_power(int on) {}
#else

#define _GET_REGULATOR(var, name) do {					\
	if (var == NULL) {						\
		var = regulator_get(NULL, name);			\
		if (IS_ERR(var)) {					\
			pr_err("'%s' regulator not found, rc=%ld\n",	\
				name, PTR_ERR(var));			\
			var = NULL;					\
		}							\
	}								\
} while (0)

static int dsub_regulator(int on)
{
	static struct regulator *dsub_reg;
	static struct regulator *mpp0_reg;
	static int dsub_reg_enabled;
	int rc = 0;

	_GET_REGULATOR(dsub_reg, "8901_l3");
	if (IS_ERR(dsub_reg)) {
		printk(KERN_ERR "%s: failed to get reg 8901_l3 err=%ld",
		       __func__, PTR_ERR(dsub_reg));
		return PTR_ERR(dsub_reg);
	}

	_GET_REGULATOR(mpp0_reg, "8901_mpp0");
	if (IS_ERR(mpp0_reg)) {
		printk(KERN_ERR "%s: failed to get reg 8901_mpp0 err=%ld",
		       __func__, PTR_ERR(mpp0_reg));
		return PTR_ERR(mpp0_reg);
	}

	if (on && !dsub_reg_enabled) {
		rc = regulator_set_voltage(dsub_reg, 3300000, 3300000);
		if (rc) {
			printk(KERN_ERR "%s: failed to set reg 8901_l3 voltage"
			       " err=%d", __func__, rc);
			goto dsub_regulator_err;
		}
		rc = regulator_enable(dsub_reg);
		if (rc) {
			printk(KERN_ERR "%s: failed to enable reg 8901_l3"
			       " err=%d", __func__, rc);
			goto dsub_regulator_err;
		}
		rc = regulator_enable(mpp0_reg);
		if (rc) {
			printk(KERN_ERR "%s: failed to enable reg 8901_mpp0"
			       " err=%d", __func__, rc);
			goto dsub_regulator_err;
		}
		dsub_reg_enabled = 1;
	} else if (!on && dsub_reg_enabled) {
		rc = regulator_disable(dsub_reg);
		if (rc)
			printk(KERN_WARNING "%s: failed to disable reg 8901_l3"
			       " err=%d", __func__, rc);
		rc = regulator_disable(mpp0_reg);
		if (rc)
			printk(KERN_WARNING "%s: failed to disable reg "
			       "8901_mpp0 err=%d", __func__, rc);
		dsub_reg_enabled = 0;
	}

	return rc;

dsub_regulator_err:
	regulator_put(mpp0_reg);
	regulator_put(dsub_reg);
	return rc;
}

static int display_power_on;
static void setup_display_power(void)
{
	if (display_power_on)
		if (lcdc_vga_enabled) {
			dsub_regulator(1);
			gpio_set_value_cansleep(GPIO_LVDS_SHUTDOWN_N, 0);
			gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, 0);
			if (machine_is_msm8x60_ffa() ||
			    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
			    )
				gpio_set_value_cansleep(GPIO_DONGLE_PWR_EN, 1);
		} else {
			dsub_regulator(0);
			gpio_set_value_cansleep(GPIO_LVDS_SHUTDOWN_N, 1);
			gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, 1);
			if (machine_is_msm8x60_ffa() ||
			    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
			    )
				gpio_set_value_cansleep(GPIO_DONGLE_PWR_EN, 0);
		}
	else {
		dsub_regulator(0);
		if (machine_is_msm8x60_ffa() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
				|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
				|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
				|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
				|| machine_is_msm8x60_presto()
#endif
				)
			gpio_set_value_cansleep(GPIO_DONGLE_PWR_EN, 0);
		/* BACKLIGHT */
		gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, 0);
		/* LVDS */
		gpio_set_value_cansleep(GPIO_LVDS_SHUTDOWN_N, 0);
	}
}

#define _GET_REGULATOR(var, name) do {					\
	if (var == NULL) {						\
		var = regulator_get(NULL, name);			\
		if (IS_ERR(var)) {					\
			pr_err("'%s' regulator not found, rc=%ld\n",	\
				name, PTR_ERR(var));			\
			var = NULL;					\
		}							\
	}								\
} while (0)

#define GPIO_RESX_N (GPIO_EXPANDER_GPIO_BASE + 2)

#ifdef CONFIG_MACH_MSM8X60_PRESTO    // p13777 kej 110623
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
static int display_common_power(int on)
{
	static struct regulator *reg_8058_l3;
	static struct regulator *reg_8058_l11;
	//static struct regulator *reg_8058_l17;
	//static struct regulator *reg_8058_l20;
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;



	if (!reg_8058_l3)
		_GET_REGULATOR(reg_8058_l3, "8058_l3");
	if (!reg_8058_l11)
		_GET_REGULATOR(reg_8058_l11, "8058_l11");

	if (on) {
		rc = regulator_set_voltage(reg_8058_l11, 1800000, 1800000);
		if (!rc)
			rc = regulator_enable(reg_8058_l11);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8058_l11", rc);
			return rc;
		}

		rc = regulator_set_voltage(reg_8058_l3, 3000000, 3000000);
		if (!rc)
			rc = regulator_enable(reg_8058_l3);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8058_l3", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {

		rc = regulator_disable(reg_8058_l3);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8058_l3", rc);
#if 0
		rc = regulator_disable(reg_8058_l11);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8058_l11", rc);
#endif
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}
#endif 
#else
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
static void display_common_power(int on)
{
	int rc;
	static struct regulator *display_reg;

	if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() ||
	    machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
	    ) {
		if (on) {
			/* LVDS */
			_GET_REGULATOR(display_reg, "8901_l2");
			if (!display_reg)
				return;
			rc = regulator_set_voltage(display_reg,
				3300000, 3300000);
			if (rc)
				goto out;
			rc = regulator_enable(display_reg);
			if (rc)
				goto out;
			rc = gpio_request(GPIO_LVDS_SHUTDOWN_N,
				"LVDS_STDN_OUT_N");
			if (rc) {
				printk(KERN_ERR "%s: LVDS gpio %d request"
					"failed\n", __func__,
					 GPIO_LVDS_SHUTDOWN_N);
				goto out2;
			}

			/* BACKLIGHT */
			rc = gpio_request(GPIO_BACKLIGHT_EN, "BACKLIGHT_EN");
			if (rc) {
				printk(KERN_ERR "%s: BACKLIGHT gpio %d request"
					"failed\n", __func__,
					 GPIO_BACKLIGHT_EN);
				goto out3;
			}

			if (machine_is_msm8x60_ffa() ||
			    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
			    ) {
				rc = gpio_request(GPIO_DONGLE_PWR_EN,
						  "DONGLE_PWR_EN");
				if (rc) {
					printk(KERN_ERR "%s: DONGLE_PWR_EN gpio"
					       " %d request failed\n", __func__,
					       GPIO_DONGLE_PWR_EN);
					goto out4;
				}
			}

			gpio_direction_output(GPIO_LVDS_SHUTDOWN_N, 0);
			gpio_direction_output(GPIO_BACKLIGHT_EN, 0);
			if (machine_is_msm8x60_ffa() ||
			    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
					|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
					|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
					|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
					|| machine_is_msm8x60_presto()
#endif
			    )
				gpio_direction_output(GPIO_DONGLE_PWR_EN, 0);
			mdelay(20);
			display_power_on = 1;
			setup_display_power();
		} else {
			if (display_power_on) {
				display_power_on = 0;
				setup_display_power();
				mdelay(20);
				if (machine_is_msm8x60_ffa() ||
				    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
						|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
						|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
						|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
						|| machine_is_msm8x60_presto()
#endif
				    )
					gpio_free(GPIO_DONGLE_PWR_EN);
				goto out4;
			}
		}
	}
#if defined(CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT) || \
	defined(CONFIG_FB_MSM_LCDC_AUO_WVGA)
	else if (machine_is_msm8x60_fluid()) {
		static struct regulator *fluid_reg;
		static struct regulator *fluid_reg2;

		if (on) {
			_GET_REGULATOR(fluid_reg, "8901_l2");
			if (!fluid_reg)
				return;
			_GET_REGULATOR(fluid_reg2, "8058_s3");
			if (!fluid_reg2) {
				regulator_put(fluid_reg);
				return;
			}
			rc = gpio_request(GPIO_RESX_N, "RESX_N");
			if (rc) {
				regulator_put(fluid_reg2);
				regulator_put(fluid_reg);
				return;
			}
			regulator_set_voltage(fluid_reg, 2850000, 2850000);
			regulator_set_voltage(fluid_reg2, 1800000, 1800000);
			regulator_enable(fluid_reg);
			regulator_enable(fluid_reg2);
			msleep(20);
			gpio_direction_output(GPIO_RESX_N, 0);
			udelay(10);
			gpio_set_value_cansleep(GPIO_RESX_N, 1);
			display_power_on = 1;
			setup_display_power();
		} else {
			gpio_set_value_cansleep(GPIO_RESX_N, 0);
			gpio_free(GPIO_RESX_N);
			msleep(20);
			regulator_disable(fluid_reg2);
			regulator_disable(fluid_reg);
			regulator_put(fluid_reg2);
			regulator_put(fluid_reg);
			display_power_on = 0;
			setup_display_power();
			fluid_reg = NULL;
			fluid_reg2 = NULL;
		}
	}
#endif
	return;

out4:
	gpio_free(GPIO_BACKLIGHT_EN);
out3:
	gpio_free(GPIO_LVDS_SHUTDOWN_N);
out2:
	regulator_disable(display_reg);
out:
	regulator_put(display_reg);
	display_reg = NULL;
}
#endif

#endif
#undef _GET_REGULATOR
#endif

//static int mipi_dsi_panel_power(int on);

#define LCDC_NUM_GPIO 28
#define LCDC_GPIO_START 0

#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
static void lcdc_samsung_panel_power(int on)
{
	int n, ret = 0;

	display_common_power(on);

	for (n = 0; n < LCDC_NUM_GPIO; n++) {
		if (on) {
			ret = gpio_request(LCDC_GPIO_START + n, "LCDC_GPIO");
			if (unlikely(ret)) {
				pr_err("%s not able to get gpio\n", __func__);
				break;
			}
		} else
			gpio_free(LCDC_GPIO_START + n);
	}

	if (ret) {
		for (n--; n >= 0; n--)
			gpio_free(LCDC_GPIO_START + n);
	}

//	mipi_dsi_panel_power(0); /* set 8058_ldo0 to LPM */
}
#endif

//pz1946 20111002 leakeage current recover
//#ifdef CONFIG_SKY_CHARGING			// kobj 110827
#if defined(CONFIG_SKY_CHARGING) || defined(CONFIG_SKY_SMB_CHARGER)
// paiksun... trickle
void gpio_set_132_trickle_leakeage(void)
{
	int rc = 0;

	rc = gpio_tlmm_config(GPIO_CFG(132, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),GPIO_CFG_ENABLE);
	if (!rc) {
		gpio_set_value_cansleep(132,1);
		mdelay(200);
		gpio_set_value_cansleep(132,0);
	}
}
#endif 
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
#define _GET_REGULATOR(var, name) do {				\
	var = regulator_get(NULL, name);			\
	if (IS_ERR(var)) {					\
		pr_err("'%s' regulator not found, rc=%ld\n",	\
			name, IS_ERR(var));			\
		var = NULL;					\
		return -ENODEV;					\
	}							\
} while (0)

// 201110422, kkcho,  for MHL-power
#ifdef PANTECH_MHL_SII9244_POWER_CTRL
int mhl_power_ctrl(int on)
{
	static struct regulator *reg_8058_l12;
	static struct regulator *reg_8058_l25;
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	printk(KERN_ERR "[SKY_MHL]+%s 1st needed mhl-power-on\n", __FUNCTION__);

	if (!reg_8058_l12)
		_GET_REGULATOR(reg_8058_l12, "8058_l12");
	if (!reg_8058_l25)
		_GET_REGULATOR(reg_8058_l25, "8058_l25");

	if (on) {

		hdmi_core_power(1, 0);
		
		rc = regulator_set_voltage(reg_8058_l25, 1200000, 1200000);
		if (!rc)
			rc = regulator_enable(reg_8058_l25);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
					"8058_l25", rc);
			return rc;
		}
		rc = regulator_set_voltage(reg_8058_l12, 3300000, 3300000);
		if (!rc)
			rc = regulator_enable(reg_8058_l12);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
					"8058_l12", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {

		hdmi_core_power(0, 0);
	
		rc = regulator_disable(reg_8058_l12);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
					"8058_l12", rc);
		rc = regulator_disable(reg_8058_l25);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
					"8058_l25", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}
EXPORT_SYMBOL(mhl_power_ctrl);
#endif
static int hdmi_enable_5v(int on)
{
	static struct regulator *reg_8901_hdmi_mvs;	/* HDMI_5V */
	static struct regulator *reg_8901_mpp0;		/* External 5V */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

#ifdef PANTECH_MHL_SII9244_POWER_CTRL
	return 0;
#endif

	if (!reg_8901_hdmi_mvs)
		_GET_REGULATOR(reg_8901_hdmi_mvs, "8901_hdmi_mvs");
	if (!reg_8901_mpp0)
		_GET_REGULATOR(reg_8901_mpp0, "8901_mpp0");

	if (on) {
		rc = regulator_enable(reg_8901_mpp0);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8901_mpp0", rc);
			return rc;
		}
		rc = regulator_enable(reg_8901_hdmi_mvs);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8901_hdmi_mvs", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8901_hdmi_mvs);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8901_hdmi_mvs", rc);
		rc = regulator_disable(reg_8901_mpp0);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8901_mpp0", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}

static int hdmi_core_power(int on, int show)
{
#ifndef PANTECH_MHL_SII9244_POWER_CTRL  // 20110516, kkcho, remove for MHL Device.
	static struct regulator *reg_8058_l16;		/* VDD_HDMI */
#endif
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

// 201110422, kkcho,  for MHL-power
#ifdef PANTECH_MHL_SII9244_POWER_CTRL
	//mhl_power_ctrl(on);
#endif

#ifndef PANTECH_MHL_SII9244_POWER_CTRL  // 20110516, kkcho, remove for MHL Device.
	if (!reg_8058_l16)
		_GET_REGULATOR(reg_8058_l16, "8058_l16");
#endif

	if (on) {
#ifndef PANTECH_MHL_SII9244_POWER_CTRL // 20110516, kkcho, remove for MHL Device.
		rc = regulator_set_voltage(reg_8058_l16, 1800000, 1800000);
		if (!rc)
			rc = regulator_enable(reg_8058_l16);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8058_l16", rc);
			return rc;
		}
#endif
		rc = gpio_request(170, "HDMI_DDC_CLK");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_DDC_CLK", 170, rc);
			goto error1;
		}
		rc = gpio_request(171, "HDMI_DDC_DATA");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_DDC_DATA", 171, rc);
			goto error2;
		}
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= TP10))	 
		rc = gpio_request(172, "HDMI_HPD");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
					"HDMI_HPD", 172, rc);
			goto error3;
		}
#endif
		pr_info("%s(on): success\n", __func__);
	} else {
		gpio_free(170);
		gpio_free(171);
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= TP10))	 
		gpio_free(172);
#endif
#ifndef PANTECH_MHL_SII9244_POWER_CTRL  // 20110516, kkcho, remove for MHL Device.
		rc = regulator_disable(reg_8058_l16);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8058_l16", rc);
#endif
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= TP10))
error3:	
	gpio_free(172);
#endif	
error2:
	gpio_free(171);
error1:
	gpio_free(170);
	return rc;
}


#ifdef PANTECH_MHL_SII9244_POWER_CTRL
static int hdmi_cec_power(int on)
{
	return 0;
}
#else
static int hdmi_cec_power(int on)
{
	static struct regulator *reg_8901_l3;		/* HDMI_CEC */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_8901_l3)
		_GET_REGULATOR(reg_8901_l3, "8901_l3");

	if (on) {
		rc = regulator_set_voltage(reg_8901_l3, 3300000, 3300000);
		if (!rc)
			rc = regulator_enable(reg_8901_l3);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8901_l3", rc);
			return rc;
		}
		rc = gpio_request(169, "HDMI_CEC_VAR");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_CEC_VAR", 169, rc);
			goto error;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		gpio_free(169);
		rc = regulator_disable(reg_8901_l3);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8901_l3", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
error:
	regulator_disable(reg_8901_l3);
	return rc;
}
#endif
#undef _GET_REGULATOR

#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
static int lcdc_panel_power(int on)
{
	int flag_on = !!on;
	static int lcdc_power_save_on;

	if (lcdc_power_save_on == flag_on)
		return 0;

	lcdc_power_save_on = flag_on;

	lcdc_samsung_panel_power(on);

	return 0;
}
#endif  //CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
#ifdef CONFIG_MSM_BUS_SCALING
#ifdef CONFIG_FB_MSM_LCDC_DSUB
static struct msm_bus_vectors mdp_init_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_smi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 388800000,
		.ib = 486000000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_ebi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 388800000,
		.ib = 486000000 * 2,
	},
};
static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 458092800,
		.ib = 572616000,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 458092800,
		.ib = 572616000 * 2,
	},
};
static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 471744000,
		.ib = 589680000,
	},
	/* Master and slaves can be from different fabrics */
       {
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
	       .ab = 471744000,
	       .ib = 589680000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 575424000,
		.ib = 719280000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 575424000,
		.ib = 719280000 * 2,
	},
};

#else

static struct msm_bus_vectors mdp_init_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

#ifndef CONFIG_FB_MSM_MIPI_DSI
static struct msm_bus_vectors mdp_sd_smi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 175110000,
		.ib = 218887500,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_ebi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000,
		.ib = 270000000 * 2,
	},
};
static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 216000000,
		.ib = 270000000,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 230400000,
		.ib = 288000000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 230400000,
		.ib = 288000000 * 2,
	},
};
#endif

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 334080000,
		.ib = 417600000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 334080000,
		.ib = 550000000 * 2,
	},
};

#endif
static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
#ifndef CONFIG_FB_MSM_MIPI_DSI
	{
		ARRAY_SIZE(mdp_sd_smi_vectors),
		mdp_sd_smi_vectors,
	},
	{
		ARRAY_SIZE(mdp_sd_ebi_vectors),
		mdp_sd_ebi_vectors,
	},
	{
		ARRAY_SIZE(mdp_vga_vectors),
		mdp_vga_vectors,
	},
	{
		ARRAY_SIZE(mdp_720p_vectors),
		mdp_720p_vectors,
	},
#else
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
#endif
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
};
static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};

#endif
#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors dtv_bus_init_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};
static struct msm_bus_vectors dtv_bus_def_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 566092800,
		.ib = 707616000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 566092800,
		.ib = 707616000,
	},
};
static struct msm_bus_paths dtv_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(dtv_bus_init_vectors),
		dtv_bus_init_vectors,
	},
	{
		ARRAY_SIZE(dtv_bus_def_vectors),
		dtv_bus_def_vectors,
	},
};
static struct msm_bus_scale_pdata dtv_bus_scale_pdata = {
	dtv_bus_scale_usecases,
	ARRAY_SIZE(dtv_bus_scale_usecases),
	.name = "dtv",
};

static struct lcdc_platform_data dtv_pdata = {
	.bus_scale_table = &dtv_bus_scale_pdata,
};
#endif

#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
static struct lcdc_platform_data lcdc_pdata = {
	.lcdc_power_save   = lcdc_panel_power,
};
#endif //CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT


#ifdef CONFIG_FB_MSM_MIPI_DSI
#if defined(CONFIG_FB_MSM_MIPI_DSI_SAMSUNG) || defined(CONFIG_FB_MSM_MIPI_DSI_SONY)
static unsigned LCD_VCC_POWER[] = {
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),		/* RFR */
};

static int init_power_gpio(void)
{
	int rc = 0;
	int pin = 0;

	for (pin = 0; pin < ARRAY_SIZE(LCD_VCC_POWER); pin++) {
		rc = gpio_tlmm_config(LCD_VCC_POWER[pin], GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, LCD_VCC_POWER[pin], rc);
			return -EIO;
		}
	}
	return rc;
}

static int mipi_dsi_panel_power(int on)
{
	//printk(KERN_ERR "%s: %d\n",__func__, on);

	if (on) {
		gpio_set_value(49, on);
#if defined(CONFIG_FB_MSM_MIPI_DSI_SAMSUNG)
		//msleep(100);
#elif defined(CONFIG_FB_MSM_MIPI_DSI_SONY)
		usleep(10);
#endif
	} else {
		gpio_set_value(49, on);
	}
	return 0;
}

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.dsi_power_save   = mipi_dsi_panel_power,
};
#else
// #define MDP_VSYNC_GPIO			28

/*
 * MIPI_DSI only use 8058_LDO0 which need always on
 * therefore it need to be put at low power mode if
 * it was not used instead of turn it off.
 */
static int mipi_dsi_panel_power(int on)
{
	int flag_on = !!on;
	static int mipi_dsi_power_save_on;
	static struct regulator *ldo0;
	int rc = 0;

	if (mipi_dsi_power_save_on == flag_on)
		return 0;

	mipi_dsi_power_save_on = flag_on;

	if (ldo0 == NULL) {	/* init */
		ldo0 = regulator_get(NULL, "8058_l0");
		if (IS_ERR(ldo0)) {
			pr_debug("%s: LDO0 failed\n", __func__);
			rc = PTR_ERR(ldo0);
			return rc;
		}

		rc = regulator_set_voltage(ldo0, 1200000, 1200000);
		if (rc)
			goto out;

		rc = regulator_enable(ldo0);
		if (rc)
			goto out;
	}

	if (on) {
		/* set ldo0 to HPM */
		rc = regulator_set_optimum_mode(ldo0, 100000);
		if (rc < 0)
			goto out;
	} else {
		/* set ldo0 to LPM */
		rc = regulator_set_optimum_mode(ldo0, 9000);
		if (rc < 0)
			goto out;
	}

	return 0;
out:
	regulator_disable(ldo0);
	regulator_put(ldo0);
	ldo0 = NULL;
	return rc;
}

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio = MDP_VSYNC_GPIO,
	.dsi_power_save   = mipi_dsi_panel_power,
};
#endif
#endif // CONFIG_FB_MSM_MIPI_DSI

#ifdef CONFIG_FB_MSM_TVOUT
static struct regulator *reg_8058_l13;

static int atv_dac_power(int on)
{
	int rc = 0;
	#define _GET_REGULATOR(var, name) do {				\
		var = regulator_get(NULL, name);			\
		if (IS_ERR(var)) {					\
			pr_info("'%s' regulator not found, rc=%ld\n",	\
				name, IS_ERR(var));			\
			var = NULL;					\
			return -ENODEV;					\
		}							\
	} while (0)

	if (!reg_8058_l13)
		_GET_REGULATOR(reg_8058_l13, "8058_l13");
	#undef _GET_REGULATOR

	if (on) {
		rc = regulator_set_voltage(reg_8058_l13, 2050000, 2050000);
		if (rc) {
			pr_info("%s: '%s' regulator set voltage failed,\
				rc=%d\n", __func__, "8058_l13", rc);
			return rc;
		}

		rc = regulator_enable(reg_8058_l13);
		if (rc) {
			pr_err("%s: '%s' regulator enable failed,\
				rc=%d\n", __func__, "8058_l13", rc);
			return rc;
		}
	} else {
		rc = regulator_force_disable(reg_8058_l13);
		if (rc)
			pr_warning("%s: '%s' regulator disable failed, rc=%d\n",
				__func__, "8058_l13", rc);
	}
	return rc;

}
#endif

#define MDP_VSYNC_GPIO			28
#ifdef CONFIG_FB_MSM_MIPI_DSI
int mdp_core_clk_rate_table[] = {
// [LS5] 2011.08.18 by lived
#if defined (CONFIG_FB_MSM_MIPI_DSI_SAMSUNG) || defined (CONFIG_FB_MSM_MIPI_DSI_SONY)
	128000000,
	128000000,
	177780000,
#else
	85330000,
	85330000,
	160000000,
#endif
	200000000,
};
#else
int mdp_core_clk_rate_table[] = {
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	85330000,
#else
	59080000,
#endif
	85330000,
	128000000,
	200000000,
};
#endif

static struct msm_panel_common_pdata mdp_pdata = {
	//.gpio = MDP_VSYNC_GPIO,
#ifdef CONFIG_FB_MSM_MIPI_DSI
	.mdp_core_clk_rate = 128000000,
#else
	.mdp_core_clk_rate = 59080000,
#endif
	.mdp_core_clk_table = mdp_core_clk_rate_table,
	.num_mdp_clk = ARRAY_SIZE(mdp_core_clk_rate_table),
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
};

#ifdef CONFIG_FB_MSM_TVOUT

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors atv_bus_init_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};
static struct msm_bus_vectors atv_bus_def_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 236390400,
		.ib = 265939200,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 236390400,
		.ib = 265939200,
	},
};
static struct msm_bus_paths atv_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(atv_bus_init_vectors),
		atv_bus_init_vectors,
	},
	{
		ARRAY_SIZE(atv_bus_def_vectors),
		atv_bus_def_vectors,
	},
};
static struct msm_bus_scale_pdata atv_bus_scale_pdata = {
	atv_bus_scale_usecases,
	ARRAY_SIZE(atv_bus_scale_usecases),
	.name = "atv",
};
#endif

static struct tvenc_platform_data atv_pdata = {
	.poll		 = 0,
	.pm_vid_en	 = atv_dac_power,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &atv_bus_scale_pdata,
#endif
};
#endif

static void __init msm_fb_add_devices(void)
{
#ifdef CONFIG_FB_MSM_LCDC_DSUB
	mdp_pdata.mdp_core_clk_table = NULL;
	mdp_pdata.num_mdp_clk = 0;
	mdp_pdata.mdp_core_clk_rate = 200000000;
#endif
	if (machine_is_msm8x60_rumi3())
		msm_fb_register_device("mdp", NULL);
	else
		msm_fb_register_device("mdp", &mdp_pdata);

#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
	msm_fb_register_device("lcdc", &lcdc_pdata);
	display_common_power(1);//kkcho
#endif
// 201110422, kkcho,  for MHL-power
#ifdef PANTECH_MHL_SII9244_POWER_CTRL
	//mhl_power_ctrl(1);  // 20110811, kkcho, remove
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI
#if defined (CONFIG_FB_MSM_MIPI_DSI_SAMSUNG) || defined (CONFIG_FB_MSM_MIPI_DSI_SONY)
	msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
#else
	msm_fb_register_device("mipi_dsi", NULL);
#endif
#if defined (CONFIG_FB_MSM_MIPI_DSI_SAMSUNG) || defined (CONFIG_FB_MSM_MIPI_DSI_SONY)
	init_power_gpio();
#endif
#if defined (CONFIG_FB_MSM_MIPI_DSI_SAMSUNG)
	//mipi_dsi_panel_power(1);
#endif
#endif

#ifdef CONFIG_MSM_BUS_SCALING
	msm_fb_register_device("dtv", &dtv_pdata);
#endif
#ifdef CONFIG_FB_MSM_TVOUT
	msm_fb_register_device("tvenc", &atv_pdata);
	msm_fb_register_device("tvout_device", NULL);
#endif
}

#ifdef CONFIG_PANTECH_BT //lsi@ps2.20110408 bluez by KSJ_device 2011_05_12

static unsigned bt_config_power_on[] = {
#if 1
	GPIO_CFG(128, 0, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_WAKE_MSM : Maoint */
	GPIO_CFG(138, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* MSM_WAKE_BT */
	GPIO_CFG(158, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_SYSRST */
#ifndef CONFIG_MACH_MSM8X60_PRESTO		// SWB-B42 EF39S/EF40S/EF40K
	GPIO_CFG(101, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_REG_ON */
#endif
	GPIO_CFG( 56, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG( 55, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG( 54, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Rx : Maoint */
	GPIO_CFG( 53, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(114, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_CLK */
	GPIO_CFG(113, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(112, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DOUT */
#else
	GPIO_CFG(128, 0, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_WAKE_MSM / BT_HOST_WAKE : Maoint */
	GPIO_CFG( 56, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG( 55, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG( 54, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Rx : Maoint */
	GPIO_CFG( 53, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(112, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(113, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(114, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_CLK */
	GPIO_CFG(138, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_WAKE_MSM / BT_EXT_WAKE*/
	GPIO_CFG(158, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* BT_SYSRST */
	/*	GPIO_CFG(101, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),*/	/* BT_REG_ON */
#endif
};

static unsigned bt_config_power_off[] = {
#if 1
	GPIO_CFG(128, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* BT_WAKE_MSM : Maoint */
	GPIO_CFG(138, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* MSM_WAKE_BT */
	GPIO_CFG(158, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* BT_SYSRST */
#ifndef CONFIG_MACH_MSM8X60_PRESTO		//SWB-B42 EF39S/EF40S/EF40K
	GPIO_CFG(101, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* BT_REG_ON */
#endif
	GPIO_CFG( 56, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG( 55, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG( 54, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Rx : Maoint */
	GPIO_CFG( 53, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(114, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_CLK */
	GPIO_CFG(113, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(112, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(111, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DOUT */
#else
	GPIO_CFG(128, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* BT_WAKE_MSM / BT_HOST_WAKE : Maoint */
	GPIO_CFG( 56, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG( 55, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG( 54, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Rx : Maoint */
	GPIO_CFG( 53, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(111, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(112, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(113, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(114, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_CLK */
	GPIO_CFG(138, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* MSM_WAKE_BT / BT_EXT_WAKE */
	GPIO_CFG(158, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* BT_SYSRST */
	/*	GPIO_CFG(158, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),*/ /* BT_SYSRST */
	/*	GPIO_CFG(101, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),*/	/* BT_REG_ON */
#endif
};


static int bluetooth_power(int on)
{
	int pin, rc;

	printk(KERN_ERR "BT - %s: on(%d)\n",__func__, on);

	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
			rc = gpio_tlmm_config(bt_config_power_on[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, bt_config_power_on[pin], rc);
				return -EIO;
			}
		}
#if 1 //SWB-B42
		gpio_set_value(158, on); /* BT_SYSRST */
#ifndef CONFIG_MACH_MSM8X60_PRESTO	//SWB-B42 EF39S/EF40S/EF40K
		gpio_set_value(101, on); /* BT_REG_ON */
#endif
		mdelay(20);
		gpio_set_value(158, 0); /* BT_SYSRST */
#ifndef CONFIG_MACH_MSM8X60_PRESTO	//SWB-B42 EF39S/EF40S/EF40K
		gpio_set_value(101, 0); /* BT_REG_ON */
#endif
		mdelay(100);
		gpio_set_value(158, on); /* BT_SYSRST */
		mdelay(4);
#ifndef CONFIG_MACH_MSM8X60_PRESTO	//SWB-B42 EF39S/EF40S/EF40K
		gpio_set_value(101, on); /* BT_REG_ON */
#endif
		mdelay(100);
		printk(KERN_ERR "BT - %s: BT_REG_ON/BT_SYSRST on(%d) \n",__func__, on);
#else  //SWB-B42
		gpio_set_value(158, on); /* BT_SYSRST */
		mdelay(10);
		gpio_set_value(158, 0); /* BT_SYSRST */
		mdelay(10);
		gpio_set_value(158, on); /* BT_SYSRST */
#endif  //SWB-B42
	} else {
		gpio_set_value(158, on); /* BT_SYSRST */
#ifndef CONFIG_MACH_MSM8X60_PRESTO	//SWB-B42 EF39S/EF40S/EF40K
		gpio_set_value(101, on); /* BT_REG_ON */
#endif
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off); pin++) {
			rc = gpio_tlmm_config(bt_config_power_off[pin], GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
						__func__, bt_config_power_off[pin], rc);
				return -EIO;
			}
		}
	}
	printk(KERN_ERR "BT - %s: on(%d) Done!! \n",__func__, on);
	return 0;
}

static void __init bt_power_init(void)
{
	printk(KERN_ERR "BT - %s: \n",__func__);
	bluetooth_power(0);
	msm_bt_power_device.dev.platform_data = &bluetooth_power;
}

#else //CONFIG_PANTECH_BT

#if (defined(CONFIG_MARIMBA_CORE)) && \
	(defined(CONFIG_MSM_BT_POWER) || defined(CONFIG_MSM_BT_POWER_MODULE))

static const struct {
	char *name;
	int vmin;
	int vmax;
} bt_regs_info[] = {
	{ "8058_s3", 1800000, 1800000 },
	{ "8058_s2", 1300000, 1300000 },
	{ "8058_l8", 2900000, 3050000 },
};

static struct {
	bool enabled;
} bt_regs_status[] = {
	{ false },
	{ false },
	{ false },
};
static struct regulator *bt_regs[ARRAY_SIZE(bt_regs_info)];

static int bahama_bt(int on)
{
	int rc;
	int i;
	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};

	struct bahama_variant_register {
		const size_t size;
		const struct bahama_config_register *set;
	};

	const struct bahama_config_register *p;

	u8 version;

	const struct bahama_config_register v10_bt_on[] = {
		{ 0xE9, 0x00, 0xFF },
		{ 0xF4, 0x80, 0xFF },
		{ 0xE4, 0x00, 0xFF },
		{ 0xE5, 0x00, 0x0F },
#ifdef CONFIG_WLAN
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0xE9, 0x21, 0xFF },
		{ 0x01, 0x0C, 0x1F },
		{ 0x01, 0x08, 0x1F },
	};

	const struct bahama_config_register v20_bt_on_fm_off[] = {
		{ 0x11, 0x0C, 0xFF },
		{ 0x13, 0x01, 0xFF },
		{ 0xF4, 0x80, 0xFF },
		{ 0xF0, 0x00, 0xFF },
		{ 0xE9, 0x00, 0xFF },
#ifdef CONFIG_WLAN
		{ 0x81, 0x00, 0x7F },
		{ 0x82, 0x00, 0xFF },
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0xE9, 0x21, 0xFF },
	};

	const struct bahama_config_register v20_bt_on_fm_on[] = {
		{ 0x11, 0x0C, 0xFF },
		{ 0x13, 0x01, 0xFF },
		{ 0xF4, 0x86, 0xFF },
		{ 0xF0, 0x06, 0xFF },
		{ 0xE9, 0x00, 0xFF },
#ifdef CONFIG_WLAN
		{ 0x81, 0x00, 0x7F },
		{ 0x82, 0x00, 0xFF },
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0xE9, 0x21, 0xFF },
	};

	const struct bahama_config_register v10_bt_off[] = {
		{ 0xE9, 0x00, 0xFF },
	};

	const struct bahama_config_register v20_bt_off_fm_off[] = {
		{ 0xF4, 0x84, 0xFF },
		{ 0xF0, 0x04, 0xFF },
		{ 0xE9, 0x00, 0xFF }
	};

	const struct bahama_config_register v20_bt_off_fm_on[] = {
		{ 0xF4, 0x86, 0xFF },
		{ 0xF0, 0x06, 0xFF },
		{ 0xE9, 0x00, 0xFF }
	};
	const struct bahama_variant_register bt_bahama[2][3] = {
		{
			{ ARRAY_SIZE(v10_bt_off), v10_bt_off },
			{ ARRAY_SIZE(v20_bt_off_fm_off), v20_bt_off_fm_off },
			{ ARRAY_SIZE(v20_bt_off_fm_on), v20_bt_off_fm_on }
		},
		{
			{ ARRAY_SIZE(v10_bt_on), v10_bt_on },
			{ ARRAY_SIZE(v20_bt_on_fm_off), v20_bt_on_fm_off },
			{ ARRAY_SIZE(v20_bt_on_fm_on), v20_bt_on_fm_on }
		}
	};

	u8 offset = 0; /* index into bahama configs */

	on = on ? 1 : 0;
	version = read_bahama_ver();

	if (version ==  VER_UNSUPPORTED) {
		dev_err(&msm_bt_power_device.dev,
			"%s: unsupported version\n",
			__func__);
		return -EIO;
	}

	if (version == VER_2_0) {
		if (marimba_get_fm_status(&config))
			offset = 0x01;
	}

	/* Voting off 1.3V S2 Regulator,BahamaV2 used in Normal mode */
	if (on && (version == VER_2_0)) {
		for (i = 0; i < ARRAY_SIZE(bt_regs_info); i++) {
			if ((!strcmp(bt_regs_info[i].name, "8058_s2"))
				&& (bt_regs_status[i].enabled == true)) {
				if (regulator_disable(bt_regs[i])) {
					dev_err(&msm_bt_power_device.dev,
						"%s: regulator disable failed",
						__func__);
				}
				bt_regs_status[i].enabled = false;
				break;
			}
		}
	}

	p = bt_bahama[on][version + offset].set;

	dev_info(&msm_bt_power_device.dev,
		"%s: found version %d\n", __func__, version);

	for (i = 0; i < bt_bahama[on][version + offset].size; i++) {
		u8 value = (p+i)->value;
		rc = marimba_write_bit_mask(&config,
			(p+i)->reg,
			&value,
			sizeof((p+i)->value),
			(p+i)->mask);
		if (rc < 0) {
			dev_err(&msm_bt_power_device.dev,
				"%s: reg %d write failed: %d\n",
				__func__, (p+i)->reg, rc);
			return rc;
		}
		dev_dbg(&msm_bt_power_device.dev,
			"%s: reg 0x%02x write value 0x%02x mask 0x%02x\n",
				__func__, (p+i)->reg,
				value, (p+i)->mask);
	}
	/* Update BT Status */
	if (on)
		marimba_set_bt_status(&config, true);
	else
		marimba_set_bt_status(&config, false);

	return 0;
}

static int bluetooth_use_regulators(int on)
{
	int i, recover = -1, rc = 0;

	for (i = 0; i < ARRAY_SIZE(bt_regs_info); i++) {
		bt_regs[i] = on ? regulator_get(&msm_bt_power_device.dev,
						bt_regs_info[i].name) :
				(regulator_put(bt_regs[i]), NULL);
		if (IS_ERR(bt_regs[i])) {
			rc = PTR_ERR(bt_regs[i]);
			dev_err(&msm_bt_power_device.dev,
				"regulator %s get failed (%d)\n",
				bt_regs_info[i].name, rc);
			recover = i - 1;
			bt_regs[i] = NULL;
			break;
		}

		if (!on)
			continue;

		rc = regulator_set_voltage(bt_regs[i],
					  bt_regs_info[i].vmin,
					  bt_regs_info[i].vmax);
		if (rc < 0) {
			dev_err(&msm_bt_power_device.dev,
				"regulator %s voltage set (%d)\n",
				bt_regs_info[i].name, rc);
			recover = i;
			break;
		}
	}

	if (on && (recover > -1))
		for (i = recover; i >= 0; i--) {
			regulator_put(bt_regs[i]);
			bt_regs[i] = NULL;
		}

	return rc;
}

static int bluetooth_switch_regulators(int on)
{
	int i, rc = 0;

	for (i = 0; i < ARRAY_SIZE(bt_regs_info); i++) {
		if (on && (bt_regs_status[i].enabled == false)) {
			rc = regulator_enable(bt_regs[i]);
			if (rc < 0) {
				dev_err(&msm_bt_power_device.dev,
					"regulator %s %s failed (%d)\n",
					bt_regs_info[i].name,
					"enable", rc);
				if (i > 0) {
					while (--i) {
						regulator_disable(bt_regs[i]);
						bt_regs_status[i].enabled
								 = false;
					}
					break;
				}
			}
			bt_regs_status[i].enabled = true;
		} else if (!on && (bt_regs_status[i].enabled == true)) {
			rc = regulator_disable(bt_regs[i]);
			if (rc < 0) {
				dev_err(&msm_bt_power_device.dev,
					"regulator %s %s failed (%d)\n",
					bt_regs_info[i].name,
					"disable", rc);
				break;
			}
			bt_regs_status[i].enabled = false;
		}
	}
	return rc;
}

static struct msm_xo_voter *bt_clock;

static int bluetooth_power(int on)
{
	int rc = 0;
	int id;

	/* In case probe function fails, cur_connv_type would be -1 */
	id = adie_get_detected_connectivity_type();
	if (id != BAHAMA_ID) {
		pr_err("%s: unexpected adie connectivity type: %d\n",
			__func__, id);
		return -ENODEV;
	}

	if (on) {

		rc = bluetooth_use_regulators(1);
		if (rc < 0)
			goto out;

		rc = bluetooth_switch_regulators(1);

		if (rc < 0)
			goto fail_put;

		bt_clock = msm_xo_get(MSM_XO_TCXO_D0, "bt_power");

		if (IS_ERR(bt_clock)) {
			pr_err("Couldn't get TCXO_D0 voter\n");
			goto fail_switch;
		}

		rc = msm_xo_mode_vote(bt_clock, MSM_XO_MODE_ON);

		if (rc < 0) {
			pr_err("Failed to vote for TCXO_DO ON\n");
			goto fail_vote;
		}

		rc = bahama_bt(1);

		if (rc < 0)
			goto fail_clock;

		msleep(10);

		rc = msm_xo_mode_vote(bt_clock, MSM_XO_MODE_PIN_CTRL);

		if (rc < 0) {
			pr_err("Failed to vote for TCXO_DO pin control\n");
			goto fail_vote;
		}
	} else {
		/* check for initial RFKILL block (power off) */
		/* some RFKILL versions/configurations rfkill_register */
		/* calls here for an initial set_block */
		/* avoid calling i2c and regulator before unblock (on) */
		if (platform_get_drvdata(&msm_bt_power_device) == NULL) {
			dev_info(&msm_bt_power_device.dev,
				"%s: initialized OFF/blocked\n", __func__);
			goto out;
		}

		bahama_bt(0);

fail_clock:
		msm_xo_mode_vote(bt_clock, MSM_XO_MODE_OFF);
fail_vote:
		msm_xo_put(bt_clock);
fail_switch:
		bluetooth_switch_regulators(0);
fail_put:
		bluetooth_use_regulators(0);
	}

out:
	if (rc < 0)
		on = 0;
	dev_info(&msm_bt_power_device.dev,
		"Bluetooth power switch: state %d result %d\n", on, rc);

	return rc;
}

#endif /*CONFIG_MARIMBA_CORE, CONFIG_MSM_BT_POWER, CONFIG_MSM_BT_POWER_MODULE*/
#endif //CONFIG_PANTECH_BT

#ifdef CONFIG_PN544
static unsigned nfc_config_init[] = {
	GPIO_CFG(NFC_WAKEUP_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(NFC_ENABLE_GPIO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(NFC_FW_DL_GPIO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	
/* check i2c : remove the comment, you can test i2c */
//	GPIO_CFG(NFC_WAKEUP_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
//	GPIO_CFG(NFC_ENABLE_GPIO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};


static void __init nfc_power_init(void)
{

	int rc = 0;
	int pin = 0;

	for (pin = 0; pin < ARRAY_SIZE(nfc_config_init); pin++) {
		rc = gpio_tlmm_config(nfc_config_init[pin], GPIO_CFG_ENABLE);
		if (rc) {
			return;
		}
	}
/* check i2c : remove the comment, you can test i2c */
//    gpio_set_value(NFC_ENABLE_GPIO, 1);
    return;
}
#endif /* CONFIG_PN544 */

static void __init msm8x60_cfg_smsc911x(void)
{
	smsc911x_resources[1].start =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
	smsc911x_resources[1].end =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
}

#ifdef CONFIG_MSM_RPM
static struct msm_rpm_platform_data msm_rpm_data = {
	.reg_base_addrs = {
		[MSM_RPM_PAGE_STATUS] = MSM_RPM_BASE,
		[MSM_RPM_PAGE_CTRL] = MSM_RPM_BASE + 0x400,
		[MSM_RPM_PAGE_REQ] = MSM_RPM_BASE + 0x600,
		[MSM_RPM_PAGE_ACK] = MSM_RPM_BASE + 0xa00,
	},

	.irq_ack = RPM_SCSS_CPU0_GP_HIGH_IRQ,
	.irq_err = RPM_SCSS_CPU0_GP_LOW_IRQ,
	.irq_vmpm = RPM_SCSS_CPU0_GP_MEDIUM_IRQ,
	.msm_apps_ipc_rpm_reg = MSM_GCC_BASE + 0x008,
	.msm_apps_ipc_rpm_val = 4,
};
#endif

void msm_fusion_setup_pinctrl(void)
{
	struct msm_xo_voter *a1;

	if (socinfo_get_platform_subtype() == 0x3) {
		/*
		 * Vote for the A1 clock to be in pin control mode before
		* the external images are loaded.
		*/
		a1 = msm_xo_get(MSM_XO_TCXO_A1, "mdm");
		BUG_ON(!a1);
		msm_xo_mode_vote(a1, MSM_XO_MODE_PIN_CTRL);
	}
}

struct msm_board_data {
	struct msm_gpiomux_configs *gpiomux_cfgs;
};

static struct msm_board_data msm8x60_rumi3_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_surf_ffa_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_sim_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_surf_ffa_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_surf_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_surf_ffa_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_ffa_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_surf_ffa_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_fluid_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_fluid_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_charm_surf_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_charm_gpiomux_cfgs,
};

static struct msm_board_data msm8x60_charm_ffa_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_charm_gpiomux_cfgs,
};
#ifdef CONFIG_SKY_BATTERY_MAX17040  // p14682 kobj 110607
static unsigned max17040_config_gpio[] = {
	/* FUEL_I2C_SCL */
	GPIO_CFG(103, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/* FUEL_I2C_SDA */
	GPIO_CFG(104, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static void max17040_init(void) //ps2 team shs : add filed
{
	int rc = 0;
	int pin = 0;
	printk(KERN_ERR "%s: \n",__func__);
	for (pin = 0; pin < ARRAY_SIZE(max17040_config_gpio); pin++) {
		rc = gpio_tlmm_config(max17040_config_gpio[pin], GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
			__func__, max17040_config_gpio[pin], rc);
		}
	}
}

#endif //CONFIG_SKY_BATTERY_MAX17040

//pz1946 20110907 interrupt pin change
#ifdef CONFIG_SKY_SMB_CHARGER
static unsigned smb137b_config_gpio[] = {
        GPIO_CFG(SC_STAT_IRQ_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),       
};

static void smb137b_init(void) 
{
        int rc = 0;
        int pin = 0;
        printk(KERN_ERR "%s: \n",__func__);
        for (pin = 0; pin < ARRAY_SIZE(smb137b_config_gpio); pin++) {
                rc = gpio_tlmm_config(smb137b_config_gpio[pin], GPIO_CFG_ENABLE);
                if (rc) {
                        printk(KERN_ERR "%s: ~~~~pz1946 gpio_tlmm_config(%#x)=%d\n",
                        __func__, smb137b_config_gpio[pin], rc);
                }
        }
}
#endif

static void __init msm8x60_init(struct msm_board_data *board_data)
{
	uint32_t soc_platform_version;

#if 0//def CONFIG_FB_MSM_MIPI_DSI
	uint32_t buf_size = 1280*800*3;
	void *lk_buf;
	lk_buf = ioremap_nocache(0x43E00000, buf_size);
	memset(lk_buf, 0x0, buf_size);
#endif
	/*
	 * Initialize RPM first as other drivers and devices may need
	 * it for their initialization.
	 */
#ifdef CONFIG_MSM_RPM
	BUG_ON(msm_rpm_init(&msm_rpm_data));
#endif
	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");

	if (socinfo_init() < 0)
		printk(KERN_ERR "%s: socinfo_init() failed!\n",
		       __func__);
#ifdef CONFIG_MSM_KGSL_2D
	msm8x60_check_2d_hardware();
#endif

	/* Change SPM handling of core 1 if PMM 8160 is present. */
	soc_platform_version = socinfo_get_platform_version();
	if (SOCINFO_VERSION_MAJOR(soc_platform_version) == 1 &&
			SOCINFO_VERSION_MINOR(soc_platform_version) >= 2) {
		struct msm_spm_platform_data *spm_data;

		spm_data = &msm_spm_data_v1[1];
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] &= ~0x0F00UL;
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] |= 0x0100UL;

		spm_data = &msm_spm_data[1];
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] &= ~0x0F00UL;
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] |= 0x0100UL;
	}

	/*
	 * Initialize SPM before acpuclock as the latter calls into SPM
	 * driver to set ACPU voltages.
	 */
	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
		msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	else
		msm_spm_init(msm_spm_data_v1, ARRAY_SIZE(msm_spm_data_v1));

	/*
	 * Set regulators 8901_l4 and 8901_l6 to be always on in HPM for SURF
	 * devices so that the RPM doesn't drop into a low power mode that an
	 * un-reworked SURF cannot resume from.
	 */
	if (machine_is_msm8x60_surf()) {
		rpm_vreg_init_pdata[RPM_VREG_ID_PM8901_L4]
			.init_data.constraints.always_on = 1;
		rpm_vreg_init_pdata[RPM_VREG_ID_PM8901_L6]
			.init_data.constraints.always_on = 1;
	}

	/*
	 * Disable regulator info printing so that regulator registration
	 * messages do not enter the kmsg log.
	 */
	regulator_suppress_info_printing();

	/* Initialize regulators needed for clock_init. */
	platform_add_devices(early_regulators, ARRAY_SIZE(early_regulators));

	msm_clock_init(msm_clocks_8x60, msm_num_clocks_8x60);

	/* Buses need to be initialized before early-device registration
	 * to get the platform data for fabrics.
	 */
	msm8x60_init_buses();
	platform_add_devices(early_devices, ARRAY_SIZE(early_devices));
	/* CPU frequency control is not supported on simulated targets. */
	if (!machine_is_msm8x60_rumi3() && !machine_is_msm8x60_sim())
		msm_acpu_clock_init(&msm8x60_acpu_clock_data);

	/* No EBI2 on 8660 charm targets */
	if (!machine_is_msm8x60_charm_surf() && !machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			&& !machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			&& !machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			&& !machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			&& !machine_is_msm8x60_presto()
#endif
	)
	msm8x60_init_ebi2();
	msm8x60_init_tlmm();
	msm8x60_init_gpiomux(board_data->gpiomux_cfgs);
	msm8x60_init_uart12dm();
	msm8x60_init_mmc();

#if defined(CONFIG_PMIC8058_OTHC) || defined(CONFIG_PMIC8058_OTHC_MODULE)
	msm8x60_init_pm8058_othc();
#endif

	if (machine_is_msm8x60_fluid()) {
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_KPD].
			platform_data = &fluid_keypad_data;
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_KPD].data_size
			= sizeof(fluid_keypad_data);
	} else {
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_KPD].
			platform_data = &ffa_keypad_data;
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_KPD].data_size
			= sizeof(ffa_keypad_data);
	}

	/* Disable END_CALL simulation function of powerkey on fluid */
	if (machine_is_msm8x60_fluid()) {
		pwrkey_pdata.pwrkey_time_ms = 0;
	}

#ifdef CONFIG_USB_ANDROID
	if (machine_is_msm8x60_charm_surf() ||
			machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			) {
#ifndef CONFIG_USB_ANDROID_PANTECH
		android_usb_pdata.product_id = 0x9032;
		android_usb_pdata.functions = charm_usb_functions_all,
		android_usb_pdata.num_functions =
				ARRAY_SIZE(charm_usb_functions_all);
		android_usb_pdata.products = charm_usb_products;
		android_usb_pdata.num_products =
					ARRAY_SIZE(charm_usb_products);

		/* change pid if h/w is of type svlte-2*/
		if (socinfo_get_platform_subtype() == 3) {
			android_usb_pdata.product_id = 0x9038;
			android_usb_pdata.functions = svlte2_usb_functions_all,
			android_usb_pdata.num_functions =
				ARRAY_SIZE(svlte2_usb_functions_all);
			android_usb_pdata.products = svlte2_usb_products;
			android_usb_pdata.num_products =
					ARRAY_SIZE(svlte2_usb_products);
		}
#endif /*CONFIG_USB_ANDROID_PANTECH*/
	}
#endif

	if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() ||
	    machine_is_msm8x60_fluid() || machine_is_msm8x60_charm_surf() ||
	    machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
	    ) {
		msm8x60_cfg_smsc911x();
		if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
			platform_add_devices(msm_footswitch_devices,
					     msm_num_footswitch_devices);
		platform_add_devices(surf_devices,
				     ARRAY_SIZE(surf_devices));

#ifdef CONFIG_MSM_DSPS
		if (machine_is_msm8x60_fluid()) {
			platform_device_unregister(&msm_gsbi12_qup_i2c_device);
			msm8x60_init_dsps();
		}
#endif

#ifdef CONFIG_USB_EHCI_MSM_72K
	/*
	 * Drive MPP2 pin HIGH for PHY to generate ID interrupts on 8660
	 * fluid
	 */
	if (machine_is_msm8x60_fluid()) {
		pm8901_mpp_config_digital_out(1,
			PM8901_MPP_DIG_LEVEL_L5, 1);
	}
		msm_add_host(0, &msm_usb_host_pdata);
#endif
	} else {
		msm8x60_configure_smc91x();
		platform_add_devices(rumi_sim_devices,
				     ARRAY_SIZE(rumi_sim_devices));
	}
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa())
		msm8x60_cfg_isp1763();
#endif
#ifdef CONFIG_BATTERY_MSM8X60
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() ||
		machine_is_msm8x60_charm_surf() ||
		machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
		|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
		|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
		|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
		|| machine_is_msm8x60_presto()
#endif
		)
		platform_device_register(&msm_charger_device);
#endif

	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40S
			|| machine_is_msm8x60_ef40s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
			)
		platform_add_devices(charm_devices, ARRAY_SIZE(charm_devices));

	if (!machine_is_msm8x60_fluid())
		pm8058_platform_data.charger_sub_device
			= &pm8058_charger_sub_dev;

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
	if (machine_is_msm8x60_fluid())
{
#ifndef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020  // jmlee 20110505 add
		platform_device_register(&msm_gsbi10_qup_spi_device);
#endif
}
	else
		platform_device_register(&msm_gsbi1_qup_spi_device);
#endif

#if defined(CONFIG_SKY_TDMB_SPI_QUP)
	if (machine_is_msm8x60_fluid())
		platform_device_register(&msm_gsbi2_qup_spi_device);
#endif

//#ifdef CONFIG_SKY_TDMB_SPI_GPIO
//	platform_device_register(&gpio_spi_tdmb_device);
//#endif

#if defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C) || \
		defined(CONFIG_TOUCHSCREEN_CYTTSP_I2C_MODULE)
	if (machine_is_msm8x60_fluid())
		cyttsp_set_params();
#endif
	if (!machine_is_msm8x60_sim())
		msm_fb_add_devices();
	fixup_i2c_configs();
	register_i2c_devices();

	platform_device_register(&smsc911x_device);

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200  // jmlee 20110505 add
        snd_subsystem_Init();
#endif

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 //jmlee
        snddev_PAN_audio_gpio_init();
        printk( "[Snd_audience_a2020][-=AUDIO=-] i2c_register_board_info !!!\n");
        i2c_register_board_info(MSM_AUDIENCE_A2020_I2C_BUS_ID, msm_audience_i2c_board_info,
	 		ARRAY_SIZE(msm_audience_i2c_board_info));
#if (BOARD_REV > WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
        snd_audience_a2020_api_Init();
#endif
#endif

#ifdef LCD_BL_I2C_CONTROL
	//led_device_i2c_init();
	i2c_register_board_info(MSM_LED_I2C_BUS_ID, i2c_led_devices,
			ARRAY_SIZE(i2c_led_devices));
#endif /* LCD_BL_I2C_CONTROL */

//---	added by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	mpu3050_power_up();
#endif
#if defined(CONFIG_MACH_MSM8X60_PRESTO)
	sensors_power_up();
#endif
//---	edited by hkkwon@PS2
#if defined(CONFIG_EF39S_SENSORS_APDS9900_HW) || defined(CONFIG_EF39S_SENSORS_APDS9900_SW) || \
	defined(CONFIG_EF40K_SENSORS_APDS9900_HW) || defined(CONFIG_EF40K_SENSORS_APDS9900_SW) || \
	defined(CONFIG_EF40S_SENSORS_APDS9900_HW) || defined(CONFIG_EF40S_SENSORS_APDS9900_SW)
	proximity_sensor_power_up();
#endif

#if defined(CONFIG_INPUT_SENSOR)
	sensors_hw_init();

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
	i2c_register_board_info(I2C_DEV_INDEX_GEOMAGNETIC, geomagnetic_i2c_info,
			ARRAY_SIZE(geomagnetic_i2c_info));
	i2c_register_board_info(I2C_DEV_INDEX_ACCELEROMETER, accelerometer_i2c_info,
			ARRAY_SIZE(accelerometer_i2c_info));
#else
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV < TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV < WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV < WS20))
	i2c_register_board_info(I2C_DEV_INDEX_GYROSCOPE, gyroscope_i2c_info,
			ARRAY_SIZE(gyroscope_i2c_info));
#endif
#endif

	i2c_register_board_info(I2C_DEV_INDEX_PROXIMITY, proximity_i2c_info,
			ARRAY_SIZE(proximity_i2c_info));
#endif

#ifdef MHL_I2C_CONTROL
	// MHL-IC PIN-config
	i2c_register_board_info(MSM_MHL_I2C_BUS_ID, i2c_mhl_devices,
			ARRAY_SIZE(i2c_mhl_devices));
#endif
#ifdef CONFIG_TOUCHSCREEN_MELFAS_TKI
	i2c_register_board_info(MSM_TKI_I2C_BUS_ID, i2c_tki_devices,
			ARRAY_SIZE(i2c_tki_devices));
#endif /* CONFIG_TOUCHSCREEN_MELFAS_TKI */
#if (defined(CONFIG_SPI_QUP)) && \
	(defined(CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT) || \
	defined(CONFIG_FB_MSM_LCDC_AUO_WVGA))

	if (machine_is_msm8x60_fluid()) {
#ifdef CONFIG_FB_MSM_LCDC_SAMSUNG_OLED_PT
		if (SOCINFO_VERSION_MAJOR(soc_platform_version) < 3) {
			spi_register_board_info(lcdc_samsung_spi_board_info,
				ARRAY_SIZE(lcdc_samsung_spi_board_info));
		} else
#endif
		{
#ifdef CONFIG_FB_MSM_LCDC_AUO_WVGA
			spi_register_board_info(lcdc_auo_spi_board_info,
				ARRAY_SIZE(lcdc_auo_spi_board_info));
#endif
		}
	}
#endif

#ifdef CONFIG_SKY_TDMB_SPI_IF
	spi_register_board_info(tdmb_spi_board_info, ARRAY_SIZE(tdmb_spi_board_info));
#endif

	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
	msm_cpuidle_set_states(msm_cstates, ARRAY_SIZE(msm_cstates),
				msm_pm_data);

#ifdef CONFIG_SENSORS_MSM_ADC
	if (machine_is_msm8x60_fluid()) {
		msm_adc_pdata.dev_names = msm_adc_fluid_device_names;
		msm_adc_pdata.num_adc = ARRAY_SIZE(msm_adc_fluid_device_names);
		if (SOCINFO_VERSION_MAJOR(soc_platform_version) < 3)
			msm_adc_pdata.gpio_config = APROC_CONFIG;
		else
			msm_adc_pdata.gpio_config = MPROC_CONFIG;
	}
	msm_adc_pdata.target_hw = MSM_8x60;
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	msm_snddev_init();
#endif
#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)
	if (machine_is_msm8x60_fluid())
		platform_device_register(&fluid_leds_gpio);
	else
		platform_device_register(&gpio_leds);
#ifdef CONFIG_SKY_BATTERY_MAX17040  // p14682 kobj 110607
	max17040_init();
#endif //CONFIG_SKY_BATTERY_MAX17040
//pz1946 20110907 interrupt pin change
#ifdef CONFIG_SKY_SMB_CHARGER
	smb137b_init();
#endif
#ifdef CONFIG_PANTECH_BT
	bt_power_init();
#endif // CONFIG_PANTECH_BT
#endif

#ifdef CONFIG_SKY_WLAN
#ifdef CONFIG_WIFI_CONTROL_FUNC
	msm8x60_wlan_init();
#else
	wlan_init();
#endif //CONFIG_WIFI_CONTROL_FUNC
#endif //CONFIG_SKY_WLAN

#if defined(CONFIG_PN544)
    nfc_power_init();
#endif

	/* configure pmic leds */
	if (machine_is_msm8x60_fluid()) {
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_LED].
			platform_data = &pm8058_fluid_flash_leds_data;
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_LED].data_size
			= sizeof(pm8058_fluid_flash_leds_data);
	} else {
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_LED].
			platform_data = &pm8058_flash_leds_data;
		pm8058_platform_data.sub_devices[PM8058_SUBDEV_LED].data_size
			= sizeof(pm8058_flash_leds_data);
	}

	msm8x60_multi_sdio_init();

	if (machine_is_msm8x60_charm_surf() || machine_is_msm8x60_charm_ffa())
		msm_fusion_setup_pinctrl();
}

static void __init msm8x60_rumi3_init(void)
{
	msm8x60_init(&msm8x60_rumi3_board_data);
}

static void __init msm8x60_sim_init(void)
{
	msm8x60_init(&msm8x60_sim_board_data);
}

static void __init msm8x60_surf_init(void)
{
	msm8x60_init(&msm8x60_surf_board_data);
}

static void __init msm8x60_ffa_init(void)
{
	msm8x60_init(&msm8x60_ffa_board_data);
}

static void __init msm8x60_fluid_init(void)
{
	msm8x60_init(&msm8x60_fluid_board_data);
}

static void __init msm8x60_charm_surf_init(void)
{
	msm8x60_init(&msm8x60_charm_surf_board_data);
}

static void __init msm8x60_charm_ffa_init(void)
{
	msm8x60_init(&msm8x60_charm_ffa_board_data);
}
MACHINE_START(MSM8X60_RUMI3, "QCT MSM8X60 RUMI3")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io  = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_rumi3_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_SIM, "QCT MSM8X60 SIMULATOR")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io  = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_sim_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_SURF, "QCT MSM8X60 SURF")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_surf_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_FFA, "QCT MSM8X60 FFA")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_ffa_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_FLUID, "QCT MSM8X60 FLUID")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_fluid_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_CHARM_SURF, "QCT MSM8X60 CHARM SURF")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_surf_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_CHARM_FFA, "QCT MSM8X60 CHARM FFA")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_ffa_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_EF39S, "PANTECH MSM8X60 EF39S")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_ffa_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_EF40S, "PANTECH MSM8X60 EF40S")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_ffa_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_EF40K, "PANTECH MSM8X60 EF40K")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_ffa_init,
	.timer = &msm_timer,
MACHINE_END

MACHINE_START(MSM8X60_PRESTO, "PANTECH MSM8X60 PRESTO")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.map_io = msm8x60_map_io,
	.init_irq = msm8x60_init_irq,
	.init_machine = msm8x60_charm_ffa_init,
	.timer = &msm_timer,
MACHINE_END
