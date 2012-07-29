GCC_VERSION      := $(shell $(CONFIG_SHELL) $(PWD)/scripts/gcc-version.sh $(CROSS_COMPILE)gcc)
ifeq ($(GCC_VERSION),0404)
CFLAGS_REMOVE_msm_vfe8x.o = -Wframe-larger-than=1024
endif

ifeq ($(CONFIG_MSM_CAMERA_V4L2),y)
  obj-$(CONFIG_MSM_CAMERA) += msm_isp.o msm.o
else
  obj-$(CONFIG_MSM_CAMERA) += msm_camera.o
endif
obj-$(CONFIG_MSM_CAMERA) += msm_axi_qos.o
obj-$(CONFIG_MSM_CAMERA_FLASH) += flash.o
obj-$(CONFIG_ARCH_MSM_ARM11) += msm_vfe7x.o msm_io7x.o
obj-$(CONFIG_ARCH_MSM7X30) += msm_vfe31.o msm_io_vfe31.o msm_vpe1.o
obj-$(CONFIG_ARCH_QSD8X50) += msm_vfe8x.o msm_vfe8x_proc.o msm_io8x.o
obj-$(CONFIG_ARCH_MSM8X60) += msm_vfe31.o msm_io_8x60.o msm_vpe1.o
obj-$(CONFIG_MT9T013) += mt9t013.o mt9t013_reg.o
obj-$(CONFIG_SN12M0PZ) += sn12m0pz.o sn12m0pz_reg.o
obj-$(CONFIG_MT9P012) += mt9p012_reg.o
obj-$(CONFIG_MSM_CAMERA_AF_FOXCONN) += mt9p012_fox.o
obj-$(CONFIG_MSM_CAMERA_AF_BAM) += mt9p012_bam.o
obj-$(CONFIG_MT9P012_KM) += mt9p012_km.o mt9p012_km_reg.o
obj-$(CONFIG_MT9E013) += mt9e013.o mt9e013_reg.o
obj-$(CONFIG_S5K3E2FX) += s5k3e2fx.o
#FIXME: Merge the two ifeq causes VX6953 preview not coming up.
ifeq ($(CONFIG_MSM_CAMERA_V4L2),y)
  obj-$(CONFIG_VX6953) += vx6953_v4l2.o vx6953_reg_v4l2.o
else
  obj-$(CONFIG_VX6953) += vx6953.o vx6953_reg.o
endif
obj-$(CONFIG_QS_S5K4E1) += qs_s5k4e1.o qs_s5k4e1_reg.o
obj-$(CONFIG_VB6801) += vb6801.o
obj-$(CONFIG_IMX074) += imx074.o imx074_reg.o
obj-$(CONFIG_WEBCAM_OV9726) += ov9726.o ov9726_reg.o
obj-$(CONFIG_WEBCAM_OV7692) += ov7692.o
obj-$(CONFIG_MT9D112) += mt9d112.o mt9d112_reg.o
obj-$(CONFIG_MSM_GEMINI) += msm_gemini_dev.o msm_gemini_sync.o msm_gemini_core.o msm_gemini_hw.o msm_gemini_platform.o

obj-$(CONFIG_PANTECH_CAMERA_CE1612) += ce1612.o ce1612_reg.o
obj-$(CONFIG_PANTECH_CAMERA_ICP_HD) += icp_hd.o icp_hd_reg.o camsensor_icp_hd_tuneup.o camsensor_parser.o
obj-$(CONFIG_PANTECH_CAMERA_MT9P111) += mt9p111.o mt9p111_reg.o camsensor_mt9p111_tuneup.o
obj-$(CONFIG_PANTECH_CAMERA_S5K4ECGX) += s5k4ecgx_r2.o s5k4ecgx_cfg.o 
#mt9d113 camera module needs 2 tunning values, for SKT and none-SKT.
ifeq ($(CONFIG_MACH_MSM8X60_EF40S),y)
  obj-$(CONFIG_PANTECH_CAMERA_MT9D113) += mt9d113_r2.o mt9d113_cfg_SKT.o
else
  obj-$(CONFIG_PANTECH_CAMERA_MT9D113) += mt9d113_r2.o mt9d113_cfg.o
endif
obj-$(CONFIG_PANTECH_CAMERA_S5K6AAFX13) += s5k6aafx13_r2.o s5k6aafx13_cfg.o
obj-$(CONFIG_PANTECH_CAMERA_MT9V113) += mt9v113_r2.o mt9v113_cfg.o

obj-$(CONFIG_PANTECH_CAMERA) += sensor_i2c.o sensor_ctrl.o
obj-$(CONFIG_PANTECH_CAMERA_TUNER) += ptune_parser.o
