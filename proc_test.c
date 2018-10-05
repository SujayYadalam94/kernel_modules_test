
/*
Web link of this source:
http://tuxthink.blogspot.com/2013/10/creating-read-write-proc-entry-in.html

Modifications made by me in comments
*/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
/* need to include this header as well in latest kernel version */
#include <linux/uaccess.h>

int len,temp;

char *msg;

/*Added this struct to store the return value of proc_create */
struct proc_dir_entry* my_proc_file;

ssize_t read_proc(struct file *filp,char *buf,size_t count,loff_t *offp ) 
{
printk(KERN_INFO "length to read = %u, temp = %u\n",count,temp);
if(count>temp)
{
count=temp;
}
temp=temp-count;
copy_to_user(buf,msg, count);
if(count==0)
temp=len;
   
return count;
}

ssize_t write_proc(struct file *filp,const char *buf,size_t count,loff_t *offp)
{
copy_from_user(msg,buf,count);
len=count;
temp=len;
return count;
}


static struct file_operations proc_fops = {
	.read = &read_proc,
	.write = &write_proc
};

/*Changes made: make sure that parameter is void, dont leave it empty*/
static void create_new_proc_entry(void) 
{
my_proc_file = proc_create("hello",0,NULL,&proc_fops);
if (my_proc_file == NULL)
	printk(KERN_ERR "proc_create failed\n");
else
	printk(KERN_INFO "proc_create passed\n");
//msg=kmalloc(GFP_KERNEL,10*sizeof(char));
/* Kmalloc prototype changed in latest kernel version*/
msg = kmalloc(10*sizeof(char),GFP_KERNEL);
}


int proc_init (void) {
 create_new_proc_entry();
 return 0;
}

void proc_cleanup(void) {
 remove_proc_entry("hello",NULL);
 /* need to free the memory once the module is removed */
 kfree(msg);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);

