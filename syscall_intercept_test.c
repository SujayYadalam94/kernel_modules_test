#include<linux/module.h> 
#include<linux/version.h> 
#include<linux/kernel.h> 
#include<linux/init.h> 
#include<linux/kprobes.h> 
#include<net/ip.h> 


int my_handler (int pid){ 
    printk(KERN_ALERT "Syscall intercepted pid=%d\n",pid);
    jprobe_return(); 
} 
 
static struct jprobe my_probe; 
 
int myinit(void) 
{ 
    int ret;
    my_probe.kp.symbol_name = "sys_syscall_test"; 
    my_probe.entry = (kprobe_opcode_t *)my_handler; 
    ret = register_jprobe(&my_probe); 
    if(ret == 0)
	printk(KERN_INFO "Module initialized\n");
    else
	printk(KERN_ERR "Register return value=%d\n",ret);
    return 0; 
} 
 
void myexit(void) 
{ 
    unregister_jprobe(&my_probe); 
    printk(KERN_INFO "module removed\n "); 
} 
 
module_init(myinit); 
module_exit(myexit); 
 
/*Kernel module Comments*/
MODULE_LICENSE("GPL"); 

