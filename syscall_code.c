#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>

int main(void)
{
	long int ret = syscall(333,10);
	printf("System call return value:%ld\n",ret);
	return 0;
}
