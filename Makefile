#obj-m += hello_5.o
#obj-m += hello_4.o
#obj-m += hello_2.o
#obj-m += hello_world.o
#obj-m += proc_test.o
#obj-m += jprobe_test.o
#obj-m += kprobe_example.o
#obj-m += syscall_intercept_test.o
obj-m += pagefault_intercept.o

all:
	make -C /home/sujay/Documents/linux M=$(PWD) modules

clean:
	make -C /home/sujay/Documents/linux M=/home/sujay/Documents/kernel_modules_test clean
