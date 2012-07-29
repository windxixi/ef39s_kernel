#
# Makefile for the earjack drivers.
#


obj-$(CONFIG_TOUCHSCREEN_QT602240) += qt602240.o
#obj-m := qt602240.o

KDIR := ../../../../../out/target/product/msm8660_surf/obj/KERNEL_OBJ/
PWD := $(shell pwd)

#EXTRA_CFLAGS += -include ../../../../../../../BOARD_REV.h

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -f -r *.o *.ko .*cmd *.mod.c .tmp* core *.i modules.order Module.symvers modules.builtin
