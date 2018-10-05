#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "Hello world\n");
	return 0; //non 0 return value means init_module failed;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world\n");
}
