# CROSS_COMPILE ARCH PATH 都在 /etc/environment 中配置了，可以直接使用
# KERNEL_DIR 是从 CMAKE 中传过来的
# MODULE_PATH 也是从 CMAKE 传过来的

MODULE_NAME=semaphore_drv

ifneq ($(KERNELRELEASE), )

obj-m := ${MODULE_NAME}.o
${MODULE_NAME}-objs := semaphore.o
KBUILD_EXTRA_SYMBOLS += ${SYMVERS_DIR}/common.symvers
EXTRA_CFLAGS += -I${KO_INC}

else

PWD := $(shell pwd)

default:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	# ${CROSS_COMPILE}strip -d *.ko
	mv Module.symvers $(MODULE_NAME).symvers
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.c
	rm -rf Module.s* modules.order *.markers .tmp_versions

endif
