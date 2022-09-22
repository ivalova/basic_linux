obj-m := morse.o

ifdef KDIR
KERNEL_SOURCES = $(KDIR)
else
KERNEL_UNAME := $(shell uname -r)
KERNEL_SOURCES = /lib/modules/$(KERNEL_UNAME)/build
endif

EXTRA_CFLAGS=-I$(PWD)/../common

default: modules
.PHONY: default

modules:
	$(MAKE) -C $(KERNEL_SOURCES) M=${PWD} $@

clean:
	$(MAKE) -C $(KERNEL_SOURCES) M=${PWD} $@
