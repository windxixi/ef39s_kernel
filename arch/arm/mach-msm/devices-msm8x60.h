/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __ARCH_ARM_MACH_MSM_DEVICES_MSM8X60_H
#define __ARCH_ARM_MACH_MSM_DEVICES_MSM8X60_H

#define MSM_GSBI3_QUP_I2C_BUS_ID 0
#define MSM_GSBI4_QUP_I2C_BUS_ID 1
#define MSM_GSBI9_QUP_I2C_BUS_ID 2
#define MSM_GSBI8_QUP_I2C_BUS_ID 3
#define MSM_GSBI7_QUP_I2C_BUS_ID 4
#define MSM_GSBI12_QUP_I2C_BUS_ID 5
#define MSM_SSBI1_I2C_BUS_ID     6
#define MSM_SSBI2_I2C_BUS_ID     7
#define MSM_SSBI3_I2C_BUS_ID     8

#ifdef CONFIG_TOUCHSCREEN_MELFAS_TKI
#define MSM_TKI_I2C_BUS_ID 12
#endif
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020  // jmlee 20110505 add
//#define MSM_GSBI10_QUP_I2C_BUS_ID     9
#define MSM_AUDIENCE_A2020_I2C_BUS_ID     9
#endif
#define MSM_LED_I2C_BUS_ID 10
#define MSM_MHL_I2C_BUS_ID 11
#ifdef CONFIG_TOUCHSCREEN_MELFAS_TKI
#define MSM_TKI_I2C_BUS_ID 12
#endif

#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20))
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
#define MSM_GSBI5_QUP_I2C_BUS_ID 33
#endif
#endif

#ifdef CONFIG_SKY_SMB_CHARGER
#define MSM_SMB_I2C_BUS_ID 39
#endif

#if defined(CONFIG_PN544)
#define MSM_GSBI10_QUP_I2C_BUS_ID 21
#endif

#ifdef CONFIG_SKY_BATTERY_MAX17040   // p14682 kobj  110607 
#define MSM_GSBI11_QUP_I2C_BUS_ID	16
#endif  //CONFIG_SKY_BATTERY_MAX17040
#ifdef CONFIG_SPI_QUP
extern struct platform_device msm_gsbi1_qup_spi_device;
extern struct platform_device msm_gsbi10_qup_spi_device;
#endif

#ifdef CONFIG_SKY_TDMB_SPI_QUP
extern struct platform_device msm_gsbi2_qup_spi_device;
#endif

#ifdef CONFIG_MSM_BUS_SCALING
extern struct platform_device msm_bus_apps_fabric;
extern struct platform_device msm_bus_sys_fabric;
extern struct platform_device msm_bus_mm_fabric;
extern struct platform_device msm_bus_sys_fpb;
extern struct platform_device msm_bus_cpss_fpb;
#endif

extern struct platform_device msm_device_smd;
extern struct platform_device msm_kgsl_3d0;
#ifdef CONFIG_MSM_KGSL_2D
extern struct platform_device msm_kgsl_2d0;
extern struct platform_device msm_kgsl_2d1;
#endif
extern struct platform_device msm_device_gpio;
extern struct platform_device msm_device_vidc;

extern struct platform_device msm_charm_modem;

#ifdef CONFIG_HW_RANDOM_MSM
extern struct platform_device msm_device_rng;
#endif

void __init msm8x60_init_irq(void);
#ifdef CONFIG_MSM_KGSL_2D
void __init msm8x60_check_2d_hardware(void);
#endif

#ifdef CONFIG_FB_MSM_MHL_SII9244  // MHL_KKCHO
int mhl_power_ctrl(int on);
#endif

#ifdef CONFIG_MSM_DSPS
extern struct platform_device msm_dsps_device;
#endif

#if defined(CONFIG_MSM_RPM_STATS_LOG)
extern struct platform_device msm_rpm_stat_device;
#endif
#endif
