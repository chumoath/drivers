CROSS_COMPILE := arm-linux-gnueabihf-
ARCH := arm

ifneq ($(KERNELRELEASE), )

obj-m := led.o

else

KERNEL_DIR := /home/wjxh/linux/linux/linux

CC_PREFIX := /home/wjxh/linux/tool/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin

MODULE_DIR := $(shell pwd)

all:
	pwd
	echo ${PATH}
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(MODULE_DIR) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(MODULE_DIR) clean
endif