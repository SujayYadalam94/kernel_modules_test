#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

int hello_2_init(void)
{
	printk(KERN_INFO "Hello 2\n");
	return 0;
}

void hello_2_cleanup(void)
{
	printk(KERN_INFO "Goodbye 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_cleanup);

