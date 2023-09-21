obj-m += custom_syscall.o

install:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules