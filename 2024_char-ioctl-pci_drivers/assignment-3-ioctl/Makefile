obj-m := HW3_DF.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C  $(KERNEL_DIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C  $(KERNEL_DIR) M=$(shell pwd) clean
