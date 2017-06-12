obj-m += kprobe_example.o
obj-m += jprobe_example.o
obj-m += kretprobe_example.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
