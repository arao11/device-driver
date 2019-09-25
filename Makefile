obj-m := simple_buffer.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		#make -C /lib/modules/`uname -r`/build M=$PWD modules_install

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

