
# Kernel makefile for motions sensors
#
# 

# MPU
obj-$(CONFIG_EF39S_SENSORS_MPU3050)	+= mpu3050.o
mpu3050-objs += mpuirq.o \
	slaveirq.o \
	mpu-dev.o \
	mpu-i2c.o \
	mlsl-kernel.o \
	mlos-kernel.o \
	$(MLLITE_DIR)mldl_cfg.o

#
# Accel options
#
mpu3050-$(CONFIG_EF39S_SENSORS_BMA150) += bma150.o
mpu3050-$(CONFIG_EF39S_SENSORS_BMA250) += bma250.o

#
# Compass options
#
#mpu3050-$(CONFIG_EF39S_SENSORS_YAS529) += yas529-kernel.o \
#	yas_mag_kernel_driver.c 
mpu3050-$(CONFIG_EF39S_SENSORS_YAS530) += yas_mag_kernel_driver.o \
	yas529-kernel.o

EXTRA_CFLAGS += -I$(M)/$(MLLITE_DIR) \
                -I$(M)/../../include \
		-Idrivers/misc/mpu3050 \
                -Iinclude/linux

ifdef CONFIG_SENSORS_MPU_DEBUG
EXTRA_CFLAGS += -DDEBUG 
endif

