# multiple files module :
obj-m+=gpio_custom_debounced.o

#CROSS_COMPILE	=	$(CROSS_COMPILE_BUILDROOT)
CROSS_COMPILE	= armv5l-linux-
ARCH		      =	arm
CC		        =	$(CROSS_COMPILE)gcc
KDIR          = /home/user/Projects/bsp/ev_link_bsp/output/build/linux-4.4.14
PWM           = `pwd`

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) $@

clean:
	rm -rf *.o *.ko *.mod.c *.cmd .*.cmd  .tmp_versions Module.symvers
