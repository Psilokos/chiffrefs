ifeq ($(KERNELRELEASE),)
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)
    RULE = $(MAKE) -C $(KERNELDIR) M=$(PWD) $@

modules:
	$(RULE)
modules_install:
	$(RULE)
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions test test.o modules.order Module.symvers

.PHONY: modules modules_install clean
else
    obj-m := chiffrefs.o
    ccflags-y := # -Wno-declaration-after-statement
endif
