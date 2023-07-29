# CROSS_COMPILE ARCH PATH 都在 /etc/environment 中配置了，可以直接使用
# KERNEL_DIR 是从 CMAKE 中传过来的
# MODULE_PATH 也是从 CMAKE 传过来的
ifneq ($(KERNELRELEASE), )

obj-m := i2c_drv.o
i2c_drv-objs := i2c.o
KBUILD_EXTRA_SYMBOLS += ${SYMVERS_DIR}/common.symvers
EXTRA_CFLAGS += -I${KO_INC}
else

PWD := $(shell pwd)

default:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	# ${CROSS_COMPILE}strip -d *.ko
	mv Module.symvers i2c.symvers
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.c
	rm -rf Module.s* modules.order *.markers .tmp_versions

endif
