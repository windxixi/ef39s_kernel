/* drivers/video/msm/logo.c
 *
 * Show Logo in RLE 565 format
 *
 * Copyright (C) 2008 Google Incorporated
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/vt_kern.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/irq.h>
#include <asm/system.h>

#if defined(CONFIG_SKY_CHARGING) || defined(CONFIG_SKY_SMB_CHARGER)
#include "msm_fb.h"
#endif

#define fb_width(fb)	((fb)->var.xres)
#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 2)

#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
typedef unsigned int IBUF_TYPE;
#if 0
static void memset24(void *_ptr, unsigned short val, unsigned count)
{
    unsigned char *ptr = _ptr;
    unsigned char r, g, b;

    r = (unsigned char)((val & 0xf800) >> 8);
    g = (unsigned char)((val & 0x07e0) >> 3);
    b = (unsigned char)((val & 0x001f) << 3);

    count >>= 1;
    while (count--)
    {
        *ptr++ = b;
        *ptr++ = g;
        *ptr++ = r;
        *ptr++ = 0; // 32bpp
    }
}
#else
static void memset32(void *_ptr, unsigned int val, unsigned count)
{
	unsigned int *ptr = _ptr;
	count >>= 2;
	while (count--)
		*ptr++ = val;
}
#endif

#else
typedef unsigned short IBUF_TYPE;

static void memset16(void *_ptr, unsigned short val, unsigned count)
{
	unsigned short *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#endif  // CONFIG_F_SKYDISP_FRAMEBUFFER_32

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
int load_565rle_image(char *filename)
{
	struct fb_info *info;
	int fd, count, err = 0;
	unsigned max;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
    IBUF_TYPE *data, *bits, *ptr;
#else
	unsigned short *data, *bits, *ptr;
#endif

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
			__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, filename);
		return -ENOENT;
	}
	count = sys_lseek(fd, (off_t)0, 2);
	if (count <= 0) {
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if (sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = data;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
	bits = (IBUF_TYPE *)(info->screen_base);
#else
	bits = (unsigned short *)(info->screen_base);
#endif
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
	while (count > 7)
#else
	while (count > 3)
#endif
	{
		unsigned n = ptr[0];
		if (n > max)
			break;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
		memset32((unsigned int *)bits, ptr[1], n << 2);
#else
		memset16(bits, ptr[1], n << 1);
#endif
		bits += n;
		max -= n;
		ptr += 2;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
		count -= 8;
#else
		count -= 4;
#endif
	}

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(load_565rle_image);

#ifdef CONFIG_SW_RESET
int load_raw_image(char *filename)
{
	struct fb_info *info;
	int fd, count, err = 0;
	unsigned max;
	IBUF_TYPE *bits;
	char *data;

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
				__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
				__func__, filename);
		return -ENOENT;
	}
	count = sys_lseek(fd, (off_t)0, 2);
	if (count <= 0) {
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if (sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	bits = (IBUF_TYPE *)(info->screen_base);
	while (max--) {
		bits[max] = data[max*3-1] << 16;
		bits[max] |= data[max*3-2] << 8;
		bits[max] |= data[max*3];
	}

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(load_raw_image);
#endif

#if defined(CONFIG_SKY_CHARGING) || defined(CONFIG_SKY_SMB_CHARGER)
#ifdef CONFIG_FB_MSM_MIPI_DSI
void mdp4_dsi_video_overlay(struct msm_fb_data_type *mfd);
#endif
int display_low_battery_image(void)
{
	struct fb_info *info;
	int fd, count, err = 0;
	unsigned max;
	IBUF_TYPE *data, *bits, *ptr;
	char *filename = "/logo3.rle";

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
				__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
				__func__, filename);
		return -ENOENT;
	}
	count = sys_lseek(fd, (off_t)0, 2);
	if (count <= 0) {
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if (sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = data;
	bits = (IBUF_TYPE *)(info->screen_base);
	bits += info->var.xres * info->var.yres;

	while (count > 7) {
		unsigned n = ptr[0];
		if (n > max)
			break;
		memset32((unsigned int *)bits, ptr[1], n << 2);
		bits += n;
		max -= n;
		ptr += 2;
		count -= 8;
	}

	info->var.yoffset = info->var.yres;
#ifdef CONFIG_FB_MSM_MIPI_DSI
	mdp4_dsi_video_overlay((struct msm_fb_data_type *)info->par);
#endif

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(display_low_battery_image);
#endif
