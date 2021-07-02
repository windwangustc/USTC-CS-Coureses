CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_FORCE=n
CONFIG_MODULE_SIG_ALL=n
obj-m += sysfs_test.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules

clean:
	rm -rf *.o  *.mod.c .*.cmd .tmp_versions *.order *.symvers
