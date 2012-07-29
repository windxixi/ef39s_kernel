/* Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_sony.h"
#include <mach/gpio.h>
#include <asm/irq.h>
#include <asm/system.h>

#define GPIO_HIGH_VALUE 1
#define GPIO_LOW_VALUE  0

#define NOP()	do {asm volatile ("NOP");} while(0);
#define DELAY_3NS() do { \
    asm volatile ("NOP"); \
    asm volatile ("NOP"); \
    asm volatile ("NOP");} while(0);

#define LCD_RESET      50
#define LCD_BL_EN      13
#define LCD_POWER      49
#define LCD_DEBUG_MSG

#ifdef LCD_DEBUG_MSG
#define ENTER_FUNC()        printk(KERN_INFO "[SKY_LCD] +%s \n", __FUNCTION__);
#define EXIT_FUNC()         printk(KERN_INFO "[SKY_LCD] -%s \n", __FUNCTION__);
#define ENTER_FUNC2()       printk(KERN_ERR "[SKY_LCD] +%s\n", __FUNCTION__);
#define EXIT_FUNC2()        printk(KERN_ERR "[SKY_LCD] -%s\n", __FUNCTION__);
#define PRINT(fmt, args...) printk(KERN_INFO fmt, ##args)
#define DEBUG_EN 1
#else
#define PRINT(fmt, args...)
#define ENTER_FUNC2()
#define EXIT_FUNC2()
#define ENTER_FUNC()
#define EXIT_FUNC()
#define DEBUG_EN 0
#endif

static struct msm_panel_common_pdata *mipi_sony_pdata;

static struct dsi_buf sony_tx_buf;
static struct dsi_buf sony_rx_buf;

static uint32_t lcd_gpio_init_table[] = {
	GPIO_CFG(LCD_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(LCD_BL_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),    
};

struct lcd_state_type {
    boolean disp_powered_up;
    boolean disp_initialized;
    boolean disp_on;
};

static struct lcd_state_type sony_state = { 0, };

static void lcd_gpio_init(uint32_t *table, int len, unsigned enable)
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

//EF40K REGISTER SET 250Mbps
#if defined (MIPI_CLOCK_250MBPS)
char passwd1[3]     = {0xF0, 0x5A, 0x5A};
char passwd2[3]     = {0xF1, 0x5A, 0x5A};
char id1wr[2]       = {0xB1, 0x00};
char id2wr[2]       = {0xB2, 0x00};
char id3wr[2]       = {0xB3, 0x00};
char miectl_1[4]    = {0xC0, 0x80, 0x90, 0x10};
char bcmode[2]      = {0xC1, 0x01};
char miectl_2[2]    = {0xC2, 0x08};
char wrblctl[4]     = {0xC3, 0x00, 0x00, 0x20};
char source_ctl[4]     = {0xF2, 0x03, 0x33, 0x81};
char power_ctl_1[13]     = {0xF4, 0x0A, 0x0B, 0x07, 0x07, 0x20, 0x14, 0x0D, 0x0C, 0xB9, 0x00, 0x33, 0x33};
char power_ctl_2[21]     = {0xF6, 0x02, 0x11, 0x0F, 0x25, 0x0A, 0x00, 0x13, 0x22, 0x1B, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x08, 0x0C, 0x08, 0x51};
char positive_gamma[64]     = {0xFA, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A, 0x2B, 0x27,
	0x23, 0x28, 0x2B, 0x2E, 0x32, 0x37, 0x3C, 0x3F, 0x3F, 0x3F,
	0x26, 0x00, 0x3F, 0x29, 0x27, 0x34, 0x34, 0x2D, 0x2C, 0x2C,
	0x28, 0x24, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3C, 0x3A,
	0x3C, 0x1E, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A,
	0x2B, 0x27, 0x23, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3D,
	0x3C, 0x3E, 0x1F};
char negative_gamma[64]    = {0xFB, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A, 0x2B, 0x27,
	0x23, 0x28, 0x2B, 0x2E, 0x32, 0x37, 0x3C, 0x3F, 0x3F, 0x3F,
	0x26, 0x00, 0x3F, 0x29, 0x27, 0x34, 0x34, 0x2D, 0x2C, 0x2C,
	0x28, 0x24, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3C, 0x3A,
	0x3C, 0x1E, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A,
	0x2B, 0x27, 0x23, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3D,
	0x3C, 0x3E, 0x1F};
char amp_type_1[3]     = {0xFC, 0x5A, 0x5A};
char amp_type_2[5]     = {0xF5, 0x5A, 0x55, 0x38, 0x00};

#elif !defined (MIPI_CLOCK_500MBPS)
//EF40K REGISTER SET 400/450Mbps
char passwd1[3]     = {0xF0, 0x5A, 0x5A};
char passwd2[3]     = {0xF1, 0x5A, 0x5A};

#if defined (MIPI_CLOCK_400MBPS)
char id1wr[2]       = {0xB1, 0x53};
char id2wr[2]       = {0xB2, 0xa9};
char id3wr[2]       = {0xB3, 0x60};
#elif defined (MIPI_CLOCK_450MBPS)
char id1wr[2]       = {0xB1, 0x00};
char id2wr[2]       = {0xB2, 0x00};
char id3wr[2]       = {0xB3, 0x00};
#endif
char miectl_1[4]    = {0xC0, 0x80, 0x80, 0x10};
char bcmode[2]      = {0xC1, 0x01};
char miectl_2[2]    = {0xC2, 0x08};
char wrblctl[4]     = {0xC3, 0x00, 0x00, 0x20};
char source_ctl[5]  = {0xF2, 0x03, 0x33, 0xA1, 0x85};
char power_ctl_1[13]= {0xF4, 0x0A, 0x0B, 0x07, 0x07, 0x10, 0x14, 0x0D, 0x0C, 0xAD, 0x00, 0x33, 0x33};
#if defined (MIPI_CLOCK_400MBPS)
char power_ctl_2[21]= {0xF6, 0x02, 0x0F, 0x0E, 0x22, 0x09, 0x00, 0x11, 0x1F, 0x18, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x09, 0x10, 0x51};
#elif defined (MIPI_CLOCK_450MBPS)
char power_ctl_2[21]= {0xF6, 0x02, 0x0E, 0x0C, 0x1E, 0x08, 0x00, 0x0F, 0x1B, 0x16, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x09, 0x10, 0x51};
#endif
char positive_gamma[64]= {0xFA, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A, 0x2B, 0x27,
	0x23, 0x28, 0x2B, 0x2E, 0x32, 0x37, 0x3C, 0x3F, 0x3F, 0x3F,
	0x26, 0x00, 0x3F, 0x29, 0x27, 0x34, 0x34, 0x2D, 0x2C, 0x2C,
	0x28, 0x24, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3C, 0x3A,
	0x3C, 0x1E, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A,
	0x2B, 0x27, 0x23, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3D,
	0x3C, 0x3E, 0x1F};
char negative_gamma[64]= {0xFB, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A, 0x2B, 0x27,
	0x23, 0x28, 0x2B, 0x2E, 0x32, 0x37, 0x3C, 0x3F, 0x3F, 0x3F,
	0x26, 0x00, 0x3F, 0x29, 0x27, 0x34, 0x34, 0x2D, 0x2C, 0x2C,
	0x28, 0x24, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3C, 0x3A,
	0x3C, 0x1E, 0x00, 0x3F, 0x28, 0x27, 0x33, 0x32, 0x2B, 0x2A,
	0x2B, 0x27, 0x23, 0x28, 0x2B, 0x2E, 0x32, 0x36, 0x3A, 0x3D,
	0x3C, 0x3E, 0x1F};

char amp_type_1[3]     = {0xFC, 0x5A, 0x5A};
char amp_type_2[4]     = {0xF5, 0x5A, 0x55, 0x38};
#endif

char sleep_out[2]   = {0x11, 0x00};
char display_ctl[2]  = {0x36, 0x40};
char disp_on[2]     = {0x29, 0x00};
char sleep_in[2]    = {0x10, 0x00};
char disp_off[2]    = {0x28, 0x00};

#if 0
//EF40K REGISTER SET gamma 1.8
char passwd1[3]     = {0xF0, 0x5A, 0x5A};
char passwd2[3]     = {0xF1, 0x5A, 0x5A};
char id1wr[2]       = {0xB1, 0x00};
char id2wr[2]       = {0xB2, 0x00};
char id3wr[2]       = {0xB3, 0x00};
char miectl_1[4]    = {0xC0, 0x80, 0x80, 0x10};
char bcmode[2]      = {0xC1, 0x01};
char miectl_2[2]    = {0xC2, 0x08};
char wrblctl[4]     = {0xC3, 0x00, 0x00, 0x20};
char source_ctl[5]     = {0xF2, 0x03, 0x33, 0xA1, 0x85};
char power_ctl_1[13]     = {0xF4, 0x0A, 0x0B, 0x07, 0x07, 0x10, 0x14, 0x0D, 0x0C, 0xAD, 0x00, 0x33, 0x33};
char power_ctl_2[21]     = {0xF6, 0x02, 0x11, 0x0F, 0x25, 0x0A, 0x00, 0x13, 0x22, 0x1B, 0x03, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x09, 0x10, 0x51};
char positive_gamma[64]     = {0xFA, 0x00, 0x3F, 0x25, 0x22, 0x2c, 0x2b, 0x24, 0x20, 0x21, 0x1b, 
	0x17, 0x18, 0x1B, 0x1b, 0x1d, 0x22, 0x24, 0x25, 0x21, 0x22, 
	0x06, 0x00, 0x3F, 0x26, 0x22, 0x2c, 0x2b, 0x24, 0x21, 0x21, 
	0x1c, 0x17, 0x18, 0x1c, 0x1c, 0x1e, 0x23, 0x26, 0x26, 0x22, 
	0x22, 0x06, 0x00, 0x3F, 0x23, 0x1f, 0x29, 0x29, 0x22, 0x1f, 
	0x1f, 0x1a, 0x16, 0x18, 0x1B, 0x1c, 0x1e, 0x22, 0x25, 0x26, 
	0x22, 0x25, 0x0a};
char negative_gamma[64]    = {0xFB, 0x00, 0x3F, 0x25, 0x22, 0x2c, 0x2b, 0x24, 0x20, 0x21, 0x1b, 
	0x17, 0x18, 0x1B, 0x1b, 0x1d, 0x22, 0x24, 0x25, 0x21, 0x22, 
	0x06, 0x00, 0x3F, 0x26, 0x22, 0x2c, 0x2b, 0x24, 0x21, 0x21, 
	0x1c, 0x17, 0x18, 0x1c, 0x1c, 0x1e, 0x23, 0x26, 0x26, 0x22, 
	0x22, 0x06, 0x00, 0x3F, 0x23, 0x1f, 0x29, 0x29, 0x22, 0x1f, 
	0x1f, 0x1a, 0x16, 0x18, 0x1B, 0x1c, 0x1e, 0x22, 0x25, 0x26, 
	0x22, 0x25, 0x0a};
char amp_type_1[3]     = {0xFC, 0x5A, 0x5A};
char amp_type_2[4]     = {0xF5, 0x5A, 0x55, 0x38};

char sleep_out[2]   = {0x11, 0x00};
char display_ctl[2]  = {0x36, 0x40};
char disp_on[2]     = {0x29, 0x00};
char sleep_in[2]    = {0x10, 0x00};
char disp_off[2]    = {0x28, 0x00};
#endif

#if 0
//EF40K REGISTER SET gamma2.0
char passwd1[3]     = {0xF0, 0x5A, 0x5A};
char passwd2[3]     = {0xF1, 0x5A, 0x5A};
char id1wr[2]       = {0xB1, 0x00};
char id2wr[2]       = {0xB2, 0x00};
char id3wr[2]       = {0xB3, 0x00};
char miectl_1[4]    = {0xC0, 0x80, 0x80, 0x10};
char bcmode[2]      = {0xC1, 0x01};
char miectl_2[2]    = {0xC2, 0x08};
char wrblctl[4]     = {0xC3, 0x00, 0x00, 0x20};
char source_ctl[5]     = {0xF2, 0x03, 0x33, 0xA1, 0x85};
char power_ctl_1[13]     = {0xF4, 0x0A, 0x0B, 0x07, 0x07, 0x10, 0x14, 0x0D, 0x0C, 0xAD, 0x00, 0x33, 0x33};
char power_ctl_2[21]     = {0xF6, 0x02, 0x11, 0x0F, 0x25, 0x0A, 0x00, 0x13, 0x22, 0x1B, 0x03, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x09, 0x10, 0x51};
char positive_gamma[64]     = {0xFA, 0x00, 0x3F, 0x28, 0x25, 0x30, 0x2f, 0x28, 0x26, 0x27, 0x22, 
	0x1d, 0x20, 0x23, 0x25, 0x27, 0x2c, 0x2f, 0x31, 0x2f, 0x31, 
	0x14, 0x00, 0x3F, 0x28, 0x26, 0x30, 0x30, 0x29, 0x26, 0x28, 
	0x22, 0x1e, 0x20, 0x24, 0x25, 0x28, 0x2e, 0x31, 0x32, 0x2e, 
	0x2e, 0x0f, 0x00, 0x3F, 0x26, 0x22, 0x2d, 0x2e, 0x27, 0x24, 
	0x26, 0x20, 0x1d, 0x20, 0x23, 0x26, 0x28, 0x2c, 0x2f, 0x31, 
	0x30, 0x33, 0x17};
char negative_gamma[64]    = {0xFB, 0x00, 0x3F, 0x28, 0x25, 0x30, 0x2f, 0x28, 0x26, 0x27, 0x22, 
	0x1d, 0x20, 0x23, 0x25, 0x27, 0x2c, 0x2f, 0x31, 0x2f, 0x31, 
	0x14, 0x00, 0x3F, 0x28, 0x26, 0x30, 0x30, 0x29, 0x26, 0x28, 
	0x22, 0x1e, 0x20, 0x24, 0x25, 0x28, 0x2e, 0x31, 0x32, 0x2e, 
	0x2e, 0x0f, 0x00, 0x3F, 0x26, 0x22, 0x2d, 0x2e, 0x27, 0x24, 
	0x26, 0x20, 0x1d, 0x20, 0x23, 0x26, 0x28, 0x2c, 0x2f, 0x31, 
	0x30, 0x33, 0x17};
char amp_type_1[3]     = {0xFC, 0x5A, 0x5A};
char amp_type_2[4]     = {0xF5, 0x5A, 0x55, 0x38};

char sleep_out[2]   = {0x11, 0x00};
char display_ctl[2]  = {0x36, 0x40};
char disp_on[2]     = {0x29, 0x00};
char sleep_in[2]    = {0x10, 0x00};
char disp_off[2]    = {0x28, 0x00};
#endif

static struct dsi_cmd_desc sony_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(disp_off), disp_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(sleep_in), sleep_in}
};

static struct dsi_cmd_desc sony_display_on_cmds[] = {
#if defined (MIPI_CLOCK_250MBPS) || defined (MIPI_CLOCK_500MBPS)
	{DTYPE_DCS_WRITE, 1, 0, 0, 140, sizeof(sleep_out), sleep_out},
#endif
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(display_ctl), display_ctl},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(disp_on), disp_on}
};

#if !defined (MIPI_CLOCK_500MBPS)
static struct dsi_cmd_desc sony_display_init_cmds[] = {
#if !defined (MIPI_CLOCK_250MBPS)
    {DTYPE_DCS_WRITE, 1, 0, 0, 140, sizeof(sleep_out), sleep_out},
#endif
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(passwd1), passwd1},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(passwd2), passwd2},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(id1wr), id1wr},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(id2wr), id2wr},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(id3wr), id3wr},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(miectl_1), miectl_1},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(bcmode), bcmode},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(miectl_2), miectl_2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(wrblctl), wrblctl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(source_ctl), source_ctl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(power_ctl_1), power_ctl_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(power_ctl_2), power_ctl_2},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(positive_gamma), positive_gamma},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(negative_gamma), negative_gamma},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(amp_type_1), amp_type_1},
    {DTYPE_GEN_LWRITE, 1, 0, 0, 10, sizeof(amp_type_2), amp_type_2},
};
#endif
/*
 0. sony_display_init_cmds
 1. sony_display_veil_init0_cmds
 2. sony_display_veil_lut_cmds
 3. sony_display_veil_init1_cmds
 4. sony_display_veil_tex_cmds
 5. sony_display_veil_colormap_cmds
 6. sony_display_veil_init2_cmds
 7. dsi_cmd_desc sony_display_on_cmds
 */

static int mipi_sony_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

    ENTER_FUNC2();

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	//mutex_lock(&mfd->dma->ov_mutex);
	if (sony_state.disp_initialized == false) {
		//PRINT("[LIVED] LCD RESET!!\n");
		gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
		usleep(10);//msleep(1);
		gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
		usleep(10);//msleep(120);
#if !defined (MIPI_CLOCK_500MBPS)
		mipi_dsi_cmds_tx(mfd, &sony_tx_buf, sony_display_init_cmds,
				ARRAY_SIZE(sony_display_init_cmds));
#endif
		sony_state.disp_initialized = true;
	}
	mipi_dsi_cmds_tx(mfd, &sony_tx_buf, sony_display_on_cmds,
			ARRAY_SIZE(sony_display_on_cmds));
	sony_state.disp_on = true;
	//mutex_unlock(&mfd->dma->ov_mutex);

	EXIT_FUNC2();
	return 0;
}

static int mipi_sony_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

    ENTER_FUNC2();

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

    //mutex_lock(&mfd->dma->ov_mutex); 
	if (sony_state.disp_on == true) {
		gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
		usleep(10);//msleep(1);
		gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
		usleep(10);//msleep(120);

		mipi_dsi_cmds_tx(mfd, &sony_tx_buf, sony_display_off_cmds,
				ARRAY_SIZE(sony_display_off_cmds));
		sony_state.disp_on = false;
		sony_state.disp_initialized = false;
	}
    //mutex_unlock(&mfd->dma->ov_mutex);    
       EXIT_FUNC2();
	return 0;
}

static int first_enable = 1;//0;
#if (BOARD_REV < WS20)
static int prev_bl_level = 8;//0;
#else
static int prev_bl_level = 16;//0;
#endif
static void mipi_sony_set_backlight(struct msm_fb_data_type *mfd)
{
	int cnt, bl_level;
	//int count = 0;
	unsigned long flags;
	bl_level = mfd->bl_level;

	//PRINT("[LIVED] set_backlight=%d,prev=%d\n", bl_level, prev_bl_level);
	if (bl_level == prev_bl_level || sony_state.disp_on == 0) {
		PRINT("[LIVED] same! or not disp_on\n");
	} else {
		if (bl_level == 0) {
			gpio_set_value(LCD_BL_EN, GPIO_LOW_VALUE);
			usleep(250);      // Disable hold time
			PRINT("[LIVED] same! or not disp_on\n");
		} else {
			if (prev_bl_level == 0) {
				//count++;
				gpio_set_value(LCD_BL_EN, GPIO_HIGH_VALUE);
				if (first_enable == 0) {
					first_enable = 1;
					msleep(25); // Initial enable time
				} else {
					udelay(300);      // Turn on time
				}
				//PRINT("[LIVED] (0) init!\n");
			}
#if (BOARD_REV < WS20)
			if (prev_bl_level < bl_level) {
				cnt = BL_MAX - bl_level;
				cnt += prev_bl_level;
			} else {
				cnt = prev_bl_level - bl_level;
			}
			//PRINT("[LIVED] cnt=%d\n", cnt);
			while (cnt) {
				local_save_flags(flags);
				local_irq_disable();
				gpio_set_value(LCD_BL_EN, GPIO_LOW_VALUE);
				DELAY_3NS();//udelay(3);      // Turn off time
				//count++;
				gpio_set_value(LCD_BL_EN, GPIO_HIGH_VALUE);
			    local_irq_restore(flags);
				udelay(300);      // Turn on time
				//PRINT("[LIVED] (2) cnt=%d!\n", cnt);
				cnt--;
			}
			//PRINT("[LIVED] count=%d\n", count);
#else
			if (prev_bl_level < bl_level) {
				gpio_set_value(LCD_BL_EN, GPIO_LOW_VALUE);
				udelay(200);// TDIS
				cnt = BL_MAX - bl_level + 1;
			} else {
				cnt = prev_bl_level - bl_level;
			}
			//pr_info("[LIVED] cnt=%d, prev_bl_level=%d, bl_level=%d\n",
			//		cnt, prev_bl_level, bl_level);
			while (cnt) {
				local_save_flags(flags);
				local_irq_disable();
				gpio_set_value(LCD_BL_EN, GPIO_LOW_VALUE);
				DELAY_3NS();//udelay(3);      // Turn off time
				gpio_set_value(LCD_BL_EN, GPIO_HIGH_VALUE);
				local_irq_restore(flags);
				udelay(300);      // Turn on time
				cnt--;
			}
#endif
		}
		prev_bl_level = bl_level;
	}
}


static int __devinit mipi_sony_lcd_probe(struct platform_device *pdev)
{
    if (pdev->id == 0) {
        mipi_sony_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_sony_lcd_probe,
	.driver = {
		.name   = "mipi_sony",
	},
};

static struct msm_fb_panel_data sony_panel_data = {
       .on             = mipi_sony_lcd_on,
       .off            = mipi_sony_lcd_off,
       .set_backlight  = mipi_sony_set_backlight,
};

static int ch_used[3];

int mipi_sony_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_sony", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	sony_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &sony_panel_data,
		sizeof(sony_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

#ifdef CONFIG_SW_RESET
extern int msm_reset_reason_read_only(void);
#endif
static int __init mipi_sony_lcd_init(void)
{
    ENTER_FUNC2();

    lcd_gpio_init(lcd_gpio_init_table, ARRAY_SIZE(lcd_gpio_init_table), 1);

#ifdef FEATURE_SKY_BACLIGHT_MAX8831
    led_i2c_api_Init();
#endif

    sony_state.disp_powered_up = true;

    mipi_dsi_buf_alloc(&sony_tx_buf, DSI_BUF_SIZE);
    mipi_dsi_buf_alloc(&sony_rx_buf, DSI_BUF_SIZE);

#ifdef CONFIG_SW_RESET
	if (msm_reset_reason_read_only()) {
		first_enable = 0;
		prev_bl_level = 0;
	}
#endif

    EXIT_FUNC2();

    return platform_driver_register(&this_driver);
}

module_init(mipi_sony_lcd_init);
