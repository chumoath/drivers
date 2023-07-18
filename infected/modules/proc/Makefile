# CROSS_COMPILE ARCH PATH 都在 /etc/environment 中配置了，可以直接使用
# KERNEL_DIR 是从 CMAKE 中传过来的
# MODULE_PATH 也是从 CMAKE 传过来的
ifneq ($(KERNELRELEASE), )

obj-m := proc_drv.o
proc_drv-objs := proc.o

else

PWD := $(shell pwd)

default:
	#cp -f ${MODULE_PATH}/led/led_v1/led.c ${PWD}/
	#cp -f ${MODULE_PATH}/led/led_v1/led.h ${PWD}/
	make -C $(KERNEL_DIR) M=$(PWD) modules
	# ${CROSS_COMPILE}strip -d *.ko
clean:
	rm -rf Module.s* modules.order *.markers .tmp_versions
	#mv ${PWD}/test.c ${PWD}/test_c
	rm -f *.c
	#mv ${PWD}/test_c ${PWD}/test.c
	make -C $(KERNEL_DIR) M=$(PWD) clean

endif