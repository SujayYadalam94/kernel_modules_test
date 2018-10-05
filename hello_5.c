#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

static short int myshort = 1;
static int myint = 2;
static char *mystring = "empty";
static int myarray[] = {-1, -1};
static int arr_argc = 0;

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "a short integer");
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(mystring, charp, 0000);

module_param_array(myarray, int, &arr_argc, 0000);

static int __init hello_5_init(void)
{
	printk(KERN_INFO "Hello world 5\n");
	printk(KERN_INFO "myshort=%hd\n",myshort);
	printk(KERN_INFO "myint=%d\n",myint);
	printk(KERN_INFO "mystring=%s\n",mystring);
	printk(KERN_INFO "array1=%d array2=%d\n",myarray[0], myarray[1]);
	printk(KERN_INFO "got %d arguments for myarray\n",arr_argc);
	
	return 0;
}

static void __exit cleanup_hello_5(void)
{
	printk(KERN_INFO "Goodbye world\n");
}

module_init(hello_5_init);
module_exit(cleanup_hello_5);

