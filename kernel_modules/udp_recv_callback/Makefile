obj-m = udprecvcallback.o

M=$(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(M) modules

clean:
	rm -rf *.o 
