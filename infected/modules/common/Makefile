# CROSS_COMPILE ARCH PATH 都在 /etc/environment 中配置了，可以直接使用
# KERNEL_DIR 是从 CMAKE 中传过来的
# MODULE_PATH 也是从 CMAKE 传过来的
ifneq ($(KERNELRELEASE), )

obj-m := common_drv.o
common_drv-objs := common.o

else

PWD := $(shell pwd)

default:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	# ${CROSS_COMPILE}strip -d *.ko
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.c
	rm -rf Module.s* modules.order *.markers .tmp_versions

endif
